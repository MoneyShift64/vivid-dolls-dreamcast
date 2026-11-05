/* Vivid Dolls port - debug log (compile-time switch).
 *
 * Build with `make -f port/Makefile.dc DEBUG=<mask>` (or -DVD_DEBUG=<mask>) to enable.
 * Categories (bit mask):
 *   VD_DBG_BOOT  0x01  boot, thread create/start, soft reset
 *   VD_DBG_MESG  0x02  message-queue blocking / wake-ups (very verbose)
 *   VD_DBG_TASK  0x04  cooperative game-task switches (every Task_Yield; very verbose)
 *   VD_DBG_OVL   0x08  overlay loads / entries
 *   VD_DBG_GFX   0x10  graphics/audio tasks submitted / completed, swaps, frame counter
 *   VD_DBG_EVT   0x20  lost / retried events
 * Messages go to a RAM ring buffer (vd_dbg_ring, read by the emulator harness
 * tools/port/dc_harness/dcrun.py) and, for VD_DBG_BOOT|VD_DBG_OVL|VD_DBG_EVT, also to
 * dbglog (serial). The ring costs nothing on the console; serial output is slow
 * (the SCIF is polled), so the verbose categories stay in the ring only.
 */
#ifndef VD_DEBUG_H
#define VD_DEBUG_H

#define VD_DBG_BOOT 0x01
#define VD_DBG_MESG 0x02
#define VD_DBG_TASK 0x04
#define VD_DBG_OVL  0x08
#define VD_DBG_GFX  0x10
#define VD_DBG_EVT  0x20

#ifndef VD_DEBUG
#define VD_DEBUG 0
#endif

#define VD_DBG_RING_SIZE 0x10000u            /* power of two */

#if VD_DEBUG
void vd_dbg_printf(unsigned cat, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#define VD_LOG(cat, ...) do { if (VD_DEBUG & (cat)) vd_dbg_printf((cat), __VA_ARGS__); } while (0)
#else
#define VD_LOG(cat, ...) do { } while (0)
#endif

#endif
