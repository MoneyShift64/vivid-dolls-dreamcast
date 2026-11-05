/* Vivid Dolls port - VADPCM decoder and the decoded-sample cache.
 *
 * libultra's ADPCM ("VADPCM"): 9-byte frames of 16 samples; byte 0 = scale
 * (high nibble) and predictor index (low nibble), then 16 signed 4-bit
 * residuals. Each group of 8 samples is predicted from the last two outputs
 * and the earlier residuals of the group with the order-2 codebook entry:
 *   out[i] = clamp((res[i] << 11 + c1[i]*out[-2] + c2[i]*out[-1]
 *                   + sum_{j<i} c2[j]*res[i-1-j]) >> 11),   res = nibble << scale.
 * Checked offline against the banks: decoding linearly from the start gives
 * exactly the samples stored in every loop's state (the decoded frame holding
 * the loop start), so a linear decode is also exact across loops.
 *
 * Cache: when a bank file is loaded (alBnkfNew), every distinct wavetable is
 * decoded once to 16-bit PCM in RAM (both banks: ~2.1 MB), so the mixer reads
 * samples directly and spends no time decoding. Entries are keyed by the ROM
 * address of the sample data and survive soft resets (the data never
 * changes). For infinitely looping waves the sample at loop_end is replaced by
 * the one at loop_start (never played otherwise), and one zero sample follows
 * the end of every wave, so the mixer's linear interpolation can always read
 * pcm[i + 1]. If the memory cannot be allocated, the mixer decodes on the fly.
 * The wavetable's spare pad bytes hold the cache index + 1 (the game never
 * reads wavetables).
 */
#include <stdlib.h>
#include <string.h>
#include "al_internal.h"
#include "vd_debug.h"

#ifndef VD_AUDIO_PREDECODE
#define VD_AUDIO_PREDECODE 1
#endif

#define VD_PCM_MAX 128

typedef struct {
    u32 rom;                 /* ROM address of the sample data */
    u32 len;                 /* bytes */
    s16 *pcm;                /* nsamp + 1 samples */
    s32 nsamp;
} VdPcmEntry;

static VdPcmEntry pcm_cache[VD_PCM_MAX];
static s32 pcm_count;

static inline s32 clamp16(s32 x)
{
    if (x > 32767)
        return 32767;
    if (x < -32768)
        return -32768;
    return x;
}

/* One frame, 9 bytes -> 16 samples; *h2 / *h1 = the two previous outputs. */
void vd_adpcm_frame(const u8 *in, const s16 *book, s32 npred, s16 *out, s32 *h2p, s32 *h1p)
{
    u32 hdr = in[0];
    s32 shift = (s32)(hdr >> 4);
    s32 pred = (s32)(hdr & 15);
    const s16 *c1, *c2;
    s32 g, h2 = *h2p, h1 = *h1p;

    if (shift > 12)
        shift = 12;
    if (pred >= npred)
        pred = npred - 1;
    c1 = book + pred * 16;
    c2 = c1 + 8;
    for (g = 0; g < 2; g++) {
        const u8 *p = in + 1 + g * 4;
        s32 x0, x1, x2, x3, x4, x5, x6, x7, acc;
        s32 b;

        b = p[0]; x0 = ((s32)(s8)(u8)(b & 0xF0) >> 4) << shift; x1 = ((s32)(s8)(u8)(b << 4) >> 4) << shift;
        b = p[1]; x2 = ((s32)(s8)(u8)(b & 0xF0) >> 4) << shift; x3 = ((s32)(s8)(u8)(b << 4) >> 4) << shift;
        b = p[2]; x4 = ((s32)(s8)(u8)(b & 0xF0) >> 4) << shift; x5 = ((s32)(s8)(u8)(b << 4) >> 4) << shift;
        b = p[3]; x6 = ((s32)(s8)(u8)(b & 0xF0) >> 4) << shift; x7 = ((s32)(s8)(u8)(b << 4) >> 4) << shift;

        acc = (x0 << 11) + c1[0] * h2 + c2[0] * h1;
        out[0] = (s16)clamp16(acc >> 11);
        acc = (x1 << 11) + c1[1] * h2 + c2[1] * h1 + c2[0] * x0;
        out[1] = (s16)clamp16(acc >> 11);
        acc = (x2 << 11) + c1[2] * h2 + c2[2] * h1 + c2[0] * x1 + c2[1] * x0;
        out[2] = (s16)clamp16(acc >> 11);
        acc = (x3 << 11) + c1[3] * h2 + c2[3] * h1 + c2[0] * x2 + c2[1] * x1 + c2[2] * x0;
        out[3] = (s16)clamp16(acc >> 11);
        acc = (x4 << 11) + c1[4] * h2 + c2[4] * h1 + c2[0] * x3 + c2[1] * x2 + c2[2] * x1 + c2[3] * x0;
        out[4] = (s16)clamp16(acc >> 11);
        acc = (x5 << 11) + c1[5] * h2 + c2[5] * h1 + c2[0] * x4 + c2[1] * x3 + c2[2] * x2 + c2[3] * x1 +
              c2[4] * x0;
        out[5] = (s16)clamp16(acc >> 11);
        acc = (x6 << 11) + c1[6] * h2 + c2[6] * h1 + c2[0] * x5 + c2[1] * x4 + c2[2] * x3 + c2[3] * x2 +
              c2[4] * x1 + c2[5] * x0;
        out[6] = (s16)clamp16(acc >> 11);
        acc = (x7 << 11) + c1[7] * h2 + c2[7] * h1 + c2[0] * x6 + c2[1] * x5 + c2[2] * x4 + c2[3] * x3 +
              c2[4] * x2 + c2[5] * x1 + c2[6] * x0;
        out[7] = (s16)clamp16(acc >> 11);
        h2 = out[6];
        h1 = out[7];
        out += 8;
    }
    *h2p = h2;
    *h1p = h1;
}

