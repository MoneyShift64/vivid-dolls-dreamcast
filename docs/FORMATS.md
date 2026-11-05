# Vivid Dolls – technical notes

Everything here was worked out from the ROM (`nus-zsaj-0.u3`, Seta Aleck64, Visco 1998).

## ROM and memory map

| ROM range | RAM address | What |
|---|---|---|
| `0x000000–0x000040` | – | N64 header ("VIVID DOLLS", code `ZSAJ`) |
| `0x000040–0x001000` | – | IPL3 boot code |
| `0x001000–0x01D040` | `0x80000400` | **Visco game code** (425 functions) – `asm/game.s` |
| `0x01D040–0x038D50` | `0x8001C440` | libultra 2.0H, libc, rmon (395 functions) – `asm/libultra.s` |
| `0x038D50–0x03ED30` | `0x80038150` | main data + rodata |
| – | `0x8003E130–0x800C5400` | main BSS (cleared at boot) |
| `0x03ED30–0x043AD0` | `0x800E0000` | **ovl0** – test / service mode |
| `0x043AD0–0x045270` | `0x800E0000` | **ovl1** – boot / attract sequence |
| `0x045270–0x0602C0` | `0x800E0000` | **ovl2** – the game itself (95 functions) |
| `0x0602C0–0x0611F0` | `0x800E0000` | **ovl3** – small, not identified yet |
| `0x0611F0–0x196710` | (loaded on demand) | 506 asset files (sprites + photos) |
| `0x196710–0x196CC0` | – | 8×8 1-bit debug font |
| `0x196CC0–0x1982B0` | – | music instrument bank (`B1` ALBankFile, .ctl) |
| `0x1982B0–0x1D7720` | – | music ADPCM samples (.tbl) |
| `0x1D7720–0x1E2870` | – | music sequences (`S1` ALSeqFile, 15 songs) |
| `0x1E2870–0x1E3EA0` | – | sound-effect bank (`B1`, .ctl) |
| `0x1E3EA0–0x23AFE0` | – | sound-effect ADPCM samples (.tbl) |
| `0x23AFE0–0x800000` | – | unused (`0xFF`) |

* The chip dump is byte-swapped; `tools/prepare_rom.py` fixes that.
* Code is GCC-compiled (moves are `addu`, struct copies use `lwl/lwr`).
* Overlays: table `gOverlayTable` (`0x8003A280`, 16 bytes each: vram start, vram end,
  rom start, rom end). `Overlay_Load` (`0x8000C0D4`) clears 0x4D000 bytes at
  0x800E0000 and DMAs the overlay in. Each overlay's entry point is at 0x800E0000 and
  takes a pointer to three words; `Main_GameLoop` (`0x8001C000`) drives them.
* All four overlays share the same RAM, so the build prefixes their symbols
  (`ovl2_func_800E1234`) – see `tools/fix_labels.py`.

## Asset files

Game code refers to assets as `(pointer & 0xFFFFFF) + 0x611F0`. Assets start on 4-byte
boundaries. Each is an IFF-style **Siff** container, either stored raw or LZSS-compressed.
`docs/assets.csv` lists all 506.

### LZSS (`Asset_LoadLzss`, `0x8001BC30`)
* 4-byte **little-endian** decompressed size.
* Flag byte, read MSB first. `0` = copy one literal byte; `1` = 2-byte back-reference
  `LD DD`: length = `L + 3`, distance = `0xDDD + 1`.

### Siff chunks
Big-endian: 4-character tag, u32 size, data. `Siff` and `Sp3d` hold sub-chunks.

### Sprites – `Sp3d` (472 files)
| chunk | contents |
|---|---|
| `vrtx` | one `(x, y)` s16 pair per placed tile (y grows upwards) |
| `pal8` | 256 colours, RGBA5551 big-endian (alpha bit 0 = transparent) |
| `tile` | 8-bit tiles; 8×32, 16×16, 16×32, 32×16, 32×32, 64×16 or 64×32 |
| `dlst` | byte code placing tiles (below) |
| `info` | s16[12]: `[1]` tile width, `[2]` tile height, `[5]` tile count, `[10]` mode |

