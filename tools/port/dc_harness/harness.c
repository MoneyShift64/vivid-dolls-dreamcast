/* Minimal headless libretro frontend for the Flycast core (Vivid Dolls port harness).
 *
 * Built as a shared library (libharness.so) and driven from Python (ctypes, see
 * dcrun.py): load core + disc, run frames with scripted pad input, read RAM and
 * SH4 registers, read back the rendered frame (EGL surfaceless + Mesa llvmpipe).
 * Serial output (SCIF) is written by the patched core to $FLYCAST_SERIAL.
 */
#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <EGL/egl.h>
#include <EGL/eglext.h>
#define GL_GLEXT_PROTOTYPES 0
#include <GL/glcorearb.h>

#include "libretro.h"

/* ---- core entry points ---- */
static void *core;
static void (*c_init)(void);
static void (*c_deinit)(void);
static void (*c_set_env)(retro_environment_t);
static void (*c_set_video)(retro_video_refresh_t);
static void (*c_set_audio)(retro_audio_sample_t);
static void (*c_set_audio_batch)(retro_audio_sample_batch_t);
static void (*c_set_input_poll)(retro_input_poll_t);
static void (*c_set_input_state)(retro_input_state_t);
static bool (*c_load_game)(const struct retro_game_info *);
static void (*c_run)(void);
static void (*c_get_av)(struct retro_system_av_info *);
static void *(*c_get_mem)(unsigned);
static size_t (*c_get_mem_size)(unsigned);
static void (*c_dbg_regs)(uint32_t *);
static void (*c_set_controller)(unsigned, unsigned);

static char sysdir[512], savedir[512];
static FILE *logf;
static struct retro_hw_render_callback hw;
static int have_hw;

/* ---- GL / EGL ---- */
static EGLDisplay dpy;
static EGLContext ctx;
static GLuint fbo, rb_color, rb_depth;
static int fb_w = 640, fb_h = 480;
static PFNGLGENFRAMEBUFFERSPROC p_glGenFramebuffers;
static PFNGLBINDFRAMEBUFFERPROC p_glBindFramebuffer;
static PFNGLGENRENDERBUFFERSPROC p_glGenRenderbuffers;
static PFNGLBINDRENDERBUFFERPROC p_glBindRenderbuffer;
static PFNGLRENDERBUFFERSTORAGEPROC p_glRenderbufferStorage;
static PFNGLFRAMEBUFFERRENDERBUFFERPROC p_glFramebufferRenderbuffer;
static PFNGLREADPIXELSPROC p_glReadPixels;
static PFNGLCHECKFRAMEBUFFERSTATUSPROC p_glCheckFramebufferStatus;
static PFNGLPIXELSTOREIPROC p_glPixelStorei;
static PFNGLFINISHPROC p_glFinish;

/* ---- state ---- */
static uint32_t pad_mask[4];
static int16_t pad_axis[4][4];       /* lx ly rx ry */
static int16_t pad_trig[4][2];       /* L2 R2 analog */
static int last_w, last_h, last_valid;
static unsigned long frames_run, frames_presented;
static const void *last_sw_data;
static size_t last_sw_pitch;

static void hlog(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    vfprintf(logf ? logf : stderr, fmt, ap);
    va_end(ap);
    fflush(logf ? logf : stderr);
}

static void core_log(enum retro_log_level level, const char *fmt, ...)
{
    static const char *lv[] = { "D", "I", "W", "E" };
    va_list ap;
    FILE *f = logf ? logf : stderr;
    fprintf(f, "[core %s] ", lv[level & 3]);
    va_start(ap, fmt);
    vfprintf(f, fmt, ap);
    va_end(ap);
    fflush(f);
}

/* core option values */
static struct { char key[64]; char val[128]; } opts[64];
static int nopts;

int h_set_option(const char *key, const char *val)
{
    int i;
    for (i = 0; i < nopts; i++)
        if (!strcmp(opts[i].key, key))
            break;
    if (i == nopts) {
        if (nopts == 64)
            return -1;
        nopts++;
    }
    snprintf(opts[i].key, sizeof(opts[i].key), "%s", key);
    snprintf(opts[i].val, sizeof(opts[i].val), "%s", val);
    return 0;
}

static uintptr_t get_cur_fb(void) { return fbo; }

