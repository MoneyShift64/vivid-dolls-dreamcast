#!/usr/bin/env python3
"""Differential test of src/game/system.c (TLB setup, task pool, guMtxF2L/L2F,
UART, boot/thread entry points, ROM/overlay DMA, input thread, RCP scheduler)
against the original MIPS code.

Every libultra/OS call and every callee in other areas is replaced by a Python
stub that records its name and arguments (stack addresses are recorded as 'STK',
addresses of C functions are translated to the original function's address).
The UART registers at 0x02000040 are an MMIO device that serves scripted status
values and records every access. Before each call the data+BSS, the test memory
and any extra regions are restored from a snapshot and set up identically from
the same random seed; afterwards the return value, the call/IO log and all
those regions are compared. Thread functions that never return are stopped when
their message script runs out (or after an instruction budget for spin loops)."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
import emu as EMU
from unicorn.mips_const import UC_MIPS_REG_SP
from game_env import make_env
from mipsbuild import CODE_BASE
from draft_mips import build_clean, symbol_map

DATA, DATA_SIZE = 0x80038000, 0x800C5400 - 0x80038000     # .data tail + whole BSS
TEST, TEST_SIZE = 0x80400000, 0x80000
STACK_LO, STACK_HI = 0x807FD600, 0x80800000   # (frame buffer 1 ends at 0x807FD600)
UART = 0x02000000
SCREEN_W = 0x800C30D0
FB0, FB1, FB_SIZE = 0x807B2600, 0x807D7E00, 0x25800
OVL, OVL_SIZE = 0x800E0000, 0x4D000
TASK_POOL_MEM = 0x8069E600

ORIG = {
    'Tlb_ClearAll': 0x8000A5A0, 'Tlb_Map': 0x8000A628, 'Task_InitPool': 0x8000A6D0,
    'Task_Create': 0x8000A76C, 'Task_Free': 0x8000A8D0, 'Mtx_ToFixed': 0x8000A924,
    'Mtx_FromFixed': 0x8000A9E8, 'Math_AbsF': 0x8000AAE0, 'Uart_Init': 0x8000AAF0,
    'Uart_PutByte': 0x8000ABA0, 'Uart_GetByte': 0x8000AC24, 'Uart_ReadData': 0x8000AC88,
    'Uart_ReadStatus': 0x8000AC98, 'Uart_Delay': 0x8000ACA4, 'Main_Boot': 0x8000B210,
    'Main_IdleThread': 0x8000B2D0, 'Input_Thread': 0x8000B3C8, 'Main_GfxThread': 0x8000BD28,
    'Rom_Read': 0x8000BFE8, 'Overlay_Load': 0x8000C0D4, 'Main_ForceLink': 0x8000C218,
    'Main_InitSystems': 0x8000C2A8, 'Cont_Thread': 0x8000C424, 'Fault_Thread': 0x8000C510,
    'Sched_Create': 0x8000C590, 'Sched_AddClient': 0x8000C6D8, 'Sched_RemoveClient': 0x8000C730,
    'Sched_GetCmdQ': 0x8000C7C0, 'Sched_Main': 0x8000C7C8, 'Sched_HandleRetrace': 0x8000C9D8,
    'Sched_HandleRSP': 0x8000CADC, 'Sched_HandleRDP': 0x8000CBCC, 'Sched_TaskReady': 0x8000CC5C,
    'Sched_TaskComplete': 0x8000CCB0, 'Sched_AppendList': 0x8000CD8C, 'Sched_Exec': 0x8000CDE4,
    'Sched_Yield': 0x8000CEF8, 'Sched_Schedule': 0x8000CF30,
}

# stubbed callees: name -> (arg indices to record, default return)
STUBS = {
    'osCreateThread': (range(6), 0), 'osStartThread': ([0], 0), 'osSetThreadPri': ([0, 1], 0),
    'osCreateMesgQueue': ([0, 1, 2], 0), 'osRecvMesg': ([0, 1, 2], 0), 'osSendMesg': ([0, 1, 2], 0),
    'osSetEventMesg': ([0, 1, 2], 0), 'osCreatePiManager': ([0, 1, 2, 3], 0),
    'osInvalDCache': ([0, 1], 0), 'osInvalICache': ([0, 1], 0), 'osWritebackDCache': ([0, 1], 0),
    'osSetIntMask': ([0], 0), 'osCreateViManager': ([0], 0), 'osViSetMode': ([0], 0),
    'osViBlack': ([0], 0), 'osViSetEvent': ([0, 1, 2], 0), 'osSpTaskYield': ([], 0),
    'osSpTaskYielded': ([0], 0), 'osViGetCurrentFramebuffer': ([], 0),
    'osViGetNextFramebuffer': ([], 0), '__osContRamRead': ([0, 1, 2, 3], 0),
    'func_8001CAC0': ([], 0), 'func_8001C990': ([], 0), 'func_80025950': (range(7), 0),
    'func_80021280': ([0], 0), 'func_8002148C': ([0], 0), 'func_800211E0': ([0, 2, 3], 0),
    'func_80021B00': ([0], 0), 'func_8001D340': ([], 0), 'func_80021FC0': ([0], 0),
    'func_80025DC0': ([0, 1], 0), 'func_8001D350': ([0, 1], 0), 'func_80021EA0': ([0, 1], 0),
    'func_80024750': ([0, 1, 2], 0), 'func_80023F90': ([0, 1, 2], 0), 'func_800221A0': ([0, 1], 0),
    'func_800236C0': (range(5), 0),
    'func_8000ADA4': ([0, 1], None), 'func_8000B150': ([0], 0),
    'func_8000F3E0': ([], 0), 'func_8000F6EC': ([], 0), 'func_8000F750': ([], 0),
    'func_8000F8F4': ([], 0), 'func_8000FA34': ([], 0), 'func_8000FA4C': ([], 0),
    'func_8000F890': ([], 0), 'func_8000F350': ([], 0), 'func_8000FDB0': ([], 0),
    'func_8000FF64': ([0], 0), 'func_8000D150': ([0], 0), 'func_80014E90': ([], 0),
    'func_800154D0': ([], 0), 'func_8001BB18': ([0], 0), 'func_8001BE80': ([], 0),
    'func_8001BE88': ([], 0), 'func_8001BE90': ([], 0), 'func_8001BE98': ([0], 0),
    'func_8001BEA0': ([], 0), 'func_8001BF08': ([], 0), 'Audio_Init': ([], 0),
    'Main_ParseArgs': ([0], 0), 'Camera_Init': ([], 0), 'Light_Init': ([0], 0),
    'Heap_Init': ([0, 1], 0), 'AllocList_Init': ([0, 1], 0), 'AllocList_FreeAll': ([0, 1], 0),
    'Scene_Init': ([], 0), 'ObjList_Init': ([], 0), 'Ring_SetStop': ([0], 0),
    'Scene_ConvertMatrices': ([0], 0),
}


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/system.c'], name='system')
        smap = symbol_map()
        e = self.emu = make_env()
        e.stubs.pop(0x8000BFE8, None)          # we test the real Rom_Read
        e.write(CODE_BASE, blob)
        self.c2o = {self.syms[n]: a for n, a in ORIG.items()}
        self.log = []
        self.cfg = {}
        for name, (idx, ret) in STUBS.items():
            addr = smap.get(name) or int(name[5:], 16)
            e.stub(addr, self._make_stub(name, list(idx), ret))
        e.mu.mmio_map(UART, 0x1000, self._io_read, None, self._io_write, None)
        e.w32(SCREEN_W, 320)
        self.snap = [(DATA, e.read(DATA, DATA_SIZE)), (TEST, bytes(TEST_SIZE))]
        self.big = {r: e.read(r[0], r[1]) for r in [(OVL, OVL_SIZE), (FB0, FB_SIZE), (FB1, FB_SIZE),
                                                    (TASK_POOL_MEM, 0x8000)]}

    # -- stubs
    def norm(self, v):
        if STACK_LO <= v < STACK_HI:
            return 'STK'
        return self.c2o.get(v, v)

    def _args(self, e, idx):
        sp = e.mu.reg_read(UC_MIPS_REG_SP)
        return tuple(self.norm(e.arg(i) if i < 4 else e.r32(sp + 0x10 + 4 * (i - 4))) for i in idx)

    def _make_stub(self, name, idx, ret):
        def fn(e):
            a = self._args(e, idx)
            self.log.append((name,) + a)
            h = self.cfg.get(name)
            if h is not None:
                return h(e)
            if name == 'func_8000ADA4':        # Tlb_Write: record the entry, return it
                self.log.append(('tlb',) + struct.unpack('>4I', e.read(e.arg(1), 16)))
                return e.arg(1)
            if name == 'func_8000B150':        # Task_SaveContext: fill frame with a pattern
                e.write(e.arg(0), bytes((i * 7 + 3) & 0xFF for i in range(0xC0)))
                return 0
            if name == 'func_80025950':        # osPiStartDma: do the copy
                sp = e.mu.reg_read(UC_MIPS_REG_SP)
                dev, va, n = e.arg(3), e.r32(sp + 0x10), e.r32(sp + 0x14)
                e.write(va, e.rom[dev:dev + n].ljust(n, b'\0'))
                return 0
            if name == 'osRecvMesg':
                return self.recv(e)
            if name == 'osSetIntMask':
                return (e.arg(0) * 0x1F3 + 0x55) & 0xFFFF
            return ret
        return fn

    def recv(self, e):
        """osRecvMesg: next (ret, msg) from the script (a list, or a dict per queue).
        Empty: non-blocking -> -1; blocking -> stop the run (threads) or return 0."""
        lst = self.script.get(e.arg(0), []) if isinstance(self.script, dict) else self.script
        if not lst:
            if e.arg(2) == 0:
                return 0xFFFFFFFF
            if self.stop_when_empty:
                e.mu.emu_stop()
            return 0
        r, m = lst.pop(0)
        if r == 0 and e.arg(1):
            e.w32(e.arg(1), m)
        return r

    def _io_read(self, uc, off, size, ud):
        if off == 0x42:
            v = self.io_status.pop(0) if self.io_status else 0
        else:
            v = self.io_data
        self.log.append(('ioR', off, size, v))
        return v

    def _io_write(self, uc, off, size, value, ud):
        self.log.append(('ioW', off, size, value))

    # -- runs
    def restore(self, extra):
        for a, b in self.snap:
            self.emu.write(a, b)
        for r in extra:
            self.emu.write(r[0], self.big[r])
        self.log, self.cfg, self.script, self.stop_when_empty = [], {}, [], True
        self.io_status, self.io_data = [], 0

    def run(self, name, addr, seed):
        case = CASES[name]
        rng = random.Random(seed * 7919 + sum(name.encode()))
        extra = getattr(case, 'extra', [])
        self.restore(extra)
        spec = case(self, rng) or {}
        v0, _ = self.emu.call(addr, a=spec.get('a', []), f=spec.get('f', []),
                              max_insns=spec.get('max_insns', 20_000_000))
        res = [self.emu.f0_bits() if spec.get('float') else (v0 if spec.get('ret', True) else None),
               list(self.log)]
        for a, n in [(DATA, DATA_SIZE), (TEST, TEST_SIZE)] + list(extra):
            res.append(self.emu.read(a, n))
        return res

    def o(self, name, *a):
        return self.emu.call(ORIG[name], a=list(a))[0]


# ------------------------------------------------------------------ helpers

class Mem:
    def __init__(self, emu, base=TEST + 0x40000):
        self.emu, self.p = emu, base

    def alloc(self, data, align=8):
        self.p = (self.p + align - 1) & ~(align - 1)
        a = self.p
        self.emu.write(a, data)
        self.p += max(len(data), 4)
        return a

    def words(self, ws):
        return self.alloc(b''.join(struct.pack('>I', w & 0xFFFFFFFF) for w in ws))


def rbytes(rng, n):
    return bytes(rng.getrandbits(8) for _ in range(n))


def msg(m, typ):
    return m.alloc(struct.pack('>h', typ) + bytes(6))


fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
CASES = {}


def case(name, extra=()):
    def deco(fn):
        fn.extra = list(extra)
        CASES[name] = fn
        return fn
    return deco


# ------------------------------------------------------------------ TLB, tasks, maths

@case('Tlb_ClearAll')
def _(env, rng):
    return {'ret': False}


@case('Tlb_Map')
def _(env, rng):
    return {'a': [rng.randrange(32), rng.getrandbits(32), rng.getrandbits(32),
                  rng.choice([0, 1, 2, 3, 4, 5, 6, 7, rng.randrange(8)]),
                  rng.choice([0, 1, 0x100, 0xFFFFFFFF])]}


@case('Task_InitPool')
def _(env, rng):
    pool = TEST + rng.randrange(0, 0x100) * 4
    bs = rng.choice([0x10, 0x40, 0x100, 0x4000, rng.randrange(1, 0x3000)])
    size = rng.choice([0x30, 0x31, 0x40, 0x30 + bs + 4, rng.randrange(0x30, 0x30000), rng.randrange(0, 0x30)])
    if size < 0x30:            # n wraps to a huge count: keep it small enough to avoid overrun
        size = 0x30
    return {'a': [pool, size, bs], 'ret': False}


def make_pool(env, rng, m):
    """Pool via the original Task_InitPool, with some tasks created by the original."""
    pool = TEST + 0x100
    bs = rng.choice([0x100, 0x200, 0x400])
    nblk = rng.randrange(1, 12)
    env.o('Task_InitPool', pool, 0x30 + nblk * (bs + 4) + rng.randrange(0, 4), bs)
    tasks = []
    for _ in range(rng.randrange(0, nblk + 1)):
        t = env.emu.call(ORIG['Task_Create'], a=[rng.getrandbits(32), rng.randrange(-3, 4), 8,
                                                 rng.getrandbits(32), rng.getrandbits(32)])[0]
        if t:
            tasks.append(t)
    env.log.clear()
    return tasks


@case('Task_Create')
def _(env, rng):
    m = Mem(env.emu)
    make_pool(env, rng, m)
    nargs = rng.choice([0, 1, 2, 3, 4, 5, 8, 12, 15, 16, 17, 20])
    args = [rng.getrandbits(32) for _ in range(nargs)]
    return {'a': [rng.getrandbits(32), rng.choice([-5, -3, 0, 1, 2, 3, rng.getrandbits(32)]), nargs * 4] + args}


@case('Task_Free')
def _(env, rng):
    m = Mem(env.emu)
    tasks = make_pool(env, rng, m)
    while not tasks:
        tasks = make_pool(env, rng, m)
    return {'a': [rng.choice(tasks)]}


@case('Mtx_ToFixed')
def _(env, rng):
    vals = [rng.choice([0.0, -0.0, 1.0, -1.0, 0.5, rng.uniform(-1, 1), rng.uniform(-32767, 32767),
                        rng.uniform(-2, 2), 1.0 / 65536, -1.0 / 65536, 32767.99, -32768.0])
            for _ in range(16)]
    mf = TEST + 0x1000
    env.emu.write(mf, struct.pack('>16f', *vals))
    out = TEST + 0x2000 + rng.choice([0, 0, 4])
    env.emu.write(out, rbytes(rng, 0x40))
    return {'a': [out, mf], 'ret': False}


@case('Mtx_FromFixed')
def _(env, rng):
    src = TEST + 0x1000
    env.emu.write(src, rbytes(rng, 0x40) if rng.random() < 0.8 else bytes(0x40))
    out = TEST + 0x2000
    return {'a': [out, src], 'ret': False}


@case('Math_AbsF')
def _(env, rng):
    v = rng.choice([0.0, -0.0, 1.5, -1.5, float('inf'), float('-inf'), rng.uniform(-1e6, 1e6),
                    struct.unpack('>f', struct.pack('>I', rng.getrandbits(32)))[0]])
    return {'f': [v], 'float': True}


# ------------------------------------------------------------------ UART

@case('Uart_Init')
def _(env, rng):
    return {'ret': False}


def status_script(env, rng, bit):
    k = rng.choice([0, 1, 2, 5, 50, 999, 1000, 1001, 1100])
    env.io_status = [rng.getrandbits(16) & ~bit for _ in range(k)] + [rng.getrandbits(16) | bit]
    env.io_data = rng.getrandbits(16)


@case('Uart_PutByte')
def _(env, rng):
    status_script(env, rng, 1)
    return {'a': [rng.getrandbits(32)]}


@case('Uart_GetByte')
def _(env, rng):
    status_script(env, rng, 2)
    return {}


@case('Uart_ReadData')
def _(env, rng):
    env.io_data = rng.getrandbits(16)
    return {}


@case('Uart_ReadStatus')
def _(env, rng):
    env.io_status = [rng.getrandbits(16)]
    return {}


@case('Uart_Delay')
def _(env, rng):
    return {'a': [rng.choice([0, 1, 2, 10, rng.randrange(50)])], 'ret': False}


# ------------------------------------------------------------------ boot / threads

@case('Main_Boot')
def _(env, rng):
    return {'a': [rng.getrandbits(32)], 'ret': False}


@case('Main_IdleThread')
def _(env, rng):
    env.emu.w32(0x800B04C8, rng.choice([0, 1, rng.getrandbits(32)]))     # gArgDebug
    return {'a': [rng.getrandbits(32)], 'ret': False, 'max_insns': 20000}


def rand_globals(env, rng, spec):
    for addr, size, choices in spec:
        v = rng.choice(choices) if choices else rng.getrandbits(8 * size)
        if size == 4:
            env.emu.w32(addr, v)
        else:
            env.emu.w16(addr, v)


def seq_returns(env, rng, name, choices):
    vals = [rng.choice(choices) for _ in range(64)]
    st = {'i': 0}

    def fn(e):
        v = vals[st['i'] % 64]
        st['i'] += 1
        return v
    env.cfg[name] = fn


@case('Input_Thread')
def _(env, rng):
    m = Mem(env.emu)
    env.script = [(0, msg(m, rng.choice([1, 1, 1, 4, 0, 2, 5, -1]))) for _ in range(rng.randrange(1, 8))]
    seq_returns(env, rng, 'func_8000F750', [0, 1, 0x100])
    btn = lambda: rng.choice([0, 0xC00, 0x300, 0xF00, rng.getrandbits(16), 0x8000 | rng.getrandbits(12)])
    spec = [(0x80043310, 4, [0, 1]), (0x800B04D0, 4, [0, 1]), (0x800B0430, 4, [0, 1]),
            (0x800B0720, 4, [0, 1, 2, 2, 2, 3, -1]), (0x800C1674, 4, [0, 0, 1]),
            (0x800C53D8, 2, [0, 0, 1]), (0x800C53DA, 2, [0, 0, 1]), (0x800C1898, 2, [0, 0, 1]),
            (0x800C30C8, 4, [0, 50, 97, 98, 99, 100, 0xFFFFFFFF]), (0x800B0574, 2, [0, 0, 3]),
            (0x800B0576, 2, [0, 0, 3]), (0x80043314, 4, None), (0x800C53F8, 4, None)]
    for a in (0x800B058C, 0x800C5334, 0x800B0596, 0x800B043A, 0x800AF222, 0x800AF238,
              0x800B0498, 0x800C532C, 0x800B4A80):
        for i in range(2):
            spec.append((a + 2 * i, 2, [btn()]))
    rand_globals(env, rng, spec)
    return {'a': [rng.getrandbits(32)], 'ret': False}


@case('Main_GfxThread')
def _(env, rng):
    m = Mem(env.emu)
    env.script = [(0, msg(m, rng.choice([1, 1, 1, 2, 2, 0, 3]))) for _ in range(rng.randrange(1, 12))]
    rand_globals(env, rng, [(0x800C53F8, 4, [0, 0, 1, 2, 3]), (0x800C316C, 4, None), (0x800C30C0, 4, None),
                            (0x800C0AB4, 4, None), (0x800AF234, 4, None)])
    return {'a': [rng.getrandbits(32)], 'ret': False}


@case('Rom_Read')
def _(env, rng):
    size = rng.choice([0, 1, 4, 0x3FF, 0x400, 0x401, 0x800, 0x1234, rng.randrange(0, 0x8000)])
    src = rng.randrange(0x1000, 0x3E000)
    dst = TEST + rng.randrange(0, 0x100) * 4
    env.stop_when_empty = False
    return {'a': [src, dst, size], 'ret': False}


@case('Overlay_Load', extra=[(OVL, OVL_SIZE)])
def _(env, rng):
    idx = rng.randrange(4)
    if rng.random() < 0.5:                 # made-up table entry: random size / destination
        rom = rng.randrange(0x1000, 0x30000)
        env.emu.write(0x8003A280 + idx * 16, struct.pack('>4I', rng.choice([OVL, OVL + 0x40, TEST]), 0, rom,
                                                         rom + rng.choice([0, 1, 0x400, 0x401, rng.randrange(0x8000)])))
    env.stop_when_empty = False
    return {'a': [idx], 'ret': False}


@case('Main_ForceLink')
def _(env, rng):
    return {'ret': False}


@case('Main_InitSystems', extra=[(TASK_POOL_MEM, 0x8000)])
def _(env, rng):
    return {'ret': False}


@case('Cont_Thread')
def _(env, rng):
    m = Mem(env.emu)
    env.script = [(0, msg(m, rng.choice([5, 5, 5, 1, 0, 4]))) for _ in range(rng.randrange(1, 10))]
    seq_returns(env, rng, 'func_8000F750', [0, 1])
    rand_globals(env, rng, [(0x800AF254, 4, [0, 1])])
    return {'a': [rng.getrandbits(32)], 'ret': False}


@case('Fault_Thread')
def _(env, rng):
    v = rng.choice([0, 0x80123450, rng.getrandbits(32)])
    env.cfg['func_8001D340'] = lambda e: v
    env.script = [(0, rng.getrandbits(32))]
    env.stop_when_empty = False
    return {'a': [rng.getrandbits(32)], 'ret': False, 'max_insns': 20000}


# ------------------------------------------------------------------ scheduler

SC = TEST + 0x1000          # OSSched (0x284 bytes)


def make_task(env, rng, m, typ=None):
    outsize = m.alloc(rbytes(rng, 8))
    t = m.alloc(bytes(0x58))
    e = env.emu
    e.w32(t + 0x4, rng.getrandbits(6))
    e.w32(t + 0x8, rng.getrandbits(7))
    e.w32(t + 0xC, rng.getrandbits(32))
    e.write(t + 0x10, rbytes(rng, 0x40))
    e.w32(t + 0x10, typ if typ is not None else rng.choice([1, 1, 2, 2, 3]))
    e.w32(t + 0x3C, outsize)
    e.w32(t + 0x50, rng.getrandbits(32))
    e.w32(t + 0x54, rng.getrandbits(32))
    return t


def make_list(env, rng, m, n, typ=None):
    ts = [make_task(env, rng, m, typ) for _ in range(n)]
    for a, b in zip(ts, ts[1:] + [0]):
        env.emu.w32(a, b)
    return ts


def make_sched(env, rng, m, need_rsp=False, need_rdp=False):
    e = env.emu
    e.write(SC, rbytes(rng, 0x284))
    gfx = make_list(env, rng, m, rng.choice([0, 1, 2, 3]))
    aud = make_list(env, rng, m, rng.choice([1, 1, 2]), 2)
    if rng.random() < 0.2:
        aud = []
    e.w32(SC + 0x264, aud[0] if aud else 0)
    e.w32(SC + 0x26C, aud[-1] if aud else 0)
    e.w32(SC + 0x268, gfx[0] if gfx else 0)
    e.w32(SC + 0x270, gfx[-1] if gfx else 0)
    cur = make_task(env, rng, m)
    e.w32(SC + 0x274, cur if (need_rsp or rng.random() < 0.5) else 0)
    e.w32(SC + 0x278, (cur if rng.random() < 0.5 else make_task(env, rng, m))
          if (need_rdp or rng.random() < 0.5) else 0)
    e.w32(SC + 0x280, rng.choice([0, 1, 5]) if aud else 0)
    clients = [m.words([0, rng.getrandbits(32)]) for _ in range(rng.choice([0, 1, 2, 3]))]
    for a, b in zip(clients, clients[1:] + [0]):
        e.w32(a, b)
    e.w32(SC + 0x260, clients[0] if clients else 0)
    fbv = [rng.choice([0x80300000, 0x80340000]) for _ in range(2)]
    env.cfg['osViGetCurrentFramebuffer'] = lambda e_: fbv[0]
    env.cfg['osViGetNextFramebuffer'] = lambda e_: fbv[1]
    yv = rng.choice([0, 1])
    env.cfg['osSpTaskYielded'] = lambda e_: yv
    rand_globals(env, rng, [(0x80043320, 4, [0, 1, 2, 3]), (0x80038478, 4, [0, 1])])
    return gfx, aud, clients


@case('Sched_Create')
def _(env, rng):
    env.emu.write(SC, rbytes(rng, 0x284))
    return {'a': [SC, rng.getrandbits(32), rng.getrandbits(32), rng.randrange(0, 56), rng.getrandbits(8)],
            'ret': False}


@case('Sched_AddClient')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m)
    return {'a': [SC, m.alloc(rbytes(rng, 8)), rng.getrandbits(32)], 'ret': False}


@case('Sched_RemoveClient')
def _(env, rng):
    m = Mem(env.emu)
    _, _, clients = make_sched(env, rng, m)
    c = rng.choice(clients + [m.alloc(rbytes(rng, 8)), 0]) if clients else 0x80401230
    return {'a': [SC, c], 'ret': False}


@case('Sched_GetCmdQ')
def _(env, rng):
    return {'a': [rng.getrandbits(32)]}


@case('Sched_Main', extra=[(FB0, FB_SIZE), (FB1, FB_SIZE)])
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m, need_rsp=True, need_rdp=True)
    env.emu.w32(SCREEN_W, rng.choice([1, 8, 9, 16, 23, 40] + ([320] if rng.random() < 0.03 else [])))
    s = []
    for _ in range(rng.randrange(0, 4)):
        s.append(rng.choice([0x29A, 0x29A, 0x123, 0, 0x29B, 0x29C]))
    if rng.random() < 0.8:
        s.append(0x29D)
        s += [rng.choice([0x29A, 0x29A, 0x29A, 0x29B, 7]) for _ in range(rng.randrange(0, 24))]
    # handlers dereference curRSPTask/curRDPTask; give RSP/RDP done messages only up front
    env.script = {SC + 0x40: [(0, v) for v in s]}
    return {'a': [SC], 'ret': False}


@case('Sched_HandleRetrace')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m)
    env.emu.w32(SC + 0x27C, rng.getrandbits(32))
    new = [make_task(env, rng, m) for _ in range(rng.choice([0, 0, 1, 2, 3]))]
    env.script = {SC + 0x78: [(0, t) for t in new]}
    return {'a': [SC], 'ret': False}


@case('Sched_HandleRSP')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m, need_rsp=True)
    return {'a': [SC], 'ret': False}


@case('Sched_HandleRDP')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m, need_rdp=True)
    return {'a': [SC], 'ret': False}


@case('Sched_TaskReady')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m)
    return {'a': [rng.choice([0, make_task(env, rng, m)])]}


@case('Sched_TaskComplete')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m)
    return {'a': [SC, make_task(env, rng, m)]}


@case('Sched_AppendList')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m)
    return {'a': [SC, make_task(env, rng, m)], 'ret': False}


@case('Sched_Exec')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m)
    a, b = make_task(env, rng, m), make_task(env, rng, m)
    sp = rng.choice([0, a, a])
    dp = rng.choice([0, sp, b, b])
    return {'a': [SC, sp, dp], 'ret': False}


@case('Sched_Yield')
def _(env, rng):
    m = Mem(env.emu)
    make_sched(env, rng, m, need_rsp=True)
    return {'a': [SC], 'ret': False}


@case('Sched_Schedule')
def _(env, rng):
    m = Mem(env.emu)
    gfx, aud, _ = make_sched(env, rng, m)
    if rng.random() < 0.5 and gfx:          # bias towards the interesting flag combinations
        env.emu.w32(gfx[0] + 8, rng.choice([2, 3, 6, 7, 0x13, 0x63, 1, 4]))
        env.emu.w32(gfx[0] + 4, rng.choice([0, 1, 2, 3, 0x20, 0x21, 0x23, 0x10]))
    slots = m.words([0x11111111, 0x22222222])
    return {'a': [SC, slots, slots + 4, rng.choice([0, 1, 2, 3, 3, 3])]}


TRIALS = {'Tlb_ClearAll': 3, 'Uart_Init': 3, 'Main_ForceLink': 3, 'Main_InitSystems': 5,
          'Main_Boot': 50, 'Uart_PutByte': 150, 'Uart_GetByte': 150, 'Uart_Delay': 100,
          'Sched_Main': 150, 'Main_GfxThread': 300}


def main(trials=300):
    env = Env()
    syms = env.syms
    only = sys.argv[1:]
    bad_total = 0
    for name, addr in ORIG.items():
        if only and name not in only:
            continue
        n = TRIALS.get(name, trials)
        bad = 0
        for s in range(n):
            ra = env.run(name, addr, s)
            rb = env.run(name, syms[name], s)
            if ra != rb:
                if bad == 0:
                    print(f'  {name}: first mismatch seed {s}: v0 {ra[0]} vs {rb[0]}')
                    if ra[1] != rb[1]:
                        for k in range(max(len(ra[1]), len(rb[1]))):
                            x = ra[1][k] if k < len(ra[1]) else None
                            y = rb[1][k] if k < len(rb[1]) else None
                            if x != y:
                                print(f'    log[{k}]: orig {x} / C {y}')
                                break
                    for k, (x, y) in enumerate(zip(ra[2:], rb[2:])):
                        if x != y:
                            d = next(i for i in range(len(x)) if x[i] != y[i])
                            print(f'    region {k} differs at +{d:#x}: {x[d:d+16].hex()} vs {y[d:d+16].hex()}')
                bad += 1
        print(f'{name:20s} {"PASS" if not bad else "FAIL"} {n - bad}/{n}')
        bad_total += bad
    return 1 if bad_total else 0


if __name__ == '__main__':
    sys.exit(main())
