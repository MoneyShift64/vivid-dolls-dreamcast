#!/usr/bin/env python3
"""Differential test of src/game/audio.c (music/sfx player, audio manager
thread, sample DMA cache, oscillators, controller init) against the original
MIPS code.

libultra calls with side effects outside memory (players, threads, message
queues, AI/PI hardware, scheduler) are replaced by Python stubs that record
their arguments and give seed-driven answers; the recorded call logs and all
globals/memory touched are compared. Pure libultra helpers (alHeapInit,
alHeapDBAlloc, alSeqFileNew, alBnkfNew, alLink/alUnlink, alCents2Ratio, sinf)
run as original code. Function pointers in logs are mapped from the C
addresses to the original ones; stack addresses are compared as 'STK'.
The ROM's .ctl/.seq data is used for Audio_Init (read through Rom_Read)."""
import os, random, struct, sys
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env, ROM_READ
from mipsbuild import CODE_BASE
from draft_mips import build_clean
from unicorn.mips_const import UC_MIPS_REG_SP

u32 = lambda v: v & 0xFFFFFFFF
fb = lambda v: struct.unpack('>I', struct.pack('>f', v))[0]
s16 = lambda v: u32(((v & 0xFFFF) ^ 0x8000) - 0x8000)

ORIG = {
    'Audio_Nop': 0x8000D150, 'Music_Stop': 0x8000D160, 'Music_Play': 0x8000D1CC,
    'Sfx_Play': 0x8000D368, 'Sfx_PlayEx': 0x8000D49C, 'Sfx_SetSlotParams': 0x8000D5C4,
    'Sfx_StopSlot': 0x8000D67C, 'Sfx_StopAll': 0x8000D6B8, 'Sfx_LoadInstrument': 0x8000D720,
    'Sfx_SetWatchSound': 0x8000D890, 'Audio_Init': 0x8000D93C, 'AudioMgr_Create': 0x8000DCE0,
    'AudioMgr_Main': 0x8000E0A4, 'AudioMgr_HandleFrame': 0x8000E740, 'AudioMgr_Dma': 0x8000E904,
    'AudioMgr_DmaNew': 0x8000EA88, 'AudioMgr_ClearDma': 0x8000EACC, 'Osc_DepthToCents': 0x8000EB98,
    'Osc_Init': 0x8000EBDC, 'Osc_Update': 0x8000EF58, 'Osc_Stop': 0x8000F334, 'Cont_Init': 0x8000F350,
}

# libultra / other calls that get logging stubs: name -> (addr, number of args)
STUBS = {
    'osCreateMesgQueue': (0x8001C9C0, 3), 'osCreateThread': (0x8001C9F0, 6),
    'osRecvMesg': (0x8001CDF0, 3), 'osSendMesg': (0x8001CF20, 3), 'osSetEventMesg': (0x8001D050, 3),
    'osStartThread': (0x8001D180, 1), 'osVirtualToPhysical': (0x8001D2E0, 1), 'osViBlack': (0x80021E40, 1),
    'alInit': (0x8001D770, 2), 'alClose': (0x8001D79C, 1), 'alCSeqNew': (0x8001D8B0, 2),
    'alCSPGetVol': (0x8001E370, 1), 'alCSPGetState': (0x8001E380, 1), 'alCSPPlay': (0x8001E390, 1),
    'alCSPSetBank': (0x8001E3C0, 2), 'alCSPSetSeq': (0x8001E3F0, 2), 'alCSPSetVol': (0x8001E420, 2),
    'alCSPStop': (0x8001E450, 1), 'alCSPNew': (0x8001E480, 2), 'alSndpNew': (0x8001F6E0, 2),
    'alSndpAllocate': (0x8001FDE0, 2), 'alSndpDeallocate': (0x8001FEA0, 2), 'alSndpSetSound': (0x8001FEF0, 2),
    'alSndpPlay': (0x8001FF00, 1), 'alSndpGetSound': (0x8001FF60, 1), 'alSndpStop': (0x8001FF70, 1),
    'alSndpGetState': (0x8001FFC0, 1), 'alSndpSetPitch': (0x8001FFE0, 2), 'alSndpSetPriority': (0x80020030, 3),
    'alSndpSetVol': (0x80020060, 2), 'alSndpSetPan': (0x800200B0, 2), 'alSndpSetFXMix': (0x80020100, 2),
    'alAudioFrame': (0x8002041C, 4), 'osAiGetLength': (0x80021000, 0), 'osAiSetFrequency': (0x80021010, 1),
    'osAiSetNextBuffer': (0x80021140, 2), 'osPiStartDma': (0x80025950, 7), 'osContInit': (0x800222E0, 3),
    'osScAddClient': (0x8000C6D8, 3), 'func_8000F6EC': (0x8000F6EC, 0), 'Rom_Read': (ROM_READ, 3),
}
HEAP_INIT, HEAP_ALLOC = 0x8001D820, 0x8001D860

