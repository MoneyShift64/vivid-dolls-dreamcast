# ovl0: overlay 0, operator test / service mode (0x800E0000-0x800E4AB0)

Overlay 0 (ROM 0x3ED30-0x43AD0, loaded at 0x800E0000, no BSS; read-only data
0x800E4AB0-0x800E4DA0: jump table + strings) is the arcade test mode.

Source: `src/game/ovl0_test.c` / `ovl0_test.h`. Test: `python3 tools/difftest/test_ovl0.py`
(about 80 s; `TRIALS=n` to change the counts, or give function names to run only those).
All 8 functions are done and pass (300 trials for the short screens, 100-150 for
the long-running ones, 40 for the system check because of its 2 x 1M-word SDRAM loop).

How the test works: every callee is a Python stub that logs its arguments
(Text_Create / Text_SetString strings by content, because the C uses its own
string literals; C task addresses translated to the original ones). The libultra
sprintf (func_80025DC0) runs for real. Task_Yield randomises the input globals
each frame (cabinet buttons, N64 pad buttons/sticks, coins, switches, DIPs,
pressed/held words) and presses the test switch (D_800AF220) after a random number
of frames. Scene_NewNode hands out zeroed nodes in test memory and gSceneGroup1 is
a fake group, so the node linking is compared. The SDRAM test area at 0x01000000 is
mapped in the emulator; SDRAM faults are simulated with a memory-read hook, EEPROM
faults by the Eeprom_Read stub. The test compares the call log, return value
(Test_Run), main BSS, the overlay area, test memory and an SDRAM hash.

No photos are involved: the screen test shows ROM asset 0 through
Obj_NewSpritePlain (a sprite, stubbed in the test).

## Functions done

| Address | Name | What |
|---|---|---|
| 0x800E0000 | Test_Run | overlay entry: outputs off (gIoOutputMask = 0, D_800C178C = 0), Camera_Init, Light_Init, gRetracesPerFrame = 1; mode 0: menu task, then the chosen screen (jump table 0x800E4AB0: 0 system check, 1 screen, 2 input, 3 output, 4 sound; other values back to the menu), loops for ever (0xFF would leave but the menu never returns it); mode 1: coin error. Outputs back on, returns done & 0xFFFF |
| 0x800E029C | Test_ScreenTest | full-screen sprite of ROM asset 0 (0x611F0, test pattern) in gSceneGroup0 until the test switch |
| 0x800E03F0 | Test_InputTest | 56 labels: 1P/2P cabinet buttons (gIoHeld bits 0-8: U D L R P1-P4 START), both N64 pads ("BR1"/"BR2", gPadRawHeld 0x800 0x400 0x200 0x100 0x8000 0x4000 0x10 0x1 0x1000), stick values (yellow outside -4..3, red after), COIN1/2, SERVICE SW, TEST SW, DIP banks as H/L strings |
| 0x800E33C8 | Test_SoundTest | Music_Play(7) until the test switch, Music_Stop |
| 0x800E3588 | Test_SystemCheck | "VERSION 00.09", SDRAM test (1M words at 0x01000000), EEPROM write/read test (overwrites the whole EEPROM with 0..0x1FF & 0xFF) |
| 0x800E3A48 | Test_OutputTest | select lockout coil 1/2 or coin counter 1/2 with 1P up/down, hold 1P button 0x10 to drive it (D_800C178C = 4/8/1/2) |
| 0x800E40D0 | Test_Menu | TEST MODE MENU, 5 entries, cursor colour 0xFA11, controller type lines (JAMMASTICK if DIP bit 4, 3DSTICK if pad reports U+D or L+R, else JOYSTICK); *done = 0x10000 / choice |
| 0x800E4988 | Test_CoinError | "COIN ERROR" for 300 frames, then gCoinJam[0..1] = 0 |

## Data symbols

