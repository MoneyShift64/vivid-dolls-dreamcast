/* Vivid Dolls - scene graph, camera and light. See scene.h. */
#include "scene.h"
#include "libm.h"

/* Command-line flags (Main_ParseArgs) */
extern s32 D_800AF230;          /* -v */
extern s32 D_800B04C8;          /* -d */
extern s32 D_800B04B4;          /* -s */
extern s32 D_800B0728;          /* -l */

/* Scene node pool and the fixed part of the tree (Scene_Init) */
extern u8 D_800B4AB0[];         /* block pool, 0xC000 bytes of 0xE0-byte nodes */
extern SceneNode *D_800C316C, *D_800C30C0, *D_800C0AB4, *D_800AF234;   /* 4 root groups */
extern SceneNode *D_800B061C, *D_800B04A0, *D_800AF240, *D_800B0500;   /* their first child groups */

extern s32 gScreenWidth;        /* 0x800C30D0 */

/* Camera */
extern Vec3f D_800B0444;        /* eye */
extern Vec3f D_800B04B8;        /* target */
extern Vec3f D_800C1664;        /* up */
extern f32   D_800C51D4;        /* field of view (degrees) */

/* Block pool (not decompiled yet) */
void  func_80009D40(void *pool, u32 size, u32 block_size);   /* Pool_Init */
void *func_80009DB4(void *pool);                             /* Pool_Alloc (NULL when empty) */
s32   func_80009DF4(void *pool);                             /* Pool_CountFree */
void  func_80009E2C(void *pool, void *block);                /* Pool_Free */
/* World MtxF -> N64 fixed-point Mtx (guMtxF2L style) */
void  func_8000A924(void *dst, const MtxF src);
/* Viewport display list commands; returns the new end of the list */
SpriteGfx *func_80010900(SpriteGfx *gfx, s32 x, s32 y, s32 w, s32 h, s32 sx, s32 sy);
/* out = a x b (safe when out aliases a or b) */
void  func_80015988(Vec3f *out, const Vec3f *a, const Vec3f *b);

#define PTR32(p) ((u32)(uintptr_t)(p))

#define G_DL_W0      0x06000000u    /* gSPDisplayList */
#define G_MTX_LOAD_W0 0x01020040u   /* gSPMatrix(MODELVIEW | LOAD | NOPUSH), 0x40 bytes */

/* @ 0x800113E0
 * Split a command line at spaces (writing NULs over them, like argv) and set
 * the flags for "-v", "-d", "-s" and "-l". Stops at the first argument that
 * doesn't start with '-'. Other letters are ignored. Quirk kept: the original
 * has room for 31 arguments only (more would overrun its stack frame). */
void Main_ParseArgs(u8 *s)
{
    u8 *argv[32];
    s32 argc, i;

    D_800AF230 = 0;
    D_800B04C8 = 0;
    D_800B04B4 = 0;
    D_800B0728 = 0;
    if (s == NULL || *s == 0)
        return;
    argc = 1;
    do {
        while (*s != 0 && *s == ' ')
            *s++ = 0;
        if (*s != 0)
            argv[argc++] = s;
        while (*s != 0 && *s != ' ')
            s++;
    } while (*s != 0);

    for (i = 1; argc >= 2; i++, argc--) {
        u8 *a = argv[i];
        if (a[0] != '-')
            return;
        switch (a[1]) {
        case 'v': D_800AF230 = 1; break;
        case 'd': D_800B04C8 = 1; break;
        case 's': D_800B04B4 = 1; break;
        case 'l': D_800B0728 = 1; break;
        }
    }
}

/* Append node to the end of group's child list (just before the tail sentinel). */
static void Scene_AppendChild(SceneNode *group, SceneNode *node)
{
    group->group.tail->prev->next = node;
    node->prev = group->group.tail->prev;
    node->next = group->group.tail;
    group->group.tail->prev = node;
}

/* @ 0x80011550
 * Set up the node pool and the fixed tree: four root groups, each with one
 * child group. */
void Scene_Init(void)
{
    func_80009D40(D_800B4AB0, 0xC000, sizeof(SceneNode));
    D_800C316C = Scene_NewNode(SCENE_GROUP);
    D_800C30C0 = Scene_NewNode(SCENE_GROUP);
    D_800C0AB4 = Scene_NewNode(SCENE_GROUP);
    D_800AF234 = Scene_NewNode(SCENE_GROUP);
    D_800B061C = Scene_NewNode(SCENE_GROUP);
    Scene_AppendChild(D_800C316C, D_800B061C);
    D_800B04A0 = Scene_NewNode(SCENE_GROUP);
    Scene_AppendChild(D_800C30C0, D_800B04A0);
    D_800AF240 = Scene_NewNode(SCENE_GROUP);
    Scene_AppendChild(D_800C0AB4, D_800AF240);
    D_800B0500 = Scene_NewNode(SCENE_GROUP);
    Scene_AppendChild(D_800AF234, D_800B0500);
}

