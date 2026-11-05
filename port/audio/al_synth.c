/* Vivid Dolls port - libultra synthesizer driver (alInit, alAudioFrame, alSyn*)
 * with a software mixer on the SH4 instead of RSP audio command lists.
 *
 * Driver logic as in the ROM (0x80020150-0x80020FA0): the players (sequence
 * player, sound player) are ALPlayer clients; alAudioFrame first calls every
 * client whose next callback falls inside this frame (all "early", at the
 * start of the frame, as libultra does), then renders the frame. The players'
 * alSyn* calls do not act at once: each becomes a parameter update stamped
 * with the sample time of the callback (paramSamples, 16-sample aligned) and
 * is applied to the physical voice at that sample while the frame is mixed.
 *
 * Rendering (the part the RSP did): for each physical voice, between its
 * update times:
 *   - ADPCM decode of the bank sample (libultra VADPCM: 9-byte frames of 16
 *     samples, order-2 predictor book; loops restart from the frame holding the
 *     loop start, whose decoded samples are stored in the loop state), read
 *     straight from the ROM image (no DMA cache needed),
 *   - resampling by the voice pitch (linear interpolation; ratio < 2 as libultra),
 *   - envelope mixer: volume = vol^2 (as libultra's envmixer), equal-power pan
 *     (cosine table), dry/wet split by the effect mix, exponential volume ramps
 *     updated every 8 samples (as the RSP envelope mixer),
 *   - the effect bus (fxType 1 = small room, or 6 = the game's custom table):
 *     libultra's reverb network (delay line, all-pass/comb sections with
 *     feed-forward / feedback coefficients, one-pole low-pass, output gain),
 *     mono, added to both channels; chorus modulation is not implemented (the
 *     game's configurations have none).
 * Output: interleaved 16-bit stereo into the buffer the audio manager passed.
 * The returned command list holds one dummy command, so the game still sends
 * its audio task (the RCP completes it at once) and then hands the buffer to
 * osAiSetNextBuffer one frame later, as on the N64.
 */
#include <kos.h>
#include <math.h>
#include <string.h>
#include "al_internal.h"
#include "vd_debug.h"

#define VD_MAX_PVOICES  64
#define VD_MAX_PARAMS   1024
#define VD_MAX_FRAME    1024             /* output samples per frame (buffer is 0x1000 bytes) */
#define VD_MAX_RATIO    1.99996f
#define VD_BLOCK        128              /* output samples per inner mixing call */
#define VD_GROUP        32               /* samples per volume-ramp step (the RSP: 8) */
#define VD_PCM_BUF      (2 * VD_BLOCK + 24)

volatile VdAudioStats vd_audio_stats;
void *alGlobals;

enum { P_START, P_START_ALT, P_STOP, P_FREE, P_VOLUME, P_PAN, P_PITCH, P_FXAMT };

typedef struct VdParam_s {
    struct VdParam_s *next;
    s32 delta;                         /* sample time */
    s16 type;
    u8  pan, fxmix;
    s16 vol;
    s32 samples;
    f32 pitch;
    ALWaveTable *wave;
    struct PVoice_s *pv;
} VdParam;

typedef struct PVoice_s {
    ALLink node;                       /* free / alloc / lame list */
    ALVoice *vvoice;
    s32 offset;                        /* 512 when stolen */
    VdParam *ctrlList, *ctrlTail;

    /* source (loader) */
    const u8 *data;
    s32 type;                          /* AL_ADPCM_WAVE / AL_RAW16_WAVE, -1 none */
    s32 nsamp;
    const s16 *book;
    s32 npred;
    s32 has_loop, loop_start, loop_end, loop_count;
    const s16 *loop_state;
    /* decoder state */
    s32 looping, done;
    s32 smp;                           /* next sample to deliver */
    s32 fbuf_frame;
    s32 h2, h1;
    s16 fbuf[16];

    /* pre-decoded samples (al_pcm.c), or NULL: decode on the fly into pcm[] */
    const s16 *pre;
    s32 pos;                           /* integer sample position (pre-decoded mode) */
    s32 lim;                           /* loop end (looping) or end of the wave */

    /* resampler */
    u32 frac, step;
    s32 pcm_len, pcm_real;
    s16 pcm[VD_PCM_BUF];

    /* gains of the current 8-sample group */
    s32 gphase;                        /* samples done in the group (0..7) */
    s32 gl, gr, gw, gm;
    s32 mono;                          /* pan 64: mixed once into mix_mono */
    s32 *mono_base;                    /* mix_mono at this render's acc position */

    /* envelope mixer */
    u8  playing, silent;
    u8  pan, fxmix;
    s32 volume;                        /* vol^2 >> 15 */
    s32 dry, wet;                      /* eqpower[fxmix], eqpower[127 - fxmix] */
    f32 curL, curR, tgtL, tgtR, rateL, rateR;
    s32 segLeft;                       /* samples left in the current ramp */
} PVoice;

typedef struct {
    s32 input, output;
    s16 ffcoef, fbcoef, gain;
    s32 lp;                            /* low-pass on */
    s32 lp_fc2, lp_gain, lp_y;
} VdDelay;

typedef struct {
    s32 inited;
    ALPlayer *head;
    ALLink pFreeList, pAllocList, pLameList;
    s32 paramSamples, curSamples;
    s32 outputRate;
    s32 maxPVoices;
    VdParam *paramFree;
    /* effect */
    s32 fx_on, fx_sections, fx_len, fx_head, fx_tail;
    VdDelay fx[8];
} VdSynth;