# globals
DATA = (0x80038480, 0xE0)              # flags, counters, sfx tables
BSS = (0x800A2350, 0x4490)             # 0x800A2350-0x800A67E0
Q43330 = (0x80043330, 0x20)
STACK_LO, STACK_HI = 0x807E0000, 0x807FF000
AUDIO_HEAP = (0x80043350, 0x5F000)

SEQP, SEQ, SNDP = 0x80410000, 0x80410100, 0x80410200   # opaque player pointers (stubbed)
INSTS = 0x80400000          # 4 ALInstruments, 0x100 apart
BANK = 0x80401000
IDS = 0x80402000
SEQFILE = 0x80403000
SEQBUF = 0x80404000
SOUNDS = 0x80406000
MSGS = 0x80408000
TMP = 0x80409000            # out-params etc.
INFOS = 0x8040A000          # AudioInfo structs
DMABUF_BASE = 0x80420000


class Harness:
    def __init__(self):
        self.blob, self.syms = build_clean(['src/game/audio.c'], name='audio')
        self.emu = make_env()
        self.emu.write(CODE_BASE, self.blob)
        self.cmap = {self.syms[k]: v for k, v in ORIG.items()}
        self.cmap.update({v: v for v in ORIG.values()})
        self.log = []
        self.resp = None          # name -> fn(emu, args) giving the return value
        self.rng = None
        for name, (addr, n) in STUBS.items():
            self.emu.stub(addr, self._make_stub(name, n))

    def norm(self, v):
        v = u32(v)
        if v in self.cmap:
            return ('F', self.cmap[v])
        if STACK_LO <= v < STACK_HI:
            return 'STK'
        return v

    def args(self, n):
        e = self.emu
        a = [e.arg(i) for i in range(min(n, 4))]
        sp = e.mu.reg_read(UC_MIPS_REG_SP)
        a += [e.r32(sp + 0x10 + 4 * i) for i in range(n - 4)]
        return a

    def _make_stub(self, name, n):
        def fn(e):
            a = self.args(n)
            extra = None
            if name == 'alInit':
                c = a[1]
                extra = (e.r32(c), e.r32(c + 4), e.r32(c + 8), self.norm(e.r32(c + 0x10)), e.r32(c + 0x14),
                         e.r32(c + 0x18), e.read(c + 0x1C, 1))
                if e.read(c + 0x1C, 1)[0] == 6:
                    extra += (e.read(e.r32(c + 0x20), 0x68),)
            elif name == 'alCSPNew':
                c = a[1]
                extra = (e.r32(c), e.r32(c + 4), e.read(c + 8, 1), e.r32(c + 0xC),
                         self.norm(e.r32(c + 0x10)), self.norm(e.r32(c + 0x14)), self.norm(e.r32(c + 0x18)))
            elif name == 'alSndpNew':
                c = a[1]
                extra = (e.r32(c), e.r32(c + 4), e.r32(c + 8))
            elif name == 'osSendMesg' and STACK_LO <= a[1] < STACK_HI:
                extra = e.r32(a[1])          # the message points at a local; log its value
            self.log.append((name, tuple(self.norm(x) for x in a), extra))
            r = self.resp.get(name) if self.resp else None
            if r is not None:
                return r(e, a)
            return 0
        return fn

    # -- helpers --
    def snap(self, regions):
        return [self.emu.read(a, n) for a, n in regions]


def rand_float(rng):
    r = rng.random()
    if r < 0.1:
        return rng.choice([0.0, 1.0, -1.0, 0.5, 2.0])
    return rng.uniform(-4.0, 4.0)


