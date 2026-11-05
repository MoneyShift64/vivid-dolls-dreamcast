/* Vivid Dolls - overlay 0: operator test / service mode (ROM 0x3ED30-0x43AD0,
 * loaded at 0x800E0000, no BSS).
 *
 * Main_GameLoop runs overlay 0 with mode 0 when the test switch is on (or DIP
 * bit 7 is set) and with mode 1 when a coin jam was detected (docs/notes/mainloop.md).
 *
 *   mode 0: TEST MODE MENU, then the chosen screen, then back to the menu,
 *           for ever (the menu can never return the 0xFF "leave" code, so the
 *           operator leaves test mode by switching the machine off / on).
 *   mode 1: "COIN ERROR" for 300 frames, then the coin jam flags are cleared.
 *
 * Every screen is a task (Task_Create(fn, 0x14, 8, &done, 0)) that builds
 * its labels as screen-space TextObjs (Text_Create + Text_PlaceScreen on a
 * new SCENE_DL node linked at the end of gSceneGroup1), runs one frame per
 * Task_Yield until the test switch is pressed (D_800AF220), deletes the
 * nodes, waits two frames, deletes the texts, and writes 0x10000 | result to
 * *done.
 *
 * Label colours: Text_Create(NULL, s, 0xFFFF, 0xE739, 1, 0x4211) (white to
 * light grey); highlighted = gradient 0xFFC1 (yellow) / 0xF801 (red) /
 * 0xFA11 (menu cursor) / 0x07FF (stick type lines).
 */
#ifndef VD_OVL0_TEST_H
#define VD_OVL0_TEST_H

#include "vd_common.h"

/* argument block passed by Main_GameLoop: args[0] = mode (0 test menu, 1 coin error) */
s32  Test_Run(s32 *args);              /* 0x800E0000 overlay entry; returns done & 0xFFFF */
void Test_ScreenTest(s32 *done);       /* 0x800E029C test pattern sprite (ROM asset 0) */
void Test_InputTest(s32 *done);        /* 0x800E03F0 switches, sticks, coins, DIP switches */
void Test_SoundTest(s32 *done);        /* 0x800E33C8 plays music 7 */
void Test_SystemCheck(s32 *done);      /* 0x800E3588 version, SDRAM and EEPROM test */
void Test_OutputTest(s32 *done);       /* 0x800E3A48 lockout coils / coin counters */
void Test_Menu(s32 *done);             /* 0x800E40D0 menu; *done = 0x10000 | choice (0..4) */
void Test_CoinError(s32 *done);        /* 0x800E4988 "COIN ERROR" for 300 frames */

#endif
