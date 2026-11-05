#!/usr/bin/env python3
"""Differential test of src/game/ovl0_test.c (overlay 0: operator test mode)
against the original MIPS code, with overlay 0 loaded.

All callees (Text_*, Scene_NewNode/DeleteNode, Obj_*, Music_*, Eeprom_*,
Task_Create, Task_Yield, Camera/Light init) are Python stubs that log their
arguments (strings by content, since the C uses its own string literals;
C task function addresses are translated to the original ones). The libultra
sprintf (func_80025DC0) runs for real. Task_Yield randomises the input globals
every frame and presses the test switch (D_800AF220) after a random number of
frames. The call log, the main BSS, the overlay area, the test memory and
(for the system check) the 4 MB SDRAM test area at 0x01000000 are compared.

    python3 tools/difftest/test_ovl0.py [names...]     (TRIALS=n to change the counts)
"""
import hashlib, os, random, struct, sys, zlib
sys.path.insert(0, os.path.dirname(__file__))
from game_env import make_env
from emu import STACK_TOP
from mipsbuild import CODE_BASE
from draft_mips import build_clean, symbol_map
from unicorn import UC_HOOK_MEM_READ
from unicorn.mips_const import UC_MIPS_REG_SP

SM = symbol_map()
TRIALS = os.environ.get('TRIALS')

ORIG = {
    'Test_Run': 0x800E0000, 'Test_ScreenTest': 0x800E029C, 'Test_InputTest': 0x800E03F0,
    'Test_SoundTest': 0x800E33C8, 'Test_SystemCheck': 0x800E3588, 'Test_OutputTest': 0x800E3A48,
    'Test_Menu': 0x800E40D0, 'Test_CoinError': 0x800E4988,
}
COUNTS = {'Test_Run': 150, 'Test_ScreenTest': 300, 'Test_InputTest': 120, 'Test_SoundTest': 300,
          'Test_SystemCheck': 40, 'Test_OutputTest': 300, 'Test_Menu': 300, 'Test_CoinError': 100}

TEST = 0x80400000
TEST_SIZE = 0x80000
DONE = TEST                    # *done for the task functions / args for Test_Run
GROUP = TEST + 0x100           # fake gSceneGroup1 (only +0xD4 tail is used)
HEAD, TAIL = TEST + 0x200, TEST + 0x300
NODES = TEST + 0x1000          # fake scene nodes, 0xE0 each
FAKE_TEXT = 0x80460000         # text handles (never dereferenced)
FAKE_OBJ = 0x80470000
BSS = (0x8003E130, 0x800C5400)
OVL = (0x800E0000, 0x800E4DA0)
SDRAM, SDRAM_SIZE = 0x01000000, 0x400000

# input globals randomised every frame: (address, size)
G = {
    'gIoHeld': 0x800B0498, 'gPadRawHeld': 0x800B058C, 'gPadStickX': 0x800B04AC,
    'gPadStickY': 0x800C5298, 'coin': 0x800C0D4C, 'svc': 0x800C0AB0, 'test': 0x800C30C4,
    'dips': 0x800C52A0, 'pressed': 0x800C0D48, 'held': 0x800B060C, 'testPressed': 0x800AF220,
}


def cstr(e, a, n=64):
    b = e.read(a, n)
    return b.split(b'\0')[0]


class Ctx:
    def __init__(self, seed, stop, pin=None):
        self.rng = random.Random(seed)
        self.stop = stop             # yield count at which the test switch is pressed
        self.pin = pin               # per-run input probabilities
        self.log = []
        self.nyield = 0
        self.ntext = 0
        self.nnode = 0
        self.eeprom = None
        self.eep_corrupt = None
        self.spawn_script = None
        self.max_spawns = None
        self.nspawn = 0
        self.pending = None          # (done address, yield count, value)


