/* Vivid Dolls - overlay 2 (the game itself), part A: RAM 0x800E0000-0x800E4EDC.
 *
 * Vivid Dolls is a "Gals Panic"/Qix style game: the player moves a marker around
 * the edge of a 320x240 (or 640x240 / 320x480) photo, draws lines into the
 * unclaimed area and, when a line is closed, the side WITHOUT the monster is
 * claimed and the photo is revealed there. 70% (80% on a DIP setting) clears
 * the stage.
 *
 * This file covers:
 *  - Game_Run (overlay entry, called by Overlay_Load / Main_GameLoop) and the
 *    big game task Game_Main (stage setup, frame loop, time up, game over, stage
 *    clear, ending, teardown)
 *  - the HUD helpers (timer, credits, "press start", claim popup, stage score)
 *  - the board: a 2-bit-per-pixel cell map, the displayed image, the hidden
 *    photo, a 1-bit-per-8x8-block "dirty" map, and a scanline flood fill
 *  - per-player line drawing and the outline tracing / point-in-polygon test
 *    used to decide which side of a closed line gets claimed.
 *
 * Board cell values (2 bits, 4 cells per byte, cell i of a byte in bits 2i..2i+1):
 *   0 = unclaimed, 1 = claimed (photo shown), 2 = line being drawn, 3 = edge
 *   Board_GetCell returns 4 outside the board.
 */
#ifndef VD_OVL2_A_H
#define VD_OVL2_A_H

#include "vd_common.h"

/* One step of a player's line (10 bytes). */
typedef struct PathPoint {
    s8  dx, dy;                 /* 0x00 step taken from (x, y) */
    s16 dir;                    /* 0x02 direction 1..8 of that step (0 = none) */
    s16 x, y;                   /* 0x04 position before the step */
    u16 saved;                  /* 0x08 image pixel that was under the new position */
} PathPoint;

/* One step of an outline trace (8 bytes). Steps are 2 pixels long. */
typedef struct TraceStep {
    s8  dx, dy;                 /* 0x00 */
    s16 dir;                    /* 0x02 */
    s16 x, y;                   /* 0x04 */
} TraceStep;

#define PLAYER_PATH_MAX  0x2000
#define PLAYER_TRACE_MAX 0x2000

/* Per-player state, one 0x3429C-byte heap block (ovl2_D_800FD3F8 / ovl2_D_800FD3FC). */
typedef struct Player {
    s32 id;                     /* 0x00 0 or 1 (line colour) */
    s32 x, y;                   /* 0x04 current position */
    s32 unk0C;
    s32 touched;                /* 0x10 set when the line touched an edge cell */
    s32 timeUp;                 /* 0x14 set to 1 when the timer runs out */
    s32 active;                 /* 0x18 non-zero while playing (camera follows) */
    s32 pathLen;                /* 0x1C number of PathPoints (0 = not drawing) */
    PathPoint path[PLAYER_PATH_MAX];            /* 0x00020 */
    TraceStep trace[2][PLAYER_TRACE_MAX];       /* 0x14020 / 0x24020 */
    u16 traceLen[2];                            /* 0x34020 / 0x34022 */
    u8  lineBits[0x278];                        /* 0x34024 1 bit per 8x8 block under the line */
} Player;                       /* 0x3429C bytes */

/* Overlay entry: args[0] = mode (0 = game, 1 = ovl2_func_800F45DC), args[1] = parameter. */
s32  Game_Run(s32 *args);                                               /* 0x800E0000 */
s32  Hud_ShowStageScore(void);                                          /* 0x800E00B8 */
void Hud_UpdateTimer(void);                                             /* 0x800E0230 */
void Hud_UpdateCredits(void);                                           /* 0x800E0460 */
void Hud_UpdateJoin(s32 frame);                                         /* 0x800E0604 */
void Hud_UpdateGainPopup(void);                                         /* 0x800E0820 */
void Game_Main(s32 *result, s32 param);                                 /* 0x800E0BB8 */

void Board_Init(u16 *image, u16 *photo, u8 *cells, u8 *blocks, s32 w, s32 h); /* 0x800E2910 */
s32  Board_GetWidth(void);                                              /* 0x800E29C4 */
s32  Board_GetHeight(void);                                             /* 0x800E29D0 */
f32  Board_GetPercent(void);                                            /* 0x800E29DC */
void Board_FloodFill(s32 x, s32 y);                                     /* 0x800E2A24 */
void Board_MarkBlock(s32 x, s32 y);                                     /* 0x800E2A98 */
u8   Board_TestBlock(s32 x, s32 y);                                     /* 0x800E2AE8 */
u8   Board_GetBlockByte(s32 x, s32 y);                                  /* 0x800E2B34 */
void Board_SetEdge(s32 x, s32 y, u16 color);                            /* 0x800E2B6C */
void Board_SetEdgeCell(s32 x, s32 y);                                   /* 0x800E2BEC */
void Board_SetLine(s32 x, s32 y, u16 color);                            /* 0x800E2C40 */
void Board_Reveal(s32 x, s32 y);                                        /* 0x800E2CC0 */
void Board_Clear(s32 x, s32 y, u16 color);                              /* 0x800E2D54 */
void Board_SetPixel(s32 x, s32 y, u16 color);                           /* 0x800E2DC4 */
u16  Board_GetPixel(s32 x, s32 y);                                      /* 0x800E2DF4 */
s32  Board_GetCell(s32 x, s32 y);                                       /* 0x800E2E24 */
void Board_RevealAll(void);                                             /* 0x800E2EBC */
void Board_ShowLineBlocks(void);                                        /* 0x800E2F44 */
void Fill_Scan(s32 dx, s32 x, s32 y, s32 upBlocked, s32 downBlocked);   /* 0x800E3010 */
void Fill_Span(s32 x, s32 y);                                           /* 0x800E3718 */
void Fill_Push(s32 x, s32 y);                                           /* 0x800E3A44 */
void Fill_Pop(void);                                                    /* 0x800E3A78 */
s32  Fill_TopX(void);                                                   /* 0x800E3A90 */
s32  Fill_TopY(void);                                                   /* 0x800E3AAC */
void Board_FillPixel(s32 x, s32 y);                                     /* 0x800E3AC8 */
u8   Bits_TestAny3(u8 *a, u8 *b, u8 *c, s32 x, s32 y);                  /* 0x800E3BC0 */
u8   Bits_TestInner(u8 *bits, s32 x, s32 y);                            /* 0x800E3C64 */
u8   Player_HitTest(Player *p, s32 x, s32 y, s32 w, s32 h);             /* 0x800E3D28 */
s32  Player_InBox(Player *p, s32 x, s32 y, s32 w, s32 h);               /* 0x800E3E4C */
Player *Player_New(s32 id, s32 x, s32 y);                               /* 0x800E3EAC */
void Board_ClaimRect(s32 x, s32 y, s32 w, s32 h);                       /* 0x800E3F28 */
s32  Player_Trace(Player *p, TraceStep *out, s32 dir);                  /* 0x800E41B8 */
s32  Player_IsInside(Player *p, s32 px, s32 py);                        /* 0x800E445C */
void Player_CommitPath(Player *p);                                      /* 0x800E48B0 */
void Player_CloseArea(Player *p);                                       /* 0x800E4A30 */
void Player_Step(Player *p, s32 dx, s32 dy);                            /* 0x800E4CFC */

#endif