def setup_world(h, rng, count=None):
    """Random but valid sfx/music state in globals + test memory."""
    e = h.emu
    e.write(DATA[0], bytes(rng.getrandbits(8) for _ in range(DATA[1])))
    e.write(BSS[0], bytes(rng.getrandbits(8) for _ in range(BSS[1])))
    e.write(Q43330[0], bytes(rng.getrandbits(8) for _ in range(Q43330[1])))
    # flags
    for a in (0x80038480, 0x80038484, 0x8003849C, 0x800384A0):
        e.w32(a, rng.choice([0, 0, 1, 1, 2, rng.getrandbits(32)]))
    e.w32(0x80038498, rng.choice([0, 1, 2, 3, 4, 1, 2, 3, 7]))
    e.w16(0x80038488, rng.choice([0xFFFF, 0xFFFF, rng.randrange(0, 30)]))
    e.w16(0x8003848A, rng.choice([0, 0, 1, rng.getrandbits(16)]))
    e.w16(0x8003848C, rng.choice([0, 0, 16, rng.getrandbits(16)]))
    e.w16(0x8003848E, rng.choice([0xFFFF, 0, 1, 2, 3]))
    e.w32(0x800384A4, rng.choice([0, 1, 2, 3, rng.getrandbits(32), rng.randrange(0, 1000)]))
    e.w32(0x800384A8, rng.randrange(0, 100))
    e.w32(0x800384AC, rng.randrange(0, 2))
    for i in range(28):
        e.write(0x800384B0 + i, bytes([rng.choice([1, 1, 2, 3, 4, rng.randrange(1, 256)])]))
        e.write(0x800384CC + i, bytes([rng.choice([rng.randrange(0, 29), rng.randrange(0, 256)])]))
    for i in range(29):
        e.wf(0x800384E8 + 4 * i, rand_float(rng))
    # instruments
    for k in range(4):
        ins = INSTS + 0x100 * k
        e.write(ins, bytes(rng.getrandbits(8) for _ in range(0x10)))
        c = count if count is not None else rng.choice([0, 1, 5, 13, 28, 29, rng.randrange(0, 30)])
        e.w16(ins + 0xE, c)
        for i in range(29):
            e.w32(ins + 0x10 + 4 * i, SOUNDS + rng.randrange(0, 0x100) * 0x10)
        e.w32(BANK + 0xC + 4 * k, ins)
    e.w32(0x800A2360, BANK)
    e.w32(0x800A2364, INSTS + 0x100 * rng.randrange(0, 4))
    for i in range(29):
        e.w16(IDS + 2 * i, rng.choice([rng.randrange(0, 29), rng.getrandbits(16)]))
    e.w32(0x800A2368, IDS)
    e.w32(0x800A2350, SEQP)
    e.w32(0x800A2354, SEQ)
    e.w32(0x800A2358, SNDP)
    e.w16(0x800A236C, rng.getrandbits(16))
    e.w32(0x800A2370, SEQBUF)
    # sequence file with offsets inside the ROM's S1 data
    n = rng.randrange(1, 16)
    e.w16(SEQFILE, 1)
    e.w16(SEQFILE + 2, n)
    for i in range(n):
        e.w32(SEQFILE + 4 + 8 * i, rng.randrange(0x1D7720, 0x1E0000))
        e.w32(SEQFILE + 8 + 8 * i, rng.randrange(0, 0x2000))
    e.w32(0x800A2378, SEQFILE)
    for i in range(29):
        e.w32(0x800A6018 + 12 * i, fb(rand_float(rng)))
        e.write(0x800A6018 + 12 * i + 8, bytes([rng.choice([0, 1, 2, 3, 4, 5, 6, 0, 1, 7, 200])]))
    e.write(SEQBUF, bytes(0x2000))


WORLD_REGIONS = [DATA, BSS, Q43330, (INSTS, 0x400), (IDS, 0x40), (SEQFILE, 0x80), (SEQBUF, 0x2000), (TMP, 0x100)]