class Env:
    def __init__(self):
        blob, self.syms = build_clean(['src/game/ovl0_test.c'], name='ovl0')
        self.c2o = {self.syms[n]: a for n, a in ORIG.items()}
        e = self.emu = make_env(overlay=0)
        e.mu.mem_map(SDRAM, SDRAM_SIZE)
        e.write(CODE_BASE, blob)
        self.ctx = None
        self.stack_lo = STACK_TOP - 0x10000
        for name, n in (('Camera_Init', 0), ('Light_Init', 1), ('Scene_DeleteNode', 1),
                        ('Text_Delete', 1), ('Obj_Show', 1), ('Obj_Kill', 1),
                        ('Music_Play', 1), ('Music_Stop', 0)):
            e.stub(SM[name], self._logger(name, n))
        for name in ('Text_SetGradient', 'Text_SetColors'):
            e.stub(SM[name], self._logger(name, 5))
        e.stub(SM['Text_PlaceScreen'], self._logger('Text_PlaceScreen', 6))
        e.stub(SM['Text_Create'], self._text_create)
        e.stub(SM['Text_SetString'], self._set_string)
        e.stub(SM['Scene_NewNode'], self._new_node)
        e.stub(SM['Obj_NewSpritePlain'], self._new_sprite)
        e.stub(SM['Eeprom_Write'], self._eep_write)
        e.stub(SM['Eeprom_Read'], self._eep_read)
        e.stub(SM['Task_Create'], self._spawn)
        e.stub(SM['Task_Yield'], self._yield)
        self.base_bss = e.read(BSS[0], BSS[1] - BSS[0])

    # ---- stubs ----
    def _args(self, n):
        e = self.emu
        sp = e.mu.reg_read(UC_MIPS_REG_SP)
        return [e.arg(i) for i in range(min(n, 4))] + [e.r32(sp + 0x10 + 4 * i) for i in range(max(0, n - 4))]

    def norm(self, v):
        return 'STACK' if self.stack_lo <= v < STACK_TOP else self.c2o.get(v, v)

    def _logger(self, name, n):
        def f(e):
            self.ctx.log.append((name,) + tuple(self.norm(v) for v in self._args(n)))
            return 0
        return f

    def _text_create(self, e):
        a = self._args(6)
        c = self.ctx
        c.log.append(('Text_Create', a[0], cstr(e, a[1])) + tuple(a[2:]))
        c.ntext += 1
        return FAKE_TEXT + 0x10 * c.ntext

    def _set_string(self, e):
        a = self._args(2)
        self.ctx.log.append(('Text_SetString', a[0], cstr(e, a[1])))
        return a[0]

    def _new_node(self, e):
        c = self.ctx
        c.log.append(('Scene_NewNode', e.arg(0)))
        node = NODES + 0xE0 * c.nnode
        c.nnode += 1
        return node

    def _new_sprite(self, e):
        a = self._args(2)
        self.ctx.log.append(('Obj_NewSpritePlain',) + tuple(a))
        return FAKE_OBJ

    def _eep_write(self, e):
        a = e.arg(0)
        buf = e.read(a, 0x200)
        self.ctx.log.append(('Eeprom_Write', self.norm(a), hashlib.md5(buf).hexdigest()))
        self.ctx.eeprom = bytearray(buf)
        return 0

    def _eep_read(self, e):
        a = e.arg(0)
        c = self.ctx
        c.log.append(('Eeprom_Read', self.norm(a)))
        data = bytearray(c.eeprom if c.eeprom is not None else bytes(0x200))
        if c.eep_corrupt is not None:
            data[c.eep_corrupt] ^= 0x10
        e.write(a, data)
        return 0

    def _spawn(self, e):
        a = self._args(5)
        c = self.ctx
        c.log.append(('Task_Create', self.norm(a[0]), a[1], a[2], self.norm(a[3]), a[4]))
        c.nspawn += 1
        if c.max_spawns is not None and c.nspawn > c.max_spawns:
            e.mu.emu_stop()
            return 0
        if c.spawn_script:
            delay, val = c.spawn_script(c, self.c2o.get(a[0], a[0]))
            if delay == 0:
                e.w32(a[3], val)
            else:
                c.pending = (a[3], c.nyield + delay, val)
        return 0x80480000

    def _yield(self, e):
        c = self.ctx
        c.nyield += 1
        c.log.append(('yield', c.nyield))
        if c.pending and c.nyield >= c.pending[1]:
            e.w32(c.pending[0], c.pending[2])
            c.pending = None
        if c.pin is not None:
            self.random_inputs(e, c)
        e.w16(G['testPressed'], 1 if c.nyield == c.stop else 0)
        if c.nyield > 5000:
            e.mu.emu_stop()

    def random_inputs(self, e, c):
        r, p = c.rng, c.pin
        def bits(n, prob):
            v = 0
            for i in range(n):
                if r.random() < prob:
                    v |= 1 << i
            return v
        for k in range(2):
            e.w16(G['gIoHeld'] + 2 * k, bits(16, p['io']))
            e.w16(G['gPadRawHeld'] + 2 * k, bits(16, p['pad']))
            e.w16(G['coin'] + 2 * k, r.choice([0, 0, 0, 1, 2, 3]) if r.random() < p['io'] * 2 else 0)
            e.w16(G['pressed'] + 2 * k, bits(3, p['press']) | (bits(16, 0.05) if r.random() < 0.1 else 0))
            e.w16(G['held'] + 2 * k, bits(16, p['io']) | (0x10 if r.random() < p['press'] else 0))
            for g in ('gPadStickX', 'gPadStickY'):
                if r.random() < p['stick']:
                    v = r.choice([r.randrange(-128, 128), r.choice([-32768, 32767, -5, -4, 3, 4]),
                                  r.randrange(-32768, 32768)])
                else:
                    v = r.randrange(-4, 4)
                e.w16(G[g] + 2 * k, v)
        e.w16(G['svc'], r.choice([0, 1, 2, 0xFFFF]) if r.random() < p['io'] else 0)
        e.w16(G['test'], r.choice([0, 1, 2, 0xFFFF]) if r.random() < p['io'] else 0)
        if r.random() < 0.05:
            e.w16(G['dips'], r.getrandbits(16))

    # ---- running ----
    def setup_common(self, r):
        e = self.emu
        e.write(BSS[0], self.base_bss)
        e.write(TEST, bytes(TEST_SIZE))
        e.write(OVL[0], e.rom[0x3ED30:0x3ED30 + OVL[1] - OVL[0]])
        e.write(self.stack_lo, bytes(STACK_TOP - self.stack_lo))
        e.w32(SM['gSceneGroup1'], GROUP)
        e.w32(GROUP + 0xD0, HEAD)
        e.w32(GROUP + 0xD4, TAIL)
        e.w32(HEAD, TAIL)
        e.w32(HEAD + 4, 0)
        e.w32(TAIL, 0)
        e.w32(TAIL + 4, HEAD)
        e.w32(SM['gSceneGroup0'], 0x80420000)
        e.w16(G['dips'], r.getrandbits(16))
        for name in G:
            if name != 'dips':
                e.w32(G[name] & ~3, r.getrandbits(32))
        e.w16(G['testPressed'], 0)
        for a in (SM['gIoOutputMask'], 0x800C178C, SM['gCoinJam'], SM['gCoinJam'] + 2):
            e.w16(a, r.getrandbits(16))

    def snapshot(self, sdram):
        e = self.emu
        s = [('BSS', BSS[0], e.read(BSS[0], BSS[1] - BSS[0])),
             ('OVL', OVL[0], e.read(OVL[0], OVL[1] - OVL[0])),
             ('TEST', TEST, e.read(TEST, TEST_SIZE))]
        if sdram:
            s.append(('SDRAM', SDRAM, hashlib.md5(e.read(SDRAM, SDRAM_SIZE)).digest()))
        return s

    def run(self, name, seed, setup):
        e = self.emu
        results = []
        for which in ('orig', 'c'):
            r = random.Random(seed)
            cfg = setup(r)
            self.setup_common(random.Random(seed ^ 0x1234))
            if cfg.get('pre'):
                cfg['pre'](e)
            self.ctx = Ctx(seed * 7 + 1, cfg.get('stop'), cfg.get('pin'))
            self.ctx.eep_corrupt = cfg.get('eep_corrupt')
            self.ctx.spawn_script = cfg.get('spawn')
            self.ctx.max_spawns = cfg.get('max_spawns')
            hook = None
            if cfg.get('sdram_bad') is not None:
                # a memory read hook cannot change the value being read, so the
                # first read of word 0 (start of the verify pass) flips a bit
                # of word 'bad' (>= 1) instead
                bad = SDRAM + 4 * cfg['sdram_bad']
                def rd(mu, access, addr, size, value, user):
                    v = struct.unpack('>I', bytes(mu.mem_read(bad, 4)))[0]
                    mu.mem_write(bad, struct.pack('>I', v ^ 0x100))
                hook = e.mu.hook_add(UC_HOOK_MEM_READ, rd, begin=SDRAM, end=SDRAM + 3)
            addr = ORIG[name] if which == 'orig' else self.syms[name]
            try:
                v0, _ = e.call(addr, a=cfg['args'], max_insns=200_000_000)
            except Exception as ex:
                return f'{which}: {ex!r}'
            finally:
                if hook is not None:
                    e.mu.hook_del(hook)
            results.append((v0 & 0xFFFFFFFF, self.ctx.log, self.snapshot(cfg.get('sdram', False))))
        (va, la, sa), (vb, lb, sb) = results
        if name == 'Test_Run' and va != vb:
            return f'v0 {va:#x} != {vb:#x}'
        if la != lb:
            for k, (x, y) in enumerate(zip(la, lb)):
                if x != y:
                    return f'log[{k}] {x} != {y}'
            return f'log length {len(la)} != {len(lb)}'
        for (nm, base, x), (_, _, y) in zip(sa, sb):
            if x != y:
                if nm == 'SDRAM':
                    return 'SDRAM contents differ'
                i = next(k for k in range(len(x)) if x[k] != y[k])
                return f'{nm} differs at {base + i:08X}: {x[i:i+4].hex()} vs {y[i:i+4].hex()}'
        return None


