/* Vivid Dolls - overlay 2 part A (RAM 0x800E0000-0x800E4EDC): game task, HUD,
 * board / flood fill, player lines. See ovl2_a.h for an overview. */
#include "ovl2_a.h"
#include "heap.h"
#include "object.h"
#include "photoview.h"
#if defined(VD_PORT) && defined(VD_CHEATS)
int vd_cheat_stage_clear(void);   /* port/ultra/os_cont.c */
#endif

/* ------------------------------------------------------------------ externs */

/* main segment */
extern Heap gGameHeap;
extern ObjSceneNode *gSceneGroup0;
extern u8  D_800C1730[];        /* SceneLight */
extern s32 D_800C51CC;
extern s32 D_800C51D8;          /* attract/demo frame counter (0 = real game) */
extern s32 D_800B04D4;          /* player 1 lives (0 = not playing) */
extern s32 D_800B04D8;          /* player 2 lives */
extern s32 D_800B04DC;          /* random seed */
extern s32 D_800C30C8;          /* credits */
extern u16 D_800C30C4;          /* abort request (test mode?) */
extern s32 D_800C1674;
extern s32 D_800AF260;          /* abort request */
extern u16 D_800C0D48;          /* player 1 buttons (0x100 start, 0x10 fast-forward) */
extern u16 D_800C0D4A;          /* player 2 buttons */
extern u16 D_800C52A0;          /* DIP switches */
extern s32 D_800C172C;          /* accumulated percentage score */
extern u32 D_800B056C;
extern s32 D_800B4A30;
extern s32 D_800C52F4;
extern s32 D_800AF264;
extern s32 D_800C3174;          /* stock icons (0..3) */
extern u16 D_800C53D8[2];       /* coin inputs */
extern s32 D_800B4AA0;          /* difficulty (index into the enemy count table) */
extern s32 D_800B0608;          /* current girl 0..7 */
extern s32 D_800B04CC;          /* current photo of that girl 0..2 */
extern u8  D_800C178E[8];       /* girls completed */

void  Camera_Init(void);
void  Light_Init(void *light);
u32   Rand_Range(s32 *seed, s32 range);
void  bzero(void *p, u32 n);
void  Jpeg64_Decode(u32 rom, u16 *dst, s32 a2, s32 a3, s32 *w, s32 *h);
/* task system: spawn fn as a task with the given priority, copying argBytes of the
 * arguments that follow; returns the task handle */
void *func_8000A76C(void *fn, s32 prio, s32 argBytes, ...);
void  func_8000AEAC(void);      /* task yield (wait one frame) */
s32   func_8001BA90(s32 id);    /* play sound effect */
void  func_8001BBC0(s32 id);    /* play music */
void  func_8000D6B8(void);      /* stop all sounds */

/* other parts of overlay 2 */
void ovl2_func_800F45DC(void);
void ovl2_func_800F10F0(void);
void ovl2_func_800F4FC0(void);
void ovl2_func_800F5780(void);
void ovl2_func_800F5D50(void);
void ovl2_func_800F4210(void);
void ovl2_func_800E9DC0(void);
void ovl2_func_800EA610(void);
void ovl2_func_800EAD80(void);
void ovl2_func_800EC0CC(void);
void ovl2_func_800EEEEC(void);
void ovl2_func_800F00F4(void);
void ovl2_func_800F1D6C(void);
void ovl2_func_800F200C(void);
void ovl2_func_800F2384(s32 x, s32 y);
void ovl2_func_800F2580(s32 a, s32 b, s32 c, s32 d);

/* overlay 2 data */
extern u8  ovl2_D_800F9910[4];  /* cell clear masks */
extern u8  ovl2_D_800F9914[4];  /* cell value 1, per position */
extern u8  ovl2_D_800F9918[4];  /* cell value 2 */
extern u8  ovl2_D_800F991C[4];  /* cell value 3 */
extern u8  ovl2_D_800F9920[4];  /* cell shifts */
extern u8  ovl2_D_800F9924[8];  /* 1 << i (board block map) */
extern s16 ovl2_D_800F9950[9][2];       /* direction 0..8 -> (dx, dy) */
extern u16 ovl2_D_800F9974[3][3];       /* [dy + 1][dx + 1] -> direction */
extern u16 ovl2_D_800F9988[9][9];       /* [dir][previous dir] -> counts as a crossing */
extern u8  ovl2_D_800F9A2C[8];  /* 1 << i (player line map) */
extern u8  ovl2_D_800F9A34[8];  /* ~(1 << i) */
extern s32 ovl2_D_800F9900;     /* random seed for the claim sounds */
extern f64 ovl2_D_800FAF30;     /* 0.5 */
extern f64 ovl2_D_800FAF40;     /* 0.5 */
extern s32 ovl2_D_800F6390[];   /* enemy count per difficulty */
extern s32 ovl2_D_800FB0E0[];   /* enemy task parameters */
extern u32 ovl2_D_800F9880[];   /* photo ROM offsets [girl * 4 + photo] */
extern SceneObjDesc   ovl2_D_800F6450[];
extern SceneAssetDesc ovl2_D_800F7E54[];
extern f32 ovl2_D_800F63D0[8][2];       /* ending: positions of the 8 girl icons */
extern f32 ovl2_D_800F6410[8][2];       /* ending: their targets */
extern u8  ovl2_D_800FB050[], ovl2_D_800FB068[], ovl2_D_800F922C[], ovl2_D_800F9318[];
extern u8  ovl2_D_800F93B4[], ovl2_D_800FB200[], ovl2_D_800FB218[], ovl2_D_800F8E1C[];
extern u8  ovl2_D_800F8E4C[], ovl2_D_800FB080[], ovl2_D_800F96DC[], ovl2_D_800F97B0[];
extern u8  ovl2_D_800F9874[], ovl2_D_800F95D4[], ovl2_D_800F9488[], ovl2_D_800F9438[];
extern void *ovl2_D_800FB22C, *ovl2_D_800FB230;

/* HUD objects and their sprites */
extern Obj *ovl2_D_800FB138, *ovl2_D_800FB13C, *ovl2_D_800FB140, *ovl2_D_800FB144; /* timer */
extern Obj *ovl2_D_800FB148, *ovl2_D_800FB14C;  /* percentage digits */
extern Obj *ovl2_D_800FB154[3];                 /* stock icons */
extern Obj *ovl2_D_800FB160[4], *ovl2_D_800FB170[4]; /* lives P1 / P2 */
extern Obj *ovl2_D_800FB180[4];                 /* big digits / ending texts */
extern Obj *ovl2_D_800FB190, *ovl2_D_800FB194, *ovl2_D_800FB19C;
extern Obj *ovl2_D_800FB1A0[8];                 /* ending girl icons */
extern Obj *ovl2_D_800FB1C4, *ovl2_D_800FB1C8, *ovl2_D_800FB1CC; /* credits */
extern Obj *ovl2_D_800FB1D0, *ovl2_D_800FB1D4;  /* "press start" P1 / P2 */
extern Obj *ovl2_D_800FB1D8, *ovl2_D_800FB1DC, *ovl2_D_800FB1E0, *ovl2_D_800FB1E4;
extern Obj *ovl2_D_800FB1E8[5];                 /* claim popup: "+", digits, "%", rating */
extern void *ovl2_D_800FB390[], *ovl2_D_800FB3B8[], *ovl2_D_800FB3E0[], *ovl2_D_800FB410[];
extern void *ovl2_D_800FB408, *ovl2_D_800FB40C, *ovl2_D_800FB438, *ovl2_D_800FB454;
extern void *ovl2_D_800FB444, *ovl2_D_800FB448, *ovl2_D_800FB44C, *ovl2_D_800FB450;
extern void *ovl2_D_800FB458[], *ovl2_D_800FB480[], *ovl2_D_800FB4C8[];
extern void *ovl2_D_800FB4AC, *ovl2_D_800FB4B0, *ovl2_D_800FB4B4, *ovl2_D_800FB4B8;
extern void *ovl2_D_800FB4BC, *ovl2_D_800FB4C0, *ovl2_D_800FB4C4, *ovl2_D_800FB4F0, *ovl2_D_800FB4F4;