static retro_proc_address_t get_proc(const char *sym)
{
    return (retro_proc_address_t)eglGetProcAddress(sym);
}

static void lr_log_noop(void) {}

static bool env_cb(unsigned cmd, void *data)
{
    switch (cmd & 0xFFFF) {
    case RETRO_ENVIRONMENT_GET_SYSTEM_DIRECTORY:
        *(const char **)data = sysdir;
        return true;
    case RETRO_ENVIRONMENT_GET_SAVE_DIRECTORY:
        *(const char **)data = savedir;
        return true;
    case RETRO_ENVIRONMENT_GET_LOG_INTERFACE:
        ((struct retro_log_callback *)data)->log = core_log;
        return true;
    case RETRO_ENVIRONMENT_GET_VARIABLE: {
        struct retro_variable *v = data;
        int i;
        for (i = 0; i < nopts; i++)
            if (!strcmp(opts[i].key, v->key)) {
                v->value = opts[i].val;
                return true;
            }
        v->value = NULL;
        return false;
    }
    case RETRO_ENVIRONMENT_GET_VARIABLE_UPDATE:
        *(bool *)data = false;
        return true;
    case RETRO_ENVIRONMENT_SET_PIXEL_FORMAT:
        return true;
    case RETRO_ENVIRONMENT_GET_PREFERRED_HW_RENDER:
        *(unsigned *)data = getenv("HARNESS_GL") ? RETRO_HW_CONTEXT_OPENGL_CORE : RETRO_HW_CONTEXT_OPENGLES3;
        return true;
    case RETRO_ENVIRONMENT_SET_HW_RENDER: {
        struct retro_hw_render_callback *cb = data;
        if (cb->context_type != RETRO_HW_CONTEXT_OPENGL_CORE && cb->context_type != RETRO_HW_CONTEXT_OPENGL &&
            cb->context_type != RETRO_HW_CONTEXT_OPENGLES3 && cb->context_type != RETRO_HW_CONTEXT_OPENGLES_VERSION &&
            cb->context_type != RETRO_HW_CONTEXT_OPENGLES2) {
            hlog("[harness] refusing hw context type %d\n", cb->context_type);
            return false;
        }
        hw = *cb;
        cb->get_current_framebuffer = get_cur_fb;
        cb->get_proc_address = get_proc;
        hw.get_current_framebuffer = get_cur_fb;
        hw.get_proc_address = get_proc;
        have_hw = 1;
        hlog("[harness] hw render: type %d version %u.%u depth %d stencil %d\n", cb->context_type,
             cb->version_major, cb->version_minor, cb->depth, cb->stencil);
        return true;
    }
    case RETRO_ENVIRONMENT_GET_INPUT_BITMASKS:
        return true;
    case RETRO_ENVIRONMENT_GET_JIT_CAPABLE:
        *(bool *)data = true;
        return true;
    case RETRO_ENVIRONMENT_GET_FASTFORWARDING:
        *(bool *)data = false;
        return true;
    case RETRO_ENVIRONMENT_SET_SUPPORT_NO_GAME:
    case RETRO_ENVIRONMENT_SET_INPUT_DESCRIPTORS:
    case RETRO_ENVIRONMENT_SET_CONTROLLER_INFO:
    case RETRO_ENVIRONMENT_SET_GEOMETRY:
    case RETRO_ENVIRONMENT_SET_SYSTEM_AV_INFO:
    case RETRO_ENVIRONMENT_SET_ROTATION:
    case RETRO_ENVIRONMENT_SET_CORE_OPTIONS_DISPLAY:
    case RETRO_ENVIRONMENT_SET_VARIABLES:
        return true;
    case RETRO_ENVIRONMENT_SET_MESSAGE: {
        const struct retro_message *m = data;
        hlog("[harness] core message: %s\n", m->msg);
        return true;
    }
    case RETRO_ENVIRONMENT_GET_CORE_OPTIONS_VERSION:
        *(unsigned *)data = 0;
        return true;
    default:
        return false;
    }
}

static void video_cb(const void *data, unsigned w, unsigned h, size_t pitch)
{
    frames_presented++;
    if (data == NULL)
        return;          /* dupe */
    last_w = (int)w;
    last_h = (int)h;
    last_valid = 1;
    if (data != RETRO_HW_FRAME_BUFFER_VALID) {
        last_sw_data = data;
        last_sw_pitch = pitch;
    } else {
        last_sw_data = NULL;
    }
}