static VdSynth syn;
static PVoice pvoices[VD_MAX_PVOICES];
static VdParam params[VD_MAX_PARAMS];
static s16 fx_line[8192] __attribute__((aligned(32)));
static s32 mix_acc[2 * VD_MAX_FRAME] __attribute__((aligned(32)));
static s32 mix_wet[VD_MAX_FRAME] __attribute__((aligned(32)));
static s32 mix_mono[VD_MAX_FRAME] __attribute__((aligned(32)));   /* centre-panned voices */
static s16 eqpower[128];
static s32 stereo_used;              /* a non-centre voice wrote mix_acc this frame */

/* libultra's small-room reverb (alFxNew, fxType 1); the same values as the ROM's
   table: sections, length; then per section: input, output, fbcoef, ffcoef,
   gain, chorus rate, chorus depth, low-pass coefficient. (x ms = x * 40 samples) */
static const s32 SMALLROOM_PARAMS[26] = {
    3, 100 * 40,
    0, 54 * 40, 9830, -9830, 0, 0, 0, 0,
    19 * 40, 38 * 40, 3276, -3276, 0x3FFF, 0, 0, 0,
    0, 60 * 40, 5000, 0, 0, 0, 0, 0x5000
};

static inline s32 clamp16(s32 x)
{
    if (x > 32767)
        return 32767;
    if (x < -32768)
        return -32768;
    return x;
}

static void eqpower_init(void)
{
    s32 i;

    /* round(32767 * cos(i * pi / 254)): identical to libultra's table */
    for (i = 0; i < 128; i++)
        eqpower[i] = (s16)floor(32767.0 * cos((f64)i * 3.14159265358979323846 / 254.0) + 0.5);
}

/* ------------------------------------------------------------ parameters */

static VdParam *param_alloc(void)
{
    VdParam *p = syn.paramFree;

    if (p == NULL) {
        vd_audio_stats.params_lost++;
        return NULL;
    }
    syn.paramFree = p->next;
    memset(p, 0, sizeof(*p));
    return p;
}

static void param_free(VdParam *p)
{
    p->next = syn.paramFree;
    syn.paramFree = p;
}

static void param_add(PVoice *pv, VdParam *p)
{
    p->next = NULL;
    if (pv->ctrlTail)
        pv->ctrlTail->next = p;
    else
        pv->ctrlList = p;
    pv->ctrlTail = p;
}

static VdParam *voice_param(ALVoice *v, s32 type)
{
    VdParam *p;

    if (v == NULL || v->pvoice == NULL)
        return NULL;
    p = param_alloc();
    if (p == NULL)
        return NULL;
    p->delta = syn.paramSamples + v->pvoice->offset;
    p->type = (s16)type;
    return p;
}

static s32 time_to_samples(ALMicroTime t)
{
    /* _timeToSamples: rounded to a multiple of 16 samples */
    s32 s = (s32)((f32)((f64)((f32)t * (f32)syn.outputRate) / 1000000.0 + 0.5));

    return s & ~0xF;
}

static s32 time_to_samples_noround(ALMicroTime t)
{
    return (s32)((f32)((f64)((f32)t * (f32)syn.outputRate) / 1000000.0 + 0.5));
}

/* ------------------------------------------------------------ voices */

static void pv_free(PVoice *pv)
{
    alUnlink(pv);
    alLink(pv, &syn.pLameList);
}

static s32 pv_allocate(PVoice **out, s16 priority)
{
    ALLink *dl;
    PVoice *pv;
    s32 stolen = 0;

    *out = NULL;
    if ((dl = syn.pLameList.next) != NULL) {
        *out = (PVoice *)dl;
        alUnlink(dl);
        alLink(dl, &syn.pAllocList);
    } else if ((dl = syn.pFreeList.next) != NULL) {
        *out = (PVoice *)dl;
        alUnlink(dl);
        alLink(dl, &syn.pAllocList);
    } else {
        for (dl = syn.pAllocList.next; dl != NULL; dl = dl->next) {
            pv = (PVoice *)dl;
            if (pv->vvoice && pv->vvoice->priority <= priority && pv->offset == 0) {
                *out = pv;
                priority = pv->vvoice->priority;
                stolen = 1;
            }
        }
    }
    return stolen;
}

void alSynAddPlayer(ALSynth *drvr, ALPlayer *client)
{
    irq_mask_t s = irq_disable();

    (void)drvr;
    client->samplesLeft = syn.curSamples;
    client->next = syn.head;
    syn.head = client;
    irq_restore(s);
}

s32 alSynAllocVoice(ALSynth *drvr, ALVoice *voice, ALVoiceConfig *vc)
{
    PVoice *pv = NULL;
    VdParam *p;
    s32 stolen;

    (void)drvr;
    voice->priority = vc->priority;
    voice->unityPitch = vc->unityPitch;
    voice->table = 0;
    voice->fxBus = vc->fxBus;
    voice->state = AL_STOPPED;
    voice->pvoice = 0;
    if (!syn.inited)
        return 0;
    stolen = pv_allocate(&pv, vc->priority);
    if (pv) {
        if (stolen) {
            pv->offset = 512;
            p = param_alloc();
            if (p) {
                p->delta = syn.paramSamples;
                p->type = P_STOP;
                param_add(pv, p);
            }
        } else {
            pv->offset = 0;
        }
        pv->vvoice = voice;
        voice->pvoice = pv;
    }
    return pv != NULL;
}

void alSynFreeVoice(ALSynth *drvr, ALVoice *voice)
{
    VdParam *p;

    (void)drvr;
    if (voice->pvoice == NULL)
        return;
    if (voice->pvoice->offset) {         /* stolen: free after the old voice's stop */
        p = param_alloc();
        if (p) {
            p->delta = syn.paramSamples + voice->pvoice->offset;
            p->type = P_FREE;
            p->pv = voice->pvoice;
            param_add(voice->pvoice, p);
        }
    } else {
        pv_free(voice->pvoice);
    }
    voice->pvoice = 0;
}

