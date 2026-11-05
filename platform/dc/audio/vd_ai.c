/* Vivid Dolls port - the N64 audio interface (osAi*) on the Dreamcast AICA.
 *
 * osAiSetNextBuffer(buf, size) queues the 16-bit stereo PCM the synthesizer
 * rendered (one game frame) into a ring buffer; a KOS thread polls a KOS
 * snd_stream (stereo, 44100 Hz) every few ms, and the stream's callback hands
 * the ring's data to the AICA (KOS de-interleaves and copies it to sound RAM).
 *
 * osAiGetLength() is what keeps production and consumption equal: the game's
 * audio manager sizes each frame as frameSize - samplesLeft + 80 (min 720
 * samples), where samplesLeft is what the AI still has to play. Here it is the
 * ring fill above a target level, so the game renders a bit more when the ring
 * runs low and less when it is full; the AICA's clock is the reference (no
 * drift). At start-up and after an underrun the stream outputs silence until
 * the ring holds the target amount again.
 *
 * Latency: about 1 frame (the audio task) + the ring target (~35 ms) + the
 * AICA buffer (2048 samples, refilled in halves).
 */
#include <kos.h>
#include <dc/sound/stream.h>
#include <string.h>
#include "al_internal.h"
#include "vd_debug.h"

#define AI_RING        8192u                    /* stereo frames, power of two */
#define AI_TARGET      1536u                    /* ring level the feedback aims at */
#define AI_PRIME       1536u                    /* level needed to (re)start output */
#define AI_STREAM_BUF  4096                     /* bytes per channel in sound RAM */
#define AI_FREQ        44100

static u32 ai_ring[AI_RING] __attribute__((aligned(32)));   /* L | R << 16 */
static volatile u32 ai_head, ai_tail;                       /* write / read counters */
static volatile int ai_priming = 1;
static u32 ai_out[AI_STREAM_BUF] __attribute__((aligned(32)));  /* 16 KB: >= 2 x buffer */
static snd_stream_hnd_t ai_hnd = SND_STREAM_INVALID;
static kthread_t *ai_thd;
static int ai_inited;

static void *ai_stream_cb(snd_stream_hnd_t hnd, int req, int *got)
{
    u32 n = (u32)req / 4, have, k, i;
    irq_mask_t s;

    (void)hnd;
    if (n > AI_STREAM_BUF)
        n = AI_STREAM_BUF;
    s = irq_disable();
    have = ai_head - ai_tail;
    if (ai_priming && have >= AI_PRIME)
        ai_priming = 0;
    k = 0;
    if (!ai_priming) {
        k = have < n ? have : n;
        i = ai_tail & (AI_RING - 1);
        if (i + k <= AI_RING) {
            memcpy(ai_out, &ai_ring[i], k * 4);
        } else {
            memcpy(ai_out, &ai_ring[i], (AI_RING - i) * 4);
            memcpy(&ai_out[AI_RING - i], &ai_ring[0], (k - (AI_RING - i)) * 4);
        }
        ai_tail += k;
        if (k < n) {
            ai_priming = 1;                     /* underrun: wait for a full ring again */
            vd_audio_stats.ai_underruns++;
        }
    }
    irq_restore(s);
    if (k < n)
        memset(&ai_out[k], 0, (n - k) * 4);
    vd_audio_stats.ai_pulled += k;
    *got = (int)(n * 4);
    return ai_out;
}

static void *ai_thread(void *arg)
{
    (void)arg;
    for (;;) {
        if (ai_hnd != SND_STREAM_INVALID)
            snd_stream_poll(ai_hnd);
        thd_sleep(4);
    }
    return NULL;
}

void vd_ai_init_once(void)
{
    kthread_attr_t attr;

    if (ai_inited)
        return;
    ai_inited = 1;
    if (snd_stream_init() < 0) {
        dbglog(DBG_ERROR, "Vivid Dolls: snd_stream_init failed, no sound\n");
        return;
    }
    ai_hnd = snd_stream_alloc(ai_stream_cb, AI_STREAM_BUF);
    if (ai_hnd == SND_STREAM_INVALID) {
        dbglog(DBG_ERROR, "Vivid Dolls: snd_stream_alloc failed, no sound\n");
        return;
    }
    snd_stream_volume(ai_hnd, 255);
    snd_stream_start(ai_hnd, AI_FREQ, 1);
    memset(&attr, 0, sizeof(attr));
    attr.create_detached = true;
    attr.stack_size = 8 * 1024;
    attr.prio = 12;                             /* above the game threads; works ~0.1 ms per call */
    attr.label = "vd_ai";
    ai_thd = thd_create_ex(&attr, ai_thread, NULL);
}

void vd_ai_reset(void)
{
    irq_mask_t s = irq_disable();

    ai_head = ai_tail = 0;
    ai_priming = 1;
    irq_restore(s);
}

/* ---------------------------------------------------------------- libultra */

/* Bytes "left in the DAC". Capped like the real AI (never more than one DMA
   buffer): the game computes frameSize - left + 80, which must not go negative. */
#define AI_LEN_MAX 768u

s32 osAiGetLength(void)
{
    u32 fill = ai_head - ai_tail, left;

    left = fill > AI_TARGET ? fill - AI_TARGET : 0;
    if (left > AI_LEN_MAX)
        left = AI_LEN_MAX;
    return (s32)(left * 4);
}

/* libultra: dacRate = round(VI clock / freq); returns VI clock / dacRate
   (44100 -> 44095, the rate the game then gives the synthesizer). The AICA
   always plays 44100 Hz (0.01 % difference). */
s32 osAiSetFrequency(u32 freq)
{
    const u32 clock = 48681812;                 /* NTSC VI clock */
    f32 f = (f32)clock / (f32)freq + 0.5f;
    u32 dacRate = (u32)f;

    vd_ai_init_once();
    if (dacRate < 132)
        return -1;
    return (s32)(clock / dacRate);
}

s32 osAiSetNextBuffer(void *buf, u32 size)
{
    const u32 *src = buf;
    u32 n = size / 4, i, fill;
    irq_mask_t s;

    if (src == NULL || n == 0)
        return 0;
    if (n > 1024)                               /* the game's buffers are 0x1000 bytes */
        n = 1024;
    s = irq_disable();
    fill = ai_head - ai_tail;
    if (fill + n > AI_RING) {                   /* overflow (output stalled): drop the oldest */
        u32 drop = fill + n - AI_RING;
        ai_tail += drop;
        vd_audio_stats.ai_dropped += drop;
    }
    i = ai_head & (AI_RING - 1);
    if (i + n <= AI_RING) {
        memcpy(&ai_ring[i], src, n * 4);
    } else {
        memcpy(&ai_ring[i], src, (AI_RING - i) * 4);
        memcpy(&ai_ring[0], src + (AI_RING - i), (n - (AI_RING - i)) * 4);
    }
    ai_head += n;
    vd_audio_stats.ai_fill = ai_head - ai_tail;
    irq_restore(s);
    vd_audio_stats.ai_buffers++;
    vd_audio_stats.ai_samples += n;
    return 0;
}

/* Boot and soft reset (called by vd_ultra_init before the game starts): silence,
   empty synthesizer; the stream itself keeps running. */
void vd_audio_stub_reset(void)
{
    vd_ai_init_once();
    vd_synth_reset();
    vd_bank_reset();
    vd_ai_reset();
}

VD_ALIAS(func_80021000, osAiGetLength);
VD_ALIAS(func_80021010, osAiSetFrequency);
VD_ALIAS(func_80021140, osAiSetNextBuffer);