static void audio_cb(int16_t l, int16_t r) { (void)l; (void)r; }
static size_t audio_batch_cb(const int16_t *d, size_t n) { (void)d; return n; }
static void input_poll_cb(void) {}

static int16_t input_state_cb(unsigned port, unsigned device, unsigned index, unsigned id)
{
    if (port >= 4)
        return 0;
    if ((device & 0xFF) == RETRO_DEVICE_JOYPAD) {
        if (id == RETRO_DEVICE_ID_JOYPAD_MASK)
            return (int16_t)pad_mask[port];
        return (pad_mask[port] >> id) & 1;
    }
    if ((device & 0xFF) == RETRO_DEVICE_ANALOG) {
        if (index == RETRO_DEVICE_INDEX_ANALOG_BUTTON) {
            if (id == RETRO_DEVICE_ID_JOYPAD_L2) return pad_trig[port][0];
            if (id == RETRO_DEVICE_ID_JOYPAD_R2) return pad_trig[port][1];
            return ((pad_mask[port] >> id) & 1) ? 0x7FFF : 0;
        }
        if (index <= 1 && id <= 1)
            return pad_axis[port][index * 2 + id];
    }
    return 0;
}

static int egl_setup(void)
{
    PFNEGLGETPLATFORMDISPLAYEXTPROC gpd =
        (PFNEGLGETPLATFORMDISPLAYEXTPROC)eglGetProcAddress("eglGetPlatformDisplayEXT");
    EGLint major, minor, n;
    EGLConfig cfg;
    int gles = hw.context_type == RETRO_HW_CONTEXT_OPENGLES3 || hw.context_type == RETRO_HW_CONTEXT_OPENGLES2 ||
               hw.context_type == RETRO_HW_CONTEXT_OPENGLES_VERSION;
    EGLint cattr[] = { EGL_SURFACE_TYPE, 0, EGL_RENDERABLE_TYPE, gles ? EGL_OPENGL_ES3_BIT : EGL_OPENGL_BIT, EGL_RED_SIZE, 8,
                       EGL_GREEN_SIZE, 8, EGL_BLUE_SIZE, 8, EGL_NONE };
    EGLint xattr[16];
    int k = 0;

    if (gpd == NULL) {
        hlog("[harness] no eglGetPlatformDisplayEXT\n");
        return -1;
    }
    dpy = gpd(EGL_PLATFORM_SURFACELESS_MESA, EGL_DEFAULT_DISPLAY, NULL);
    if (dpy == EGL_NO_DISPLAY || !eglInitialize(dpy, &major, &minor)) {
        hlog("[harness] eglInitialize failed 0x%x\n", eglGetError());
        return -1;
    }
    eglBindAPI(gles ? EGL_OPENGL_ES_API : EGL_OPENGL_API);
    if (!eglChooseConfig(dpy, cattr, &cfg, 1, &n) || n < 1) {
        hlog("[harness] eglChooseConfig failed\n");
        return -1;
    }
    if (gles) {
        xattr[k++] = EGL_CONTEXT_MAJOR_VERSION; xattr[k++] = 3;
        xattr[k++] = EGL_CONTEXT_MINOR_VERSION; xattr[k++] = hw.context_type == RETRO_HW_CONTEXT_OPENGLES_VERSION ? (EGLint)hw.version_minor : 0;
    } else if (hw.context_type == RETRO_HW_CONTEXT_OPENGL_CORE) {
        xattr[k++] = EGL_CONTEXT_MAJOR_VERSION; xattr[k++] = hw.version_major ? (EGLint)hw.version_major : 3;
        xattr[k++] = EGL_CONTEXT_MINOR_VERSION; xattr[k++] = hw.version_major ? (EGLint)hw.version_minor : 2;
        xattr[k++] = EGL_CONTEXT_OPENGL_PROFILE_MASK; xattr[k++] = EGL_CONTEXT_OPENGL_CORE_PROFILE_BIT;
    }
    xattr[k] = EGL_NONE;
    ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, xattr);
    if (ctx == EGL_NO_CONTEXT) {
        hlog("[harness] eglCreateContext failed 0x%x\n", eglGetError());
        return -1;
    }
    if (!eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, ctx)) {
        hlog("[harness] eglMakeCurrent failed 0x%x\n", eglGetError());
        return -1;
    }