void alSynStartVoice(ALSynth *drvr, ALVoice *voice, ALWaveTable *w)
{
    VdParam *p = voice_param(voice, P_START);

    (void)drvr;
    if (p) {
        p->wave = w;
        param_add(voice->pvoice, p);
    }
}

void alSynStartVoiceParams(ALSynth *drvr, ALVoice *voice, ALWaveTable *w, f32 pitch, s16 vol,
                           ALPan pan, u8 fxmix, ALMicroTime t)
{
    VdParam *p = voice_param(voice, P_START_ALT);

    (void)drvr;
    if (p) {
        p->wave = w;
        p->pitch = pitch;
        p->vol = vol;
        p->pan = pan;
        p->fxmix = fxmix;
        p->samples = time_to_samples(t);
        param_add(voice->pvoice, p);
    }
}

void alSynStopVoice(ALSynth *drvr, ALVoice *voice)
{
    VdParam *p = voice_param(voice, P_STOP);

    (void)drvr;
    if (p)
        param_add(voice->pvoice, p);
}

void alSynSetVol(ALSynth *drvr, ALVoice *voice, s16 vol, ALMicroTime t)
{
    VdParam *p = voice_param(voice, P_VOLUME);

    (void)drvr;
    if (p) {
        p->vol = vol;
        p->samples = time_to_samples(t);
        param_add(voice->pvoice, p);
    }
}

void alSynSetPitch(ALSynth *drvr, ALVoice *voice, f32 pitch)
{
    VdParam *p = voice_param(voice, P_PITCH);

    (void)drvr;
    if (p) {
        p->pitch = pitch;
        param_add(voice->pvoice, p);
    }
}

void alSynSetPan(ALSynth *drvr, ALVoice *voice, ALPan pan)
{
    VdParam *p = voice_param(voice, P_PAN);

    (void)drvr;
    if (p) {
        p->pan = pan;
        param_add(voice->pvoice, p);
    }
}

void alSynSetFXMix(ALSynth *drvr, ALVoice *voice, u8 fxmix)
{
    VdParam *p = voice_param(voice, P_FXAMT);

    (void)drvr;
    if (p) {
        p->fxmix = fxmix;
        param_add(voice->pvoice, p);
    }
}

void alSynSetPriority(ALSynth *drvr, ALVoice *voice, s16 priority)
{
    (void)drvr;
    voice->priority = priority;
}

/* ------------------------------------------------------------ the loader */

static void src_set_wave(PVoice *v, ALWaveTable *w)
{
    u32 base, len;

    v->type = -1;
    v->has_loop = 0;
    if (w == NULL || vd_rom_image == NULL)
        return;
    base = (u32)(uintptr_t)w->base;
    len = (u32)w->len;
    if (base >= vd_rom_size || len > vd_rom_size - base)
        return;
    v->data = vd_rom_image + base;
    if (w->type == AL_ADPCM_WAVE) {
        ALADPCMBook *b = w->waveInfo.adpcm.book;
        ALADPCMloop *l = w->waveInfo.adpcm.loop;
        if (b == NULL || b->order != 2 || b->npredictors < 1)
            return;
        v->book = b->book;
        v->npred = b->npredictors;
        v->nsamp = (s32)(len / 9) * 16;
        if (l && l->count != 0 && l->end > l->start) {
            v->has_loop = 1;
            v->loop_start = (s32)l->start;
            v->loop_end = (s32)l->end;
            v->loop_count = (s32)l->count;
            v->loop_state = l->state;
        }
        v->type = AL_ADPCM_WAVE;
    } else if (w->type == AL_RAW16_WAVE) {
        ALRawLoop *l = w->waveInfo.raw.loop;
        v->nsamp = (s32)(len / 2);
        if (l && l->count != 0 && l->end > l->start) {
            v->has_loop = 1;
            v->loop_start = (s32)l->start;
            v->loop_end = (s32)l->end;
            v->loop_count = (s32)l->count;
            v->loop_state = NULL;
        }
        v->type = AL_RAW16_WAVE;
    }
    if (v->has_loop && v->loop_end > v->nsamp)
        v->loop_end = v->nsamp;
    if (v->has_loop && v->loop_start >= v->loop_end)
        v->has_loop = 0;
    /* the cache's loop patch covers infinite loops only; finite ones decode on the fly */
    v->pre = NULL;
    if (!v->has_loop || v->loop_count == -1) {
        s32 n;
        const s16 *pre = vd_pcm_lookup(w, &n);
        if (pre && n == v->nsamp) {
            v->pre = pre;
            v->lim = v->has_loop ? v->loop_end : v->nsamp;
        }
    }
}

static void src_start(PVoice *v)
{
    v->pos = 0;
    v->gphase = 0;
    v->smp = 0;
    v->fbuf_frame = -1;
    v->h1 = v->h2 = 0;
    v->looping = v->has_loop;
    v->done = (v->type < 0);
    v->frac = 0;
    v->pcm_len = 0;
    v->pcm_real = 0;
    v->silent = (u8)v->done;
}

/* Deliver n samples of the voice's sample stream (loops applied; zeros after the
   end). Returns how many of them are real samples. */
