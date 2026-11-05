"""Run original Vivid Dolls MIPS functions in the Unicorn emulator.

Used to check that decompiled C behaves exactly like the original code:
    emu = N64Emu(); v0, f0 = emu.call(0x80000450, f=[1.0, 2.0])
"""
import os, struct
# set LIBUNICORN_PATH if the Unicorn library isn't on the default search path
from unicorn import Uc, UC_ARCH_MIPS, UC_MODE_MIPS32, UC_MODE_BIG_ENDIAN, UC_HOOK_CODE
from unicorn.mips_const import *

ROM_PATH = os.path.join(os.path.dirname(__file__), '..', '..', 'baserom.z64')
RAM_BASE, RAM_SIZE = 0x80000000, 0x800000
RET_ADDR = 0x80000000              # 'ra' points here; reaching it ends the call
STACK_TOP = 0x807FF000
OVERLAYS = {0: (0x3ED30, 0x43AD0, 0), 1: (0x43AD0, 0x45270, 0x160),
            2: (0x45270, 0x602C0, 0x2500), 3: (0x602C0, 0x611F0, 0xD0)}

A_REGS = [UC_MIPS_REG_A0, UC_MIPS_REG_A1, UC_MIPS_REG_A2, UC_MIPS_REG_A3]


class N64Emu:
    def __init__(self, overlay=None):
        self.rom = open(ROM_PATH, 'rb').read()
        mu = self.mu = Uc(UC_ARCH_MIPS, UC_MODE_MIPS32 + UC_MODE_BIG_ENDIAN)
        mu.mem_map(0, RAM_SIZE)       # KSEG0 0x80000000 = physical 0, as on the N64
        self.write(0x80000400, self.rom[0x1000:0x3ED30])        # main code + data
        self.mu = mu
        if overlay is not None:
            s, e, bss = OVERLAYS[overlay]
            self.write(0x800E0000, self.rom[s:e])
        # enable the FPU (Status.CU1) and use FR=0 like the N64 o32 ABI
        st = mu.reg_read(UC_MIPS_REG_CP0_STATUS)
        mu.reg_write(UC_MIPS_REG_CP0_STATUS, st | (1 << 29))
        self.stubs = {}
        # stubs are hooked only at their own addresses (a hook on every instruction
        # would run a Python callback per instruction and be ~15x slower)

    # -- memory helpers --
    @staticmethod
    def phys(a): return a & 0x1FFFFFFF
    def w32(self, a, v): self.mu.mem_write(self.phys(a), struct.pack('>I', v & 0xFFFFFFFF))
    def r32(self, a): return struct.unpack('>I', self.mu.mem_read(self.phys(a), 4))[0]
    def w16(self, a, v): self.mu.mem_write(self.phys(a), struct.pack('>H', v & 0xFFFF))
    def r16(self, a): return struct.unpack('>H', self.mu.mem_read(self.phys(a), 2))[0]
    def wf(self, a, v): self.mu.mem_write(self.phys(a), struct.pack('>f', v))
    def rf(self, a): return struct.unpack('>f', self.mu.mem_read(self.phys(a), 4))[0]
    def write(self, a, b): self.mu.mem_write(self.phys(a), bytes(b))
    def read(self, a, n): return bytes(self.mu.mem_read(self.phys(a), n))

    def stub(self, addr, fn):
        """Replace the function at addr with a Python callback fn(emu) -> v0 (or None)."""
        if addr not in self.stubs:
            self.mu.hook_add(UC_HOOK_CODE, self._hook, begin=addr, end=addr)
        self.stubs[addr] = fn

    def _hook(self, mu, addr, size, user):
        fn = self.stubs.get(addr)
        if fn is not None:
            r = fn(self)
            if r is not None:
                mu.reg_write(UC_MIPS_REG_V0, r & 0xFFFFFFFF)
            mu.reg_write(UC_MIPS_REG_PC, mu.reg_read(UC_MIPS_REG_RA))

    def arg(self, i):
        return self.mu.reg_read(A_REGS[i])

    def call(self, addr, a=(), f=(), d=(), stack=(), max_insns=50_000_000):
        """Call addr. a = integer args (a0..a3, then stack), f = float args
        (f12, f14), d = double args (f12, f14). Returns (v0, f0 as float)."""
        mu = self.mu
        sp = STACK_TOP - 0x100
        for i, v in enumerate(list(a)[4:] + list(stack)):
            self.w32(sp + 0x10 + 4 * i, v)
        for i, v in enumerate(list(a)[:4]):
            mu.reg_write(A_REGS[i], v & 0xFFFFFFFF)
        for i, v in enumerate(f):
            mu.reg_write(UC_MIPS_REG_F12 + 2 * i, struct.unpack('>I', struct.pack('>f', v))[0])
        for i, v in enumerate(d):
            hi, lo = struct.unpack('>II', struct.pack('>d', v))
            mu.reg_write(UC_MIPS_REG_F12 + 2 * i, lo)      # FR=0: low word in even reg
            mu.reg_write(UC_MIPS_REG_F13 + 2 * i, hi)
        mu.reg_write(UC_MIPS_REG_SP, sp)
        mu.reg_write(UC_MIPS_REG_RA, RET_ADDR)
        mu.emu_start(addr, RET_ADDR, count=max_insns)
        v0 = mu.reg_read(UC_MIPS_REG_V0)
        f0 = struct.unpack('>f', struct.pack('>I', mu.reg_read(UC_MIPS_REG_F0) & 0xFFFFFFFF))[0]
        return v0, f0

    def f0_bits(self):
        return self.mu.reg_read(UC_MIPS_REG_F0) & 0xFFFFFFFF