/* overlay 2 BSS */
extern s32  ovl2_D_800FBA70;    /* board width */
extern s32  ovl2_D_800FBA74;    /* board height */
extern u8  *ovl2_D_800FBA78;    /* block map: 1 bit per 8x8 block */
extern u8  *ovl2_D_800FBA7C;    /* cells: 2 bits per pixel */
extern u16 *ovl2_D_800FBA80;    /* displayed image */
extern u16 *ovl2_D_800FBA84;    /* hidden photo */
extern s32  ovl2_D_800FBA88;    /* claimed pixel count */
extern s32  ovl2_D_800FB660;    /* flood fill stack depth */
extern s16  ovl2_D_800FB670[256][2];    /* flood fill stack */
extern s32  ovl2_D_800FD2B0, ovl2_D_800FD500, ovl2_D_800FD2F0;
extern s32  ovl2_D_800FD508[];
extern void *ovl2_D_800FB600[];
extern void *ovl2_D_800FB640, *ovl2_D_800FB644, *ovl2_D_800FB648, *ovl2_D_800FB64C;
extern void *ovl2_D_800FB650, *ovl2_D_800FB654, *ovl2_D_800FB658;
extern s16  ovl2_D_800FD2B4;    /* monster y (read as s16) */
extern s16  ovl2_D_800FD2C4;    /* monster x (read as s16) */
extern s32  ovl2_D_800FD2B8;    /* photo scroll x */
extern s32  ovl2_D_800FD2C8;    /* photo scroll y */
extern u8  *ovl2_D_800FD2BC;    /* cell buffer */
extern s32  ovl2_D_800FD2C0;
extern s32  ovl2_D_800FD2CC;    /* time-up flag */
extern f32  ovl2_D_800FD2D0;    /* claim popup x */
extern f32  ovl2_D_800FD2DC;    /* claim popup y */
extern u16 *ovl2_D_800FD2D4;    /* photo buffer */
extern s32  ovl2_D_800FD2D8;    /* game over / leaving */
extern Player *ovl2_D_800FD3F8, *ovl2_D_800FD3FC;
extern s32  ovl2_D_800FD400;    /* claim sound pending */
extern s32  ovl2_D_800FD404;    /* last claim in percent */
extern s32  ovl2_D_800FD424;    /* timer in frames */
extern s32  ovl2_D_800FD428;    /* claim popup frames left */
extern u8  *ovl2_D_800FD42C;    /* block map buffer */
extern s32  ovl2_D_800FD4F8;
extern u16 *ovl2_D_800FD548;    /* displayed image buffer */

#define gBoardW   ovl2_D_800FBA70
#define gBoardH   ovl2_D_800FBA74
#define gBlocks   ovl2_D_800FBA78
#define gCells    ovl2_D_800FBA7C
#define gImage    ovl2_D_800FBA80
#define gPhoto    ovl2_D_800FBA84
#define gFilled   ovl2_D_800FBA88

#define DIR_DX(d) (ovl2_D_800F9950[d][0])
#define DIR_DY(d) (ovl2_D_800F9950[d][1])

/* byte index of the 8x8 block containing (x, y) in a 1-bit block map of a board w pixels wide */
#define BLOCK_BYTE(x, y, w) ((((x) >> 3) + ((y) >> 3) * ((w) >> 3)) >> 3)

/* ------------------------------------------------------------------ board */

/* @ 0x800E2910 */
/* Set up the board buffers and clear the block map (w*h/8 bytes - far more than
 * needed, kept) and the cell map (w*h/4 bytes). */
void Board_Init(u16 *image, u16 *photo, u8 *cells, u8 *blocks, s32 w, s32 h) {
    s32 i;

    gBlocks = blocks;
    for (i = 0; i < (w * h) / 8; i++) {
        *blocks++ = 0;
    }
    gCells = cells;
    for (i = 0; i < (w * h) / 4; i++) {
        *cells++ = 0;
    }
    gImage = image;
    gPhoto = photo;
    gBoardW = w;
    gBoardH = h;
    gFilled = 0;
}

/* @ 0x800E29C4 */
s32 Board_GetWidth(void) {
    return gBoardW;
}

/* @ 0x800E29D0 */
s32 Board_GetHeight(void) {
    return gBoardH;
}

/* @ 0x800E29DC */
/* Claimed area in percent. */
f32 Board_GetPercent(void) {
    return ((f32)gFilled / (f32)(gBoardW * gBoardH)) * 100.0f;
}

/* @ 0x800E2A24 */
/* Scanline flood fill of unclaimed cells starting at (x, y). */
void Board_FloodFill(s32 x, s32 y) {
    s32 fx, fy;

    ovl2_D_800FB660 = 0;
    Fill_Push(x, y);
    do {
        Fill_Pop();
        fx = Fill_TopX();
        fy = Fill_TopY();
        Board_FillPixel(fx, fy);
        Fill_Span(fx, fy);
    } while (ovl2_D_800FB660 != 0);
}

/* @ 0x800E2A98 */
/* Mark the 8x8 block containing (x, y) as changed. */
void Board_MarkBlock(s32 x, s32 y) {
    s32 bx = x >> 3;
    u8 *p = gBlocks + ((bx + (y >> 3) * (gBoardW >> 3)) >> 3);

    *p |= ovl2_D_800F9924[bx & 7];
}

/* @ 0x800E2AE8 */
u8 Board_TestBlock(s32 x, s32 y) {
    s32 bx = x >> 3;

    return gBlocks[(bx + (y >> 3) * (gBoardW >> 3)) >> 3] & ovl2_D_800F9924[bx & 7];
}

/* @ 0x800E2B34 */
u8 Board_GetBlockByte(s32 x, s32 y) {
    return gBlocks[BLOCK_BYTE(x, y, gBoardW)];
}

/* @ 0x800E2B6C */
/* Make (x, y) an edge cell (3) and set its pixel. */
void Board_SetEdge(s32 x, s32 y, u16 color) {
    u8 *p = gCells + ((x + y * gBoardW) >> 2);

    *p = ovl2_D_800F991C[x & 3] | (*p & ovl2_D_800F9910[x & 3]);
    gImage[x + y * gBoardW] = color;
}

/* @ 0x800E2BEC */
void Board_SetEdgeCell(s32 x, s32 y) {
    u8 *p = gCells + ((x + y * gBoardW) >> 2);

    *p = ovl2_D_800F991C[x & 3] | (*p & ovl2_D_800F9910[x & 3]);
}

/* @ 0x800E2C40 */
/* Make (x, y) a line cell (2) and set its pixel. */
void Board_SetLine(s32 x, s32 y, u16 color) {
    u8 *p = gCells + ((x + y * gBoardW) >> 2);

    *p = ovl2_D_800F9918[x & 3] | (*p & ovl2_D_800F9910[x & 3]);
    gImage[x + y * gBoardW] = color;
}

/* @ 0x800E2CC0 */
/* Claim (x, y) (cell 1) and show the photo there. */
void Board_Reveal(s32 x, s32 y) {
    u8 *p = gCells + ((x + y * gBoardW) >> 2);

    *p = ovl2_D_800F9914[x & 3] | (*p & ovl2_D_800F9910[x & 3]);
    gImage[x + y * gBoardW] = gPhoto[x + y * gBoardW];
}

/* @ 0x800E2D54 */
/* Make (x, y) unclaimed (cell 0) and set its pixel. */
void Board_Clear(s32 x, s32 y, u16 color) {
    u8 *p = gCells + ((x + y * gBoardW) >> 2);

    *p &= ovl2_D_800F9910[x & 3];
    gImage[x + y * gBoardW] = color;
}

/* @ 0x800E2DC4 */
void Board_SetPixel(s32 x, s32 y, u16 color) {
    gImage[x + y * gBoardW] = color;
}

/* @ 0x800E2DF4 */
u16 Board_GetPixel(s32 x, s32 y) {
    return gImage[x + y * gBoardW];
}

/* @ 0x800E2E24 */
/* Cell value 0..3 at (x, y), 4 outside the board. */
s32 Board_GetCell(s32 x, s32 y) {
    if (x < 0 || x >= gBoardW || y < 0 || y >= gBoardH) {
        return 4;
    }
    return (gCells[(x + y * gBoardW) >> 2] & ~ovl2_D_800F9910[x & 3]) >> ovl2_D_800F9920[x & 3];
}

/* @ 0x800E2EBC */
/* Show the whole photo (stage clear). Column by column. */
void Board_RevealAll(void) {
    s32 x, y;
    u16 *dst = gImage, *src = gPhoto;

    for (x = 0; x < gBoardW; x++) {
        for (y = 0; y < gBoardH; y++) {
            dst[y * gBoardW] = src[y * gBoardW];
        }
        src++;
        dst++;
    }
}

/* @ 0x800E2F44 */
/* Paint white every pixel whose 8x8 block is set in player 1's line map (debug view). */
void Board_ShowLineBlocks(void) {
    s32 x, y;
    u16 *dst = gImage;

    for (x = 0; x < gBoardW; x++) {
        for (y = 0; y < gBoardH; y++) {
            s32 bx = x >> 3;
            if (ovl2_D_800F9924[bx & 7] &
                ovl2_D_800FD3F8->lineBits[(bx + (y >> 3) * (gBoardW >> 3)) >> 3]) {
                dst[y * gBoardW] = 0xFFFF;
            }
        }
        dst++;
    }
}

/* ------------------------------------------------------------------ flood fill */

