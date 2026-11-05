/* Vivid Dolls port - controllers and the Aleck64 cabinet inputs.
 *
 * Dreamcast pads 1 and 2 (the first two maple controllers) are read once per
 * vblank (vd_input_vblank, interrupt context) into a snapshot, which feeds
 * both input paths of the game:
 *   - libultra controllers (osContInit / osContStartReadData /
 *     osContGetReadData) with the N64 button layout, used when the "use pads"
 *     DIP setting is selected (the default: bit 4 of the DIP word off);
 *   - the Aleck64 JAMMA registers at 0x02000000+ (vd_io_reg, see
 *     port/sys/vd_port_io.h), used with DIP bit 4 on.
 *
 * Mapping (both paths):
 *   D-pad or analog stick   joystick (N64: d-pad bits + analog stick)
 *   A, B                    button 1, 2 (N64 A, B)
 *   Y                       button 3 (N64 Z)
 *   Start                   start (P1/P2 start on the cabinet)
 *   X                       coin (slot 1 for pad 1, slot 2 for pad 2): one coin
 *                           per press, never a coin jam (see "coin pulse")
 *   L + R + Y (pad 1)       TEST switch (held)   } hidden combos; X/Y do not
 *   L + R + X (pad 1)       SERVICE switch (held)} act as coin/button there
 *   L, R triggers           N64 L, R
 *   A + B + X + Y + Start   soft reset (any pad)
 *   L + R (any pad, no X/Y) TEST CHEAT, CHEAT=1 builds only: the current round ends as a 99%
 *                           perfect clear (vd_cheat_stage_clear, Game_Main)
 * DIP switches: vd_dip_switches (1 = ON); default 0 = all OFF (factory).
 */
#include <kos.h>
#include <string.h>
#include "ultra_internal.h"

/* N64 pad bits */
#define N64_A      0x8000
#define N64_B      0x4000
#define N64_Z      0x2000
#define N64_START  0x1000
#define N64_UP     0x0800
#define N64_DOWN   0x0400
#define N64_LEFT   0x0200
#define N64_RIGHT  0x0100
#define N64_L      0x0020
#define N64_R      0x0010

#define CONT_TYPE_NORMAL        0x0005
#define CONT_NO_RESPONSE_ERROR  0x8

typedef struct {
    u16 button;
    s8  stick_x;
    s8  stick_y;
    u8  errnum;
    u8  pad;
} VdContPad;

typedef struct {
    u16 type;
    u8  status;
    u8  errnum;
} VdContStatus;

typedef struct {
    int present;
    u16 n64;          /* N64 button word */
    s8  sx, sy;       /* N64 stick */
    u8  cab;          /* cabinet byte, 1 = pressed: U D L R b1 b2 b3 b4 */
    u8  start;
    u8  coin;         /* coin line active */
    u8  test, service;
} VdPadSnap;

volatile u16 vd_dip_switches;
void (*vd_soft_reset_request)(void);

static VdPadSnap vd_snap[2];
#ifdef VD_CHEATS
static volatile int vd_cheat_lr;   /* L+R held (without X/Y) on any pad */
#endif
static VdContPad vd_read_data[4];
static u8  vd_coin_timer[2];
/* Coin pulse: Coin_Update (input thread, once per scheduler retrace message)
 * counts a coin when it sees the coin line on for 2..29 of its calls. When
 * frames take longer than a vblank the input thread runs late and in bursts,
 * so a pulse timed in vblanks alone could be seen once (no coin). The line
 * therefore stays on until the game has read 0x02000004 at least
 * VD_COIN_READS times (Coin_Update and Io_ReadButtons read it once each per
 * pass, so >= 3 samples) and for at least 4 vblanks, then goes off. */
#define VD_COIN_READS 6
static volatile u8 vd_coin_reads[2];
static volatile u8 vd_coin_on[2];
static u8  vd_prev_x[2];
static u8  vd_reset_armed = 1;
static volatile u16 vd_io_regs[8];

void vd_ultra_cont_reset(void)
{
    irq_mask_t s = irq_disable();

    memset(vd_read_data, 0, sizeof(vd_read_data));
    memset(vd_coin_timer, 0, sizeof(vd_coin_timer));
    vd_coin_on[0] = vd_coin_on[1] = 0;
    vd_reset_armed = 0;              /* the reset combo must be released first */
    irq_restore(s);
}

