/* Vivid Dolls port - libultra audio library: heap, links, alCents2Ratio, and the
 * sequence-file / bank-file loaders (alSeqFileNew, alBnkfNew).
 *
 * alSeqFileNew / alBnkfNew relocate the offsets in the files as libultra does,
 * and on little-endian builds also convert every field of the big-endian ROM
 * files to native order in place (shared envelopes, key maps, loops and ADPCM
 * books are converted once). After these calls the game C and the players read
 * all fields natively (see port/ENDIAN_AUDIT.md). The compact sequences
 * themselves stay big-endian bytes (al_cseq.c reads them bytewise).
 * Wavetable `base` becomes the ROM address of the sample data; the
 * synthesizer reads the samples from the ROM image (vd_rom_image).
 */
#include <string.h>
#include "al_internal.h"

/* ------------------------------------------------------------ byte order */
#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#define VD_LE 1
static inline u16 sw16(u16 v) { return (u16)((v << 8) | (v >> 8)); }
static inline u32 sw32(u32 v) { return (v << 24) | ((v & 0xFF00u) << 8) | ((v >> 8) & 0xFF00u) | (v >> 24); }
#else
#define VD_LE 0
static inline u16 sw16(u16 v) { return v; }
static inline u32 sw32(u32 v) { return v; }
#endif
#define FIX16(x) ((x) = (__typeof__(x))sw16((u16)(x)))
#define FIX32(x) ((x) = (__typeof__(x))sw32((u32)(x)))

#define AL_CACHE_ALIGN 15

/* ------------------------------------------------------------ heap, links */

void alHeapInit(void *hpv, u8 *base, s32 len)
{
    ALHeap *hp = hpv;
    s32 extra = (AL_CACHE_ALIGN + 1) - ((s32)(uintptr_t)base & AL_CACHE_ALIGN);

    hp->base = (extra != AL_CACHE_ALIGN + 1) ? base + extra : base;
    hp->len = len;
    hp->cur = hp->base;
    hp->count = 0;
}

void *alHeapDBAlloc(u8 *file, s32 line, void *hpv, s32 num, s32 size)
{
    ALHeap *hp = hpv;
    s32 bytes = (num * size + AL_CACHE_ALIGN) & ~AL_CACHE_ALIGN;
    u8 *p = NULL;

    (void)file; (void)line;
    if (hp->cur + bytes <= hp->base + hp->len) {
        p = hp->cur;
        hp->cur += bytes;
    }
    hp->count++;
    return p;
}

void alLink(void *lnv, void *tov)
{
    ALLink *ln = lnv, *to = tov;

    ln->next = to->next;
    ln->prev = to;
    if (to->next)
        to->next->prev = ln;
    to->next = ln;
}

void alUnlink(void *lnv)
{
    ALLink *ln = lnv;

    if (ln->next)
        ln->next->prev = ln->prev;
    if (ln->prev)
        ln->prev->next = ln->next;
}

f32 alCents2Ratio(s32 cents)
{
    f32 x, ratio = 1.0f;

    if (cents >= 0) {
        x = 1.00057779f;            /* 2^(1/1200) */
    } else {
        x = 0.9994225441f;          /* 2^(-1/1200) */
        cents = -cents;
    }
    while (cents) {
        if (cents & 1)
            ratio *= x;
        x *= x;
        cents >>= 1;
    }
    return ratio;
}

/* ------------------------------------------------- sequence and bank files */

void alSeqFileNew(void *fv, u32 base)
{
    ALSeqFile *f = fv;
    s32 i;

    if (VD_LE) {
        FIX16(f->revision);
        FIX16(f->seqCount);
        for (i = 0; i < f->seqCount; i++) {
            FIX32(f->seqArray[i].offset);
            FIX32(f->seqArray[i].len);
        }
    }
    for (i = 0; i < f->seqCount; i++)
        f->seqArray[i].offset = f->seqArray[i].offset + base;
}

/* Objects can be shared (envelopes, key maps, loops, books); swap each once. */
#define VD_MAX_SWAPPED 1024
static const void *vd_swapped[VD_MAX_SWAPPED];
static int vd_nswapped;

static int vd_first_visit(const void *p)
{
    int i;

    for (i = 0; i < vd_nswapped; i++)
        if (vd_swapped[i] == p)
            return 0;
    if (vd_nswapped < VD_MAX_SWAPPED)
        vd_swapped[vd_nswapped++] = p;
    return 1;
}

#define PATCH(p, base) ((p) = (__typeof__(p))((u8 *)(p) + (u32)(uintptr_t)(base)))

