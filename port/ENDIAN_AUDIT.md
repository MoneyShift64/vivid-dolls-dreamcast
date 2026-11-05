# Endianness audit of the game C (`src/game/*.c`)

Target: the game C must run correctly on a little-endian 32-bit CPU (Dreamcast SH4) and stay
bit-identical on big-endian MIPS (the difftests). Rules: ARCHITECTURE.md sections 3 and 4.

Helpers (in `src/game/vd_common.h`):

| Helper | What it does |
|---|---|
| `VD_BIG_ENDIAN` | 1 / 0 from `__BYTE_ORDER__` (`#error` if the compiler does not say) |
| `BE16(p)`, `BE16S(p)`, `BE32(p)`, `BEF32(p)` | read a big-endian value through a byte pointer. Plain load on BE builds; byte assembly on LE builds (no alignment needed) |
| `WBE16(p, v)`, `WBE32(p, v)` | write a big-endian value (ROM-format output). Plain store on BE builds |
| `SWAP16`, `SWAP32`, `BE16_TO_NATIVE`, `BE32_TO_NATIVE` | value swaps |
| `SwapArray16(p, n)` | LE builds only: swap n u16 in place (asset texels/TLUTs to native) |

Every swap-in-place step is inside `#if !VD_BIG_ENDIAN` (macros `SPRITE_SWAP_NATIVE`,
`MODEL_SWAP_NATIVE` are no-ops on BE builds), so the MIPS build does exactly what it did.

Status column: **fixed** = code changed; **ok** = checked, byte-order independent; **port** =
nothing to change in the game C, but another area must do something (see the notes).

## A. ROM-format data read at run time

| File | Function | What | Status / how |
|---|---|---|---|
| siff.c | `SiffIter_Enter` | chunk tag and size read as native u32 | **fixed**: `BE32()` |
| siff.c | `SiffWriter_Begin/End`, `Siff_WriteString`, `Siff_WriteU32`, `Siff_WriteData` | tag/size (and the WriteU32 value) written as native u32, i.e. not the format the reader expects on LE | **fixed**: `WBE32()`. Writer is unused by the game; output is now ROM format on both byte orders. `Siff_WriteData`'s payload is copied as words (byte-exact copy, caller's data). |
| siff.c | `Asset_LoadLzss` | 4-byte little-endian size header | **ok**: already byte reads |
| sprite.c | `Sprite_Parse`, `Sprite_ParseScreen` | `info` fields, `vrtx` x/y, `dlst` command words read with a native u16 load (`rd16`) | **fixed**: `rd16()` = `BE16()` |
| sprite.c | `Sprite_Parse` | `pal4`/`pal8` copied word by word into the sprite block; the F3D interpreter reads the TLUT as native u16 | **fixed**: LE builds swap the copied palette once at the end of parsing (`sprite_swap_native`) |
| sprite.c | `Sprite_Parse` | `tile` data copied; RGBA16 sprites (format 2) have 16-bit texels | **fixed**: LE builds swap the copied tiles if the final format is RGBA16; CI4/CI8 tiles stay bytes |
| sprite.c | `Sprite_Parse` | vertices (`SpriteVtx`) handed to the RSP via G_VTX | **ok**: built field by field from BE16 values, so native |
| sprite.c | `Sprite_Parse` | the generated display list (u32 pairs) | **ok**: built natively |
| sprite.c | `Sprite_ParseScreen` | `dlst` copied as u16 into `rect_cmds`, read natively by `Sprite_DrawScreen` | **fixed**: converted while copying (`*dst++ = BE16(src)`) |
| sprite.c | `Sprite_ParseScreen` | palette copy (screen sprites are CI4/CI8 only) | **fixed**: LE swap as above |
| text.c | `Text_RenderString/RenderInto` | 1-bit font glyph rows read from ROM | **ok**: bytes |
| text.c | `Text_ExpandGlyph` | 8 CI4 texels stored as one native u32 (texel 0 in the top nibble): on LE the 4 bytes would be reversed | **fixed**: `WBE32(dst, out)`; the bitmap is byte data (as TMEM expects) on both byte orders |
| jpeg64.c | `Jpeg64_Decode` | header words: file size (+4), width (+0x34), height (+0x40) as u32, scale (+0x4E) as u16 | **fixed**: `BE32()` / `BE16()` |
| jpeg64.c | `Jpeg64_GetBit/GetBits/PutBits` | bitstream | **ok**: 24-bit window assembled from bytes |
| jpeg64.c | `Jpeg64_Decode` | decoded image written as native u16 RGBA5551 | **ok**: native, which is what PhotoView/F3D expect |
| object.c | `Model_Load` | u32[8] header (header0, off1..off6, gfx count) | **fixed**: `BE32()` |
| object.c | `Model_Load` | u16 command stream (and the vertex index word after 01xx) | **fixed**: `BE16()` |
| object.c | `Model_Load` | vertices [off1,off2) given straight to the RSP; texture records (u32 unk0, u16 w/h, u32 aux/image offsets, relocated by the game); RGBA16 images | **fixed**: `model_swap_native` (LE only, before relocation): Vtx s16/u16 fields, record fields, RGBA16 images once each (found by walking the command stream). Section2 [off2,off3), CI images and aux are left as bytes (unknown layout, never read as words by the game). No model asset exists in the ROM (docs/notes/object.md), so this path is untested on real data. |
| audio.c | `Audio_Init` | `D_800A2378->seqCount` read from the raw 4-byte S1 header before `alSeqFileNew` | **fixed**: `BE16S((u8 *)D_800A2378 + 2)` |
| audio.c | `Audio_Init`, `Music_Play`, `Sfx_*`, `AudioMgr_Main` | `.ctl` bank files and the sequence-file header: the game reads `seqArray[].offset/len`, `bankArray[0]`, `instArray[]`, `soundCount`, `soundArray[]` **after** `alSeqFileNew` / `alBnkfNew` | **port**: the port's `alSeqFileNew` and `alBnkfNew` must convert these files to native order in place (all fields of ALSeqFile, ALBankFile, ALBank, ALInstrument, ALSound, ALEnvelope, ALKeyMap, ALWaveTable, ADPCM book/loop) as they patch the offsets. Then the game C is correct as is. |
| audio.c | `Music_Play` | compact sequence loaded to `D_800A2370` and given to `alCSeqNew` | **port**: the port's `alCSeqNew`/sequence player reads the compact-sequence header (16 BE u32 track offsets) with BE reads |