/* @ 0x800E3010 */
/* Walk from (x, y) in steps of dx while the cells are unclaimed, filling them and
 * pushing the start of every new unclaimed run above and below. An edge cell with a
 * claimed cell right behind it is claimed too, so one-pixel edges left between two
 * claimed areas disappear. */
void Fill_Scan(s32 dx, s32 x, s32 y, s32 upBlocked, s32 downBlocked) {
    s32 c, prev;

    for (;;) {
        c = Board_GetCell(x, y);
        if (c != 0) {
            break;
        }
        Board_FillPixel(x, y);

        c = Board_GetCell(x, y - 1);
        prev = upBlocked;
        upBlocked = (c != 0);
        if (prev != 0 && !upBlocked) {
            Fill_Push(x, y - 1);
        }
        if (c == 3 && Board_GetCell(x, y - 2) == 1) {
            Board_FillPixel(x, y - 1);
        }

        c = Board_GetCell(x, y + 1);
        prev = downBlocked;
        downBlocked = (c != 0);
        if (prev != 0 && !downBlocked) {
            Fill_Push(x, y + 1);
        }
        if (c == 3 && Board_GetCell(x, y + 2) == 1) {
            Board_FillPixel(x, y + 1);
        }
        x += dx;
    }

    if (c == 3) {
        if (Board_GetCell(x + dx, y) == 1) {
            Board_FillPixel(x, y);
        }
        if (Board_GetCell(x, y - 1) == 3 && Board_GetCell(x, y - 2) == 1) {
            Board_FillPixel(x, y - 1);
        }
        if (Board_GetCell(x, y + 1) == 3 && Board_GetCell(x, y + 2) == 1) {
            Board_FillPixel(x, y + 1);
        }
    }
}

/* @ 0x800E3718 */
/* Fill the horizontal run through (x, y) (which has just been filled). */
void Fill_Span(s32 x, s32 y) {
    s32 c, up, down;

    c = Board_GetCell(x, y - 1);
    up = 1;
    if (c == 0) {
        Fill_Push(x, y - 1);
        up = 0;
    }
    if (c == 3 && Board_GetCell(x, y - 2) == 1) {
        Board_FillPixel(x, y - 1);
    }
    c = Board_GetCell(x, y + 1);
    down = 1;
    if (c == 0) {
        Fill_Push(x, y + 1);
        down = 0;
    }
    if (c == 3 && Board_GetCell(x, y + 2) == 1) {
        Board_FillPixel(x, y + 1);
    }
    Fill_Scan(-1, x - 1, y, up, down);
    Fill_Scan(1, x + 1, y, up, down);
}

/* @ 0x800E3A44 */
/* No overflow check (256 entries). */
void Fill_Push(s32 x, s32 y) {
    s32 n = ovl2_D_800FB660;

    ovl2_D_800FB670[n][0] = x;
    ovl2_D_800FB670[n][1] = y;
    ovl2_D_800FB660 = n + 1;
}

/* @ 0x800E3A78 */
void Fill_Pop(void) {
    ovl2_D_800FB660--;
}

/* @ 0x800E3A90 */
s32 Fill_TopX(void) {
    return ovl2_D_800FB670[ovl2_D_800FB660][0];
}

/* @ 0x800E3AAC */
s32 Fill_TopY(void) {
    return ovl2_D_800FB670[ovl2_D_800FB660][1];
}

/* @ 0x800E3AC8 */
/* Claim one pixel: count it, mark its block, set the cell to 1, show the photo. */
void Board_FillPixel(s32 x, s32 y) {
    s32 bx = x >> 3;
    u8 *p;

    gFilled++;
    p = gBlocks + ((bx + (y >> 3) * (gBoardW >> 3)) >> 3);
    *p |= ovl2_D_800F9924[bx & 7];
    p = gCells + ((x + y * gBoardW) >> 2);
    *p = ovl2_D_800F9914[x & 3] | (*p & ovl2_D_800F9910[x & 3]);
    gImage[x + y * gBoardW] = gPhoto[x + y * gBoardW];
}

/* ------------------------------------------------------------------ line maps */

/* @ 0x800E3BC0 */
/* Is the 8x8 block of (x, y) set in any of three line maps? */
u8 Bits_TestAny3(u8 *a, u8 *b, u8 *c, s32 x, s32 y) {
    s32 i = BLOCK_BYTE(x, y, Board_GetWidth());

    return ovl2_D_800F9A2C[(x >> 3) & 7] & (c[i] | (a[i] | b[i]));
}

/* @ 0x800E3C64 */
/* Block bit of (x, y) in a line map; 1 (blocked) within 16 pixels of the border. */
u8 Bits_TestInner(u8 *bits, s32 x, s32 y) {
    s32 w = Board_GetWidth();
    s32 h = Board_GetHeight();

    if (x < 16 || x >= w - 16 || y < 16) {
        return 1;
    }
    if (y >= h - 16) {
        return 1;
    }
    return bits[BLOCK_BYTE(x, y, w)] & ovl2_D_800F9A2C[(x >> 3) & 7];
}

/* @ 0x800E3D28 */
/* Does a w*h box centred on (x, y) hit player p's line? Only while p is drawing:
 * the player's own position counts, otherwise the line map (0 near the border). */
u8 Player_HitTest(Player *p, s32 x, s32 y, s32 w, s32 h) {
    s32 d, bw, bh;

    if (p->pathLen == 0) {
        return 0;
    }
    d = p->x - x + (w >> 1);
    if (d < w && d >= 0) {
        d = p->y - y + (h >> 1);
        if (d < h && d >= 0) {
            return 1;
        }
    }
    bw = Board_GetWidth();
    bh = Board_GetHeight();
    if (x < 8 || x >= bw - 8 || y < 8) {
        return 0;
    }
    if (y >= bh - 8) {
        return 0;
    }
    return p->lineBits[BLOCK_BYTE(x, y, bw)] & ovl2_D_800F9A2C[(x >> 3) & 7];
}

/* @ 0x800E3E4C */
/* Is player p (not drawing) inside the w*h box centred on (x, y)? */
s32 Player_InBox(Player *p, s32 x, s32 y, s32 w, s32 h) {
    s32 d;

    if (p->pathLen != 0) {
        return 0;
    }
    d = p->x - x + (w >> 1);
    if (d < w && d >= 0) {
        d = p->y - y + (h >> 1);
        if (d < h) {
            return d >= 0;
        }
    }
    return 0;
}

/* @ 0x800E3EAC */
Player *Player_New(s32 id, s32 x, s32 y) {
    Player *p = Heap_Alloc(&gGameHeap, sizeof(Player));

    bzero(p, sizeof(Player));
    p->x = x;
    p->y = y;
    p->id = id;
    p->timeUp = 0;
    return p;
}

/* @ 0x800E3F28 */
/* Draw a white edge rectangle (x, y)-(x+w, y+h) and claim its inside (the starting
 * area). In attract mode (D_800C51D8) the rectangle is twice as wide. */
void Board_ClaimRect(s32 x, s32 y, s32 w, s32 h) {
    s32 i;

    if (D_800C51D8 != 0) {
        w *= 2;
    }
    for (i = x; i <= x + w; i++) {
        Board_SetEdge(i, y, 0xFFFF);
        Board_MarkBlock(i, y);
    }
    for (i = x; i <= x + w; i++) {
        Board_SetEdge(i, y + h, 0xFFFF);
        Board_MarkBlock(i, y + h);
    }
    for (i = y; i <= y + h; i++) {
        Board_SetEdge(x, i, 0xFFFF);
        Board_MarkBlock(x, i);
    }
    for (i = y; i <= y + h; i++) {
        Board_SetEdge(x + w, i, 0xFFFF);
        Board_MarkBlock(x + w, i);
    }
    Board_FloodFill(x + 1, y + 1);
}

/* ------------------------------------------------------------------ player lines */

/* @ 0x800E41B8 */
/* Follow the edge from the start of p's line, leaving it in direction dir, in steps
 * of 2 pixels until the line (cell 2) is reached again; records the steps in out.
 * At each point the three directions dir-2, dir, dir+2 (rotated: (dir+5)&7 .. ) are
 * tried and the first with an edge two pixels away (or line next to edge) is taken;
 * with exactly one candidate that one is taken. Quirks kept: each recorded step
 * stores the dx/dy of the previous direction but the new dir; with no or several
 * non-matching candidates it loops forever. */