def make_resp(seed):
    """Seed-driven answers for stubs (re-created identically for both runs)."""
    rng = random.Random(seed)
    def rom_read(e, a):
        e.write(a[1], e.rom[a[0]:a[0] + a[2]])
        return 0
    return {
        'alCSPGetState': lambda e, a: rng.choice([0, 0, 1, 2]),
        'alCSPGetVol': lambda e, a: s16(rng.getrandbits(16)),
        'alSndpGetState': lambda e, a: rng.choice([0, 0, 1, 1, 2]),
        'alSndpGetSound': lambda e, a: s16(rng.choice([0xFFFF, rng.randrange(0, 29)])),
        'alSndpAllocate': lambda e, a: s16(rng.choice([0xFFFF, rng.randrange(0, 40)])),
        'osVirtualToPhysical': lambda e, a: a[0] & 0x1FFFFFFF,
        'osAiSetFrequency': lambda e, a: rng.choice([44100, 44100, 44095, 32000]),
        'Rom_Read': rom_read,
    }


def run_pair(h, name, setup, call, regions, seed, cmp_v0=True, cmp_f0=False):
    """Run original and C with identical setup; returns mismatch description or None."""
    res = []
    for impl in ('orig', 'c'):
        addr = ORIG[name] if impl == 'orig' else h.syms[name]
        setup()
        h.log = []
        h.resp = make_resp(seed)
        v0, _ = call(addr)
        res.append((h.norm(v0) if cmp_v0 else None, h.emu.f0_bits() if cmp_f0 else None,
                    list(h.log), h.snap(regions)))
    a, b = res
    if a[0] != b[0]:
        return f'v0 orig {a[0]} C {b[0]}'
    if a[1] != b[1]:
        return f'f0 orig {a[1]:#x} C {b[1]:#x}'
    if a[2] != b[2]:
        for i, (x, y) in enumerate(zip(a[2], b[2])):
            if x != y:
                return f'call {i}: orig {x} C {y}'
        return f'call count orig {len(a[2])} C {len(b[2])}'
    for (ra, _), x, y in zip(regions, a[3], b[3]):
        if x != y:
            off = next(i for i in range(len(x)) if x[i] != y[i])
            return f'memory {ra + off:#x}: orig {x[off:off + 8].hex()} C {y[off:off + 8].hex()}'
    return None


class Tally:
    def __init__(self):
        self.res = {}

    def add(self, name, err, info=''):
        ok, n, msgs = self.res.get(name, (0, 0, []))
        if err is None:
            ok += 1
        elif len(msgs) < 3:
            msgs.append(f'{info}: {err}')
        self.res[name] = (ok, n + 1, msgs)

    def report(self):
        bad = 0
        for name, (ok, n, msgs) in self.res.items():
            print(f'{name:22s} {"PASS" if ok == n else "FAIL"} {ok}/{n}')
            for m in msgs:
                print('    ', m)
            bad += ok != n
        return bad


def test_simple(h, T, rng, N):
    e = h.emu
    for t in range(N):
        seed = rng.getrandbits(32)
        def setup(seed=seed):
            setup_world(h, random.Random(seed))
        r2 = random.Random(seed ^ 0x55)
        T.add('Audio_Nop', run_pair(h, 'Audio_Nop', setup, lambda a: e.call(a), WORLD_REGIONS, seed, False), t)
        T.add('Music_Stop', run_pair(h, 'Music_Stop', setup, lambda a: e.call(a), WORLD_REGIONS, seed, False), t)
        song = r2.choice([0, 1, 5, 14, 15, 16, r2.randrange(0, 20), r2.getrandbits(32)])
        T.add('Music_Play', run_pair(h, 'Music_Play', setup, lambda a: e.call(a, a=[song]),
                                     WORLD_REGIONS, seed, False), t)
        sid = r2.randrange(0, 28)
        T.add('Sfx_Play', run_pair(h, 'Sfx_Play', setup, lambda a: e.call(a, a=[sid]), WORLD_REGIONS, seed), t)
        args = [sid, fb(rand_float(r2)), s16(r2.getrandbits(16)), r2.getrandbits(8), r2.getrandbits(8)]
        T.add('Sfx_PlayEx', run_pair(h, 'Sfx_PlayEx', setup, lambda a: e.call(a, a=args), WORLD_REGIONS, seed), t)
        slot = r2.choice([r2.randrange(0, 30), 0xFFFF, 0x8000, r2.getrandbits(16)])
        args = [slot, fb(rand_float(r2)), s16(r2.getrandbits(16)), r2.getrandbits(8), r2.getrandbits(8)]
        T.add('Sfx_SetSlotParams', run_pair(h, 'Sfx_SetSlotParams', setup, lambda a: e.call(a, a=args),
                                            WORLD_REGIONS, seed, False), t)
        T.add('Sfx_StopSlot', run_pair(h, 'Sfx_StopSlot', setup, lambda a: e.call(a, a=[slot]),
                                       WORLD_REGIONS, seed, False), t)
        T.add('Sfx_StopAll', run_pair(h, 'Sfx_StopAll', setup, lambda a: e.call(a), WORLD_REGIONS, seed, False), t)
        inst = r2.choice([0, 0, 0, 1, 2, 3, 0xFFFF])
        T.add('Sfx_LoadInstrument', run_pair(h, 'Sfx_LoadInstrument', setup, lambda a: e.call(a, a=[inst]),
                                             WORLD_REGIONS, seed, False), t)
        T.add('Sfx_SetWatchSound', run_pair(h, 'Sfx_SetWatchSound', setup, lambda a: e.call(a, a=[slot]),
                                            WORLD_REGIONS, seed, False), t)