No overlay reads ROM data directly (the only `Rom_Read` callers are audio.c, jpeg64.c,
text.c and system.c itself; all overlays go through Sprite_*/Model_*/Jpeg64_Decode).

## B. Type punning on native memory

Found by: grep for every pointer cast to a scalar type and every union; `clang -ast-dump`
of all extern declarations (types per symbol across files, tool
`(scratch scripts, not in the repo) externs.py`); an address-range overlap check of all sized
externs against the symbol table (`overlap.py`: no extern covers another symbol of a
different element width).

| File | Function / symbol | What | Status / how |
|---|---|---|---|
| system.c | `Mtx_FromFixed` | fraction half-words read through a `u16 *` view of the s32 Mtx words | **fixed**: read the word, take `>> 16` / `& 0xFFFF` |
| system.c | `Mtx_ToFixed` | builds the Mtx as whole s32 words | **ok** (interpreter must read Mtx as 16 u32 words) |
| system.c | `Input_Thread`, `Main_GfxThread`, `Cont_Thread` | `*(s16 *)msg` | **ok**: every sender points at an s16 `type` field (OSScMsg.type, AudioMsg, `D_800A6810` s16) |
| system.c / audio.c | `D_800B0430` | `s32` in system.c (`!= 0` test), `u8[]` in audio.c (`D_800B0430 + 3` = osContInit bit pattern) | **ok**: the word's other bytes are always 0 (BSS), so `!= 0` is the same on both byte orders. Cleaner: read the byte. Not changed (would alter nothing on BE). |
| system.c | `Task_Create` | arguments copied as u32 words | **ok** (all arguments are words; the port re-implements the task switch) |
| system.c | `Overlay_Load`, `ObjList_Init`, `Obj_Create`, `Scene_NewNode`, `SiffIter_Init` | structs cleared through `u32 *` | **ok**: zero fill |
| gfx.c | `Video_Init` / `Video_ApplyMode` / `Video_SetOffset` | OSViMode handled as `u32[20]` words; `D_8003B110` (`u8[]`) bcopy'd into `D_800A6850` (`u32[20]`), osViModeTable copied as words | **port / dataload**: see section D |
| gfx.c | screen quads `D_800386D0/10/50/90` | static Vtx declared `u8[4][16]`, colour bytes written at 12..15 | **fixed** (typing): declared `SpriteVtx [4]`, writes are `.r/.g/.b/.a` (same byte stores) so the loader types them as Vtx |
| object.c | `D_80038F20` (`gObjDefaultDL`) | `SpriteGfx[]` but holds 1 Gfx + 4 Vtx (G_VTX at `D_80038F68` points to +8) | **fixed** (typing): declared `struct { SpriteGfx dl[1]; SpriteVtx vtx[4]; }`; users take `.dl` |
| gfx.c | `Pfs_HandleError` | `((void **)pfs)[1]`, `((s32 *)pfs)[2]` | **ok**: whole-word reads of OSPfs words |
| gfx.c | `IO_REG16` reads (players, extra, DIPs) and output write | 16-bit register values split by shifts | **ok** in the game; the port's I/O shim returns native u16 values |
| collide.c | `Mesh_BuildVertexBatches` | rows `{s32 index; u8 key[]}` sorted by `RadixSort_Records` on the byte keys only | **ok**: radix sort keys are bytes (keyofs 4); the s32 is only read back as s32 |
| geom.c | `RadixSort_Records` / `RadixSort_Ptrs` | word copies of records; byte-wise radix keys | **ok** for the only caller (byte keys). A caller sorting by a multi-byte integer key would be byte-order dependent - there is none. |
| ovl2_a.c | `PATH_DIR_AT` | `*(s16 *)(p + 0x22 + i*10)` = `PathPoint.dir` (s16 at +2) | **ok**: same width as the field |
| ovl2_c.c | `Boss_Task` | `((f32 *)&arms[k])[i] = 0.0f` over BossArm | **ok**: zero fill |
| ovl2_a.c / ovl2_c.c | `ovl2_D_800FD2B4/FD2C4` | written and read as s16 everywhere | **ok** |
| sprite.c | `Sprite_PlaceCustom` | `union { u32 u; f32 f; }` constant | **ok**: same width |
| libm.c | `fbits` / `bitsf` | `union { f32; u32; }` | **ok**: same width |
| audio.h | `OscState.d0/d1` | union of bytes / f32 / s32 | **ok**: each oscillator type writes and reads the same member |
| object.h / scene.h | `ObjSceneNode.u` / `SceneNode` union | per node kind; `vp` is s16[6] | **ok**: each kind uses one member; Vp s16 read natively by the interpreter |
| audio.c | `dmaState` = `*(AMDMAState *)D_800A47E0` | struct view of a BSS byte array | **ok**: only accessed as that struct |
| scene.h / gfx.c | `SceneLight` Lights1 block, `VdLookAt` | byte arrays given to G_MOVEMEM | **ok**: bytes (interpreter reads them as bytes) |
| strutil.c | `Str_HexDump*`, `Crc16_Ccitt` | byte views of memory | **ok**: CRC over bytes; hex dumps are debug output (show native order) |
| ovl0_test.c | EEPROM test, `SDRAM_TEST` | byte buffer / u32 pattern | **ok** |
| mainloop.c | `Game_ResetProgress` | clears 8 bytes below `D_800C1795` | **ok**: zero fill |