s32 Player_Trace(Player *p, TraceStep *out, s32 dir) {
    struct { s32 dir, far, near; } cand[3];
    s32 x, y, count, n, i, d, near, far;

    x = p->path[0].x;
    y = p->path[0].y;
    out->x = x;
    out->y = y;
    out->dx = DIR_DX(dir);
    out->dir = dir;
    out->dy = DIR_DY(dir);
    count = 1;
    x += DIR_DX(dir) * 2;
    y += DIR_DY(dir) * 2;
    if (Board_GetCell(x, y) == 2) {
        return 1;
    }
    out++;
    do {
        do {
            n = 0;
            for (i = 0; i < 6; i += 2) {
                d = (((dir + 5) & 7) + i & 7) + 1;
                near = Board_GetCell(x + DIR_DX(d), y + DIR_DY(d));
                far = Board_GetCell(x + DIR_DX(d) * 2, y + DIR_DY(d) * 2);
                if (far >= 2 && near >= 2) {
                    cand[n].dir = d;
                    cand[n].far = far;
                    cand[n].near = near;
                    n++;
                }
            }
            for (i = 0; i < n; i++) {
                if (cand[i].far == 3 || (cand[i].far == 2 && cand[i].near == 3)) {
                    cand[0].dir = cand[i].dir;
                    cand[0].far = cand[i].far;
                    n = 1;
                    break;
                }
            }
        } while (n != 1);
        out->x = x;
        out->y = y;
        out->dx = DIR_DX(dir);
        out->dy = DIR_DY(dir);
        count++;
        dir = cand[0].dir;
        out->dir = dir;
        out++;
        x += DIR_DX(dir) * 2;
        y += DIR_DY(dir) * 2;
    } while (count < PLAYER_TRACE_MAX && cand[0].far != 2);
    return count;
}

/* the original reads path entries before the start of the array for short lines */
#define PATH_DIR_AT(p, i) (*(s16 *)((u8 *)(p) + 0x22 + (i) * 10))

/* @ 0x800E445C */
/* Is (px, py) inside the closed shape formed by p's line (every other point) and
 * the shorter of its two traces? Counts crossings of a ray towards +x; 0 when px is
 * outside the shape's x range. */
s32 Player_IsInside(Player *p, s32 px, s32 py) {
    s32 minx = 1000, maxx = 0, cross = 0;
    s32 i, x, y, d, pd, n, last, k;
    s32 t = p->traceLen[1] < p->traceLen[0];
    TraceStep *tr = p->trace[t];
    PathPoint *q;

    for (i = 0; i < p->pathLen; i += 2) {
        x = p->path[i].x;
        y = p->path[i].y;
        if (x < minx) minx = x;
        if (maxx < x) maxx = x;
        d = p->path[i].dir;
        if (d == 0 || x < px) {
            continue;
        }
        if (y == py) {
            if (i >= 2) {
                pd = PATH_DIR_AT(p, i - 2);
                if (pd == 0) {
                    pd = PATH_DIR_AT(p, i - 4);
                }
            } else {
                pd = (tr[0].dir + 4) & 7;
            }
            cross += ovl2_D_800F9988[p->path[i].dir][pd] != 0;
        } else if (d == 5) {
            cross += (y + 1 == py);
        } else if (d == 1) {
            cross += (y - 1 == py);
        }
    }

    x = tr[0].x;
    y = tr[0].y;
    if (x < minx) minx = x;
    if (maxx < x) maxx = x;
    if (x >= px) {
        if (tr[0].dir == 5) {
            cross += (y + 1 == py);
        } else if (tr[0].dir == 1) {
            cross += (y - 1 == py);
        }
    }

    n = p->traceLen[t];
    for (i = 1; i < n; i++) {
        x = tr[i].x;
        y = tr[i].y;
        if (x < minx) minx = x;
        if (maxx < x) maxx = x;
        if (x < px) {
            continue;
        }
        if (y == py) {
            cross += ovl2_D_800F9988[tr[i].dir][tr[i - 1].dir] != 0;
        } else if (tr[i].dir == 5) {
            cross += (y + 1 == py);
        } else if (tr[i].dir == 1) {
            cross += (y - 1 == py);
        }
    }
    last = (n > 1) ? n - 1 : 0;

    /* the end of the trace, joined back to the last point of the line */
    x = tr[last].x + tr[last].dx * 2;
    y = tr[last].y + tr[last].dy * 2;
    if (x < minx) minx = x;
    if (maxx < x) maxx = x;
    if (x >= px) {
        k = p->pathLen - 2;
        if (p->path[k].dir == 0) {
            k = p->pathLen - 4;
        }
        q = (PathPoint *)((u8 *)p->path + k * 10);
        d = ovl2_D_800F9974[(q->y - y) / 2 + 1][(q->x - x) / 2 + 1];
        if (y == py) {
            cross += ovl2_D_800F9988[d][tr[last].dir] != 0;
        }
    }
    if (px < minx || maxx < px) {
        cross = 0;
    }
    return cross & 1;
}

/* @ 0x800E48B0 */
/* Turn p's line into edge: both pixels of every step become white edge cells and
 * are removed from the line map. */
void Player_CommitPath(Player *p) {
    s32 i, x, y;
    PathPoint *e = p->path;

    for (i = 0; i < p->pathLen; i++, e++) {
        Board_SetEdge(e->x, e->y, 0xFFFF);
        Board_MarkBlock(e->x, e->y);
        x = e->x;
        y = e->y;
        p->lineBits[BLOCK_BYTE(x, y, Board_GetWidth())] &= ovl2_D_800F9A34[(x >> 3) & 7];
        Board_SetEdge(e->x + e->dx, e->y + e->dy, 0xFFFF);
        Board_MarkBlock(e->x + e->dx, e->y + e->dy);
        x = e->x + e->dx;
        y = e->y + e->dy;
        p->lineBits[BLOCK_BYTE(x, y, Board_GetWidth())] &= ovl2_D_800F9A34[(x >> 3) & 7];
    }
    p->pathLen = 0;
}

/* @ 0x800E4A30 */
/* p's line has reached an edge: trace the edge both ways from the line's start,
 * claim the side of the line that does not contain the monster, start the claim
 * popup and turn the line into edge.
 * Quirk: if fewer than two edge directions are found next to the line's start the
 * original passes uninitialised stack values to Player_Trace (here: 0). */
void Player_CloseArea(Player *p) {
    s32 dirs[2] = { 0, 0 };
    s32 n = 0, d, sd, a, b;
    s16 ax, bx, ay, by;
    s32 x0, y0;
    f32 before, after;

    p->touched = 0;
    for (d = 1; d < 8; d += 2) {
        if (Board_GetCell(p->path[0].x + DIR_DX(d), p->path[0].y + DIR_DY(d)) == 3) {
            if (n >= 2) {
                break;
            }
            dirs[n++] = d;
        }
    }
    p->traceLen[0] = Player_Trace(p, p->trace[0], dirs[0]);
    p->traceLen[1] = Player_Trace(p, p->trace[1], dirs[1]);

    /* the two pixels beside the line's first step */
    sd = p->path[0].dir;
    a = ((sd + 1) & 7) + 1;
    b = ((sd + 5) & 7) + 1;
    x0 = p->path[0].x + DIR_DX(sd);
    y0 = p->path[0].y + DIR_DY(sd);
    ax = (u16)DIR_DX(a) + x0;
    bx = (u16)DIR_DX(b) + x0;
    ay = (u16)DIR_DY(a) + y0;
    by = (u16)DIR_DY(b) + y0;

    before = Board_GetPercent();
    if (Player_IsInside(p, ax, ay) != 0) {
        if (Player_IsInside(p, ovl2_D_800FD2C4, ovl2_D_800FD2B4) == 0) {
            Board_FloodFill(ax, ay);
        } else {
            Board_FloodFill(bx, by);
        }
    } else if (Player_IsInside(p, ovl2_D_800FD2C4, ovl2_D_800FD2B4) != 0) {
        Board_FloodFill(ax, ay);
    } else {
        Board_FloodFill(bx, by);
    }
    after = Board_GetPercent();

    ovl2_D_800FD428 = 60;
    ovl2_D_800FD400 = 1;
    ovl2_D_800FD2D0 = (f32)(p->x - ovl2_D_800FD2B8);
    ovl2_D_800FD2DC = (f32)(p->y - ovl2_D_800FD2C8);
    ovl2_D_800FD404 = (s32)((f64)(after - before) + ovl2_D_800FAF40);
    Player_CommitPath(p);
}

/* @ 0x800E4CFC */
/* Move p one pixel by (dx, dy) while drawing: record the step, remember the pixel
 * under the new position, draw it as line in the player's colour, set its block
 * in the line map. */
void Player_Step(Player *p, s32 dx, s32 dy) {
    s32 x = p->x, y = p->y;
    s32 nx = x + dx, ny = y + dy;
    u16 col;

    p->path[p->pathLen].dx = dx;
    p->path[p->pathLen].dy = dy;
    p->path[p->pathLen].x = x;
    p->path[p->pathLen].y = y;
    p->path[p->pathLen].dir = ovl2_D_800F9974[dy + 1][dx + 1];
    p->path[p->pathLen].saved = Board_GetPixel(nx, ny);
    p->pathLen++;
    p->touched |= (Board_GetCell(nx, ny) == 3);
    if (p->id == 0) {
        col = 0xFD29;
    } else {
        col = 0xA53F;
    }
    Board_SetLine(nx, ny, col);
    nx = x + dx;
    ny = y + dy;
    p->lineBits[BLOCK_BYTE(nx, ny, Board_GetWidth())] |= ovl2_D_800F9A2C[(nx >> 3) & 7];
    p->x = nx;
    p->y = ny;
}