static s8 vd_axis(int v)
{
    v = v * 80 / 127;                /* DC -128..127 -> N64 about -80..80 */
    if (v > 80) v = 80;
    if (v < -80) v = -80;
    return (s8)v;
}

void vd_input_vblank(void)
{
    int p, any_reset = 0, any_cheat = 0;

    for (p = 0; p < 2; p++) {
        VdPadSnap *s = &vd_snap[p];
        maple_device_t *dev = maple_enum_type(p, MAPLE_FUNC_CONTROLLER);
        cont_state_t *st = dev ? (cont_state_t *)maple_dev_status(dev) : NULL;
        u32 b;
        int lr, x, y;

        memset(s, 0, sizeof(*s));
        if (st == NULL) {
            vd_prev_x[p] = 0;
            vd_coin_timer[p] = 0;
            vd_coin_on[p] = 0;
            continue;
        }
        s->present = 1;
        b = st->buttons;
        if ((b & CONT_RESET_BUTTONS) == CONT_RESET_BUTTONS)
            any_reset = 1;
        lr = st->ltrig > 128 && st->rtrig > 128;
#ifdef VD_CHEATS
        if (lr && !(b & (CONT_X | CONT_Y)))
            any_cheat = 1;
#endif
        x = st->joyx;
        y = st->joyy;

        if (b & CONT_A)          { s->n64 |= N64_A; s->cab |= 0x10; }
        if (b & CONT_B)          { s->n64 |= N64_B; s->cab |= 0x20; }
        if ((b & CONT_Y) && !lr) { s->n64 |= N64_Z; s->cab |= 0x40; }
        if (b & CONT_START)      { s->n64 |= N64_START; s->start = 1; }
        if (b & CONT_DPAD_UP)    { s->n64 |= N64_UP; s->cab |= 0x01; }
        if (b & CONT_DPAD_DOWN)  { s->n64 |= N64_DOWN; s->cab |= 0x02; }
        if (b & CONT_DPAD_LEFT)  { s->n64 |= N64_LEFT; s->cab |= 0x04; }
        if (b & CONT_DPAD_RIGHT) { s->n64 |= N64_RIGHT; s->cab |= 0x08; }
        if (st->ltrig > 128)     s->n64 |= N64_L;
        if (st->rtrig > 128)     s->n64 |= N64_R;
        if (y < -64) s->cab |= 0x01;
        if (y > 64)  s->cab |= 0x02;
        if (x < -64) s->cab |= 0x04;
        if (x > 64)  s->cab |= 0x08;
        s->sx = vd_axis(x);
        s->sy = vd_axis(-y);         /* N64: up is positive */

        if (lr) {
            s->test = (b & CONT_Y) != 0 && p == 0;
            s->service = (b & CONT_X) != 0 && p == 0;
            vd_prev_x[p] = (b & CONT_X) != 0;
        } else {
            u8 xb = (b & CONT_X) != 0;
            if (xb && !vd_prev_x[p] && !vd_coin_on[p]) {
                vd_coin_timer[p] = 4;
                vd_coin_reads[p] = 0;
                vd_coin_on[p] = 1;
            }
            vd_prev_x[p] = xb;
        }
        if (vd_coin_timer[p] != 0)
            vd_coin_timer[p]--;
    }

    if (any_reset) {
        if (vd_reset_armed && vd_soft_reset_request != NULL) {
            vd_reset_armed = 0;
            vd_soft_reset_request();
        }
    } else {
        vd_reset_armed = 1;
    }
#ifdef VD_CHEATS
    vd_cheat_lr = any_cheat;
#else
    (void)any_cheat;
#endif
}

#ifdef VD_CHEATS
/* Test cheat (CHEAT=1 builds only): L + R held (without X or Y, which are the
 * TEST/SERVICE combos). Game_Main (ovl2_a.c) ends the round as a 99% clear. */
int vd_cheat_stage_clear(void)
{
    return vd_cheat_lr;
}
#endif

/* ------------------------------------------------- Aleck64 I/O registers */

