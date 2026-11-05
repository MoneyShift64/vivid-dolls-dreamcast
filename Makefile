# Vivid Dolls (Aleck64) decompilation - rebuilds the ROM and checks it matches.
#
#   make setup      split baserom.z64 into asm/ and assets/ (run once)
#   make            rebuild build/vividdolls.z64 and compare its SHA1
#   make port-test  build + run the portable C asset code against the ROM
#   make clean      delete build/

BASENAME := vividdolls
BUILD    := build
ROM      := $(BUILD)/$(BASENAME).z64
ELF      := $(BUILD)/$(BASENAME).elf

PYTHON  ?= python3
HOSTCC  ?= cc

# ---- find a MIPS toolchain (Linux: mips-linux-gnu-, Mac Homebrew: mips64-elf-) ----
find-command = $(shell command -v $(1) 2> /dev/null)
ifeq ($(CROSS),)
  ifneq ($(call find-command,mips-linux-gnu-ld),)
    CROSS := mips-linux-gnu-
  else ifneq ($(call find-command,mips64-linux-gnu-ld),)
    CROSS := mips64-linux-gnu-
  else ifneq ($(call find-command,mips64-elf-ld),)
    CROSS := mips64-elf-
  endif
endif

AS      := $(CROSS)as
LD      := $(CROSS)ld
OBJCOPY := $(CROSS)objcopy

# Linux-flavoured binutils call the format "tradbigmips", plain ELF ones "bigmips"
ifneq ($(findstring linux,$(CROSS)),)
  BINFMT := elf32-tradbigmips
  LDEMU  := elf32btsmip
else
  BINFMT := elf32-bigmips
  LDEMU  := elf32ebmip
endif

ASFLAGS := -march=vr4300 -mabi=32 -mno-pdr -G 0 -I include -EB
LDFLAGS := -m $(LDEMU) -T $(BASENAME).ld -Map $(BUILD)/$(BASENAME).map --no-check-sections

S_FILES   := $(shell find asm -name '*.s' 2>/dev/null)
BIN_FILES := $(shell find assets -name '*.bin' 2>/dev/null)
O_FILES   := $(S_FILES:%.s=$(BUILD)/%.o) $(BIN_FILES:%.bin=$(BUILD)/%.o)

AUTO_LD := $(wildcard undefined_syms_auto.txt undefined_funcs_auto.txt absolute_syms.txt)
LDFLAGS += $(foreach f,$(AUTO_LD),-T $(f))

all: toolcheck $(ROM) check

toolcheck:
	@if [ -z "$(CROSS)" ]; then echo "No MIPS binutils found - see README.md step 1"; exit 1; fi
	@if [ ! -d asm ]; then echo "Run 'make setup' first"; exit 1; fi

setup:
	$(PYTHON) -m splat split $(BASENAME).yaml
	$(PYTHON) tools/fix_labels.py

check: $(ROM)
	@$(PYTHON) tools/check.py $(ROM) $(BASENAME).sha1

$(ROM): $(ELF)
	$(OBJCOPY) -O binary --gap-fill=0x00 $< $@

$(ELF): $(O_FILES) $(BASENAME).ld
	$(LD) $(LDFLAGS) -o $@

$(BUILD)/%.o: %.s
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -o $@ $<

$(BUILD)/%.o: %.bin
	@mkdir -p $(dir $@)
	$(OBJCOPY) -I binary -O $(BINFMT) -B mips $< $@

# ---- portable C code (used by the Dreamcast port) ----
PORT_SRC := port/vd_assets.c port/vd_jpeg64.c port/tests/host_test.c

port/vd_jpeg64_tables.h: tools/gen_port_tables.py
	$(PYTHON) tools/gen_port_tables.py

$(BUILD)/host_test: $(PORT_SRC) port/vd_types.h port/vd_assets.h port/vd_jpeg64.h port/vd_jpeg64_tables.h
	@mkdir -p $(BUILD)
	$(HOSTCC) -std=c99 -O2 -Wall -Wextra -ffp-contract=off -o $@ $(PORT_SRC) -lm

port-test: $(BUILD)/host_test
	$(BUILD)/host_test baserom.z64

clean:
	rm -rf $(BUILD)

.PHONY: all toolcheck setup check port-test clean
