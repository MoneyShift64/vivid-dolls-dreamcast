/* Vivid Dolls port - libultra PI (cartridge DMA), caches, address translation.
 *
 * The ROM image (big-endian .z64 order, only the used first 0x23B000 bytes) is
 * in RAM; a PI DMA is a memcpy, complete at once: the OSIoMesg is posted to the
 * caller's queue before osPiStartDma returns. Reads beyond the kept image return
 * 0xFF, which is what the rest of the 8 MB ROM holds.
 */
#include <string.h>
#include "ultra_internal.h"

const u8 *vd_rom_image;
u32 vd_rom_size;

typedef struct {                 /* libultra 2.0H OSIoMesg (0x18 bytes) */
    u16   type;
    u8    pri;
    u8    status;
    void *retQueue;
    void *dramAddr;
    u32   devAddr;
    u32   size;
    void *piHandle;
} VdIoMesg;

#define OS_MESG_TYPE_DMAREAD 11

static void vd_rom_copy(u8 *dst, u32 rom, u32 n)
{
    u32 k;

    if (rom < vd_rom_size) {
        k = vd_rom_size - rom;
        if (k > n)
            k = n;
        memcpy(dst, vd_rom_image + rom, k);
        dst += k;
        n -= k;
    }
    if (n != 0)
        memset(dst, 0xFF, n);
}

s32 osPiStartDma(void *mb, s32 pri, s32 dir, u32 devAddr, void *vAddr, u32 nbytes, void *mq)
{
    VdIoMesg *m = mb;
    u32 rom = devAddr & 0x0FFFFFFFu;    /* osRomBase-relative cartridge address */

    if (dir == 0)                       /* OS_READ: ROM -> RAM */
        vd_rom_copy((u8 *)vAddr, rom, nbytes);
    /* OS_WRITE to ROM: ignored */
    if (m != NULL) {
        m->type = OS_MESG_TYPE_DMAREAD;
        m->pri = (u8)pri;
        m->status = 0;
        m->retQueue = mq;
        m->dramAddr = vAddr;
        m->devAddr = devAddr;
        m->size = nbytes;
    }
    if (mq != NULL)
        osSendMesg(mq, (OSMesg)m, OS_MESG_NOBLOCK);
    return 0;
}

void osCreatePiManager(s32 pri, void *cmdQ, OSMesg *cmdBuf, s32 cmdMsgCnt)
{
    (void)pri; (void)cmdQ; (void)cmdBuf; (void)cmdMsgCnt;
}

/* The F3D interpreter and the PVR backend read RAM directly through the CPU,
 * so the N64 cache maintenance calls have nothing to do. */
void osWritebackDCacheAll(void) { }
void osWritebackDCache(void *vaddr, s32 nbytes) { (void)vaddr; (void)nbytes; }
void osInvalDCache(void *vaddr, s32 nbytes) { (void)vaddr; (void)nbytes; }
void osInvalICache(void *vaddr, s32 nbytes) { (void)vaddr; (void)nbytes; }

/* Port pointers are used as "physical" addresses everywhere (the F3D
 * interpreter resolves only segmented addresses < 0x10000000). */
u32 osVirtualToPhysical(void *addr)
{
    return (u32)(uintptr_t)addr;
}

VD_ALIAS(func_80025950, osPiStartDma);
VD_ALIAS(func_8001C990, osWritebackDCacheAll);