static s32 wave_ok(const ALWaveTable *w, u32 *rom, u32 *len)
{
    if (w == NULL || vd_rom_image == NULL)
        return 0;
    *rom = (u32)(uintptr_t)w->base;
    *len = (u32)w->len;
    if (*rom >= vd_rom_size || *len > vd_rom_size - *rom || *len < 9)
        return 0;
    if (w->type == AL_ADPCM_WAVE) {
        const ALADPCMBook *b = w->waveInfo.adpcm.book;
        return b != NULL && b->order == 2 && b->npredictors >= 1;
    }
    return w->type == AL_RAW16_WAVE;
}

static s32 wave_nsamp(const ALWaveTable *w, u32 len)
{
    return w->type == AL_ADPCM_WAVE ? (s32)(len / 9) * 16 : (s32)(len / 2);
}

static void decode_wave(const ALWaveTable *w, const u8 *data, s16 *out, s32 nsamp)
{
    s32 i;

    if (w->type == AL_ADPCM_WAVE) {
        const ALADPCMBook *b = w->waveInfo.adpcm.book;
        s32 h2 = 0, h1 = 0, f, nf = nsamp / 16;
        for (f = 0; f < nf; f++)
            vd_adpcm_frame(data + 9 * f, b->book, b->npredictors, out + 16 * f, &h2, &h1);
    } else {
        for (i = 0; i < nsamp; i++)
            out[i] = (s16)((data[2 * i] << 8) | data[2 * i + 1]);
    }
    out[nsamp] = 0;
}

/* infinite loop: the sample after the loop's last one is the loop start */
static void patch_loop(const ALWaveTable *w, s16 *pcm, s32 nsamp)
{
    u32 start, end;
    s32 count;

    if (w->type == AL_ADPCM_WAVE && w->waveInfo.adpcm.loop) {
        start = w->waveInfo.adpcm.loop->start;
        end = w->waveInfo.adpcm.loop->end;
        count = (s32)w->waveInfo.adpcm.loop->count;
    } else if (w->type == AL_RAW16_WAVE && w->waveInfo.raw.loop) {
        start = w->waveInfo.raw.loop->start;
        end = w->waveInfo.raw.loop->end;
        count = (s32)w->waveInfo.raw.loop->count;
    } else {
        return;
    }
    if (count == -1 && end > start && (s32)end <= nsamp)
        pcm[end] = pcm[start];
}

static s32 cache_find(u32 rom, u32 len)
{
    s32 i;

    for (i = 0; i < pcm_count; i++)
        if (pcm_cache[i].rom == rom && pcm_cache[i].len == len)
            return i;
    return -1;
}

static void tag(ALWaveTable *w, s32 idx)
{
    u16 t = (u16)(idx + 1);

    memcpy(w->pad, &t, 2);
}

