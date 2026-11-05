# Vivid Dolls decompilation: developer notes

A decompilation project for **Vivid Dolls** (Visco, 1998), an arcade game that ran on
the Seta Aleck64 board (basically an N64 in an arcade cabinet). The end goal is a
native Dreamcast port. See `docs/ROADMAP.md` for the plan and `docs/FORMATS.md` for
everything known about the ROM.

**Current state:** the ROM splits and rebuilds byte-for-byte; all asset formats are
documented and have portable C code; **534 of 542 game functions (99.4% of all game code) are decompiled to clean C and
verified against the original**: all of the main code except 8 hand-written assembly
routines (task context switch, TLB access), and all four overlays (test mode, attract,
the game itself, girl select). Every function has a
first-draft C version in `src/draft/`. Run `python3 tools/progress.py` for details.

> This repository contains no game data. You need your own `nus-zsaj-0.u3` ROM dump.

## What's in here
| Path | What |
|---|---|
| `vividdolls.yaml` | splat config: how the ROM is cut up |
| `symbol_addrs.txt` | names for known functions and data |
| `Makefile` | rebuilds the ROM and checks it matches |
| `src/game/` | **decompiled game code**, clean C, each function verified against the original |
| `src/draft/` | machine-made first drafts (m2c) of all 542 game functions |
| `port/` | portable C for the asset formats (used by the Dreamcast port) |
| `tools/` | helper scripts (ROM prep, asset tools, table generator…) |
| `docs/` | technical notes, roadmap, asset list |
| `asm/`, `assets/` | *created by `make setup` from your ROM – not included* |

---

## Setup on a Mac (step by step)

You only need to do steps 1–4 once.

### 1. Install the tools
Open **Terminal** (press Cmd + Space, type `Terminal`, press Return). Copy each
command below, paste it into Terminal, and press Return. Wait for each one to finish
(you'll see the prompt again) before doing the next.

Update Homebrew's list of packages:
```
brew update
```
Install Python, GNU Make and the MIPS assembler/linker (this takes a few minutes):
```
brew install python3 make tehzz/n64-dev/mips64-elf-binutils
```

### 2. Go to the project folder
Double-click the downloaded zip in your **Downloads** folder; it unpacks into a folder
called `vividdolls-decomp`. Then in Terminal:
```
cd ~/Downloads/vividdolls-decomp
```
(Every command from now on must be run from inside this folder. If you open a new
Terminal window later, run this `cd` command again first.)

### 3. Install the Python helpers
This makes a private Python environment inside the project folder, so nothing
touches the rest of your Mac:
```
python3 -m venv .venv
```
Switch it on:
```
source .venv/bin/activate
```
Your prompt now starts with `(.venv)`. Install splat (the ROM splitter) and Pillow
(for images):
```
pip install "splat64[mips]==0.50.1" pillow
```

### 4. Add your ROM
Type the start of this command, **including the space at the end**, but don't press
Return yet:
```
python3 tools/prepare_rom.py 
```
Now drag your `nus-zsaj-0.u3` file from Finder into the Terminal window (this pastes
its location), then press Return. You should see `SHA1 OK`. This writes a correctly
byte-ordered copy called `baserom.z64`; your original file isn't changed.

Split the ROM into assembly and data files:
```
gmake setup
```

## Every time after that
Open Terminal, then go to the folder:
```
cd ~/Downloads/vividdolls-decomp
```
Switch on the Python environment:
```
source .venv/bin/activate
```

## Building
Rebuild the ROM from the split files and check it matches the original:
```
gmake
```
The last line should say `build/vividdolls.z64: OK - matches the original ROM`.

Test the portable C asset code against your ROM (decompresses all 506 assets and
decodes every sprite and photo):
```
gmake port-test
```
It should end with `failures 0`.

Write a list of all assets to `docs/assets.csv`:
```
python3 tools/assets.py list
```
Export every sprite as a PNG into `extracted/sprites/`:
```
python3 tools/assets.py sprites
```

Show how much of the game is decompiled:
```
python3 tools/progress.py
```

## Dreamcast port

The port runs the decompiled game C natively on the Dreamcast (KallistiOS), with
the N64/Aleck64 parts replaced underneath it. The design is in
`port/ARCHITECTURE.md`; notes on each part are in `port/notes/`.

* **No game data is built into the program.** The CDI holds `1ST_READ.BIN` and the
  player's own ROM (as `VIVIDOLL.ROM`), which the game reads at boot.
* Controls: X = coin, Start = start, D-pad/stick = move, A/B/Y = buttons 1-3,
  A+B+X+Y+Start = soft reset. The game has no saving, so the VMU isn't used.
* Status: the whole game, with sound, has been played on a real Dreamcast.

Building (needs KallistiOS with SH4ZAM, `source /opt/toolchains/dc/kos/environ.sh`):

| Command | Result |
|---|---|
| `make -f port/Makefile.dc` | release build in `build/dc/` |
| `make -f port/Makefile.dc CHEAT=1` | testing build in `build/dc-cheat/`: holding L+R ends a round as a 99% clear |
| `make -f port/Makefile.dc VIEWER=1` | `build/dc-viewer/`: boots straight to the unused title photo |
| `make -f port/Makefile.dc DEBUG=0x39` | debug-log build in `build/dc-debug/` |
| `tools/port/make_cdi.sh YOUR_ROM` | `build/dc/vividdolls.cdi` from the release build and your ROM (.u3 or .z64) |

`VD_FIRST_READ=build/dc-cheat/1ST_READ.BIN tools/port/make_cdi.sh YOUR_ROM OUT.cdi`
makes a disc from one of the other builds. Automated tests in an emulator
(headless Flycast) are in `tools/port/dc_harness/` (see `port/notes/boot.md`).

## How the C code is checked
Each function in `src/game/` is run side by side with the original code in an
emulator and the results are compared byte for byte. See `docs/DIFFTEST.md`.
This is optional; you don't need it to build.

## How the build works (for later)
- `make setup` runs splat, then `tools/fix_labels.py`, which gives each overlay's
  symbols a prefix (`ovl2_…`) because all four overlays load at the same address.
- Overlay 2's read-only data really starts at ROM `0x601B8`; the split uses `0x601B0`
  because GNU as pads sections to 16 bytes. It makes no difference to the output.
- Works with either `mips-linux-gnu-` (Linux) or `mips64-elf-` (Homebrew) binutils;
  both were tested and produce the matching ROM.

## Credits / tools
[splat](https://github.com/ethteck/splat) · [spimdisasm](https://github.com/Decompollaborate/spimdisasm) ·
[n64sym](https://github.com/shygoo/n64sym) · GNU binutils
