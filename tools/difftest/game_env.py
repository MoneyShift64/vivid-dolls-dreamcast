"""Common emulator setup for testing game code: heap at 0x80173600, ROM reads
served straight from the ROM image (Rom_Read is replaced by a Python stub)."""
from emu import N64Emu

GAME_HEAP = 0x80173600
GAME_HEAP_SIZE = 0x200000
ROM_READ = 0x8000BFE8
HEAP_INIT = 0x80003300


def make_env(overlay=None):
    emu = N64Emu(overlay=overlay)

    def rom_read(e):
        src, dst, size = e.arg(0), e.arg(1), e.arg(2)
        e.write(dst, e.rom[src:src + size])
        return 0
    emu.stub(ROM_READ, rom_read)
    return emu


def reset_heap(emu):
    emu.write(GAME_HEAP, bytes(0x40))
    emu.call(HEAP_INIT, a=[GAME_HEAP, GAME_HEAP_SIZE])