/* ------------------------------------------------------------------ HUD */

/* @ 0x800E00B8 */
/* Show the stage score: D_800C172C / 24 (max 99) in big digits, plus its labels.
 * Returns the shown value. */
s32 Hud_ShowStageScore(void) {
    s32 v = D_800C172C / 24;

    if (v >= 100) {
        v = 99;
    }
    if (v / 10 != 0) {
        Obj_SetData(ovl2_D_800FB180[0], ovl2_D_800FB410[v / 10]);
    } else {
        Obj_Hide(ovl2_D_800FB180[0]);
    }
    Obj_SetData(ovl2_D_800FB180[1], ovl2_D_800FB410[v % 10]);
    Obj_SetData(ovl2_D_800FB180[2], ovl2_D_800FB438);
    Obj_SetPos(ovl2_D_800FB180[2], 0xBE, 0x64, 0);
    Obj_SetData(ovl2_D_800FB180[3], ovl2_D_800FB454);
    Obj_SetPos(ovl2_D_800FB180[3], 0xA0, 0x3C, 0);
    Obj_SetPos(ovl2_D_800FB180[0], 0x87, 0x64, 0);
    Obj_SetPos(ovl2_D_800FB180[1], 0xA0, 0x64, 0);
    return v;
}

/* @ 0x800E0230 */
/* Count the timer down (frames) and show it in seconds: three big digits from 30 s
 * up, two small ones below. At exactly 30 s the "hurry up" task is started. */
void Hud_UpdateTimer(void) {
    s32 t, sec, ones, tens, hundreds;

    if (ovl2_D_800FD424 != 0) {
        ovl2_D_800FD424--;
    }
    if (ovl2_D_800FD424 == 1800) {
        func_8000A76C(ovl2_func_800F4210, 0x17, 8, 0, &ovl2_D_800FD4F8);
    }
    t = ovl2_D_800FD424;
    sec = t / 60;
    ones = sec % 10;
    tens = (sec / 10) % 10;
    hundreds = (sec / 10) / 10;
    if (t < 1800) {
        Obj_SetData(ovl2_D_800FB13C, ovl2_D_800FB3E0[tens]);
        Obj_SetPos(ovl2_D_800FB13C, 0x9A, 0x1C, 0);
        Obj_SetData(ovl2_D_800FB140, ovl2_D_800FB3E0[ones]);
        Obj_SetPos(ovl2_D_800FB140, 0xAE, 0x1C, 0);
        Obj_Hide(ovl2_D_800FB138);
        Obj_Hide(ovl2_D_800FB144);
    } else {
        Obj_SetData(ovl2_D_800FB13C, ovl2_D_800FB3B8[hundreds]);
        Obj_SetData(ovl2_D_800FB140, ovl2_D_800FB3B8[tens]);
        Obj_SetData(ovl2_D_800FB144, ovl2_D_800FB3B8[ones]);
        Obj_SetPos(ovl2_D_800FB13C, 0x98, 0x1E, 0);
        Obj_SetPos(ovl2_D_800FB140, 0xA4, 0x1E, 0);
        Obj_SetPos(ovl2_D_800FB144, 0xB0, 0x1E, 0);
        Obj_Show(ovl2_D_800FB138);
    }
}

/* @ 0x800E0460 */
/* Credits display: the number of credits (max 99), or "a/b" coins when there are
 * no credits and a credit costs two or more coins. */
void Hud_UpdateCredits(void) {
    s32 n = D_800C30C8;

    if (n >= 100) {
        n = 99;
    }
    if (n == 0 && D_800B056C >= 2) {
        s32 v;
        if (D_800B4A30 == 0) {
            Obj_SetData(ovl2_D_800FB1C4, ovl2_D_800FB480[D_800C52F4]);
            Obj_SetData(ovl2_D_800FB1C8, ovl2_D_800FB4C4);
            v = D_800B056C;
        } else {
            Obj_SetData(ovl2_D_800FB1C4, ovl2_D_800FB480[D_800C52F4]);
            Obj_SetData(ovl2_D_800FB1C8, ovl2_D_800FB4C4);
            v = D_800AF264;
        }
        Obj_SetData(ovl2_D_800FB1CC, ovl2_D_800FB480[v]);
        return;
    }
    if (n / 10 != 0) {
        Obj_SetData(ovl2_D_800FB1C4, ovl2_D_800FB480[n / 10]);
    } else {
        Obj_Hide(ovl2_D_800FB1C4);
    }
    Obj_SetData(ovl2_D_800FB1C8, ovl2_D_800FB480[n % 10]);
    Obj_Hide(ovl2_D_800FB1CC);
}

/* @ 0x800E0604 */
/* Blinking "press start" / "insert coin" for players not in the game; a start
 * press with a credit joins (lives = 4 - DIP bits 2-3). */
void Hud_UpdateJoin(s32 frame) {
    if (D_800B04D4 == 0) {
        if (D_800C30C8 != 0) {
            Obj_SetData(ovl2_D_800FB1D0, (frame & 0x40) ? ovl2_D_800FB4B0 : ovl2_D_800FB4B4);
            if (D_800C0D48 & 0x100) {
                D_800B04D4 = 4 - ((D_800C52A0 >> 2) & 3);
                D_800C30C8--;
            }
        } else {
            Obj_SetData(ovl2_D_800FB1D0, (frame & 0x40) ? ovl2_D_800FB4AC : ovl2_D_800FB4B4);
        }
        if (frame & 0x20) {
            Obj_Show(ovl2_D_800FB1D0);
        } else {
            Obj_Hide(ovl2_D_800FB1D0);
        }
    } else {
        Obj_Hide(ovl2_D_800FB1D0);
    }

    if (D_800B04D8 == 0) {
        if (D_800C30C8 != 0) {
            Obj_SetData(ovl2_D_800FB1D4, (frame & 0x40) ? ovl2_D_800FB4BC : ovl2_D_800FB4C0);
            if (D_800C0D4A & 0x100) {
                D_800B04D8 = 4 - ((D_800C52A0 >> 2) & 3);
                D_800C30C8--;
            }
        } else {
            Obj_SetData(ovl2_D_800FB1D4, (frame & 0x40) ? ovl2_D_800FB4B8 : ovl2_D_800FB4C0);
        }
        if (frame & 0x20) {
            Obj_Show(ovl2_D_800FB1D4);
        } else {
            Obj_Hide(ovl2_D_800FB1D4);
        }
    } else {
        Obj_Hide(ovl2_D_800FB1D4);
    }
}

/* @ 0x800E0820 */
/* "+NN%" popup after a claim (ovl2_D_800FD428 frames), drifting up half a pixel per
 * frame, with a rating sprite. On its first frame a random voice is played and the
 * timer gets 1 second per percent. */
void Hud_UpdateGainPopup(void) {
    s32 pts, x, n, tens, idx;
    Obj **o = ovl2_D_800FB1E8;

    if (ovl2_D_800FD428 == 0) {
        Obj_Hide(o[0]);
        Obj_Hide(o[1]);
        Obj_Hide(o[2]);
        Obj_Hide(o[3]);
        Obj_Hide(o[4]);
        return;
    }
    pts = ovl2_D_800FD404;
    x = (s32)ovl2_D_800FD2D0;
    n = 1;
    Obj_SetData(o[0], ovl2_D_800FB4F0);
    Obj_SetPos(o[0], x, (s32)ovl2_D_800FD2DC, 0);
    tens = pts / 10;
    x += 10;
    if (tens != 0) {
        Obj_SetData(o[1], ovl2_D_800FB4C8[tens]);
        n = 2;
        Obj_SetPos(o[1], x, (s32)ovl2_D_800FD2DC, 0);
        x += 10;
    }
    Obj_SetData(o[n], ovl2_D_800FB4C8[pts % 10]);
    Obj_SetPos(o[n], x, (s32)ovl2_D_800FD2DC, 0);
    n++;
    x += 10;
    Obj_SetData(o[n], ovl2_D_800FB4F4);
    Obj_SetPos(o[n], x, (s32)ovl2_D_800FD2DC, 0);
    n++;
    for (; n < 4; n++) {
        Obj_Hide(o[n]);
    }
    if (ovl2_D_800FD400 != 0) {
        switch (Rand_Range(&ovl2_D_800F9900, 3)) {
            case 0: func_8001BA90(8); break;
            case 1: func_8001BA90(9); break;
            case 2: func_8001BA90(10); break;
        }
        ovl2_D_800FD400 = 0;
        ovl2_D_800FD424 += ovl2_D_800FD404 * 60;
    }
    if (ovl2_D_800FD404 < 2) {
        idx = 12;
    } else if (ovl2_D_800FD404 < 15) {
        idx = 13;
    } else if (ovl2_D_800FD404 < 25) {
        idx = 14;
    } else if (ovl2_D_800FD404 < 45) {
        idx = 15;
    } else {
        idx = 16;
    }
    Obj_SetData(o[4], ovl2_D_800FB4C8[idx]);
    Obj_SetPos(o[4], (s32)(ovl2_D_800FD2D0 + 16.0f), (s32)(ovl2_D_800FD2DC - 16.0f), 0);
    ovl2_D_800FD428--;
    ovl2_D_800FD2DC = (f32)((f64)ovl2_D_800FD2DC - ovl2_D_800FAF30);
}