static void vd_io_refresh(int count)
{
    u16 players = 0, extra = 0;
    int p;
    irq_mask_t s = irq_disable();

    players = (u16)(vd_snap[0].cab | (vd_snap[1].cab << 8));
    if (vd_snap[0].start)   extra |= 0x01;
    if (vd_snap[1].start)   extra |= 0x02;
    for (p = 0; p < 2; p++) {
        if (!vd_coin_on[p])
            continue;
        if (vd_coin_timer[p] == 0 && vd_coin_reads[p] >= VD_COIN_READS) {
            vd_coin_on[p] = 0;          /* released: the game counts the coin now */
            continue;
        }
        if (count && vd_coin_reads[p] < 255)
            vd_coin_reads[p]++;
        extra |= p ? 0x08 : 0x04;
    }
    if (vd_snap[0].service) extra |= 0x10;
    if (vd_snap[0].test)    extra |= 0x20;
    irq_restore(s);
    /* all inputs are active low */
    vd_io_regs[0] = (u16)~vd_dip_switches;      /* 0x02000000 DIP switches */
    vd_io_regs[1] = (u16)~players;              /* 0x02000002 P1 / P2 */
    vd_io_regs[2] = (u16)~extra;                /* 0x02000004 start, coin, service, test */
}

/* Pointer to the 16-bit register at `addr` (0x02000000..0x0200000F). Inputs are
 * refreshed on every access; writes (0x0200000A coin counters / lockouts) are
 * kept and otherwise ignored. */
volatile u16 *vd_io_reg(u32 addr)
{
    u32 i = (addr >> 1) & 7;

    if (i <= 2)
        vd_io_refresh(i == 2);       /* reads of 0x02000004 count for the coin pulse */
    return &vd_io_regs[i];
}

/* ------------------------------------------------------ libultra controller */

s32 osContInit(void *mq, u8 *bitpattern, void *status)
{
    VdContStatus *st = status;
    u8 bits = 0;
    int i;

    /* Pads 1 and 2 are always reported as connected. The game reads the
     * controllers only if osContInit found one (D_800B0430 != 0), and it is called
     * during boot, possibly before the first vblank has filled vd_snap (that made
     * the pads dead in some builds: Start did nothing on the title screen). A pad
     * plugged in later then works too; a missing pad just reads as no buttons. */
    (void)mq;
    for (i = 0; i < 4; i++) {
        int present = i < 2;
        if (present)
            bits |= (u8)(1 << i);
        if (st != NULL) {
            st[i].type = present ? CONT_TYPE_NORMAL : 0;
            st[i].status = 0;
            st[i].errnum = present ? 0 : CONT_NO_RESPONSE_ERROR;
        }
    }
    if (bitpattern != NULL)
        *bitpattern = bits;
    return 0;
}

/* Starts a read: the data is latched now and the SI-done event is posted. */
s32 osContStartReadData(void *mq)
{
    int i;
    irq_mask_t s = irq_disable();

    (void)mq;
    for (i = 0; i < 4; i++) {
        VdContPad *d = &vd_read_data[i];
        if (i < 2 && vd_snap[i].present) {
            d->button = vd_snap[i].n64;
            d->stick_x = vd_snap[i].sx;
            d->stick_y = vd_snap[i].sy;
            d->errnum = 0;
        } else {
            d->button = 0;
            d->stick_x = d->stick_y = 0;
            d->errnum = CONT_NO_RESPONSE_ERROR;
        }
    }
    irq_restore(s);
    vd_post_event(OS_EVENT_SI);
    return 0;
}

void osContGetReadData(void *pads)
{
    VdContPad *p = pads;
    int i;

    for (i = 0; i < 4; i++)
        p[i] = vd_read_data[i];
}

/* Called by Cont_Thread only when D_800AF254 != 0, which nothing sets. */
s32 __osContRamRead(void *mq, s32 a1, s32 a2, void *buf)
{
    (void)mq; (void)a1; (void)a2; (void)buf;
    return 0;
}

VD_ALIAS(func_800222E0, osContInit);
VD_ALIAS(func_80021FC0, osContStartReadData);
VD_ALIAS(func_80022048, osContGetReadData);