# ---------------- DMA cache ----------------
DMA_REGIONS = [(0x800A47E0, 0xA10), (0x80038480, 0x40), (TMP, 0x100), (INFOS, 0x400), BSS]


def setup_dma(h, rng):
    """Random used (sorted by startAddr) and free lists over the 128 buffers."""
    e = h.emu
    setup_world(h, rng)
    idx = list(range(128))
    rng.shuffle(idx)
    nused = rng.choice([0, 1, 2, 5, 20, 60, 127, rng.randrange(0, 128)])
    nfree = rng.choice([0, 1, 2, 10, 128 - nused]) if rng.random() < 0.3 else 128 - nused
    nfree = min(nfree, 128 - nused)
    used, free = idx[:nused], idx[nused:nused + nfree]
    base = lambda i: 0x800A47F0 + 0x14 * i
    starts = sorted(rng.sample(range(0x1E3EA0, 0x23A000, 2), nused)) if rng.random() < 0.7 else \
        sorted(rng.randrange(0, 0x400000) for _ in range(nused))
    frame = e.r32(0x800384A4)
    for lst, st in ((used, starts), (free, None)):
        for k, i in enumerate(lst):
            nx = base(lst[k + 1]) if k + 1 < len(lst) else 0
            pv = base(lst[k - 1]) if k > 0 else 0
            e.w32(base(i), nx)
            e.w32(base(i) + 4, pv)
            e.w32(base(i) + 8, st[k] if st else rng.getrandbits(32))
            e.w32(base(i) + 0xC, u32(frame - rng.choice([0, 1, 2, 3, 4, 100])) if rng.random() < 0.8
                  else rng.getrandbits(32))
            e.w32(base(i) + 0x10, DMABUF_BASE + 0x600 * i)
    e.write(0x800A47E0, bytes([rng.choice([0, 1, 1, 0x80])]))
    e.w32(0x800A47E4, base(used[0]) if used else 0)
    e.w32(0x800A47E8, base(free[0]) if free else 0)
    return starts, bool(free)


def test_dma(h, T, rng, N):
    e = h.emu
    for t in range(N):
        seed = rng.getrandbits(32)
        starts, has_free = setup_dma(h, random.Random(seed))
        r2 = random.Random(seed ^ 1)
        def setup(seed=seed):
            setup_dma(h, random.Random(seed))
        T.add('AudioMgr_ClearDma', run_pair(h, 'AudioMgr_ClearDma', setup, lambda a: e.call(a), DMA_REGIONS, seed), t)
        T.add('AudioMgr_DmaNew', run_pair(h, 'AudioMgr_DmaNew', setup, lambda a: e.call(a, a=[TMP]),
                                          DMA_REGIONS, seed), t)
        # pick an address: inside a cached buffer, straddling, or elsewhere
        for k in range(2):
            if starts and r2.random() < 0.6:
                s = r2.choice(starts)
                addr = s + r2.choice([0, 1, r2.randrange(0, 0x600), 0x5FF, 0x600, -1, -2])
            else:
                addr = r2.randrange(0, 0x400000)
            ln = r2.choice([1, 2, 9, 0x40, 0x100, r2.randrange(0, 0x600), 0x600, 0x601])
            if not has_free:
                # a miss would take a buffer from an empty free list (NULL in the original too); only allow hits
                hit = False
                for s in starts:          # same walk as the code: sorted list, unsigned/signed compares
                    if u32(addr) < s:
                        break
                    sg = lambda v: u32(v) - (1 << 32) if u32(v) & 0x80000000 else u32(v)
                    if sg(addr + ln) <= sg(s + 0x600):
                        hit = True
                        break
                if not hit:
                    continue
            T.add('AudioMgr_Dma', run_pair(h, 'AudioMgr_Dma', setup, lambda a: e.call(a, a=[u32(addr), ln, 0x800A47E0]),
                                           DMA_REGIONS, seed), t)