def rand_pin(r):
    return {'io': r.choice([0.0, 0.02, 0.1, 0.3, 0.6]), 'pad': r.choice([0.0, 0.02, 0.1, 0.4]),
            'stick': r.choice([0.0, 0.05, 0.3, 0.8]), 'press': r.choice([0.0, 0.1, 0.3, 0.7])}


def task_setup(stop_range):
    def f(r):
        return {'args': [DONE, 0], 'stop': r.choice([1, 2, r.randrange(*stop_range)]), 'pin': rand_pin(r)}
    return f


def coin_setup(r):
    return {'args': [DONE, 0], 'stop': r.choice([None, 1, 5, 300, 301, r.randrange(1, 320)]),
            'pin': rand_pin(r) if r.random() < 0.5 else None}


def syscheck_setup(r):
    cfg = {'args': [DONE, 0], 'stop': r.choice([1, 2, r.randrange(1, 20)]), 'pin': None, 'sdram': True}
    k = r.random()
    if k < 0.3:
        cfg['sdram_bad'] = r.choice([1, 2, 0xFFFFF, r.randrange(1, 0x100000)])
    if r.random() < 0.4:
        cfg['eep_corrupt'] = r.choice([0, 0x1FF, r.randrange(0, 0x200)])
    return cfg


def run_setup(r):
    mode = r.choice([0, 0, 0, 1, 1])
    menu_vals = [0x10000, 0x10001, 0x10002, 0x10003, 0x10004, 0x10005, 0x100FF, 0x10006,
                 0x1F0003, 0x12345602, 0xFF]

    def spawn(c, fn):
        delay = c.rng.choice([0, 1, 1, 2, 5, c.rng.randrange(1, 40)])
        if fn == ORIG['Test_Menu']:
            v = c.rng.choice(menu_vals) if c.rng.random() < 0.97 else c.rng.getrandbits(32) | 1
        else:
            v = c.rng.choice([0x10000, 1, 0x100FF, c.rng.getrandbits(32) | 1])
        return delay, v
    stack_word = STACK_TOP - 0x100 - 0x28 + 0x18

    def pre(e):
        e.w32(DONE, mode)
        e.w32(stack_word, 0)
    return {'args': [DONE], 'stop': None, 'pin': None, 'spawn': spawn,
            'max_spawns': r.choice([1, 2, 3, 5, 10, r.randrange(1, 40)]), 'pre': pre}


