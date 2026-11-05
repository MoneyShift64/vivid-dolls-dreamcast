/* Vivid Dolls port - debug ring buffer (see vd_debug.h). */
#include "vd_debug.h"

#if VD_DEBUG
#include <kos.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

extern volatile uint32_t vd_vi_retrace_count;

/* read by the harness: head = total bytes ever written; byte i is at ring[i % size] */
volatile uint32_t vd_dbg_head;
char vd_dbg_ring[VD_DBG_RING_SIZE];
const uint32_t vd_dbg_ring_size = VD_DBG_RING_SIZE;

void vd_dbg_printf(unsigned cat, const char *fmt, ...)
{
    char line[192];
    va_list ap;
    int n, k;
    uint32_t h;
    irq_mask_t s;

    n = snprintf(line, sizeof(line), "%5u ", (unsigned)vd_vi_retrace_count);
    va_start(ap, fmt);
    k = vsnprintf(line + n, sizeof(line) - (size_t)n, fmt, ap);
    va_end(ap);
    if (k < 0)
        k = 0;
    n += k;
    if (n > (int)sizeof(line) - 1)
        n = (int)sizeof(line) - 1;
    s = irq_disable();
    h = vd_dbg_head;
    for (k = 0; k < n; k++)
        vd_dbg_ring[(h + (uint32_t)k) & (VD_DBG_RING_SIZE - 1)] = line[k];
    vd_dbg_head = h + (uint32_t)n;
    irq_restore(s);
    if ((cat & (VD_DBG_BOOT | VD_DBG_OVL | VD_DBG_EVT)) && !irq_inside_int())
        dbglog(DBG_INFO, "%s", line);
}
#endif