def setup_frame(h, rng):
    starts, has_free = setup_dma(h, rng)
    e = h.emu
    for k in range(3):
        info = INFOS + 0x100 * k
        e.write(info, bytes(rng.getrandbits(8) for _ in range(0x80)))
        e.w32(info, DMABUF_BASE + 0x1000 * k)
    e.w32(0x800A2390, 0x80430000)
    e.w32(0x800A2394, 0x80438000)
    e.w32(0x800A51F4, rng.choice([0x2E0, 0x300, 0x10, rng.randrange(0, 0x1000)]))
    e.w32(0x800A51F0, rng.choice([0x2D0, 0x2F0, 0, rng.randrange(0, 0x1000), 0xFFFFFFF0]))
    e.w32(0x800B0568, 0x800C1000)


def test_frame(h, T, rng, N):
    e = h.emu
    for t in range(N):
        seed = rng.getrandbits(32)
        r2 = random.Random(seed ^ 2)
        last = r2.choice([0, INFOS + 0x100 * r2.randrange(0, 3)])
        info = INFOS + 0x100 * r2.randrange(0, 3)
        def setup(seed=seed):
            setup_frame(h, random.Random(seed))
        def call(a):
            resp_rng = random.Random(seed ^ 3)
            def frame(em, args):
                em.w32(args[1], resp_rng.choice([0, 1, 7, resp_rng.randrange(0, 0x1800)]))
                return args[0] + 8 * resp_rng.randrange(0, 0x1800)
            h.resp['alAudioFrame'] = frame
            h.resp['osAiGetLength'] = lambda em, args: resp_rng.choice([0, 4, 0x400, resp_rng.getrandbits(32)])
            return e.call(a, a=[info, last])
        T.add('AudioMgr_HandleFrame', run_pair(h, 'AudioMgr_HandleFrame', setup, call,
                                               DMA_REGIONS + [(0x800B0568, 4)], seed), t)


# ---------------- audio thread ----------------
def test_main(h, T, rng, N):
    e = h.emu
    for t in range(N):
        seed = rng.getrandbits(32)
        r2 = random.Random(seed ^ 4)
        nmsg = r2.randrange(1, 12)
        script = []
        for i in range(nmsg):
            script.append(r2.choice([1, 1, 1, 1, 1, 4, 2, 0]))
        def setup(seed=seed, script=script):
            setup_world(h, random.Random(seed))
            for i, ty in enumerate(script):
                e.w16(MSGS + 8 * i, ty)
                e.w32(MSGS + 8 * i + 4, 0)
            for k in range(3):        # reply messages
                e.w16(MSGS + 0x200 + 8 * k, 2)
                e.w32(MSGS + 0x200 + 8 * k + 4, INFOS + 0x100 * k)
            for k in range(3):
                e.w32(0x800A2398 + 4 * k, INFOS + 0x100 * k)
        def call(a, script=script, seed=seed):
            st = {'i': 0, 'k': 0}
            resp_rng = random.Random(seed ^ 5)
            def recv(em, args):
                if args[0] == 0x800A2670:            # reply queue
                    em.w32(args[1], MSGS + 0x200 + 8 * (st['k'] % 3))
                    st['k'] += 1
                    return 0
                if st['i'] >= len(script):
                    em.mu.emu_stop()
                    return 0
                em.w32(args[1], MSGS + 8 * st['i'])
                st['i'] += 1
                return 0
            h.resp['osRecvMesg'] = recv
            def frame(em):
                args = h.args(2)
                h.log.append(('HandleFrame', (args[0], args[1]), None))
                return resp_rng.choice([0, 1, 1])
            e.stub(ORIG['AudioMgr_HandleFrame'], frame)
            e.stub(h.syms['AudioMgr_HandleFrame'], frame)
            return e.call(a, a=[0])
        T.add('AudioMgr_Main', run_pair(h, 'AudioMgr_Main', setup, call, WORLD_REGIONS, seed, False), t)
    # restore the real functions for any later tests
    del e.stubs[ORIG['AudioMgr_HandleFrame']]
    del e.stubs[h.syms['AudioMgr_HandleFrame']]