Extern type disagreements between files (all checked, none mixes widths on the same bytes
except the ones above): `D_800A6810` (s16 / u8[] - address only), `D_800A6840` (s16/u16),
`D_800AF254` (u32/s32), `D_800AF222/238`, `D_800B043A/058C/0596`, `D_800C5334` (u16[4]/u16[2]),
`D_800AF264`, `D_800B0428`, `D_800B04D4`, `D_800B056C`, `D_800B4A30`, `D_800C30C8`, `D_800C51C4`,
`D_800C52F4` (s32[2]/s32/u32), `D_800B0498/4A80/C532C/0574/C53D8/C0D48` (u16 / u16[2]),
`D_800C1730`/`gSceneLight` (SceneLight / u8[] - address only), `D_800B0520`, `D_800C0AC0`,
`D_800C30A8`, `D_800C30E0` (struct / u8[] - address only), `D_800B0568` (pointer types),
`D_807B2600/7D7E00` (u16[N]/u16[]), `gRetracesPerFrame`, `D_800C51CC`, `D_800C53F8` (s32/u32),
`osViModeTable` (u32[] / u8[][80] - see D), `ovl2_D_800FD3F8/3FC` (pointer types),
`ovl2_D_800FD4F0` (u8[] / s32[] - address only, task argument block).

## C. Bitfields