/* @ 0x800116B8
 * Allocate a zeroed node of the given type with an identity local matrix.
 * Groups get an empty child list (two sentinel nodes); viewports get the
 * full-screen defaults. Returns NULL when the pool is empty (quirk kept: a
 * group whose sentinels can't be allocated writes through NULL). */
SceneNode *Scene_NewNode(s32 type)
{
    SceneNode *node = func_80009DB4(D_800B4AB0);
    u32 *p;
    s32 n;

    if (node == NULL)
        return NULL;
    p = (u32 *)node;
    for (n = sizeof(SceneNode) / 4; n != 0; n--)
        *p++ = 0;
    node->type = type;
    if (type == SCENE_GROUP) {
        node->group.head = Scene_NewNode(SCENE_SENTINEL);
        node->group.tail = Scene_NewNode(SCENE_SENTINEL);
        node->group.head->prev = NULL;
        node->group.tail->next = NULL;
        node->group.head->next = node->group.tail;
        node->group.tail->prev = node->group.head;
    } else if (type == SCENE_VIEWPORT) {
        node->vp.h = 239;
        node->vp.x = 0;
        node->vp.y = 0;
        node->vp.sx = 0x2000;
        node->vp.sy = 0x2000;
        node->vp.w = (s16)(gScreenWidth - 1);
    }
    Mtx_Identity(node->mtx);
    return node;
}

/* @ 0x8001179C
 * Number of nodes still free in the pool. */
s32 Scene_CountFreeNodes(void)
{
    return func_80009DF4(D_800B4AB0);
}

/* @ 0x800117BC
 * Delete a node: groups first delete all their children and sentinels; then
 * the node is unlinked from its sibling list and returned to the pool.
 * The node must be in a list (prev and next non-NULL). */
void Scene_DeleteNode(SceneNode *node)
{
    if (node->type == SCENE_GROUP) {
        SceneNode *c = node->group.head->next;
        SceneNode *next;
        while ((next = c->next) != NULL) {
            Scene_DeleteNode(c);
            c = next;
        }
        func_80009E2C(D_800B4AB0, node->group.head);
        func_80009E2C(D_800B4AB0, node->group.tail);
    }
    node->prev->next = node->next;
    node->next->prev = node->prev;
    func_80009E2C(D_800B4AB0, node);
}

/* @ 0x80011868
 * world = mtx * parent (row by row, same summation order as the original),
 * then recurse into children of groups with this node's world matrix. */
void Scene_UpdateWorld(SceneNode *node, const MtxF parent)
{
    s32 i;

    for (i = 0; i < 4; i++) {
        f32 l0 = node->mtx[i][0], l1 = node->mtx[i][1];
        f32 l2 = node->mtx[i][2], l3 = node->mtx[i][3];
        f32 r0 = l0 * parent[0][0] + l1 * parent[1][0] + l2 * parent[2][0] + l3 * parent[3][0];
        f32 r1 = l0 * parent[0][1] + l1 * parent[1][1] + l2 * parent[2][1] + l3 * parent[3][1];
        f32 r2 = l0 * parent[0][2] + l1 * parent[1][2] + l2 * parent[2][2] + l3 * parent[3][2];
        f32 r3 = l0 * parent[0][3] + l1 * parent[1][3] + l2 * parent[2][3] + l3 * parent[3][3];
        node->world[i][0] = r0;
        node->world[i][1] = r1;
        node->world[i][2] = r2;
        node->world[i][3] = r3;
    }
    if (node->type == SCENE_GROUP) {
        SceneNode *c = node->group.head->next;
        SceneNode *next;
        while ((next = c->next) != NULL) {
            Scene_UpdateWorld(c, node->world);
            c = next;
        }
    }
}

/* @ 0x80011C20
 * Convert world matrices to N64 fixed point (gfx_mtx), recursively. */
void Scene_ConvertMatrices(SceneNode *node)
{
    func_8000A924(node->gfx_mtx, node->world);
    if (node->type == SCENE_GROUP) {
        SceneNode *c = node->group.head->next;
        SceneNode *next;
        while ((next = c->next) != NULL) {
            Scene_ConvertMatrices(c);
            c = next;
        }
    }
}

