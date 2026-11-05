# Area "mainloop": jpeg64 codec, sound wrappers, main game loop (0x80019CA0-0x8001C440)

C: `src/game/jpeg64.c` / `jpeg64.h` (21 functions), `src/game/mainloop.c` / `mainloop.h` (13 functions).
Tests: `python3 tools/difftest/test_jpeg64.py` and `python3 tools/difftest/test_mainloop.py`
(all 34 functions PASS, 300-400 trials each; about 35 s in total).

The jpeg64 tests use **synthetic data only**: random coefficient blocks, random pixel
and YCbCr data, random bit streams, symbol streams written by the game's own (unused)
encoder, and tiny made-up images (16-64 x 16-48 pixels, random coefficients encoded
by the original encoder) served from fake ROM addresses by a `Rom_Read` stub. The ROM's
photos are never read, and decoded pixels only ever exist in emulator memory.
The whole BSS block 0x800A6C90-0x800A7FB0, output buffers, bit position and (for
Jpeg64_Decode) the first 32 KB of the game heap are compared.

Main_GameLoop never returns: the test stubs the overlay entry 0x800E0000 (and
Overlay_Load, func_8000AEAC, the audio calls, func_8000D6B8), gives scripted random results,
randomly raises the service-request flags / changes D_800C3174 and the settings word,
and stops the emulator after 1-80 overlay runs. The call log (with the 3-word argument
block and a snapshot of 19 globals + the 8 cleared bytes at every call) is compared.
All overlay/argument combinations are reached (ovl0 0/1, ovl1 0/1/2/3, ovl2 0/1, ovl3 0).

## Functions done

