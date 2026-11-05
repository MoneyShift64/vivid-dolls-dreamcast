# Area "text": 8x8 font text objects (0x80015AA0-0x80016D20)

C: `src/game/text.c`, `src/game/text.h`. Test: `python3 tools/difftest/test_text.py`
(all 13 functions PASS, 300-400 random trials each; heap, TextObj, SceneNode,
bitmap buffers and gScreenWidth compared after every call).

## Functions done

| Address | Name | What |
|---|---|---|
| 0x80015AA0 | Text_Create | create/init a TextObj for a string (pads odd length with ' '), sets colours, renders bitmap |
| 0x80015BE4 | Text_Delete | free bitmap (and the object if Text_Create allocated it); returns 0 |
| 0x80015C40 | Text_Place | node mtx = scale(s,s,1)*translate(x+.5,y+.5,z); build quad DL; attach to node |
| 0x80015D04 | Text_PlaceRotated | same with a Z rotation (func_800071CC(m, angle, 0,0,1)) |
| 0x80015E14 | Text_PlaceScreen | build texrect DL at (s16)x,(s16)y scaled sx,sy; attach to node |
| 0x80015EA4 | Text_SetColors | palette: [0]=0 [1]=bg [2]=shadow, [3..6]=top->white ramp, [7..10]=bottom->white ramp |
| 0x80016094 | Text_SetGradient | palette: [3]=from, [4..11]=lerp(from,to,k/8) |
| 0x800161E8 | Text_SetString | re-render new string (cut to capacity) into the existing bitmap |
| 0x80016298 | Text_BuildQuadDL | F3D DL: load CI4 bitmap + TLUT, G_VTX 4 (quad from template) |
| 0x8001650C | Text_BuildRectDL | F3D DL: load TLUT + bitmap, scissored G_TEXRECT (RDPHALF_1/2) |
| 0x800169B4 | Text_RenderString | alloc bitmap (len*32 + 16) and render each glyph from ROM font |
| 0x80016B00 | Text_RenderInto | render string into an existing bitmap |
| 0x80016C3C | Text_ExpandGlyph | 1bpp 8x8 glyph -> CI4 rows with per-row ink index and drop shadow |

## Data symbols

```
Text_Create = 0x80015AA0; // type:func
Text_Delete = 0x80015BE4; // type:func
Text_Place = 0x80015C40; // type:func
Text_PlaceRotated = 0x80015D04; // type:func
Text_PlaceScreen = 0x80015E14; // type:func
Text_SetColors = 0x80015EA4; // type:func
Text_SetGradient = 0x80016094; // type:func
Text_SetString = 0x800161E8; // type:func
Text_BuildQuadDL = 0x80016298; // type:func
Text_BuildRectDL = 0x8001650C; // type:func
Text_RenderString = 0x800169B4; // type:func
Text_RenderInto = 0x80016B00; // type:func
Text_ExpandGlyph = 0x80016C3C; // type:func
gTextQuadVtx = 0x80038AD0; // type:s16 size:0x40
gTextQuadSetupDL = 0x80038B10; // size:0x48
gTextQuadFinishDL = 0x80038B58; // size:0x30
gTextRectSetupDL = 0x80038B88; // size:0x38
gTextRectFinishDL = 0x80038BC0; // size:0x18
gTextTopRamp = 0x80038BD8; // type:f32 size:0x10
gTextBottomRamp = 0x80038BE8; // type:f32 size:0x10
```
(The C currently uses the `D_8003xxxx` names; rename together if these are adopted.)
Double constants used: D_8003D840/D_8003D848 = 0.5, D_8003D850 = 0.125, D_8003D858 = 320.0,
D_8003D860 = 4.0, D_8003D868 = 4294967296.0, D_8003D870 = 1024.0, D_8003D878 = 8.0.

## Font format (ROM 0x196710-0x196CC0, "8x8 1-bit debug font")
* 0x5B0 bytes = 182 glyphs for codes 0x20..0xD5, 8 bytes per glyph, one byte per
  row top to bottom, bit 7 = leftmost pixel. Address = 0x196710 + (byte - 0x20) * 8.
* No range check: control bytes < 0x20 read the 256 bytes of ROM before the font,
  bytes > 0xD5 (EUC/SJIS lead/trail bytes) read into the following instrument bank.
  The text code has no multibyte handling at all; each byte is one 8-pixel cell.
* Read with Rom_Read 8 bytes at a time into a scratch slot after the bitmap.

## Rendered bitmap / palette
* CI4, width length*8, height 8, row stride length*4 bytes; glyph i at byte i*4.
* Indices: 1 = empty, 2 = shadow (empty pixel whose upper-left neighbour is ink),
  3+row = ink (row 0..7 -> 3..10). Text_ExpandGlyph's ink counter is a u8, so with
  ink > 8 indices exceed 15 and spill into the neighbouring nibble (kept; the game
  always uses ink=3, empty=1, shadow=2).
* palette is RGBA5551; channel expansion (c>>8)&F8, (c>>3)&F8, (c<<2)&F8, blend in
  float, pack with truncation, alpha bit always set (except entries 0..2 which are raw).
  Ramps: top towards white by 0/.3/.5/1.0, bottom by .5/.3/.1/0.

## TextObj (0x180 bytes; see text.h)
`0x000` setup DL ptr, `0x004` finish DL ptr, `0x008` Gfx dl[32] (quad DL uses 15,
texrect DL 17), `0x108` Vtx[4], `0x148` u16 palette[16], `0x168` bitmap,
`0x16C` capacity (length at creation), `0x170` length (always even), `0x174` owned
flag, `0x178` 8 unused bytes. Attaches to SceneNode fields 0xD0/0xD4/0xD8 (dl,
setup_dl, finish_dl) like sprites.

Quad: template vertices (0,0) (8,0) (8,-8) (0,-8) with s/t 0..0x200; BuildQuadDL sets
x of v1/v2 to len*8 and s to -5 / len*512-5. The finish DL (0x80038B58) holds the two
G_TRI1s (0,2,1) and (0,3,2). Texrect: x,y are in 320x240 units relative to screen
centre (ulx = (x+160)*W/320*4, uly = (120-y)*4), width len*8*sx*W/320, height 8*sy.

## Quirks not reproduced
* Text_Create / Text_SetString copy the string into a 32-byte stack buffer with no
  length check: > 30 characters overflow it in the original (would corrupt a saved
  register). The C uses `u8 buf[32]` too; callers must keep strings <= 30 chars.
  Tests stay within that.
* Text_Create with a failed Heap_Alloc bzeros/writes NULL like the original (kept).

## Remaining
None in this range.

## Note
Running `python3 tools/callgraph.py` with no arguments regenerates docs/functions.csv
(I ran it once by mistake while looking for its help text; it rebuilds the file from
asm/, so content should be unchanged unless asm/ or symbol names changed).