# ---------------- init ----------------
CREATE_HEAP = (0x80500000, 0x100000)


def test_create(h, T, rng, N):
    e = h.emu
    C, AMC = TMP, TMP + 0x40
    for t in range(N):
        seed = rng.getrandbits(32)
        r2 = random.Random(seed ^ 6)
        rate = r2.choice([44100, 44100, 32000, 22050, 48000, r2.randrange(1, 100000)])
        fx = r2.choice([1, 6, 6, 0, r2.getrandbits(8)])
        fpf = r2.choice([1, 1, 2, 3, r2.randrange(1, 5)])
        acmd = r2.choice([0x1800, 0x800, r2.randrange(1, 0x1800)])
        def setup(seed=seed):
            setup_world(h, random.Random(seed))
            e.write(CREATE_HEAP[0], bytes(0x40))
            e.call(HEAP_INIT, a=[0x800A2380, CREATE_HEAP[0], CREATE_HEAP[1]])
            e.write(C, bytes(random.Random(seed).getrandbits(8) for _ in range(0x40)))
            e.w32(C, 40); e.w32(C + 4, 40); e.w32(C + 8, 256)
            e.w32(C + 0x14, 0x800A2380)
            e.w32(C + 0x18, rate)
            e.write(C + 0x1C, bytes([fx]))
            e.w32(AMC, rate); e.w32(AMC + 4, fpf); e.w32(AMC + 8, acmd)
            e.write(0x800A47E0, bytes(0x10))
        pri = r2.choice([14, 0, 3])
        def call(a):
            return e.call(a, a=[C, pri, AMC])
        regions = WORLD_REGIONS + [CREATE_HEAP, (0x800A47E0, 0xA10)]
        err = run_pair(h, 'AudioMgr_Create', setup, call, [r for r in regions if r != (TMP, 0x100)] +
                       [(C, 0x20), (C + 0x24, 0x1C), (AMC, 0xC)], seed, False)
        if err is None and fx == 6:
            # c->params points at a stack array in both versions
            if h.norm(e.r32(C + 0x20)) != 'STK':
                err = 'params not a stack pointer'
        T.add('AudioMgr_Create', err, t)


def test_audio_init(h, T, rng, N):
    e = h.emu
    regions = [DATA, BSS, Q43330, AUDIO_HEAP]
    for t in range(N):
        seed = rng.getrandbits(32)
        r2 = random.Random(seed ^ 7)
        cur = r2.choice([0xFFFF, 0xFFFF, 0, 1])
        def setup(seed=seed, cur=cur):
            setup_world(h, random.Random(seed))
            e.w16(0x8003848E, cur)
            if t < 2:
                e.write(AUDIO_HEAP[0], bytes(random.Random(seed).getrandbits(8) for _ in range(0x1000)) * 0x5F)
        T.add('Audio_Init', run_pair(h, 'Audio_Init', setup, lambda a: e.call(a), regions, seed, False), t)


# ---------------- oscillators ----------------
OSC_REGIONS = [(0x800A6194, 4), (0x800A61A0, 0x640), (TMP, 0x40)]
OSC_TYPES = [1, 2, 3, 4, 0x80, 0x81, 0x82, 0x83]


def setup_osc(h, rng):
    e = h.emu
    e.write(0x800A61A0, bytes(rng.getrandbits(8) for _ in range(0x640)))
    e.write(TMP, bytes(rng.getrandbits(8) for _ in range(0x40)))
    n = rng.choice([0, 1, 2, 5, 80])
    idx = rng.sample(range(80), n)
    for k, i in enumerate(idx):
        e.w32(0x800A61A0 + 0x14 * i, 0x800A61A0 + 0x14 * idx[k + 1] if k + 1 < n else 0)
    e.w32(0x800A6194, 0x800A61A0 + 0x14 * idx[0] if n else 0)