/* @ 0x80011C88
 * Append the F3D commands that draw node (and its subtree) at gfx; returns
 * the new end of the display list. Sentinels and unknown types emit nothing. */
SpriteGfx *Scene_BuildGfx(SpriteGfx *gfx, SceneNode *node)
{
    switch (node->type) {
    case SCENE_GROUP: {
        SceneNode *c = node->group.head->next;
        SceneNode *next;
        while ((next = c->next) != NULL) {
            gfx = Scene_BuildGfx(gfx, c);
            c = next;
        }
        break;
    }
    case SCENE_DL:
        if (node->setup_dl != NULL) {
            gfx->w0 = G_DL_W0;
            gfx->w1 = PTR32(node->setup_dl);
            gfx++;
        }
        if (node->dl != NULL) {
            gfx->w0 = G_MTX_LOAD_W0;
            gfx->w1 = PTR32(node->gfx_mtx);
            gfx++;
            gfx->w0 = G_DL_W0;
            gfx->w1 = PTR32(node->dl);
            gfx++;
        }
        if (node->finish_dl != NULL) {
            gfx->w0 = G_DL_W0;
            gfx->w1 = PTR32(node->finish_dl);
            gfx++;
        }
        break;
    case SCENE_DL_LIST: {
        u16 i;
        gfx->w0 = G_MTX_LOAD_W0;
        gfx->w1 = PTR32(node->gfx_mtx);
        gfx++;
        for (i = 0; i < node->dl_list.count; i++) {
            gfx->w0 = G_DL_W0;
            gfx->w1 = PTR32(node->dl_list.lists[i]);
            gfx++;
        }
        break;
    }
    case SCENE_VIEWPORT:
        gfx->w0 = G_MTX_LOAD_W0;
        gfx->w1 = PTR32(node->gfx_mtx);
        gfx = func_80010900(gfx + 1, node->vp.x, node->vp.y, node->vp.w, node->vp.h,
                            node->vp.sx, node->vp.sy);
        break;
    }
    return gfx;
}

/* @ 0x80011E60
 * Default camera: eye (0, 0, 405.11322), looking at the origin, up = +Y,
 * field of view 33 degrees. */
void Camera_Init(void)
{
    D_800B0444.x = 0.0f;
    D_800B0444.y = 0.0f;
    D_800B04B8.x = 0.0f;
    D_800B04B8.y = 0.0f;
    D_800B04B8.z = 0.0f;
    D_800C1664.x = 0.0f;
    D_800C1664.z = 0.0f;
    D_800B0444.z = 405.11322f;      /* 0x43CA8E7E */
    D_800C1664.y = 1.0f;
    D_800C51D4 = 33.0f;
}

/* Re-orthogonalise the up vector against the view direction. */
static void Camera_FixUp(void)
{
    Vec3f fwd, side;

    Camera_GetForward(&fwd);
    func_80015988(&side, &D_800C1664, &fwd);
    func_80015988(&D_800C1664, &fwd, &side);
}

/* @ 0x80011ED0
 * Move the camera eye and re-orthogonalise the up vector. */
void Camera_SetEye(f32 x, f32 y, f32 z)
{
    D_800B0444.x = x;
    D_800B0444.y = y;
    D_800B0444.z = z;
    Camera_FixUp();
}

/* @ 0x80011F40
 * Move the camera target and re-orthogonalise the up vector. */
void Camera_SetTarget(f32 x, f32 y, f32 z)
{
    D_800B04B8.x = x;
    D_800B04B8.y = y;
    D_800B04B8.z = z;
    Camera_FixUp();
}

/* @ 0x80011FB0
 * Unit vector from eye to target. The reciprocal is taken in double
 * precision, as in the original. */
void Camera_GetForward(Vec3f *out)
{
    f32 z, len, inv;

    out->x = D_800B04B8.x - D_800B0444.x;
    out->y = D_800B04B8.y - D_800B0444.y;
    z = D_800B04B8.z - D_800B0444.z;
    len = sqrtf(out->x * out->x + out->y * out->y + z * z);
    out->z = z;
    inv = (f32)(1.0 / (f64)len);
    out->x = out->x * inv;
    out->y = out->y * inv;
    out->z = out->z * inv;
}

/* @ 0x80012090
 * Default light: at (0, 0, 810.22644) shining at the origin, white, no
 * ambient; Lights1 built with an identity view matrix. */