static void vd_patch_wavetable(ALWaveTable *w, u8 *file, u32 table)
{
    if (w->flags)
        return;
    w->flags = 1;
    if (VD_LE)
        FIX32(w->len);
    w->base = w->base + table;
    if (w->type == AL_ADPCM_WAVE) {
        if (w->waveInfo.adpcm.book) {
            PATCH(w->waveInfo.adpcm.book, file);
            if (VD_LE && vd_first_visit(w->waveInfo.adpcm.book)) {
                ALADPCMBook *b = w->waveInfo.adpcm.book;
                s32 i, n;
                FIX32(b->order);
                FIX32(b->npredictors);
                n = b->order * b->npredictors * 8;
                for (i = 0; i < n && i < 0x1000; i++)
                    FIX16(b->book[i]);
            }
        }
        if (w->waveInfo.adpcm.loop) {
            PATCH(w->waveInfo.adpcm.loop, file);
            if (VD_LE && vd_first_visit(w->waveInfo.adpcm.loop)) {
                ALADPCMloop *l = w->waveInfo.adpcm.loop;
                s32 i;
                FIX32(l->start); FIX32(l->end); FIX32(l->count);
                for (i = 0; i < 16; i++)
                    FIX16(l->state[i]);
            }
        }
    } else if (w->type == AL_RAW16_WAVE) {
        if (w->waveInfo.raw.loop) {
            PATCH(w->waveInfo.raw.loop, file);
            if (VD_LE && vd_first_visit(w->waveInfo.raw.loop)) {
                ALRawLoop *l = w->waveInfo.raw.loop;
                FIX32(l->start); FIX32(l->end); FIX32(l->count);
            }
        }
    }
}

static void vd_patch_sound(ALSound *s, u8 *file, u32 table)
{
    if (s->flags)
        return;
    s->flags = 1;
    if (VD_LE) {
        FIX32(s->envelope);
        FIX32(s->keyMap);
        FIX32(s->wavetable);
    }
    PATCH(s->envelope, file);
    PATCH(s->keyMap, file);
    PATCH(s->wavetable, file);
    if (VD_LE && vd_first_visit(s->envelope)) {
        FIX32(s->envelope->attackTime);
        FIX32(s->envelope->decayTime);
        FIX32(s->envelope->releaseTime);
    }
    if (VD_LE && !s->wavetable->flags) {
        FIX32(s->wavetable->base);
        FIX32(s->wavetable->waveInfo.adpcm.loop);   /* also raw.loop */
        FIX32(s->wavetable->waveInfo.adpcm.book);
    }
    vd_patch_wavetable(s->wavetable, file, table);
}

static void vd_patch_inst(ALInstrument *inst, u8 *file, u32 table)
{
    s32 i;

    if (inst->flags)
        return;
    inst->flags = 1;
    if (VD_LE) {
        FIX16(inst->bendRange);
        FIX16(inst->soundCount);
    }
    for (i = 0; i < inst->soundCount; i++) {
        if (VD_LE)
            FIX32(inst->soundArray[i]);
        if (inst->soundArray[i] == NULL)
            continue;
        PATCH(inst->soundArray[i], file);
        vd_patch_sound(inst->soundArray[i], file, table);
    }
}

static void vd_patch_bank(ALBank *bank, u8 *file, u32 table)
{
    s32 i;

    if (bank->flags)
        return;
    bank->flags = 1;
    if (VD_LE) {
        FIX16(bank->instCount);
        FIX32(bank->sampleRate);
        FIX32(bank->percussion);
    }
    if (bank->percussion) {
        PATCH(bank->percussion, file);
        vd_patch_inst(bank->percussion, file, table);
    }
    for (i = 0; i < bank->instCount; i++) {
        if (VD_LE)
            FIX32(bank->instArray[i]);
        if (bank->instArray[i] == NULL)
            continue;
        PATCH(bank->instArray[i], file);
        vd_patch_inst(bank->instArray[i], file, table);
    }
}

void alBnkfNew(void *fv, u32 table)
{
    ALBankFile *f = fv;
    u8 *file = fv;
    s32 i;

    if (VD_LE) {
        if (sw16((u16)f->revision) != AL_BANK_VERSION)
            return;
        FIX16(f->revision);
        FIX16(f->bankCount);
    } else if (f->revision != AL_BANK_VERSION) {
        return;
    }
    for (i = 0; i < f->bankCount; i++) {
        if (VD_LE)
            FIX32(f->bankArray[i]);
        if (f->bankArray[i] == NULL)
            continue;
        PATCH(f->bankArray[i], file);
        vd_patch_bank(f->bankArray[i], file, table);
    }
    vd_pcm_cache_bankfile(f);
}

void vd_bank_reset(void)
{
    vd_nswapped = 0;
}

VD_ALIAS(func_8001D820, alHeapInit);
VD_ALIAS(func_8001D4A0, alSeqFileNew);
VD_ALIAS(func_8001D4E0, alBnkfNew);