/* ------------------------------------------------------------------ game task */

/* @ 0x800E0000 */
/* Overlay entry: start the game task (mode 0) or ovl2_func_800F45DC (mode 1) and
 * wait for it to finish; returns the low 16 bits of its result. */
s32 Game_Run(s32 *args) {
    s32 done = 0;

    Camera_Init();
    Light_Init(D_800C1730);
    D_800C51CC = 1;
    if (args[0] == 0) {
        func_8000A76C(Game_Main, 0x14, 8, &done, args[1]);
    } else if (args[0] == 1) {
        func_8000A76C(ovl2_func_800F45DC, 0x14, 8, &done, args[1]);
    }
    while (done == 0) {
        func_8000AEAC();
    }
    D_800C51CC = 1;
    return done & 0xFFFF;
}

/* lives display: all 4 icons of both players shown, then 5 - lives (0..4) hidden */
static void Game_ShowLives(s32 lives1, s32 lives2) {
    s32 i, n;

    for (i = 0; i < 4; i++) {
        Obj_Show(ovl2_D_800FB160[i]);
        Obj_Show(ovl2_D_800FB170[i]);
    }
    n = 5 - lives1;
    if (n < 0) n = 0;
    if (n >= 5) n = 4;
    for (i = 0; i < n; i++) {
        Obj_Hide(ovl2_D_800FB160[i]);
    }
    n = 5 - lives2;
    if (n < 0) n = 0;
    if (n >= 5) n = 4;
    for (i = 0; i < n; i++) {
        Obj_Hide(ovl2_D_800FB170[i]);
    }
}

/* stock display: n (max 3) full icons, the rest empty */
static void Game_ShowStock(s32 n) {
    s32 i = 0;

    if (n >= 4) {
        n = 3;
    }
    for (; i < n; i++) {
        Obj_SetData(ovl2_D_800FB154[i], ovl2_D_800FB408);
    }
    for (; i < 3; i++) {
        Obj_SetData(ovl2_D_800FB154[i], ovl2_D_800FB40C);
    }
}

static void Game_HidePopup(void) {
    Obj_Hide(ovl2_D_800FB1E8[0]);
    Obj_Hide(ovl2_D_800FB1E8[1]);
    Obj_Hide(ovl2_D_800FB1E8[2]);
    Obj_Hide(ovl2_D_800FB1E8[3]);
    Obj_Hide(ovl2_D_800FB1E8[4]);
}

static s32 iabs(s32 v) {
    return v < 0 ? -v : v;
}

/* after every yield: leave the game when asked to */
#define CHECK_ABORT()                                   \
    do {                                                \
        if (D_800C30C4 != 0) {                          \
            D_800C1674 = 1;                             \
            goto quit;                                  \
        }                                               \
        if (D_800AF260 != 0) {                          \
            goto quit;                                  \
        }                                               \
    } while (0)

/* @ 0x800E0BB8 */
/* The game task (one photo): decode the photo, set up the board, the players, the
 * monster and enemy tasks, then run the frame loop (HUD, camera) until the area
 * target is reached (stage clear, possibly the ending), the timer runs out (players
 * lose... continue), or everybody is out (continue countdown, then the ending
 * sequence). Writes result | 0x10000 to *result: 1 = next photo, 2 = girl done,
 * 0xFF = game finished.
 * Quirks kept: the result is uninitialised stack in the original when the game is
 * aborted early (here 0), and the random seed used for the stage clear sparkles
 * is an uninitialised local (here 0). */
void Game_Main(s32 *result, s32 param) {
    s32 w, h;                   /* photo size, written by the decoder */
    s32 res = 0;
    s32 seed = 0;
    s32 frame = 0;
    s32 i, n, pct, count, lives1, lives2, mode, dir, half;
    PhotoView *view;
    Obj *viewObj, *last;
    f32 camX, camY, tx, ty, dx, dy, d;
    void (*enemyFn)(void), (*bossFn)(void);
    u8 *enemyData, *bossData;
    Player *p1, *p2;
    u32 r;
#if defined(VD_PORT) && defined(VD_CHEATS)
    s32 cheatClear = 0;         /* Dreamcast test cheat used for this clear */
#endif

    ovl2_D_800FD2D8 = 0;
    ovl2_D_800FD424 = 7200;
    ovl2_D_800FD42C = Heap_Alloc(&gGameHeap, 0x9600);
    ovl2_D_800FD2BC = Heap_Alloc(&gGameHeap, 0x12C00);
    ovl2_D_800FD2D4 = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++) {
        ovl2_D_800FD2D4[i] = 0;
    }
    ovl2_D_800FD548 = Heap_Alloc(&gGameHeap, 0x96000);
    for (i = 0; i <= 0x4AFFF; i++) {
        ovl2_D_800FD548[i] = 0;
    }
    Jpeg64_Decode((ovl2_D_800F9880[D_800B0608 * 4 + D_800B04CC] & 0xFFFFFF) + 0x611F0,
                  ovl2_D_800FD2D4, 0, 0, &w, &h);
    Image_CopyRect(ovl2_D_800FD2D4, w, h, 0, 0, w, h, ovl2_D_800FD548, w, h, 0, 0);
    Image_Mosaic(ovl2_D_800FD548, w, h, 0, 0, w, h, 3, 0.5f);
    ovl2_D_800FD2C8 = 0;
    ovl2_D_800FD2B8 = 0;
    view = PhotoView_New(NULL, ovl2_D_800FD548, w, h, 0, 0);
    viewObj = Obj_NewDL(gSceneGroup0, view->setup_dl, view->dl, view->finish_dl);
    Obj_Show(viewObj);
    Board_Init(ovl2_D_800FD548, ovl2_D_800FD2D4, ovl2_D_800FD2BC, ovl2_D_800FD42C, w, h);
    camX = 0.0f;
    camY = camX;
    Scene_Build(ovl2_D_800F6450, ovl2_D_800F7E54);
    ovl2_func_800F1D6C();
    if (D_800C51D8 == 0) {
        p1 = Player_New(0, 0x40, 0x40);
        ovl2_D_800FD3F8 = p1;
        p2 = Player_New(1, 0x80, 0x40);
    } else {
        p1 = Player_New(0, 0x40, 0x40);
        ovl2_D_800FD3F8 = p1;
        p2 = Player_New(1, 0x80, 0x68);
    }
    ovl2_D_800FD3FC = p2;
    Board_ClaimRect(ovl2_D_800FD3F8->x, ovl2_D_800FD3F8->y, 0x50, 0x28);

    /* player tasks, monster parts, other tasks */
    ovl2_D_800FD2B0 = 0;
    ovl2_D_800FB640 = func_8000A76C(ovl2_func_800F10F0, 0x17, 0x18, &ovl2_D_800FD2B0,
                                    ovl2_D_800FB050, 16, 16, ovl2_D_800F922C, ovl2_D_800FD3F8);
    ovl2_D_800FD500 = 0;
    ovl2_D_800FB644 = func_8000A76C(ovl2_func_800F10F0, 0x17, 0x18, &ovl2_D_800FD500,
                                    ovl2_D_800FB068, 16, 16, ovl2_D_800F9318, ovl2_D_800FD3FC);
    ovl2_D_800FB64C = func_8000A76C(ovl2_func_800F4FC0, 0x17, 0x10, ovl2_D_800FB200,
                                    ovl2_D_800F93B4, 0, 0);
    ovl2_D_800FB650 = func_8000A76C(ovl2_func_800F4FC0, 0x17, 0x10, ovl2_D_800FB218,
                                    ovl2_D_800F93B4, 1, 0);
    ovl2_D_800FB654 = func_8000A76C(ovl2_func_800F5780, 0x17, 8, ovl2_D_800FB22C, ovl2_D_800F8E1C);
    ovl2_D_800FB658 = func_8000A76C(ovl2_func_800F5D50, 0x17, 8, ovl2_D_800FB230, ovl2_D_800F8E4C);

    /* one of three enemy / monster sets */
    r = Rand_Range(&D_800B04DC, 3);
    if (r == 1) {
        enemyFn = ovl2_func_800EA610;  enemyData = ovl2_D_800F97B0;
        bossFn = ovl2_func_800EEEEC;   bossData = ovl2_D_800F95D4;
    } else if (r == 0) {
        enemyFn = ovl2_func_800EAD80;  enemyData = ovl2_D_800F9874;
        bossFn = ovl2_func_800F00F4;   bossData = ovl2_D_800F9488;
    } else {
        enemyFn = ovl2_func_800E9DC0;  enemyData = ovl2_D_800F96DC;
        bossFn = ovl2_func_800EC0CC;   bossData = ovl2_D_800F9438;
    }
    for (i = 0; i < ovl2_D_800F6390[D_800B4AA0]; i++) {
        ovl2_D_800FD508[i] = 0;
        ovl2_D_800FB600[i] = func_8000A76C(enemyFn, 0x19, 0x10, &ovl2_D_800FD508[i],
                                           ovl2_D_800FB0E0[i], enemyData, 0);
    }
    ovl2_D_800FD2F0 = 0;
    ovl2_D_800FB648 = func_8000A76C(bossFn, 0x18, 0x18, &ovl2_D_800FD2F0, ovl2_D_800FB080,
                                    0xA0, 0xC8, bossData, 0);