static s32 src_read(PVoice *v, s16 *dst, s32 n)
{
    s32 real = 0;

    while (n > 0) {
        s32 lim, k, i;

        if (v->done) {
            memset(dst, 0, (size_t)n * 2);
            return real;
        }
        lim = v->looping ? v->loop_end : v->nsamp;
        if (v->smp >= lim) {
            if (v->looping) {
                if (v->loop_count != -1 && --v->loop_count == 0)
                    v->looping = 0;
                v->smp = v->loop_start;
                if (v->type == AL_ADPCM_WAVE) {
                    /* the loop state is the decoded frame holding the loop start */
                    memcpy(v->fbuf, v->loop_state, sizeof(v->fbuf));
                    v->fbuf_frame = v->loop_start >> 4;
                    v->h2 = v->fbuf[14];
                    v->h1 = v->fbuf[15];
                }
                continue;
            }
            v->done = 1;
            continue;
        }
        k = lim - v->smp;
        if (v->type == AL_ADPCM_WAVE) {
            s32 f = v->smp >> 4;
            const s16 *src;
            if (f != v->fbuf_frame) {
                vd_adpcm_frame(v->data + 9 * f, v->book, v->npred, v->fbuf, &v->h2, &v->h1);
                v->fbuf_frame = f;
            }
            i = v->smp & 15;
            if (k > 16 - i)
                k = 16 - i;
            if (k > n)
                k = n;
            src = v->fbuf + i;
            for (i = 0; i < k; i++)
                dst[i] = src[i];
        } else {
            const u8 *s = v->data + 2 * v->smp;
            if (k > n)
                k = n;
            for (i = 0; i < k; i++)
                dst[i] = (s16)((s[2 * i] << 8) | s[2 * i + 1]);
        }
        dst += k;
        n -= k;
        real += k;
        v->smp += k;
    }
    return real;
}

/* ------------------------------------------------------- envelope mixer */

static void env_targets(PVoice *v)
{
    v->mono = (v->pan == 64);
    v->tgtL = (f32)((v->volume * eqpower[v->pan]) >> 15);
    v->tgtR = (f32)((v->volume * eqpower[127 - v->pan]) >> 15);
}

/* start a ramp from the current gains to the targets over `samples` */
static void env_ramp(PVoice *v, s32 samples)
{
    if (samples <= 0) {
        v->curL = v->tgtL;
        v->curR = v->tgtR;
        v->segLeft = 0;
        return;
    }
    {
        f32 inv = (f32)VD_GROUP / (f32)samples;
        f32 cl = v->curL < 1.0f ? 1.0f : v->curL;
        f32 cr = v->curR < 1.0f ? 1.0f : v->curR;
        f32 tl = v->tgtL < 1.0f ? 1.0f : v->tgtL;
        f32 tr = v->tgtR < 1.0f ? 1.0f : v->tgtR;
        v->curL = cl;
        v->curR = cr;
        v->rateL = (tl == cl) ? 1.0f : powf(tl / cl, inv);
        v->rateR = (tr == cr) ? 1.0f : powf(tr / cr, inv);
        v->segLeft = samples;
    }
}

static void env_step8(PVoice *v)
{
    if (v->segLeft > 0) {
        v->segLeft -= VD_GROUP;
        if (v->segLeft <= 0) {
            v->curL = v->tgtL;
            v->curR = v->tgtR;
            v->segLeft = 0;
        } else {
            v->curL *= v->rateL;
            v->curR *= v->rateR;
        }
    }
}

static void set_pitch(PVoice *v, f32 pitch)
{
    if (!(pitch > 0.0f))
        pitch = 0.0f;
    if (pitch > VD_MAX_RATIO)
        pitch = VD_MAX_RATIO;
    v->step = (u32)(pitch * 65536.0f + 0.5f);
}

static void apply_param(PVoice *v, VdParam *p)
{
    v->gphase = 0;                     /* (updates are 16-sample aligned anyway) */
    switch (p->type) {
    case P_START:
        src_set_wave(v, p->wave);
        src_start(v);
        v->playing = 1;
        v->curL = v->curR = 0.0f;
        v->segLeft = 0;
        break;
    case P_START_ALT:
        src_set_wave(v, p->wave);
        src_start(v);
        v->playing = 1;
        v->volume = ((s32)p->vol * p->vol) >> 15;
        v->pan = p->pan > 127 ? 127 : p->pan;
        v->fxmix = p->fxmix > 127 ? 127 : p->fxmix;
        v->dry = eqpower[v->fxmix];
        v->wet = eqpower[127 - v->fxmix];
        set_pitch(v, p->pitch);
        env_targets(v);
        if (p->samples) {
            v->curL = v->curR = 1.0f;
            env_ramp(v, p->samples);
        } else {
            env_ramp(v, 0);
        }
        break;
    case P_STOP:
        v->playing = 0;
        break;
    case P_FREE:
        pv_free(p->pv);
        break;
    case P_VOLUME:
        v->volume = ((s32)p->vol * p->vol) >> 15;
        env_targets(v);
        env_ramp(v, p->samples);
        break;
    case P_PAN:
        v->pan = p->pan > 127 ? 127 : p->pan;
        env_targets(v);
        env_ramp(v, v->segLeft);         /* keep ramping (to the new targets) */
        break;
    case P_PITCH:
        set_pitch(v, p->pitch);
        break;
    case P_FXAMT:
        v->fxmix = p->fxmix > 127 ? 127 : p->fxmix;
        v->dry = eqpower[v->fxmix];
        v->wet = eqpower[127 - v->fxmix];
        break;
    }
}

/* ------------------------------------------------------------- mixing */

/* gains for the next 8-sample group (Q15) */
static inline void group_gains(PVoice *v)
{
    v->gl = ((s32)v->curL * v->dry) >> 15;
    v->gr = ((s32)v->curR * v->dry) >> 15;
    v->gw = ((((s32)v->curL + (s32)v->curR) * v->wet) >> 15) * 23170 >> 15;   /* x 0.7071 (aux L+R) */
    v->gm = (v->gl + v->gr) >> 1;
}