void Light_Init(SceneLight *l)
{
    MtxF m;

    Mtx_Identity(m);
    l->pos.x = 0.0f;
    l->pos.y = 0.0f;
    l->target.x = 0.0f;
    l->target.y = 0.0f;
    l->target.z = 0.0f;
    l->pos.z = 810.22644f;          /* 0x444A8E7E */
    Light_CalcDir(l, &l->dir);
    Light_SetColor(l, 255.0f, 255.0f, 255.0f);
    Light_SetAmbient(l, 0.0f, 0.0f, 0.0f);
    Light_Update(l, m);
}

/* @ 0x80012130 */
void Light_SetPos(SceneLight *l, f32 x, f32 y, f32 z)
{
    l->pos.x = x;
    l->pos.y = y;
    l->pos.z = z;
    Light_CalcDir(l, &l->dir);
}

/* @ 0x80012158 */
void Light_SetTarget(SceneLight *l, f32 x, f32 y, f32 z)
{
    l->target.x = x;
    l->target.y = y;
    l->target.z = z;
    Light_CalcDir(l, &l->dir);
}

static f32 clamp255(f32 v)
{
    if (v > 255.0f)
        v = 255.0f;
    else if (v < 0.0f)
        v = 0.0f;
    return v;
}

/* @ 0x80012180
 * Light colour, each component clamped to 0..255 (NaN passes through). */
void Light_SetColor(SceneLight *l, f32 r, f32 g, f32 b)
{
    l->color[0] = clamp255(r);
    l->color[1] = clamp255(g);
    l->color[2] = clamp255(b);
}

/* @ 0x80012250
 * Ambient colour, each component clamped to 0..255 (NaN passes through). */
void Light_SetAmbient(SceneLight *l, f32 r, f32 g, f32 b)
{
    l->ambient[0] = clamp255(r);
    l->ambient[1] = clamp255(g);
    l->ambient[2] = clamp255(b);
}

/* float -> u32 the way the original compiler did it (values >= 2^31 are
 * offset first); only the low byte is kept by the callers. */
static u32 f2u(f32 f)
{
    if (f >= 2147483648.0f)
        return (u32)(s32)(f - 2147483648.0f) | 0x80000000u;
    return (u32)(s32)f;
}

/* @ 0x80012320
 * Build the N64 Lights1 block: direction transformed by the view matrix and
 * scaled to length 120, colours converted to bytes. */
void Light_Update(SceneLight *l, const MtxF view)
{
    Vec3f v;
    f32 s;

    v = l->dir;
    Mtx_TransformVector(&v, &v, view);
    s = 120.0f / sqrtf(v.x * v.x + v.y * v.y + v.z * v.z);
    l->ldir[0] = (s8)(s32)(v.x * s);
    l->ldir[1] = (s8)(s32)(v.y * s);
    l->ldir[2] = (s8)(s32)(v.z * s);
    l->col[0] = (u8)f2u(l->color[0]);
    l->col[1] = (u8)f2u(l->color[1]);
    l->col[2] = (u8)f2u(l->color[2]);
    l->colc[0] = (u8)f2u(l->color[0]);
    l->colc[1] = (u8)f2u(l->color[1]);
    l->colc[2] = (u8)f2u(l->color[2]);
    l->col[3] = 0;
    l->colc[3] = 0;
    l->ldir[3] = 0;
    l->amb_col[0] = (u8)f2u(l->ambient[0]);
    l->amb_col[1] = (u8)f2u(l->ambient[1]);
    l->amb_col[2] = (u8)f2u(l->ambient[2]);
    l->amb_colc[0] = (u8)f2u(l->ambient[0]);
    l->amb_colc[1] = (u8)f2u(l->ambient[1]);
    l->amb_colc[2] = (u8)f2u(l->ambient[2]);
    l->amb_col[3] = 0;
    l->amb_colc[3] = 0;
}

/* @ 0x80012760
 * out = (target - pos) * (-120 / |target - pos|), i.e. the direction towards
 * the light, length 120. */
void Light_CalcDir(SceneLight *l, Vec3f *out)
{
    f32 z, s;

    out->x = l->target.x - l->pos.x;
    out->y = l->target.y - l->pos.y;
    z = l->target.z - l->pos.z;
    s = sqrtf(out->x * out->x + out->y * out->y + z * z);
    out->z = z;
    s = -120.0f / s;
    out->x = out->x * s;
    out->y = out->y * s;
    out->z = out->z * s;
}