def test_osc(h, T, rng, N):
    e = h.emu
    for d in range(256):
        T.add('Osc_DepthToCents', run_pair(h, 'Osc_DepthToCents', lambda: None, lambda a: e.call(a, a=[d]),
                                           [], 0, False, True), d)
    for t in range(N):
        seed = rng.getrandbits(32)
        r2 = random.Random(seed ^ 8)
        ty = r2.choice(OSC_TYPES * 3 + [0, 5, 0x84, r2.getrandbits(8)])
        rate, depth, delay = r2.getrandbits(8), r2.choice([0, 1, 127, 255, r2.getrandbits(8)]), r2.getrandbits(8)
        def setup(seed=seed):
            setup_osc(h, random.Random(seed))
        T.add('Osc_Init', run_pair(h, 'Osc_Init', setup,
                                   lambda a: e.call(a, a=[TMP, TMP + 4, ty, rate, depth, delay]),
                                   OSC_REGIONS, seed), t)
        # update: random state (sometimes made by Osc_Init)
        st = 0x800A61A0 + 0x14 * r2.randrange(80)
        ty = r2.choice(OSC_TYPES * 4 + [0, 5, r2.getrandbits(8)])
        mx = r2.choice([1, 2, 3, 255, 259, r2.randrange(0, 260), 0])
        cur = r2.choice([u32(mx - 1) & 0xFFFF, mx, mx + 1, 0, 0xFFFF, r2.randrange(0, 0x110)])
        vals = [r2.getrandbits(8) for _ in range(3)]
        f0, f1 = rand_float(r2), rand_float(r2)
        i0, i1 = r2.randrange(-3000, 3000), r2.randrange(-6000, 6000)
        flag = r2.choice([0, 1, 2])
        kind = r2.random()
        def setup_u(seed=seed):
            setup_osc(h, random.Random(seed))
            e.write(st + 4, bytes([ty, flag]))
            e.w16(st + 6, mx)
            e.w16(st + 8, cur)
            if ty in (0x80, 0x81) or (kind < 0.3 and ty not in (0x82, 0x83)):
                e.wf(st + 0xC, f0 * 30); e.wf(st + 0x10, f1)
            elif ty in (0x82, 0x83):
                e.w32(st + 0xC, u32(i0)); e.w32(st + 0x10, u32(i1))
            else:
                e.write(st + 0xC, bytes(vals))
        T.add('Osc_Update', run_pair(h, 'Osc_Update', setup_u, lambda a: e.call(a, a=[st, TMP + 4]),
                                     OSC_REGIONS, seed), t)
        T.add('Osc_Stop', run_pair(h, 'Osc_Stop', setup, lambda a: e.call(a, a=[st]), OSC_REGIONS, seed, False), t)


def test_cont(h, T, rng, N):
    e = h.emu
    regions = [(0x800A67E0, 0x70), (0x800AF250, 8), (0x800B0490, 8), (0x800C30A8, 0x18)]
    for t in range(N):
        seed = rng.getrandbits(32)
        def setup(seed=seed):
            r = random.Random(seed)
            for a, n in regions:
                e.write(a, bytes(r.getrandbits(8) for _ in range(n)))
        T.add('Cont_Init', run_pair(h, 'Cont_Init', setup, lambda a: e.call(a), regions, seed, False), t)


def main():
    h = Harness()
    T = Tally()
    rng = random.Random(1998)
    N = int(os.environ.get('AUDIO_TRIALS', '300'))
    test_simple(h, T, rng, N)
    test_dma(h, T, rng, N)
    test_frame(h, T, rng, N)
    test_main(h, T, rng, N)
    test_create(h, T, rng, N)
    test_osc(h, T, rng, N)
    test_cont(h, T, rng, N)
    test_audio_init(h, T, rng, int(os.environ.get('AUDIO_INIT_TRIALS', '300')))
    return 1 if T.report() else 0


if __name__ == '__main__':
    sys.exit(main())
