# Vivid Dolls - Dreamcast build (KallistiOS).
#
#   make -f platform/dc/Makefile            build build/dc/vividdolls.elf and build/dc/1ST_READ.BIN
#   make -f platform/dc/Makefile clean
#   make -f platform/dc/Makefile DEBUG=0x39 debug-log build (platform/dc/sys/vd_debug.h) in build/dc-debug
#   tools/make_cdi.sh ROMFILE      then make the .cdi disc image with your ROM
#
# Works with or without "source /opt/toolchains/dc/kos/environ.sh" first: if the
# KOS environment is not set, every command runs through environ.sh.
#
# Compiles src/*.c (the verified game C) + platform/dc/** for the SH4 with
# -m4-single (64-bit double, so struct layouts match the N64), links with the
# generated block/alias linker scripts platform/dc/gen/*_sh.ld (implicit scripts, the
# KOS link script stays), and scrambles the binary into 1ST_READ.BIN.
#
# Build-time adaptations of two game files (no source change):
#   gfx.c       IO_REG16() -> VD_IO_REG16() (Aleck64 registers emulated from the pads)
#   ovl0_test.c SDRAM_TEST -> a small RAM area (the 4 MB SDRAM check of the test mode)
# and Overlay_Load from system.c is made weak (platform/dc/sys/overlay.c replaces it).

ROOT  := $(abspath $(dir $(lastword $(MAKEFILE_LIST)))/../..)
# DEBUG=<mask>: debug log categories (platform/dc/sys/vd_debug.h), built into build/dc-debug
DEBUG ?= 0
# VIEWER=1: photo-viewer build (boots straight to the unused photo, asset 0x13B0),
# built into build/dc-viewer
VIEWER ?= 0
# CHEAT=1: testing build with the L+R stage-clear cheat (platform/dc/ultra/os_cont.c),
# built into build/dc-cheat
CHEAT ?= 0
ifneq ($(strip $(VIEWER)),0)
BUILD := $(ROOT)/build/dc-viewer
else ifneq ($(strip $(CHEAT)),0)
BUILD := $(ROOT)/build/dc-cheat
else ifeq ($(strip $(DEBUG)),0)
BUILD := $(ROOT)/build/dc
else
BUILD := $(ROOT)/build/dc-debug
endif
KOS_BASE ?= /opt/toolchains/dc/kos

ifeq ($(strip $(KOS_CFLAGS)),)
KENV := . $(KOS_BASE)/environ.sh &&
endif

CC      := $(KENV) kos-cc
AS      := $(KENV) kos-cc
OBJCOPY := $(KENV) sh-elf-objcopy
HOSTCC  ?= cc
SCRAMBLE_SRC := $(KOS_BASE)/utils/scramble/scramble.c
SCRAMBLE     := $(BUILD)/tools/scramble

# ---- SH4ZAM (maths on the SH4's XMTRX/FTRV for the F3D interpreter) ----
SHZ_INC_CANDIDATES := $(KOS_BASE)/addons/include $(KOS_BASE)/../kos-ports/include \
                      $(KOS_BASE)/../kos-ports/sh4zam/inst/include $(KOS_BASE)/../src/sh4zam/include
SHZ_INC := $(firstword $(foreach d,$(SHZ_INC_CANDIDATES),$(if $(wildcard $(d)/sh4zam/shz_xmtrx.h),$(d))))
SHZ_LIB_CANDIDATES := $(KOS_BASE)/addons/lib/dreamcast $(KOS_BASE)/../kos-ports/lib \
                      $(KOS_BASE)/../kos-ports/sh4zam/inst/lib
SHZ_LIBDIR := $(firstword $(foreach d,$(SHZ_LIB_CANDIDATES),$(if $(wildcard $(d)/libsh4zam.a),$(d))))
ifneq ($(SHZ_INC),)
SHZ_CFLAGS := -DF3D_USE_SH4ZAM -isystem $(SHZ_INC)
endif
ifneq ($(SHZ_LIBDIR),)
SHZ_LIBS := -L$(SHZ_LIBDIR) -lsh4zam
endif

# ---- flags ----
INCS := -I$(ROOT)/src -I$(ROOT)/platform/dc -I$(ROOT)/platform/dc/sys -I$(ROOT)/platform/dc/gen \
        -I$(ROOT)/platform/dc/gfx -I$(ROOT)/platform/dc/ultra -I$(ROOT)/platform/dc/audio
DEFS := -DVD_PORT -DVD_DREAMCAST -DVD_DEBUG=$(DEBUG) $(if $(filter-out 0,$(VIEWER)),-DVD_PHOTO_VIEWER) $(if $(filter-out 0,$(CHEAT)),-DVD_CHEATS)
# exact float behaviour: no fused multiply-add (the VR4300 has none), no fast-math,
# libultra's own sinf/cosf (no compile-time folding with other results)
FPFLAGS := -m4-single -ffp-contract=off -fno-fast-math -fno-builtin-sinf -fno-builtin-cosf
CFLAGS_COMMON := -O2 $(FPFLAGS) -fno-strict-aliasing -fwrapv $(DEFS) $(INCS) $(SHZ_CFLAGS)
# the decompiled game C: keep going on the warnings newer GCCs turn into errors
GAME_CFLAGS := $(CFLAGS_COMMON) -w -Wno-error=incompatible-pointer-types -Wno-error=int-conversion \
               -Wno-error=implicit-function-declaration -Wno-error=return-mismatch