#define LOADGL(t, n) p_##n = (t)eglGetProcAddress(#n)
    LOADGL(PFNGLGENFRAMEBUFFERSPROC, glGenFramebuffers);
    LOADGL(PFNGLBINDFRAMEBUFFERPROC, glBindFramebuffer);
    LOADGL(PFNGLGENRENDERBUFFERSPROC, glGenRenderbuffers);
    LOADGL(PFNGLBINDRENDERBUFFERPROC, glBindRenderbuffer);
    LOADGL(PFNGLRENDERBUFFERSTORAGEPROC, glRenderbufferStorage);
    LOADGL(PFNGLFRAMEBUFFERRENDERBUFFERPROC, glFramebufferRenderbuffer);
    LOADGL(PFNGLREADPIXELSPROC, glReadPixels);
    LOADGL(PFNGLCHECKFRAMEBUFFERSTATUSPROC, glCheckFramebufferStatus);
    LOADGL(PFNGLPIXELSTOREIPROC, glPixelStorei);
    LOADGL(PFNGLFINISHPROC, glFinish);
    p_glGenFramebuffers(1, &fbo);
    p_glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    p_glGenRenderbuffers(1, &rb_color);
    p_glBindRenderbuffer(GL_RENDERBUFFER, rb_color);
    p_glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA8, fb_w, fb_h);
    p_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, rb_color);
    p_glGenRenderbuffers(1, &rb_depth);
    p_glBindRenderbuffer(GL_RENDERBUFFER, rb_depth);
    p_glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, fb_w, fb_h);
    p_glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rb_depth);
    if (p_glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        hlog("[harness] FBO incomplete\n");
        return -1;
    }
    hlog("[harness] EGL %d.%d, GL context ok, FBO %dx%d\n", major, minor, fb_w, fb_h);
    return 0;
}

#define SYM(var, name) do { *(void **)&var = dlsym(core, name); \
    if (!var) { hlog("[harness] missing %s\n", name); return -1; } } while (0)

int h_init(const char *core_path, const char *sys, const char *save, const char *logpath)
{
    if (logpath && *logpath)
        logf = fopen(logpath, "w");
    snprintf(sysdir, sizeof(sysdir), "%s", sys);
    snprintf(savedir, sizeof(savedir), "%s", save);
    core = dlopen(core_path, RTLD_NOW | RTLD_LOCAL);
    if (!core) {
        hlog("[harness] dlopen: %s\n", dlerror());
        return -1;
    }
    SYM(c_init, "retro_init");
    SYM(c_deinit, "retro_deinit");
    SYM(c_set_env, "retro_set_environment");
    SYM(c_set_video, "retro_set_video_refresh");
    SYM(c_set_audio, "retro_set_audio_sample");
    SYM(c_set_audio_batch, "retro_set_audio_sample_batch");
    SYM(c_set_input_poll, "retro_set_input_poll");
    SYM(c_set_input_state, "retro_set_input_state");
    SYM(c_load_game, "retro_load_game");
    SYM(c_run, "retro_run");
    SYM(c_get_av, "retro_get_system_av_info");
    SYM(c_get_mem, "retro_get_memory_data");
    SYM(c_get_mem_size, "retro_get_memory_size");
    SYM(c_set_controller, "retro_set_controller_port_device");
    *(void **)&c_dbg_regs = dlsym(core, "retro_dbg_get_regs");
    (void)lr_log_noop;
    c_set_env(env_cb);
    c_set_video(video_cb);
    c_set_audio(audio_cb);
    c_set_audio_batch(audio_batch_cb);
    c_set_input_poll(input_poll_cb);
    c_set_input_state(input_state_cb);
    c_init();
    return 0;
}