/* The inner loop: cnt output samples from p (p[0] = current integer position,
   f = 16-bit fraction + whole samples already advanced) at constant gains. */
static inline __attribute__((always_inline)) u32 kern(const s16 *p, s32 *acc, s32 *wet, s32 cnt, u32 f,
                                                     u32 step, s32 gl, s32 gr, s32 gw, const int unity,
                                                     const int fx)
{
    s32 *end = acc + 2 * cnt;

    if (cnt <= 0)
        return f;
    if (unity) {
        const s16 *q = p + (f >> 16);
        s32 k;
        for (k = 0; k < cnt; k++) {
            s32 smp = q[k];
            acc[2 * k] += (smp * gl) >> 15;
            acc[2 * k + 1] += (smp * gr) >> 15;
            if (fx)
                wet[k] += (smp * gw) >> 15;
        }
        return f + ((u32)cnt << 16);
    }
    do {
        u32 i = f >> 16;
        s32 smp;
        if (unity) {
            smp = p[i];
        } else {
            s32 a = p[i];
            smp = a + (((p[i + 1] - a) * (s32)((f & 0xFFFF) >> 1)) >> 15);
        }
        acc[0] += (smp * gl) >> 15;
        acc[1] += (smp * gr) >> 15;
        if (fx)
            *wet++ += (smp * gw) >> 15;
        acc += 2;
        f += step;
    } while (acc != end);
    return f;
}

/* centre pan: one accumulator (L and R gains differ by 1.2 % at pan 64; their
   mean is used for both, -0.0/+0.1 dB) */
static inline __attribute__((always_inline)) u32 kern_mono(const s16 *p, s32 *acc, s32 *wet, s32 cnt, u32 f,
                                                          u32 step, s32 gm, s32 gw, const int unity,
                                                          const int fx)
{
    const s16 *p1 = p + 1;
    s32 k;

    if (cnt <= 0)
        return f;
    if (unity) {                       /* pitch 1: no interpolation, plain indexing */
        const s16 *q = p + (f >> 16);
        for (k = 0; k < cnt; k++) {
            s32 smp = q[k];
            acc[k] += (smp * gm) >> 15;
            if (fx)
                wet[k] += (smp * gw) >> 15;
        }
        return f + ((u32)cnt << 16);
    }
    for (k = 0; k < cnt; k++) {
        u32 i = f >> 16;
        s32 smp, x;
        if (unity) {
            smp = p[i];
        } else {
            s32 a = p[i];
            smp = a + (((p1[i] - a) * (s32)((u16)f >> 1)) >> 15);
        }
        x = acc[k];
        f += step;
        acc[k] = x + ((smp * gm) >> 15);
        if (fx)
            wet[k] += (smp * gw) >> 15;
    }
    return f;
}

/* n samples from p in 8-sample gain groups (the group position is kept in the
   voice, so a run may start or stop inside a group). Returns the new fraction. */
static inline __attribute__((always_inline)) u32 run_groups(PVoice *v, const s16 *p, s32 *acc, s32 *wet,
                                                           s32 n, u32 f, const int unity, const int fx)
{
    u32 step = v->step;

    while (n > 0) {
        s32 cnt;
        if (v->segLeft == 0) {
            /* no ramp: constant gains for the whole run */
            group_gains(v);
            cnt = n;
        } else {
            cnt = VD_GROUP - v->gphase;
            if (cnt > n)
                cnt = n;
            if (v->gphase == 0)
                group_gains(v);
        }
        if (!v->mono)
            stereo_used = 1;
        if (v->mono)
            f = kern_mono(p, v->mono_base + ((acc - mix_acc) >> 1), wet, cnt, f, step, v->gm, v->gw, unity, fx);
        else
            f = kern(p, acc, wet, cnt, f, step, v->gl, v->gr, v->gw, unity, fx);
        acc += 2 * cnt;
        wet += cnt;
        n -= cnt;
        v->gphase += cnt;
        while (v->gphase >= VD_GROUP) {
            v->gphase -= VD_GROUP;
            env_step8(v);
        }
    }
    return f;
}

static u32 run_any(PVoice *v, const s16 *p, s32 *acc, s32 *wet, s32 n, u32 f)
{
    if (v->step == 0x10000) {
        if (v->wet)
            return run_groups(v, p, acc, wet, n, f, 1, 1);
        return run_groups(v, p, acc, wet, n, f, 1, 0);
    }
    if (v->wet)
        return run_groups(v, p, acc, wet, n, f, 0, 1);
    return run_groups(v, p, acc, wet, n, f, 0, 0);
}

/* advance the envelope over n samples without output */
static void env_skip(PVoice *v, s32 n)
{
    v->gphase += n;
    while (v->gphase >= VD_GROUP) {
        v->gphase -= VD_GROUP;
        env_step8(v);
    }
}

static inline int inaudible(const PVoice *v)
{
    return v->curL < 1.0f && v->curR < 1.0f && v->tgtL < 1.0f && v->tgtR < 1.0f;
}

/* pre-decoded samples: mix straight from the PCM (no copy, no decode) */
static void render_pre(PVoice *v, s32 *acc, s32 *wet, s32 n)
{
    while (n > 0) {
        s32 m = n > VD_BLOCK ? VD_BLOCK : n;
        s32 room = v->lim - 1 - v->pos;       /* whole samples before p[i + 1] passes lim */
        u32 last = (v->frac + (u32)(m - 1) * v->step) >> 16;

        if ((s32)last > room) {               /* the loop end / wave end falls inside: split */
            if (room < 0 || v->step == 0) {
                m = 1;
            } else {
                u32 k = ((((u32)room << 16) | 0xFFFF) - v->frac) / v->step + 1;
                m = (s32)k < m ? (s32)k : m;
                if (m < 1)
                    m = 1;
            }
        }
        if (inaudible(v)) {
            env_skip(v, m);
            v->frac += (u32)m * v->step;
        } else {
            v->frac = run_any(v, v->pre + v->pos, acc, wet, m, v->frac);
        }
        v->pos += (s32)(v->frac >> 16);
        v->frac &= 0xFFFF;
        while (v->pos >= v->lim) {
            if (!v->has_loop) {
                v->silent = 1;
                break;
            }
            v->pos -= v->loop_end - v->loop_start;
        }
        n -= m;
        acc += 2 * m;
        wet += m;
        if (v->silent) {
            env_skip(v, n);
            return;
        }
    }
}

