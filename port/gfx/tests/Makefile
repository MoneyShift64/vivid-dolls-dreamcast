# Host tests for the F3D interpreter: make -C port/gfx/tests
CC ?= cc
CFLAGS ?= -std=c99 -O1 -g -Wall -Wextra -Wno-unused-parameter
SRC = test_f3d.c ../f3d.c ../null_backend.c

test_f3d: $(SRC) ../f3d.h ../f3d_backend.h ../null_backend.h
	$(CC) $(CFLAGS) -o $@ $(SRC) -lm

test_rom_dls: test_rom_dls.c ../f3d.c ../null_backend.c ../f3d.h ../f3d_backend.h
	$(CC) $(CFLAGS) -o $@ test_rom_dls.c ../f3d.c ../null_backend.c -lm

ROM ?= ../../../baserom.z64

test_texfill: test_texfill.c ../pvr_texfill.h ../f3d_backend.h
	$(CC) $(CFLAGS) -I.. -o $@ test_texfill.c

run: test_f3d test_rom_dls test_texfill
	./test_f3d
	./test_texfill
	./test_rom_dls $(ROM)

clean:
	rm -f test_f3d test_rom_dls test_f3d_shz test_texfill seam_lib.so replay-new.so replay-ref.so
# same tests with the SH4ZAM maths hooks, using SH4ZAM's portable XMTRX
# emulation (checks the multiply order used on the Dreamcast)
SHZ ?= /opt/toolchains/dc/sh4zam
test_f3d_shz: $(SRC) ../f3d.h
	$(CC) -std=gnu11 -O1 -g -DF3D_USE_SH4ZAM -I$(SHZ)/include -o $@ $(SRC) $(SHZ)/source/sw/shz_xmtrx_sw.c -lm

run-shz: test_f3d_shz
	./test_f3d_shz

.PHONY: run run-shz clean

# in-process differential replay (port/gfx/tools/replay_check.py): one shared
# library per interpreter version; F3D_REF = the reference f3d.c to compare with
F3D_REF ?= ../f3d.c
RP_FLAGS = -std=gnu11 -O2 -fPIC -shared -fvisibility=hidden -ffp-contract=off -Wall -Wno-unused-function
replay-new.so: replay_lib.c ../f3d.c ../f3d.h ../f3d_backend.h
	$(CC) $(RP_FLAGS) -DRP_STATS -I.. -o $@ replay_lib.c ../f3d.c -lm
replay-ref.so: replay_lib.c $(F3D_REF) ../f3d.h ../f3d_backend.h
	$(CC) $(RP_FLAGS) -I$(dir $(F3D_REF)) -o $@ replay_lib.c $(F3D_REF) -lm
replay: replay-new.so replay-ref.so
.PHONY: replay

# software model of the PVR output vs the RDP for the photo strips
# (port/gfx/tools/seam_check.py)
seam_lib.so: seam_lib.c ../f3d.c ../f3d.h ../f3d_backend.h ../pvr_texfill.h
	$(CC) $(RP_FLAGS) -I.. -o $@ seam_lib.c ../f3d.c -lm