```
Test_Run = 0x800E0000; // type:func segment:ovl0
Test_ScreenTest = 0x800E029C; // type:func segment:ovl0
Test_InputTest = 0x800E03F0; // type:func segment:ovl0
Test_SoundTest = 0x800E33C8; // type:func segment:ovl0
Test_SystemCheck = 0x800E3588; // type:func segment:ovl0
Test_OutputTest = 0x800E3A48; // type:func segment:ovl0
Test_Menu = 0x800E40D0; // type:func segment:ovl0
Test_CoinError = 0x800E4988; // type:func segment:ovl0
gTestMenuJumpTable = 0x800E4AB0; // type:u32 size:0x14 segment:ovl0
gTestSwitchNow = 0x800C30C4; // type:u16 size:0x2
gTestSwitchPressed = 0x800AF220; // type:u16 size:0x2
gServiceSwitchNow = 0x800C0AB0; // type:u16 size:0x2
gCoinSwitchNow = 0x800C0D4C; // type:u16 size:0x4
gOutputTestBits = 0x800C178C; // type:u16 size:0x2
gPlayerHeld = 0x800B060C; // type:u16 size:0x4
gPlayerPressed = 0x800C0D48; // type:u16 size:0x4
```
(The strings 0x800E4AC4-0x800E4D90 are plain ASCII labels; the C uses its own literals.)

## Things learnt

- **Switch naming**: the test mode labels D_800C30C4 (IO bit 5) "TEST SW" and
  D_800C0AB0 (IO bit 4) "SERVICE SW", and every screen says "PUSH TEST SW" while
  waiting for D_800AF220 (rising edge of D_800C30C4). gfx.c's comments
  (Coin_Update) have these two the other way round ("test switch" = D_800C0AB0,
  "service switch" = D_800C30C4); the test-mode labels suggest the gfx.c names
  should be swapped. ovl2 aborts the game when D_800C30C4 is set, i.e. on the
  test switch.
- D_800C178C is OR-ed into the IO outputs by Coin_Update: bit 0/1 = coin counters,
  bit 2/3 = coin lockout coils (output test drives them); Test_Run clears
  gIoOutputMask while in test mode and sets it back to 0xFFFF when leaving.
- D_800B060C[2] = mapped buttons held, D_800C0D48[2] = mapped buttons pressed
  (bit 0 up, bit 1 down, 0x10 button 1).
- gIoHeld (0x800B0498) bit layout per player: 0 U, 1 D, 2 L, 3 R, 4-7 buttons P1-P4,
  8 START.
- N64 pad raw bits shown as U D L R / P1 P2 P3 P4 / START: 0x800 0x400 0x200 0x100 /
  0x8000 (A) 0x4000 (B) 0x10 (R) 0x1 (C-right) / 0x1000 (Start). Stick dead zone
  for the display is -4..3.
- Each screen task: labels are Text_Create(NULL, s, 0xFFFF, 0xE739, 1, 0x4211) +
  Scene_NewNode(2) linked before gSceneGroup1->tail + Text_PlaceScreen(node, t, x, y,
  1, 1); teardown = yield, delete nodes (the input test yields after 22 and 38
  nodes), yield x2, delete texts (same split), yield, `*done = 0x10000 | result`.

## Quirks kept / noted

- Test_Run returns an uninitialised stack word for modes other than 0/1 (C: 0;
  never happens in the game; the test zeroes that stack slot).
- The input test's sprintf buffer for the stick readouts is the DIP switch 1
  string buffer (it is rebuilt right afterwards, so nothing visible).
- Test_OutputTest and Test_Menu zero an unused 59-word stack array at the start
  (copy of the input test's "seen" flags); not reproduced (no visible effect).
- The system check overwrites the whole EEPROM with its test pattern, so the
  saved settings/bookkeeping are lost after running it.
- The input test creates the stick labels in the order X1, Y1, X2, Y2 but deletes
  them X1, X2, Y1, Y2 (kept, via the slot field of the label table).

## Remaining

None.