PORT_CFLAGS := $(CFLAGS_COMMON) -Wall -Wno-unused-function

# ---- sources ----
GAME_C   := $(sort $(wildcard $(ROOT)/src/*.c))
GAME_FIX := gfx.c ovl0_test.c
GAME_PLAIN := $(filter-out $(addprefix $(ROOT)/src/,$(GAME_FIX)),$(GAME_C))

PORT_C := $(sort $(wildcard $(ROOT)/platform/dc/ultra/*.c)) \
          $(filter-out $(ROOT)/platform/dc/sys/dataload.c,$(sort $(wildcard $(ROOT)/platform/dc/sys/*.c))) \
          $(ROOT)/platform/dc/sys/dataload.c \
          $(ROOT)/platform/dc/gen/vd_dataload_tables.c \
          $(ROOT)/platform/dc/gfx/f3d.c $(ROOT)/platform/dc/gfx/pvr_backend.c \
          $(sort $(wildcard $(ROOT)/platform/dc/audio/*.c))
PORT_S := $(ROOT)/platform/dc/sys/task_sh4.S
PYTHON ?= python3

GAME_OBJS := $(patsubst $(ROOT)/src/%.c,$(BUILD)/game/%.o,$(GAME_PLAIN)) \
             $(patsubst %.c,$(BUILD)/game/%.o,$(GAME_FIX))
PORT_OBJS := $(patsubst $(ROOT)/%.c,$(BUILD)/%.o,$(PORT_C)) \
             $(patsubst $(ROOT)/%.S,$(BUILD)/%.o,$(PORT_S))
LDSCRIPTS := $(ROOT)/platform/dc/gen/vd_blocks_sh.ld $(ROOT)/platform/dc/gen/vd_func_aliases_sh.ld \
             $(ROOT)/platform/dc/sys/vd_sys_sh.ld

ELF := $(BUILD)/vividdolls.elf
BIN := $(BUILD)/vividdolls.bin
FIRST_READ := $(BUILD)/1ST_READ.BIN

.PHONY: all clean
all: $(ELF) $(FIRST_READ)

# game C as it is
$(BUILD)/game/%.o: $(ROOT)/src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(GAME_CFLAGS) -c $< -o $@

# system.c: Overlay_Load is replaced by platform/dc/sys/overlay.c
$(BUILD)/game/system.o: $(ROOT)/src/system.c
	@mkdir -p $(dir $@)
	$(CC) $(GAME_CFLAGS) -c $< -o $@
	$(OBJCOPY) --weaken-symbol=_Overlay_Load $@

# gfx.c / ovl0_test.c: hardware addresses redirected (copies under build/dc/gen)
$(BUILD)/gen/gfx.c: $(ROOT)/src/gfx.c
	@mkdir -p $(dir $@)
	sed -e 's|^#define IO_REG16(a) .*|#define IO_REG16(a) VD_IO_REG16(a)|' $< > $@
	@grep -q 'VD_IO_REG16' $@ || { echo "gfx.c: IO_REG16 not found - update platform/dc/Makefile"; exit 1; }

$(BUILD)/gen/ovl0_test.c: $(ROOT)/src/ovl0_test.c
	@mkdir -p $(dir $@)
	sed -e 's|^#define SDRAM_TEST .*|#define SDRAM_TEST      (vd_sdram_test_area)|' \
	    -e 's|^#define SDRAM_WORDS .*|#define SDRAM_WORDS     VD_SDRAM_TEST_WORDS|' $< > $@

$(BUILD)/game/gfx.o $(BUILD)/game/ovl0_test.o: $(BUILD)/game/%.o: $(BUILD)/gen/%.c $(ROOT)/platform/dc/sys/vd_port_io.h
	@mkdir -p $(dir $@)
	$(CC) $(GAME_CFLAGS) -include $(ROOT)/platform/dc/sys/vd_port_io.h -c $< -o $@

# port layer
$(BUILD)/platform/dc/%.o: $(ROOT)/platform/dc/%.c
	@mkdir -p $(dir $@)
	$(CC) $(PORT_CFLAGS) -c $< -o $@

$(BUILD)/platform/dc/%.o: $(ROOT)/platform/dc/%.S
	@mkdir -p $(dir $@)
	$(AS) $(FPFLAGS) -c $< -o $@

$(ELF): $(GAME_OBJS) $(PORT_OBJS) $(LDSCRIPTS)
	$(CC) -o $@ $(GAME_OBJS) $(PORT_OBJS) $(LDSCRIPTS) $(SHZ_LIBS) -lm -Wl,-Map,$(BUILD)/vividdolls.map

$(BIN): $(ELF)
	$(OBJCOPY) -R .stack -O binary $< $@

$(SCRAMBLE): $(SCRAMBLE_SRC)
	@mkdir -p $(dir $@)
	$(HOSTCC) -O2 -o $@ $<

$(FIRST_READ): $(BIN) $(SCRAMBLE)
	$(SCRAMBLE) $< $@

# header dependencies (coarse): everything is rebuilt when a header changes
HEADERS := $(wildcard $(ROOT)/src/*.h $(ROOT)/platform/dc/*.h $(ROOT)/platform/dc/*/*.h)
$(GAME_OBJS) $(PORT_OBJS): $(HEADERS)

clean:
	rm -rf $(BUILD)