/* Render n output samples of voice v into acc/wet (sample offset 0 = acc[0]). */
static void voice_render(PVoice *v, s32 *acc, s32 *wet, s32 n)
{
    if (v->silent) {                          /* the sample is over: only the envelope moves on */
        env_skip(v, n);
        return;
    }
    if (v->pre) {
        render_pre(v, acc, wet, n);
        return;
    }
    while (n > 0) {
        s32 m = n > VD_BLOCK ? VD_BLOCK : n;
        u32 adv, total;
        s32 need, k, rem;

        if (v->silent) {
            env_skip(v, n);
            return;
        }
        total = v->frac + (u32)m * v->step;
        need = (s32)((v->frac + (u32)(m - 1) * v->step) >> 16) + 2;
        if (v->pcm_len < need) {
            s32 got = src_read(v, v->pcm + v->pcm_len, need - v->pcm_len);
            if (v->pcm_real == v->pcm_len)
                v->pcm_real += got;
            v->pcm_len = need;
        }
        if (inaudible(v))
            env_skip(v, m);
        else
            (void)run_any(v, v->pcm, acc, wet, m, v->frac);
        adv = total >> 16;
        v->frac = total & 0xFFFF;
        rem = v->pcm_len - (s32)adv;
        if (rem < 0)
            rem = 0;
        for (k = 0; k < rem; k++)
            v->pcm[k] = v->pcm[adv + k];
        v->pcm_len = rem;
        v->pcm_real -= (s32)adv;
        if (v->pcm_real < 0)
            v->pcm_real = 0;
        if (v->done && v->pcm_real == 0)
            v->silent = 1;
        n -= m;
        acc += 2 * m;
        wet += m;
    }
}

/* Process one voice for the frame [curSamples, curSamples + outLen). */
static s32 voice_frame(PVoice *v, s32 outLen)
{
    s32 pos = 0, active = 0;

    while (pos < outLen) {
        VdParam *p = v->ctrlList;
        s32 end = outLen;

        if (p) {
            s32 t = p->delta - syn.curSamples;
            if (t <= pos) {
                v->ctrlList = p->next;
                if (v->ctrlList == NULL)
                    v->ctrlTail = NULL;
                apply_param(v, p);
                param_free(p);
                continue;
            }
            if (t < end)
                end = t;
        }
        if (v->playing) {
            v->mono_base = mix_mono;
            voice_render(v, &mix_acc[2 * pos], &mix_wet[pos], end - pos);
            active = 1;
        }
        pos = end;
    }
    return active;
}

/* ------------------------------------------------------------- reverb */

static void fx_setup(const s32 *prm)
{
    s32 i, j = 2;

    syn.fx_on = 0;
    if (prm == NULL)
        return;
    syn.fx_sections = prm[0];
    syn.fx_len = prm[1] / 2;
    if (syn.fx_sections < 1 || syn.fx_sections > 8 || syn.fx_len < 16 ||
        syn.fx_len > (s32)(sizeof(fx_line) / sizeof(fx_line[0])))
        return;
    for (i = 0; i < syn.fx_sections; i++) {
        VdDelay *d = &syn.fx[i];
        d->input = prm[j++] / 2;               /* the effect runs at half the output rate */
        d->output = prm[j++] / 2;
        d->fbcoef = (s16)prm[j++];
        d->ffcoef = (s16)prm[j++];
        d->gain = (s16)prm[j++];
        j += 2;                          /* chorus rate / depth: not implemented */
        d->lp = prm[j] != 0;
        /* init_lpfilter: pole a = fc/2 (Q14), gain 0x4000 - a; at half rate the pole is a^2
           (same cutoff) */
        d->lp_fc2 = ((prm[j] >> 1) * (prm[j] >> 1)) >> 14;
        d->lp_gain = 0x4000 - d->lp_fc2;
        d->lp_y = 0;
        j++;
        if (d->input < 0 || d->input >= syn.fx_len || d->output < 0 || d->output >= syn.fx_len)
            return;
    }
    memset(fx_line, 0, sizeof(fx_line));
    syn.fx_head = 0;
    syn.fx_tail = 0;
    syn.fx_on = 1;
}

static inline s32 sat16(s32 x)
{
    if ((u32)(x + 32768) > 65535u)
        x = (x >> 31) ^ 0x7FFF;
    return x;
}

/* one section over a run where neither tap wraps (flags constant: specialised) */
static inline __attribute__((always_inline)) void fx_sec(const VdDelay *d, s16 *in, s16 *outp, s32 *out,
                                                       s32 n, s32 *lpy, const int FF, const int FB,
                                                       const int LP, const int GAIN)
{
    const s32 ff = d->ffcoef, fb = d->fbcoef, gain = d->gain;
    const s32 lg = d->lp_gain, lc = d->lp_fc2;
    s32 y = *lpy, k;

    for (k = 0; k < n; k++) {
        s32 b1 = in[k], b2 = outp[k];
        if (FF)
            b2 = sat16(b2 + ((b1 * ff) >> 15));
        if (FB) {
            b1 = sat16(b1 + ((b2 * fb) >> 15));
            in[k] = (s16)b1;
        }
        if (LP) {
            y = sat16((lg * b2 + lc * y) >> 14);
            b2 = y;
        }
        outp[k] = (s16)b2;
        if (GAIN)
            out[k] = sat16(out[k] + ((b2 * gain) >> 15));
    }
    *lpy = y;
}