| Address | Name | What |
|---|---|---|
| 0x80019D70 | Jpeg64_Decode | load jpeg64 Siff file from ROM into heap, decode all 16x16 macroblocks into a 16-bit image at (x,y), stride = photo width; returns w/h |
| 0x80019FAC | Jpeg64_Init | set bitstream pointer, pos = 0; build cos table [u][x] and its transpose (calls libultra cosf) |
| 0x8001A118 | Jpeg64_GetBitPos | return bit position |
| 0x8001A124 | Jpeg64_RgbToYCbCr | encoder: 256 RGB888 -> Y,Cb,Cr (0.299/0.587/0.114, -0.1684/0.3316/0.5, 0.5/0.4187/0.0813) |
| 0x8001A2D4 | Jpeg64_YCbCrToRgba5551 | 256 Y,Cb,Cr -> RGBA5551 via 5-bit table 0x8003A014 |
| 0x8001A4A0 | Jpeg64_Downsample | encoder: macroblock -> 6 blocks (Y-128; chroma = top-left of each 2x2, no averaging) |
| 0x8001A530 | Jpeg64_Upsample | 6 blocks -> macroblock (Y+128; chroma doubled 2x2) |
| 0x8001A5D8 | Jpeg64_Fdct | encoder: float matrix FDCT of 6 blocks |
| 0x8001AA40 | Jpeg64_Idct | float matrix IDCT of 6 blocks, round half away from zero |
| 0x8001AEA8 | Jpeg64_Quantize | encoder: in / (q*scale) rounded half away; exact 0 stays 0 |
| 0x8001AFB0 | Jpeg64_Dequant | trunc(in * (q*scale)); blocks 0-3 luma table, 4-5 chroma |
| 0x8001B064 | Jpeg64_EncodeMacroblock | encoder: Huffman-code 6 blocks (ZRL, EOB; no EOB if coef 63 != 0) |
| 0x8001B250 | Jpeg64_DecodeMacroblock | decode 6 blocks (DC pred per slot, run/level AC, zigzag) |
| 0x8001B4D0 | Jpeg64_EncodeDC | encoder: size category code + bits |
| 0x8001B55C | Jpeg64_EncodeAC | encoder: run*16+size code + bits |
| 0x8001B5F4 | Jpeg64_DecodeDC | Huffman decode size + EXTEND |
| 0x8001B708 | Jpeg64_DecodeAC | Huffman decode run/size + EXTEND into *level/*run |
| 0x8001B84C | Jpeg64_ReverseBits | reverse low n bits (n as u16) |
| 0x8001B898 | Jpeg64_PutBits | encoder: write n bits MSB-first into the LSB-first stream |
| 0x8001B9A8 | Jpeg64_GetBit | read 1 bit |
| 0x8001B9F8 | Jpeg64_GetBits | read n bits, first bit = MSB |
| 0x8001BA90 | Sound_Play | play sound effect n from table 0x8003A120 via func_8000D49C; 0x7FFF if muted |
| 0x8001BB18 | Sound_PlayAlways | same, ignoring the mute flag |
| 0x8001BB8C | Sound_Stop | func_8000D67C(handle) unless muted or handle >= 0x7FFF |
| 0x8001BBC0 | Bgm_Request | func_8000D1CC(n) unless muted; n >= 6 remapped via 0x8003A240 |
| 0x8001BC00 | Bgm_Cancel | func_8000D160() unless muted |
| 0x8001BE80 | Main_NullHookA | empty (called from func_8000C2A8, boot) |
| 0x8001BE88 | Main_NullHookB | empty (called from func_8000C7C8, scheduler) |
| 0x8001BE90 | Main_NullHookC | empty (called from func_8000C7C8) |
| 0x8001BE98 | Main_NullHookD | empty (5 callers in scheduler/graphics) |
| 0x8001BEA0 | Main_NullHookE | empty (called from func_8000BD28) |
| 0x8001BEB0 | Game_ResetProgress | new game: clear D_800C3174/D_800C172C, level = 0x8003A340[settings&3], clear 8 bytes 0x800C178E-95 |
| 0x8001BF08 | Main_Start | func_8000FA34, copy settings row of 0x8003A2C0 into 8 globals, reset progress, func_8000A76C(Main_GameLoop, 10, 0) |
| 0x8001C000 | Main_GameLoop | top-level overlay loop (see below) |

## Data symbols

```
Jpeg64_GetBitPos = 0x8001A118; // type:func
Jpeg64_RgbToYCbCr = 0x8001A124; // type:func
Jpeg64_Downsample = 0x8001A4A0; // type:func
Jpeg64_Fdct = 0x8001A5D8; // type:func
Jpeg64_Quantize = 0x8001AEA8; // type:func
Jpeg64_EncodeMacroblock = 0x8001B064; // type:func
Jpeg64_EncodeDC = 0x8001B4D0; // type:func
Jpeg64_EncodeAC = 0x8001B55C; // type:func
Jpeg64_ReverseBits = 0x8001B84C; // type:func
Sound_Play = 0x8001BA90; // type:func
Sound_PlayAlways = 0x8001BB18; // type:func
Sound_Stop = 0x8001BB8C; // type:func
Bgm_Request = 0x8001BBC0; // type:func
Bgm_Cancel = 0x8001BC00; // type:func
Main_NullHookA = 0x8001BE80; // type:func
Main_NullHookB = 0x8001BE88; // type:func
Main_NullHookC = 0x8001BE90; // type:func
Main_NullHookD = 0x8001BE98; // type:func
Main_NullHookE = 0x8001BEA0; // type:func
Game_ResetProgress = 0x8001BEB0; // type:func
Main_Start = 0x8001BF08; // type:func
gJpeg64EncDcLumaCode = 0x800392D0; // type:u16 size:0x20
gJpeg64EncDcLumaLen = 0x800392F0; // type:u16 size:0x20
gJpeg64EncDcChromaCode = 0x80039310; // type:u16 size:0x20
gJpeg64EncDcChromaLen = 0x80039330; // type:u16 size:0x20
gJpeg64EncAcLumaCode = 0x80039350; // type:u16 size:0x200
gJpeg64EncAcLumaLen = 0x80039550; // type:u16 size:0x200
gJpeg64EncAcChromaCode = 0x80039750; // type:u16 size:0x200
gJpeg64EncAcChromaLen = 0x80039950; // type:u16 size:0x200
gJpeg64BitMask = 0x80039FD0; // type:u32 size:0x44
gJpeg64To5Bit = 0x8003A014; // type:u8 size:0x100
gSoundTable = 0x8003A120; // type:SoundDef size:0x120
gMusicRemap = 0x8003A240; // type:u32 size:0x40
gSettingsParams = 0x8003A2C0; // type:s32 size:0x80
gStartLevelTable = 0x8003A340; // type:s32 size:0x10
gJpeg64BitPos = 0x800A6C90; // type:s32
gJpeg64BitBuf = 0x800A6C94; // type:u8*
gJpeg64Cos = 0x800A6C98; // type:f32 size:0x100
gJpeg64CosT = 0x800A6D98; // type:f32 size:0x100
gJpeg64RgbaMb = 0x800A6E98; // type:u16 size:0x200
gJpeg64PixMb = 0x800A70A0; // type:s16 size:0x600
gJpeg64Spatial = 0x800A76A0; // type:s16 size:0x300
gJpeg64Dequant = 0x800A79A0; // type:s16 size:0x300
gJpeg64Coef = 0x800A7CA0; // type:s16 size:0x300
gJpeg64DcPred = 0x800A7FA0; // type:s16 size:0xC
gSettings = 0x800C52A0; // type:u16
gSoundMute = 0x800C51D8; // type:s32
gServiceRequest = 0x800C1674; // type:s32
gServiceRequest2 = 0x800AF260; // type:s32
gLevel = 0x800B4AA0; // type:s32
```
(The C still uses the `D_8003XXXX` names for these. If they are renamed in
symbol_addrs.txt, change the externs or keep `D_` aliases.) Note: the symbol size of
`gJpeg64HuffAcChroma` (0x1C0) overlaps `gJpeg64BitMask` at 0x80039FD0; the AC chroma
table really ends at 0x80039FD0 (size 0x1B0) - PutBits indexes the mask table as
`gJpeg64HuffAcChroma + 0x1B0` in the disassembly because of this.

## Structs / globals learnt

- Decoder Huffman tables (0x80039B50/BE0/C70/E20): `{ u16 valptr[18]; u16 mincode[18];
  u16 maxcode[18]; u16 huffval[]; }` (huffval is **u16**, maxcode 0xFFFF = no codes of
  that length). The decoder has no length limit on a code (it keeps walking past 16).
- DecodeMacroblock has no bound check on k: a bad run can write outside the block (kept).
- Jpeg64_Decode header offsets: +4 file size - 8, +0x34 width, +0x40 height, +0x4E low
  half of `sc` (used as `1.0f / (f32)(s16)sc`), bitstream at +0x58. Output stride is the
  photo width. The whole file goes into a `Heap_AllocFront` buffer, freed at the end.
- YCbCr->RGB constants are 1.402 / **0.3441 / 0.7139** / 1.7718 / 0.0012
  (docs/FORMATS.md says 0.34414 / 0.71414: the float bits are 0x3EB02DE0 / 0x3F36C227,
  i.e. 0.3441f / 0.7139f; port/vd_jpeg64.c uses the right bits).
- The full encoder exists but nothing calls it (Fdct, Quantize, EncodeMacroblock,
  RgbToYCbCr, Downsample, PutBits, GetBitPos).
- `SoundDef` (0x8003A120, 24 entries x 12 bytes): `{ u16 id; u16 pad; f32 volume;
  s16 pitch; u8 a3; u8 a4; }` -> `func_8000D49C(id, volume, pitch, a3, a4)` (returns s16 handle).
- 0x800C51D8 is a sound mute flag; the main loop sets it to 1200 for the attract demo
  and clears it at every level start. 0x800C52A0 behaves like the DIP-switch word.
- Main_GameLoop flow: ovl1(3) once at boot; then loop: service checks (ovl0 mode 0 when
  0x800C1674 set or settings bit 7, ovl0 mode 1 when 0x800AF260 set);
  title = Bgm_Cancel + ovl1(0). Result 4 -> attract demo (ovl2(0) with D_800B04CC = 2,
  mute 1200, D_800B04D4/D8 = 4, demo index 0..7 in D_800B0608) then back to title.
  Otherwise bits 0/1 of the result set D_800B04D4/D_800B04D8 = 4 - ((settings>>2)&3),
  Game_ResetProgress, then level loop: Bgm_Request(14) + ovl3(0) (result & 0xFF ->
  D_800B0608), then round loop: Bgm_Request(1), ovl1(1), ovl2(0) -> r, func_8000D6B8;
  r = 0xFF -> title; D_800C3174 >= 3 -> ovl1(2) + ovl2(1) and reset it; r = 2 -> level++
  (max 15) and next level. Any service flag set after an overlay -> func_8000D160 and restart.
  Some overlay calls only set args[0] (args[1..2] keep earlier values) - kept.

## Remaining

None in this range.