int h_load(const char *path)
{
    struct retro_game_info gi;
    struct retro_system_av_info av;

    memset(&gi, 0, sizeof(gi));
    gi.path = path;
    if (!c_load_game(&gi)) {
        hlog("[harness] retro_load_game failed\n");
        return -1;
    }
    c_get_av(&av);
    hlog("[harness] av: %ux%u (max %ux%u) fps %.3f\n", av.geometry.base_width, av.geometry.base_height,
         av.geometry.max_width, av.geometry.max_height, av.timing.fps);
    if (av.geometry.max_width > 0) {
        fb_w = (int)av.geometry.max_width;
        fb_h = (int)av.geometry.max_height;
    }
    if (have_hw) {
        if (egl_setup() != 0)
            return -2;
        if (hw.context_reset)
            hw.context_reset();
    }
    c_set_controller(0, RETRO_DEVICE_JOYPAD);
    return 0;
}

void h_set_input(int port, uint32_t mask)
{
    if (port >= 0 && port < 4)
        pad_mask[port] = mask;
}

void h_set_trig(int port, int l, int r)
{
    if (port >= 0 && port < 4) {
        pad_trig[port][0] = (int16_t)l;
        pad_trig[port][1] = (int16_t)r;
    }
}

void h_set_axis(int port, int axis, int value)
{
    if (port >= 0 && port < 4 && axis >= 0 && axis < 4)
        pad_axis[port][axis] = (int16_t)value;
}

void h_run(int n)
{
    while (n-- > 0) {
        c_run();
        frames_run++;
    }
}

unsigned long h_frames(void) { return frames_run; }
unsigned long h_presented(void) { return frames_presented; }

void *h_ram(void) { return c_get_mem(RETRO_MEMORY_SYSTEM_RAM); }
size_t h_ram_size(void) { return c_get_mem_size(RETRO_MEMORY_SYSTEM_RAM); }

int h_regs(uint32_t *out)
{
    if (!c_dbg_regs)
        return -1;
    c_dbg_regs(out);
    return 0;
}

/* Read the last presented frame as RGB (top-down) into out (w*h*3 bytes, w/h <= fb).
 * Returns 0 if a frame was available. */
int h_read_frame(uint8_t *out, int maxbytes, int *w, int *h)
{
    int x, y;
    static uint8_t *tmp;
    static int tmpsz;

    if (!last_valid)
        return -1;
    *w = last_w;
    *h = last_h;
    if (last_w * last_h * 3 > maxbytes)
        return -2;
    if (last_sw_data) {
        const uint16_t *s;
        for (y = 0; y < last_h; y++) {
            s = (const uint16_t *)((const uint8_t *)last_sw_data + y * last_sw_pitch);
            for (x = 0; x < last_w; x++) {
                uint16_t p = s[x];   /* RGB565 assumed */
                out[(y * last_w + x) * 3 + 0] = (uint8_t)(((p >> 11) & 31) * 255 / 31);
                out[(y * last_w + x) * 3 + 1] = (uint8_t)(((p >> 5) & 63) * 255 / 63);
                out[(y * last_w + x) * 3 + 2] = (uint8_t)((p & 31) * 255 / 31);
            }
        }
        return 0;
    }
    if (tmpsz < last_w * last_h * 4) {
        free(tmp);
        tmpsz = last_w * last_h * 4;
        tmp = malloc((size_t)tmpsz);
    }
    p_glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    p_glPixelStorei(GL_PACK_ALIGNMENT, 1);
    p_glReadPixels(0, 0, last_w, last_h, GL_RGBA, GL_UNSIGNED_BYTE, tmp);
    for (y = 0; y < last_h; y++) {
        int sy = hw.bottom_left_origin ? (last_h - 1 - y) : y;
        for (x = 0; x < last_w; x++) {
            out[(y * last_w + x) * 3 + 0] = tmp[(sy * last_w + x) * 4 + 0];
            out[(y * last_w + x) * 3 + 1] = tmp[(sy * last_w + x) * 4 + 1];
            out[(y * last_w + x) * 3 + 2] = tmp[(sy * last_w + x) * 4 + 2];
        }
    }
    return 0;
}

void h_deinit(void)
{
    if (c_deinit)
        c_deinit();
}

void h_prof(int period)
{
    void (*f)(int) = (void (*)(int))dlsym(core, "retro_dbg_prof");
    if (f) f(period);
}

int h_prof_read(uint32_t *out, int max)
{
    int (*f)(uint32_t *, int) = (int (*)(uint32_t *, int))dlsym(core, "retro_dbg_prof_read");
    return f ? f(out, max) : -1;
}