def main():
    env = Env()
    want = sys.argv[1:]
    setups = {
        'Test_Run': run_setup,
        'Test_ScreenTest': task_setup((1, 60)),
        'Test_InputTest': task_setup((1, 400)),
        'Test_SoundTest': task_setup((1, 60)),
        'Test_SystemCheck': syscheck_setup,
        'Test_OutputTest': task_setup((1, 400)),
        'Test_Menu': task_setup((1, 400)),
        'Test_CoinError': coin_setup,
    }
    order = ['Test_CoinError', 'Test_ScreenTest', 'Test_SoundTest', 'Test_OutputTest', 'Test_Menu',
             'Test_SystemCheck', 'Test_InputTest', 'Test_Run']
    fails = 0
    for name in order:
        if want and name not in want:
            continue
        n = int(TRIALS) if TRIALS else COUNTS[name]
        ok, first = 0, None
        for t in range(n):
            err = env.run(name, (zlib.crc32(name.encode()) ^ (t * 7919)) & 0xFFFFFFF, setups[name])
            if err is None:
                ok += 1
            elif first is None:
                first = f'trial {t}: {err}'
        print(f'{name:18s} {"PASS" if ok == n else "FAIL"} {ok}/{n}' + (f'   {first}' if first else ''))
        fails += ok != n
    sys.exit(1 if fails else 0)


if __name__ == '__main__':
    main()
