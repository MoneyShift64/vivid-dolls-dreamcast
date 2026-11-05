/* Vivid Dolls port - F3D backend for the Dreamcast PVR (KallistiOS). */
#ifndef PVR_BACKEND_H
#define PVR_BACKEND_H

#include "f3d_backend.h"

/* Call once after pvr_init() (pvr_backend_pvr_params() gives suitable params:
 * only the translucent list, autosort disabled = N64 draw order). */
struct pvr_init_params;
void pvr_backend_init(F3DBackend *be);
void pvr_backend_fill_params(void *pvr_init_params_out);   /* pvr_init_params_t * */
/* Free every cached texture (e.g. on soft reset). */
void pvr_backend_flush_textures(void);

typedef struct {
    int frames, tris, polys_hdr, tex_uploads, tex_hits, tex_evictions, tex_live, vram_fail, tex_dma;
    unsigned wait_us_last;     /* time the last frame waited in pvr_wait_ready */
} PvrBackendStats;
const PvrBackendStats *pvr_backend_stats(void);

#endif