static void fx_section_run(const VdDelay *d, s16 *in, s16 *outp, s32 *out, s32 n, s32 *lpy)
{
    s32 key = (d->ffcoef != 0) | (d->fbcoef != 0) << 1 | (d->lp != 0) << 2 | (d->gain != 0) << 3;

    switch (key) {   /* the small-room sections: 3, 3 | 8, 2 | 4 */
    case 3:  fx_sec(d, in, outp, out, n, lpy, 1, 1, 0, 0); break;
    case 11: fx_sec(d, in, outp, out, n, lpy, 1, 1, 0, 1); break;
    case 6:  fx_sec(d, in, outp, out, n, lpy, 0, 1, 1, 0); break;
    default: fx_sec(d, in, outp, out, n, lpy, d->ffcoef != 0, d->fbcoef != 0, d->lp != 0, d->gain != 0); break;
    }
}

/* in: wet bus (summed voice sends); adds the reverb output to the centre bus.
   libultra's network, processed like alFxPull (chunks, all samples of a section,
   then the next section), but at half the output rate: the input is averaged
   in pairs, the output linearly interpolated back (the port's only change, it
   halves the cost; the reverb output is low-passed and diffuse anyway). */
static s32 fx_last;                    /* last half-rate output (for the interpolation) */

static void fx_process(s32 *acc, s32 *mono, const s32 *wet, s32 n)
{
    s32 L = syn.fx_len, nsec = syn.fx_sections, done = 0, i, k;
    s32 out[80];

    while (done < n) {
        s32 m2 = n - done > 160 ? 160 : n - done;
        s32 m = m2 >> 1;                      /* frames are multiples of 16 */
        s32 head = syn.fx_head;
        for (k = 0; k < m; k++) {
            s32 h = head + k;
            if (h >= L)
                h -= L;
            fx_line[h] = (s16)sat16((wet[done + 2 * k] + wet[done + 2 * k + 1]) >> 1);
            out[k] = 0;
        }
        for (i = 0; i < nsec; i++) {
            VdDelay *d = &syn.fx[i];
            s32 ip = head - d->input, op = head - d->output, pos = 0;
            if (ip < 0)
                ip += L;
            if (op < 0)
                op += L;
            while (pos < m) {
                s32 run = m - pos;
                if (run > L - ip)
                    run = L - ip;
                if (run > L - op)
                    run = L - op;
                fx_section_run(d, &fx_line[ip], &fx_line[op], &out[pos], run, &d->lp_y);
                pos += run;
                ip += run;
                op += run;
                if (ip >= L)
                    ip -= L;
                if (op >= L)
                    op -= L;
            }
        }
        for (k = 0; k < m; k++) {
            mono[done + 2 * k] += (fx_last + out[k]) >> 1;
            mono[done + 2 * k + 1] += out[k];
            fx_last = out[k];
        }
        head += m;
        if (head >= L)
            head -= L;
        syn.fx_head = head;
        done += 2 * m;
        if (m == 0)
            break;
    }
    (void)acc;
}

/* -------------------------------------------------------------- driver */

void vd_synth_reset(void)
{
    irq_mask_t s = irq_disable();

    memset(&syn, 0, sizeof(syn));
    memset(pvoices, 0, sizeof(pvoices));
    alGlobals = NULL;
    irq_restore(s);
}

static void synth_new(const s32 *cfg)
{
    /* ALSynConfig (src/game/audio.c SynConfig): maxVVoices, maxPVoices, maxUpdates,
       unk, dmaproc, heap, outputRate, fxType (u8 at 0x1C), params (0x20) */
    s32 maxPVoices = cfg[1];
    s32 maxUpdates = cfg[2];
    void *(*dmaNew)(void **) = (void *(*)(void **))(uintptr_t)cfg[4];
    u8 fxType = *((const u8 *)cfg + 0x1C);
    const s32 *fxParams = (const s32 *)(uintptr_t)cfg[8];
    s32 i;

    memset(&syn, 0, sizeof(syn));
    memset(pvoices, 0, sizeof(pvoices));
    if (eqpower[0] == 0)
        eqpower_init();
    syn.outputRate = cfg[6] > 0 ? cfg[6] : 44100;
    syn.maxPVoices = maxPVoices > VD_MAX_PVOICES ? VD_MAX_PVOICES : (maxPVoices < 1 ? 1 : maxPVoices);
    if (maxUpdates < 64)
        maxUpdates = 64;
    if (maxUpdates > VD_MAX_PARAMS)
        maxUpdates = VD_MAX_PARAMS;
    /* the game asks for 256 updates; a few more cost nothing and avoid drops */
    maxUpdates = VD_MAX_PARAMS;
    for (i = 0; i < maxUpdates; i++) {
        params[i].next = syn.paramFree;
        syn.paramFree = &params[i];
    }
    for (i = 0; i < syn.maxPVoices; i++) {
        pvoices[i].type = -1;
        alLink(&pvoices[i], &syn.pFreeList);
    }
    if (dmaNew) {                        /* as alSynNew: lets the audio manager set up its DMA state */
        void *state;
        (void)dmaNew(&state);
    }
    if (fxType == 1)
        fx_setup(SMALLROOM_PARAMS);
    else if (fxType == 6)
        fx_setup(fxParams);
    syn.inited = 1;
    {
        extern void *sbrk(intptr_t);
        vd_audio_stats.heap_top = (u32)(uintptr_t)sbrk(0);
    }
    VD_LOG(VD_DBG_AUDIO, "synth: %d voices, %d Hz, fx %d (%s)\n", (int)syn.maxPVoices,
           (int)syn.outputRate, fxType, syn.fx_on ? "on" : "off");
}

