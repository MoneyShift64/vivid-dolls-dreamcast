/* Vivid Dolls port - Dreamcast entry point.
 *
 * Boot: 640x480 at 60 Hz (VGA, else NTSC interlaced - also on PAL consoles),
 * read and check the player's ROM (VIVIDOLL.ROM), fill the memory blocks
 * (vd_dataload), set up the PVR + F3D interpreter and the libultra shim, then
 * run the original boot code (Main_Boot: osInitialize, TLB, args, idle thread).
 * The KOS main thread then only waits for a soft reset (A+B+X+Y+Start), which
 * stops all game threads, reloads the data blocks and boots the game again.
 */
#include <kos.h>
#include <dc/biosfont.h>
#include <dc/video.h>
#include <dc/pvr.h>
#include <stdio.h>
#include <string.h>

#include "dataload.h"
#include "vd_rom.h"
#include "../ultra/ultra_internal.h"
#include "f3d.h"
#include "pvr_backend.h"
#include "vd_debug.h"

KOS_INIT_FLAGS(INIT_DEFAULT);

extern void Main_Boot(u8 *cmdline);     /* src/game/system.c */
void vd_overlay_reset(void);            /* overlay.c */

static uint8_t *vd_rom;
static uint32_t vd_rom_len;
static F3DBackend vd_backend;
static semaphore_t vd_reset_sem;

/* ---------------------------------------------------------- text screen */

static void vd_text_screen(const char *title, const char *msg)
{
    int y = 48;
    char line[64];
    const char *p = msg;

    memset(vram_s, 0, 640 * 480 * 2);
    bfont_draw_str(vram_s + y * 640 + 40, 640, true, title);
    y += 48;
    while (*p && y < 440) {
        size_t n = strcspn(p, "\n");
        if (n > sizeof(line) - 1)
            n = sizeof(line) - 1;
        memcpy(line, p, n);
        line[n] = 0;
        bfont_draw_str(vram_s + y * 640 + 40, 640, true, line);
        y += 28;
        p += n;
        if (*p == '\n')
            p++;
    }
}

static void vd_fatal(const char *msg) __attribute__((noreturn));
static void vd_fatal(const char *msg)
{
    dbglog(DBG_ERROR, "Vivid Dolls: %s\n", msg);
    vd_text_screen("Vivid Dolls", msg);
    for (;;)
        thd_sleep(1000);
}

/* ---------------------------------------------------------------- video */

static void vd_video_init(void)
{
    if (vid_check_cable() == CT_VGA)
        vid_set_mode(DM_640x480_VGA, PM_RGB565);
    else
        vid_set_mode(DM_640x480_NTSC_IL, PM_RGB565);   /* the game runs at 60 Hz */
}

static void *vd_f3d_addr(uint32_t a)
{
    return (void *)(uintptr_t)a;        /* port pointers are used directly */
}

static void vd_gfx_init(void)
{
    pvr_init_params_t params;

    memset(&params, 0, sizeof(params));
    pvr_backend_fill_params(&params);
    pvr_init(&params);
    pvr_backend_init(&vd_backend);
    f3d_init(&vd_backend, vd_f3d_addr);
    if (f3d_mtx_selftest() != 0)
        dbglog(DBG_WARNING, "Vivid Dolls: f3d_mtx_selftest failed\n");
}

/* ----------------------------------------------------------- game start */

static void vd_load_blocks(void)
{
    uint32_t bad = 0;
    char msg[160];
    int r = vd_dataload(vd_rom, vd_rom_len, &bad);

    if (r != VD_DL_OK) {
        snprintf(msg, sizeof(msg),
                 "Loading the game data failed\n(error %d at ROM offset 0x%X).\n\n"
                 "The ROM file is not the expected one.", r, (unsigned)bad);
        vd_fatal(msg);
    }
}

static void vd_ultra_init(void)
{
    vd_ultra_threads_init();
    vd_ultra_vi_init();
    vd_ultra_sp_init();
    vd_ultra_cont_reset();
    vd_audio_stub_reset();
    vd_overlay_reset();
}

static void vd_start_game(void)
{
    vd_stack_floor = vd_block_base[VD_BLK_TASKPOOL];
    VD_LOG(VD_DBG_BOOT, "Main_Boot\n");
    Main_Boot(NULL);
}

/* ----------------------------------------------------------- soft reset */

static void vd_reset_irq(void)
{
    sem_signal(&vd_reset_sem);          /* from the vblank interrupt */
}

static void vd_soft_reset(void)
{
    int i;

    dbglog(DBG_INFO, "Vivid Dolls: soft reset\n");
    vd_reset_pending = 1;               /* graphics tasks complete without drawing */
    for (i = 0; i < 100 && vd_gfx_busy; i++)
        thd_sleep(2);                   /* let a running F3D task finish its PVR scene */
    vd_ultra_vi_reset();                /* no more retrace messages */
    vd_kill_all_threads();
    vd_reset_pending = 0;
    pvr_backend_flush_textures();
    vd_ultra_init();
    vd_load_blocks();
    vd_start_game();
}

/* ----------------------------------------------------------------- main */

int main(int argc, char **argv)
{
    char err[400];

    (void)argc;
    (void)argv;
    vd_video_init();
    vd_text_screen("Vivid Dolls", "Loading...");

    if (vd_rom_load(&vd_rom, &vd_rom_len, err, sizeof(err)) != 0)
        vd_fatal(err);
    vd_rom_image = vd_rom;
    vd_rom_size = vd_rom_len;

    vd_load_blocks();
    vd_gfx_init();
    vd_eeprom_init();
    vd_ultra_init();

    sem_init(&vd_reset_sem, 0);
    vd_soft_reset_request = vd_reset_irq;

    vd_start_game();
    for (;;) {
        sem_wait(&vd_reset_sem);
        vd_soft_reset();
    }
    return 0;
}
