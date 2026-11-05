/* Vivid Dolls port - libultra VI (video interface) on the Dreamcast vblank.
 *
 * The picture itself is produced by the F3D interpreter + PVR when the graphics
 * task runs (os_sp.c); the VI shim only keeps libultra's bookkeeping: the
 * current/next frame buffer (osViSwapBuffer takes effect at the next retrace,
 * which the scheduler checks with osViGetCurrentFramebuffer() ==
 * osViGetNextFramebuffer()), the blanking flag, and the retrace message
 * (osViSetEvent) sent every `retraceCount` vblanks at 60 Hz.
 */
#include <kos.h>
#include <dc/vblank.h>
#include "ultra_internal.h"
#include "vd_debug.h"

volatile u32 vd_vi_retrace_count;
volatile int vd_vi_black;

static void *volatile vd_vi_cur;
static void *volatile vd_vi_next;
static void *volatile vd_vi_mode;
static VdMesgQueue *volatile vd_vi_mq;
static OSMesg vd_vi_msg;
static u32 vd_vi_every = 1;
static u32 vd_vi_count;
static int vd_vi_handle = -1;

static void vd_vblank(uint32_t code, void *data)
{
    (void)code;
    (void)data;
    vd_vi_retrace_count++;
    vd_vi_cur = vd_vi_next;             /* the swap requested by osViSwapBuffer happens now */
    vd_input_vblank();
    if (vd_vi_mq != NULL && ++vd_vi_count >= vd_vi_every) {
        vd_vi_count = 0;
        osSendMesg(vd_vi_mq, vd_vi_msg, OS_MESG_NOBLOCK);
    }
}

void vd_ultra_vi_reset(void)
{
    irq_mask_t s = irq_disable();

    vd_vi_mq = NULL;
    vd_vi_msg = NULL;
    vd_vi_every = 1;
    vd_vi_count = 0;
    vd_vi_cur = vd_vi_next = NULL;
    vd_vi_mode = NULL;
    vd_vi_black = 1;
    irq_restore(s);
}

void vd_ultra_vi_init(void)
{
    vd_ultra_vi_reset();
    if (vd_vi_handle < 0)
        vd_vi_handle = vblank_handler_add(vd_vblank, NULL);
}

void *vd_vi_current_fb(void)
{
    return vd_vi_cur;
}

/* ---- libultra API ---- */

void osCreateViManager(s32 pri)
{
    (void)pri;
}

void osViSetMode(void *mode)
{
    vd_vi_mode = mode;                  /* not interpreted: the DC video mode is fixed */
}

void osViSetSpecialFeatures(u32 features)
{
    (void)features;
}

void osViBlack(u32 active)
{
    VD_LOG(VD_DBG_GFX, "osViBlack %u\n", (unsigned)(active & 0xFF));
    vd_vi_black = (active & 0xFF) != 0;
}

void osViSetEvent(void *mq, OSMesg msg, u32 retraceCount)
{
    irq_mask_t s = irq_disable();

    vd_vi_mq = mq;
    vd_vi_msg = msg;
    vd_vi_every = retraceCount ? retraceCount : 1;
    vd_vi_count = 0;
    irq_restore(s);
}

void osViSwapBuffer(void *fb)
{
    VD_LOG(VD_DBG_GFX, "osViSwapBuffer %p\n", fb);
    vd_vi_next = fb;
}

void *osViGetCurrentFramebuffer(void)
{
    return vd_vi_cur;
}

void *osViGetNextFramebuffer(void)
{
    return vd_vi_next;
}

VD_ALIAS(func_80021990, osViSetSpecialFeatures);
VD_ALIAS(func_80021B00, osViSwapBuffer);