`dlst` byte code:
* `40 nn hhhh` – load `nn+1` vertices from vertex `hhhh` (one `vrtx` entry = 4 vertices).
* `80 tt` – select tile `tt`.
* `aaaa bbbb` – two triangles; the smallest vertex nibble ÷ 4 picks the quad in the batch.
* `C0` – end.
* Mode 4 (the one full-screen sprite, no `vrtx`): `80 tt xxxx yyyy` places tile `tt` at
  `(xxxx − 0x3E00, yyyy − 0x0E00)`.

Tiles are power-of-two 8-bit paletted images, which is exactly the Dreamcast PVR's PAL8
texture format, so the port can upload them as they are.

### Photos – `jpeg64 Ver0.00` (34 files)
Chunks: `type` = `jgx`, `crtl` = `jpeg64 Ver0.00`, `xw  `/`yw  ` = size
(320×240, 320×480 or 640×240), `sc  ` = quality divisor (always 2), `data` = bitstream.

Baseline-JPEG maths with every header removed:
* Standard (Annex K) Huffman and quantisation tables, quant ÷ `sc`. The decoder tables
  live in the ROM at `0x80039B50` onwards.
* **Bits are read least-significant first** from a 24-bit little-endian window;
  no `0xFF` byte stuffing, no markers.
* 16×16 macroblocks in raster order, 6 blocks each: Y0 Y1 Y2 Y3 Cb Cr (4:2:0).
* Each of the 6 block slots has its own DC predictor (reset once per photo).
* Dequantise with floats and truncate → float matrix IDCT (cosine table built at boot,
  rounded half away from zero) → Y + 128, chroma doubled 2×2 → YCbCr→RGB with
  constants 1.402 / 0.3441 / 0.7139 / 1.7718 / 0.0012 → clamp → 8-to-5-bit table
  at `0x8003A014` → RGBA5551.
* Functions: `Jpeg64_Decode` `0x80019D70`, `Jpeg64_DecodeMacroblock` `0x8001B250`,
  `Jpeg64_Dequant` `0x8001AFB0`, `Jpeg64_Idct` `0x8001AA40`, `Jpeg64_Upsample`
  `0x8001A530`, `Jpeg64_YCbCrToRgba5551` `0x8001A2D4`, bit reader `0x8001B9A8`/`0x8001B9F8`.
* `port/vd_jpeg64.c` reimplements it and reads all 34 photos to within 3 bytes of
  their exact length (the rest is padding). The ROM also contains a matching
  **encoder** (`0x8001B064`, `0x8001B4D0`, `0x8001B55C`, `0x8001B898`).

## Other identified code
| Address | Name | Notes |
|---|---|---|
| `0x8000B210` | `Main_Boot` | creates threads, Pi manager |
| `0x8000BFE8` | `Rom_Read` | DMA from ROM in 0x400-byte pieces |
| `0x8000C0D4` | `Overlay_Load` | see above |
| `0x8000D93C` | `Audio_Init` | 0x5F000-byte audio heap at `0x80043350`, loads both banks + sequences |
| `0x80003300`–`0x80003610` | `Heap_Init` / `Heap_Alloc` / `Heap_Free` / `Heap_AllocFront` … | K&R-style allocator, 16-byte units; game heap at `0x80173600` (see src/game/heap.c) |
| `0x80012820`–`0x80014E90` | `Sprite_*`, `PhotoView_*`, `Image_*` | sprite loading/display lists, photo window, mosaic (src/game/sprite.c, photoview.c) |
| `0x8001C000` | `Main_GameLoop` | picks and runs overlays |
| `0x8000C79C`… | – | N64 SDK sample scheduler (`sched.c`) |

libultra functions were identified with [n64sym](https://github.com/shygoo/n64sym)
(209 names, in `symbol_addrs.txt`).