void alInit(void *g, void *config)
{
    if (alGlobals == NULL) {
        alGlobals = g;
        synth_new(config);
    }
}

void alClose(void *g)
{
    (void)g;
    if (alGlobals) {
        vd_synth_reset();
    }
}

static ALPlayer *next_client(s32 *t)
{
    ALPlayer *cl, *best = NULL;
    s32 delta = 0x7FFFFFFF;

    for (cl = syn.head; cl != NULL; cl = cl->next) {
        if (cl->samplesLeft - syn.curSamples < delta) {
            best = cl;
            delta = cl->samplesLeft - syn.curSamples;
        }
    }
    *t = best ? best->samplesLeft : 0;
    return best;
}

void *alAudioFrame(void *cmdList, s32 *cmdLen, s16 *outBuf, s32 outLen)
{
    ALPlayer *client;
    ALLink *dl;
    s32 i, voices = 0, wet_used = 0;
    u32 peak = vd_audio_stats.peak, clipped = 0;
    uint64_t t0;

    if (!syn.inited || syn.head == NULL || outBuf == NULL) {
        *cmdLen = 0;
        return cmdList;
    }
    t0 = timer_us_gettime64();
    if (outLen > VD_MAX_FRAME)
        outLen = VD_MAX_FRAME;
    if (outLen < 0)
        outLen = 0;

    /* 1. the players: every callback due in this frame, at its (aligned) time */
    for (client = next_client(&syn.paramSamples); client != NULL && syn.paramSamples - syn.curSamples < outLen;
         client = next_client(&syn.paramSamples)) {
        syn.paramSamples &= ~0xF;
        client->samplesLeft += time_to_samples_noround((*client->handler)(client));
    }
    syn.paramSamples &= ~0xF;

    /* 2. render every physical voice (all of them: stopped or free voices may still
          have pending updates) */
    memset(mix_acc, 0, (size_t)outLen * 2 * sizeof(s32));   /* (cheap; kept simple) */
    memset(mix_mono, 0, (size_t)outLen * sizeof(s32));
    stereo_used = 0;
    if (syn.fx_on)
        memset(mix_wet, 0, (size_t)outLen * sizeof(s32));
    for (i = 0; i < syn.maxPVoices; i++) {
        PVoice *v = &pvoices[i];
        if (v->ctrlList == NULL && !v->playing)
            continue;
        if (!syn.fx_on)
            v->wet = 0;
        if (voice_frame(v, outLen)) {
            voices++;
            if (v->wet)
                wet_used = 1;
        }
    }

    /* 3. effect bus (runs on for a while after the last send: the reverb tail) */
    if (syn.fx_on) {
        if (wet_used)
            syn.fx_tail = 30;
        if (syn.fx_tail > 0) {
            fx_process(mix_acc, mix_mono, mix_wet, outLen);
            vd_audio_stats.fx_frames++;
            if (--syn.fx_tail == 0) {
                memset(fx_line, 0, sizeof(fx_line));
                for (i = 0; i < syn.fx_sections; i++)
                    syn.fx[i].lp_y = 0;
                fx_last = 0;
            }
        }
    }

    /* 4. output (interleaved L/R, 16 bit); the peak is sampled every 16th frame */
    {
        const int want_peak = (vd_audio_stats.frames & 15) == 0;
        for (i = 0; i < outLen; i++) {
            s32 m = mix_mono[i], l = m, r = m;
            if (stereo_used) {
                l += mix_acc[2 * i];
                r += mix_acc[2 * i + 1];
            }
            if ((u32)(l + 32768) > 65535u) {
                l = (l >> 31) ^ 0x7FFF;
                clipped++;
            }
            if ((u32)(r + 32768) > 65535u) {
                r = (r >> 31) ^ 0x7FFF;
                clipped++;
            }
            outBuf[2 * i] = (s16)l;
            outBuf[2 * i + 1] = (s16)r;
            if (want_peak) {
                u32 a = (u32)(l < 0 ? -l : l);
                if (a > peak)
                    peak = a;
            }
        }
    }
    syn.curSamples += outLen;

    /* collect the physical voices freed this frame */
    while ((dl = syn.pLameList.next) != NULL) {
        alUnlink(dl);
        alLink(dl, &syn.pFreeList);
    }

    /* one dummy command: the audio task still goes through the scheduler */
    ((u32 *)cmdList)[0] = 0;
    ((u32 *)cmdList)[1] = 0;
    *cmdLen = 1;

    {
        u32 us = (u32)(timer_us_gettime64() - t0);
        vd_audio_stats.frames++;
        vd_audio_stats.samples += (u32)outLen;
        vd_audio_stats.mix_us += us;
        if (us > vd_audio_stats.mix_us_max)
            vd_audio_stats.mix_us_max = us;
        vd_audio_stats.voices_mixed += (u32)voices;
        if ((u32)voices > vd_audio_stats.voices_max)
            vd_audio_stats.voices_max = (u32)voices;
        vd_audio_stats.clipped += clipped;
        vd_audio_stats.peak = (vd_audio_stats.frames & 63) ? peak : 0;   /* max of the last 64 frames (sampled) */
    }
    return (u8 *)cmdList + 8;
}

VD_ALIAS(func_8001D770, alInit);
VD_ALIAS(func_8001D79C, alClose);
VD_ALIAS(func_8002041C, alAudioFrame);