round_start:
    func_8001BBC0(D_800B0608 + 6);
    Obj_Hide(ovl2_D_800FB1E4);

    /* ---- frame loop ---- */
    for (;;) {
        Hud_UpdateCredits();
        Hud_UpdateJoin(frame);
        Game_ShowLives(D_800B04D4, D_800B04D8);
        if (D_800B04D4 == 0 && D_800B04D8 == 0) {
            goto game_over;
        }
        Hud_UpdateTimer();
        if (ovl2_D_800FD424 == 0) {
            goto time_up;
        }
        pct = (s32)Board_GetPercent() + 3;
        if (pct >= 100) {
            pct = 99;
        }
        if (pct / 10 != 0) {
            Obj_SetData(ovl2_D_800FB148, ovl2_D_800FB390[pct / 10]);
        } else {
            Obj_Hide(ovl2_D_800FB148);
        }
        Obj_SetData(ovl2_D_800FB14C, ovl2_D_800FB390[pct % 10]);
#if defined(VD_PORT) && defined(VD_CHEATS)
        /* Dreamcast test cheat (not in the arcade game): holding L + R clears
         * the round as a 99% "perfect" clear, so the bonus key is awarded. */
        if (vd_cheat_stage_clear()) {
            cheatClear = 1;
            goto stage_clear;
        }
#endif
        if ((D_800C52A0 >> 5) & 1) {
            if (!(pct < 80)) goto stage_clear;
        } else {
            if (!(pct < 70)) goto stage_clear;
        }
        Game_ShowStock(D_800C3174);
        Hud_UpdateGainPopup();
        frame++;
        func_8000AEAC();
        CHECK_ABORT();
        if (D_800C51D8 != 0) {
            if (D_800C51D8 & 0x10) {
                Obj_Show(ovl2_D_800FB1E4);
            } else {
                Obj_Hide(ovl2_D_800FB1E4);
            }
            if (--D_800C51D8 == 0) {
                goto quit;
            }
            if (D_800C30C8 != 0) {
                goto quit;
            }
        }

        /* camera: follow the players (both: their middle) */
        p1 = ovl2_D_800FD3F8;
        p2 = ovl2_D_800FD3FC;
        if (p1->active != 0) {
            if (p2->active != 0) {
                tx = (f32)((p1->x + p2->x) / 2);
                ty = (f32)((p1->y + p2->y) / 2);
            } else {
                tx = (f32)p1->x;
                ty = (f32)p1->y;
            }
        } else if (p2->active != 0 || D_800B04D8 != 0) {
            tx = (f32)p2->x;
            ty = (f32)p2->y;
        } else {
            tx = (f32)p1->x;
            ty = (f32)p1->y;
        }
        if (iabs((s32)(tx - camX)) < 5) {
            camX = tx;
        }
        if (iabs((s32)(ty - camY)) < 5) {
            camY = ty;
        }
        dy = ((ty + 0.5f) - camY) / 4.0f;
        dx = ((tx + 0.5f) - camX) / 4.0f;
        camY = camY + dy;
        d = camY - (f32)ovl2_D_800FD2C8;
        camX = camX + dx;
        if (120.0f < d) {
            ovl2_D_800FD2C8 = (s32)(camY - 120.0f);
            if (!(ovl2_D_800FD2C8 < h - 240)) {
                ovl2_D_800FD2C8 = h - 240;
            }
        } else {
            ovl2_D_800FD2C8 = (s32)(camY - 120.0f);
            if (ovl2_D_800FD2C8 < 0) {
                ovl2_D_800FD2C8 = 0;
            }
        }
        d = camX - (f32)ovl2_D_800FD2B8;
        if (160.0f < d) {
            ovl2_D_800FD2B8 = (s32)(camX - 160.0f);
            if (!(ovl2_D_800FD2B8 < w - 320)) {
                ovl2_D_800FD2B8 = w - 320;
            }
        } else {
            ovl2_D_800FD2B8 = (s32)(camX - 160.0f);
            if (ovl2_D_800FD2B8 < 0) {
                ovl2_D_800FD2B8 = 0;
            }
        }
        PhotoView_SetScroll(view, ovl2_D_800FD2B8, ovl2_D_800FD2C8);
        if (ovl2_D_800FD2C0 != 0) {
            ovl2_D_800FD2C0--;
        }
    }

time_up:
    Game_HidePopup();
    func_8001BBC0(0);
    Obj_Show(ovl2_D_800FB19C);
    ovl2_D_800FD2CC = 1;
    if (D_800B04D4 != 0) {
        ovl2_D_800FD3F8->timeUp = 1;
    }
    if (D_800B04D8 != 0) {
        ovl2_D_800FD3FC->timeUp = 1;
    }
    frame = 179;
    do {
        func_8000AEAC();
        CHECK_ABORT();
        Hud_UpdateCredits();
        frame--;
    } while (frame != -1);
    lives1 = D_800B04D4;
    lives2 = D_800B04D8;
    ovl2_D_800FD424 = 7200;
    Game_ShowLives(lives1, lives2);
    Obj_Hide(ovl2_D_800FB19C);
    ovl2_D_800FD2CC = 0;
    if (D_800B04D4 != 0 || D_800B04D8 != 0) {
        goto round_start;
    }

game_over:
    /* continue countdown: 10 seconds, coins restart it, button 0x10 speeds it up */
    Game_HidePopup();
    count = 599;
    Obj_Show(ovl2_D_800FB190);
    Obj_Show(ovl2_D_800FB194);
    func_8001BBC0(4);
    for (;;) {
        func_8000AEAC();
        CHECK_ABORT();
        count--;
        Obj_SetData(ovl2_D_800FB194, ovl2_D_800FB458[count / 60]);
        frame++;
        if (D_800C53D8[0] != 0 || D_800C53D8[1] != 0) {
            count = 599;
            func_8001BBC0(4);
        }
        if (D_800C0D48 & 0x10) {
            count -= 60;
            if (count < 0) count = 0;
        }
        Hud_UpdateCredits();
        Hud_UpdateJoin(frame);
        if (D_800B04D4 != 0 || D_800B04D8 != 0) {
            ovl2_D_800FD424 = 7200;
            Obj_Hide(ovl2_D_800FB190);
            Obj_Hide(ovl2_D_800FB194);
            D_800B4AA0 -= 2;
            if (D_800B4AA0 < 0) {
                D_800B4AA0 = 0;
            }
            goto round_start;
        }
        if (count == 0) {
            break;
        }
    }
    Obj_Hide(ovl2_D_800FB190);
    last = ovl2_D_800FB194;
    goto ending;

stage_clear:
    func_8000D6B8();
    ovl2_D_800FD2D8 = 1;
    func_8000AEAC();
    func_8000AEAC();
    Game_HidePopup();
    Obj_Hide(ovl2_D_800FB1D0);
    Obj_Hide(ovl2_D_800FB1D4);
    {
        s32 bw = Board_GetWidth();
        s32 bh = Board_GetHeight();
        if ((bw == 320) & (bh == 240)) {
            mode = 0; dir = 0; count = 300;
        } else if (bw == 640) {
            mode = 1; dir = 1; count = 640;     /* pan across a wide photo */
        } else {
            mode = 2; dir = 1; count = 480;     /* pan across a tall photo */
        }
    }
    half = count / 2;
    Board_RevealAll();
    pct = (s32)Board_GetPercent() + 3;
    if (pct >= 100) {
        pct = 99;
    }
#if defined(VD_PORT) && defined(VD_CHEATS)
    if (cheatClear) {
        pct = 99;               /* test cheat: count it as a perfect clear */
        cheatClear = 0;
    }