typedef void (*wave_fn)(ALWaveTable *w, u32 rom, u32 len);

static void for_each_wave(ALBankFile *f, wave_fn fn)
{
    s32 b, i, k;

    for (b = 0; b < f->bankCount; b++) {
        ALBank *bank = f->bankArray[b];
        if (bank == NULL)
            continue;
        for (i = -1; i < bank->instCount; i++) {
            ALInstrument *inst = i < 0 ? bank->percussion : bank->instArray[i];
            if (inst == NULL)
                continue;
            for (k = 0; k < inst->soundCount; k++) {
                ALSound *snd = inst->soundArray[k];
                ALWaveTable *w;
                u32 rom, len;
                if (snd == NULL || (w = snd->wavetable) == NULL || !wave_ok(w, &rom, &len))
                    continue;
                fn(w, rom, len);
            }
        }
    }
}

static ALWaveTable *new_wave[VD_PCM_MAX];      /* wavetable to decode for each new entry */
static u32 new_total;

static void collect_wave(ALWaveTable *w, u32 rom, u32 len)
{
    s32 n;

    if (cache_find(rom, len) >= 0 || pcm_count >= VD_PCM_MAX)
        return;
    n = wave_nsamp(w, len);
    new_wave[pcm_count] = w;
    pcm_cache[pcm_count].rom = rom;
    pcm_cache[pcm_count].len = len;
    pcm_cache[pcm_count].pcm = NULL;
    pcm_cache[pcm_count].nsamp = n;
    pcm_count++;
    new_total += ((u32)(n + 1) * 2 + 31) & ~31u;
}

static void tag_wave(ALWaveTable *w, u32 rom, u32 len)
{
    tag(w, cache_find(rom, len));
}

/* Decode every not yet cached wavetable of a bank file (called by alBnkfNew
   after the relocation / byte-order conversion) and tag the wavetables. */
void vd_pcm_cache_bankfile(void *fv)
{
#if VD_AUDIO_PREDECODE
    ALBankFile *f = fv;
    s32 first = pcm_count, i;
    u8 *mem;

    if (f == NULL || f->revision != AL_BANK_VERSION)
        return;
    new_total = 0;
    for_each_wave(f, collect_wave);
    if (pcm_count > first) {
        mem = malloc(new_total);
        if (mem == NULL) {
            dbglog(DBG_WARNING, "Vivid Dolls: no memory for decoded samples (%u KB), decoding on the fly\n",
                   (unsigned)(new_total / 1024));
            pcm_count = first;
            return;
        }
        for (i = first; i < pcm_count; i++) {
            pcm_cache[i].pcm = (s16 *)mem;
            mem += ((u32)(pcm_cache[i].nsamp + 1) * 2 + 31) & ~31u;
            decode_wave(new_wave[i], vd_rom_image + pcm_cache[i].rom, pcm_cache[i].pcm, pcm_cache[i].nsamp);
            patch_loop(new_wave[i], pcm_cache[i].pcm, pcm_cache[i].nsamp);
        }
        vd_audio_stats.pcm_kb += new_total / 1024;
    }
    for_each_wave(f, tag_wave);
    vd_audio_stats.pcm_waves = (u32)pcm_count;
    VD_LOG(VD_DBG_AUDIO, "pcm cache: %d waves, %u KB\n", (int)pcm_count, (unsigned)vd_audio_stats.pcm_kb);
#else
    (void)fv;
#endif
}

/* The decoded samples of a wavetable, or NULL (then decode on the fly). */
const s16 *vd_pcm_lookup(const ALWaveTable *w, s32 *nsamp)
{
    u16 t;
    u32 rom, len;
    s32 idx;

    if (w == NULL)
        return NULL;
    memcpy(&t, w->pad, 2);
    idx = (s32)t - 1;
    if (idx < 0 || idx >= pcm_count || pcm_cache[idx].pcm == NULL) {
        if (!wave_ok(w, &rom, &len) || (idx = cache_find(rom, len)) < 0 || pcm_cache[idx].pcm == NULL)
            return NULL;
    } else {
        rom = (u32)(uintptr_t)w->base;
        if (pcm_cache[idx].rom != rom)
            return NULL;
    }
    *nsamp = pcm_cache[idx].nsamp;
    return pcm_cache[idx].pcm;
}