None in `src/game` (checked with grep for `: N;` member declarations).

## D. Static data read with a width other than its declared / disassembled type (for dataload)

| Symbol | Declared | Used as | Loader must |
|---|---|---|---|
| `D_800386D0`, `D_80038710`, `D_80038750`, `D_80038790` | now `SpriteVtx[4]` (was `u8[4][16]`; splat: `.byte`) | Vtx for G_VTX, colour bytes written by the game | swap as Vtx: 6 x 16-bit, 4 bytes |
| `D_80038F20` (gObjDefaultDL) +8..+0x47 | now `{Gfx; SpriteVtx[4]}` (splat: `.word`) | 4 Vtx loaded by `D_80038F68` | swap +0..7 as 2 x u32, +8.. as Vtx |
| `D_80038AD0` (gTextQuadVtx) | `SpriteVtx[4]` (splat: `.short`) | Vtx (copied into TextObj) | Vtx: the C type must win over `.short` (colour bytes) |
| `D_8003B110` | `u8[]` | bcopy'd into `D_800A6850` (`u32[20]`), then read/written as u32 words | swap as 20 x u32 (an OSViMode as words). If left as bytes, `D_800A6850`/`D_800A68A8` hold byte-reversed words on LE; harmless only if nothing interprets the VI mode (dataload notes say the VI shim ignores it) |
| `osViModeTable` | `u32[]` (gfx.c), `u8[][80]` (system.c) | words (gfx.c copies/writes words; system.c passes the address) | 32-bit words, including word 0 (`type` byte + pad): a VI shim that wants `type` must read `w[0] >> 24` |
| `gJpeg64HuffDcLuma/DcChroma/AcLuma/AcChroma` | `Jpeg64Huff` with `huffval[1]` | u16 over the whole symbol | all u16 to the symbol end (AC chroma ends at 0x80039FD0, before the bit masks) |
| `ovl2_D_800F8E1C`, `F8E4C`, `F922C`, `F9318`, `F93B4` | `u8[]` | `EffFrame **` (Item/Clock/player/Crawler tasks) | pointer arrays (+ relocation); targets EffFrame `{void **, s32}` |
| `ovl2_D_800F95D4`, `F9488` | `u8[]` | `AnimFrame **` (Snake_Task) / `EffFrame **` (SpinBoss_Task) | as above |
| `ovl2_D_800F9438` | `u8[]` | `AnimFrame **` (Boss_Task) | as above |
| `ovl2_D_800F97B0`, `F96DC` | `u8[]` | `EnemyAnims *` (Enemy_WanderTask / Enemy_BounceTask) | EnemyAnims layout (ovl2_b.h) |
| `ovl2_D_800F9874` | `u8[]` | `AnimFrame **` (Minion_Task) | as above |
| `SceneObjDesc.arg` (all scene tables) | `void *` | a ROM address for kinds 2-5, a pointer for 1, 6-8, 10, 12 | relocate only the words that are pointers (ROM addresses are < 0x01000000, so `.word SYMBOL` already tells them apart) |
| `D_8003B790`, `D_8003BA50`, `D_8003C250`, `D_8003CA50`, microcode text | `u8[]` | RSP data | bytes (unused by the port) |
| `gCharTableLower/Upper`, `D_800383A0`, `D_8003A014`, `D_800384B0/CC`, `D_80038AB0/AC0`, ovl2 `u8[4..16]` tables | `u8` | bytes | bytes |

No static texture or TLUT is referenced from static display lists (all G_SETTIMG
addresses are built at run time), so no static 16-bit texel data needs a u16 rule.

## E. Contracts the rest of the port relies on

* F3D interpreter (gfx area): Gfx = 2 native u32; Vtx = native s16 x,y,z, u16 flag, s16 s,t +
  bytes r,g,b,a; Mtx = 16 native u32 words (halves by shifts); Vp = native s16[8];
  Lights/LookAt = bytes; RGBA16 texels and TLUT entries = native u16; CI4/CI8 texels =
  bytes in memory order (CI4: high nibble first). The text bitmap and sprite tiles follow this.
* Audio (libultra audio port): `alSeqFileNew` / `alBnkfNew` convert their files to native
  order in place; the compact-sequence reader uses BE reads (section A).
* Input shim: `osContGetReadData` fills `PadData` natively; `IO_REG16` returns native values.
* libultra shim: OSMesg pointers to s16 `type` fields as on the N64 (no change).