#endif
    if (pct / 10 != 0) {
        Obj_SetData(ovl2_D_800FB180[0], ovl2_D_800FB410[pct / 10]);
    } else {
        Obj_Hide(ovl2_D_800FB180[0]);
    }
    Obj_SetData(ovl2_D_800FB180[1], ovl2_D_800FB410[pct % 10]);
    Obj_Show(ovl2_D_800FB180[2]);
    Obj_Show(ovl2_D_800FB180[3]);
    D_800C172C += pct;
    func_8001BBC0(pct < 99 ? 2 : 3);
    frame = 0;
    do {
        s32 sx, sy;
        func_8000AEAC();
        frame++;
        CHECK_ABORT();
        Hud_UpdateCredits();
        sx = Rand_Range(&seed, 360);
        sy = Rand_Range(&seed, 240);
        ovl2_func_800F2384(sx, sy);
        if ((frame & 7) == 0) {
            func_8001BA90(0x11);
        }
        switch (mode) {
            case 0:
                count--;
                break;
            case 1:
                if (dir != 0) {
                    ovl2_D_800FD2B8++;
                    dir &= -(ovl2_D_800FD2B8 < 319);
                } else {
                    ovl2_D_800FD2B8--;
                    if (ovl2_D_800FD2B8 <= 0) dir = 1;
                }
                count--;
                break;
            case 2:
                if (dir != 0) {
                    ovl2_D_800FD2C8++;
                    dir &= -(ovl2_D_800FD2C8 < 239);
                } else {
                    ovl2_D_800FD2C8--;
                    if (ovl2_D_800FD2C8 <= 0) dir = 1;
                }
                count--;
                break;
        }
        if (half != 0) {
            half--;
            if (half == 0) {
                Obj_Hide(ovl2_D_800FB180[0]);
                Obj_Hide(ovl2_D_800FB180[1]);
                Obj_Hide(ovl2_D_800FB180[2]);
                Obj_Hide(ovl2_D_800FB180[3]);
            }
        }
        PhotoView_SetScroll(view, ovl2_D_800FD2B8, ovl2_D_800FD2C8);
    } while (count > 0);

    if (pct >= 99) {
        /* perfect: three jingles, bonus effect, one more stock icon */
        func_8001BA90(2);
        Obj_Show(ovl2_D_800FB1D8);
        frame = 0;
        do {
            func_8000AEAC();
            frame++;
            Hud_UpdateCredits();
            CHECK_ABORT();
        } while (frame < 21);
        func_8001BA90(2);
        Obj_Show(ovl2_D_800FB1DC);
        frame = 0;
        do {
            func_8000AEAC();
            frame++;
            Hud_UpdateCredits();
            CHECK_ABORT();
        } while (frame < 21);
        func_8001BA90(2);
        Obj_Show(ovl2_D_800FB1E0);
        frame = 0;
        do {
            func_8000AEAC();
            frame++;
            Hud_UpdateCredits();
            CHECK_ABORT();
        } while (frame < 41);
        Obj_Hide(ovl2_D_800FB1E0);
        frame = 0;
        ovl2_func_800F2580(0xA0, 0x94, 0xEA, 0x14);
        do {
            func_8000AEAC();
            frame++;
            Hud_UpdateCredits();
            CHECK_ABORT();
        } while (frame < 61);
        if (D_800C3174 < 3) {
            D_800C3174++;
            Game_ShowStock(D_800C3174);
        }
        func_8001BA90(1);
        frame = 0;
        do {
            func_8000AEAC();
            frame++;
            Hud_UpdateCredits();
            CHECK_ABORT();
        } while (frame < 61);
        Obj_Hide(ovl2_D_800FB1D8);
        Obj_Hide(ovl2_D_800FB1DC);
        Obj_Hide(ovl2_D_800FB1E0);
    }

    /* next photo; after the third photo the girl is done */
    D_800B04CC++;
    if (D_800B04CC >= 3) {
        res = 2;
        D_800C178E[D_800B0608] = 1;
    } else {
        res = 1;
    }
    n = 0;
    for (i = 0; i < 8; i++) {
        n += D_800C178E[i] != 0;
    }
    if (n != 8) {
        goto quit;
    }

    /* all eight girls done: final texts and score */
    func_8001BA90(0x18);
    func_8001BBC0(0xE);
    Obj_SetData(ovl2_D_800FB180[0], ovl2_D_800FB444);
    frame = 0;
    Obj_SetPos(ovl2_D_800FB180[0], 0xA0, 0x50, 0);
    for (;;) {
        func_8000AEAC();
        frame++;
        CHECK_ABORT();
        if (frame >= 21) break;
        Hud_UpdateCredits();
        ovl2_func_800F200C();
    }
    Obj_SetData(ovl2_D_800FB180[1], ovl2_D_800FB448);
    frame = 0;
    Obj_SetPos(ovl2_D_800FB180[1], 0xA0, 0x6E, 0);
    for (;;) {
        func_8000AEAC();
        frame++;
        CHECK_ABORT();
        if (frame >= 21) break;
        Hud_UpdateCredits();
        ovl2_func_800F200C();
    }
    Obj_SetData(ovl2_D_800FB180[2], ovl2_D_800FB44C);
    frame = 0;
    Obj_SetPos(ovl2_D_800FB180[2], 0xA0, 0x8C, 0);
    for (;;) {
        func_8000AEAC();
        frame++;
        CHECK_ABORT();
        if (frame >= 21) break;
        Hud_UpdateCredits();
        ovl2_func_800F200C();
    }
    Obj_SetData(ovl2_D_800FB180[3], ovl2_D_800FB450);
    frame = 0;
    Obj_SetPos(ovl2_D_800FB180[3], 0xA0, 0xAA, 0);
    for (;;) {
        func_8000AEAC();
        frame++;
        CHECK_ABORT();
        if (frame >= 241) break;
        Hud_UpdateCredits();
        ovl2_func_800F200C();
    }
    Obj_Hide(ovl2_D_800FB180[0]);
    frame = 0;
    Obj_Hide(ovl2_D_800FB180[1]);
    res = 0xFF;
    Obj_Hide(ovl2_D_800FB180[2]);
    Obj_Hide(ovl2_D_800FB180[3]);
    func_8000AEAC();
    Hud_ShowStageScore();
    do {
        func_8000AEAC();
        frame++;
        CHECK_ABORT();
        Hud_UpdateCredits();
        ovl2_func_800F200C();
    } while (frame < 241);
    Obj_Hide(ovl2_D_800FB180[0]);
    Obj_Hide(ovl2_D_800FB180[1]);
    Obj_Hide(ovl2_D_800FB180[2]);
    last = ovl2_D_800FB180[3];

ending:
    /* the eight girl icons glide to their places */
    Obj_Hide(last);
    D_800B04D8 = 0;
    D_800B04D4 = 0;
    for (i = 0; i < 8; i++) {
        Obj_SetPos(ovl2_D_800FB1A0[i], (s32)ovl2_D_800F63D0[i][0], (s32)ovl2_D_800F63D0[i][1], 0);
        Obj_Show(ovl2_D_800FB1A0[i]);
    }
    func_8001BBC0(5);
    frame = 179;
    do {
        func_8000AEAC();
        Hud_UpdateCredits();
        CHECK_ABORT();
        for (i = 0; i < 8; i++) {
            ovl2_D_800F63D0[i][0] = ovl2_D_800F63D0[i][0] +
                                    (ovl2_D_800F6410[i][0] - ovl2_D_800F63D0[i][0]) / 8.0f;
            ovl2_D_800F63D0[i][1] = ovl2_D_800F63D0[i][1] +
                                    (ovl2_D_800F6410[i][1] - ovl2_D_800F63D0[i][1]) / 8.0f;
            Obj_SetPos(ovl2_D_800FB1A0[i], (s32)ovl2_D_800F63D0[i][0],
                       (s32)ovl2_D_800F63D0[i][1], 0);
        }
        frame--;
    } while (frame != -1);
    res = 0xFF;

quit:
    /* fade out with the mosaic effect and free everything */
    ovl2_D_800FD2D8 = 1;
    func_8000AEAC();
    func_8000AEAC();
    for (i = 1; i < 7; i++) {
        Image_Mosaic(ovl2_D_800FD548, w, h, 0, 0, w, h, i, 1.0f);
        func_8000AEAC();
    }
    Obj_Kill(viewObj);
    Scene_Destroy(ovl2_D_800F6450, ovl2_D_800F7E54);
    func_8000AEAC();
    func_8000AEAC();
    PhotoView_Delete(view);
    Heap_Free(&gGameHeap, ovl2_D_800FD548);
    Heap_Free(&gGameHeap, ovl2_D_800FD2D4);
    Heap_Free(&gGameHeap, ovl2_D_800FD2BC);
    Heap_Free(&gGameHeap, ovl2_D_800FD42C);
    Heap_Free(&gGameHeap, ovl2_D_800FD3F8);
    Heap_Free(&gGameHeap, ovl2_D_800FD3FC);
    *result = res | 0x10000;
}
