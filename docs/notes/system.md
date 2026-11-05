# System area (0x8000A5A0-0x8000D000)

Code: `src/game/system.c`, `src/game/system.h`. Test: `python3 tools/difftest/test_system.py`
(38 functions, all PASS; 300 random trials each except the tiny/no-argument ones; takes about 6 minutes).
You can test only some functions: `python3 tools/difftest/test_system.py Sched_Main Task_Create`.

How the test works: every libultra call and every callee from other areas is a Python stub
that records its name and arguments. Stack addresses are recorded as `STK`, and addresses of
the C functions are translated to the original ones (for thread entry points). The UART at
0x02000040 is an emulated MMIO device that gives scripted status values and records every
access. `osRecvMesg` feeds scripted messages (one script per queue where needed). A thread
that never returns is stopped when its script runs out, or after a set number of instructions
for spin loops. After each call the test compares the return value, the call/IO log, the
data+BSS (0x80038000-0x800C5400), the test memory and any extra regions (overlay area, frame
buffers, task pool).

## Functions done
| Address | Name | What it does |
|---|---|---|
| 0x8000A5A0 | Tlb_ClearAll | write all 32 TLB entries with harmless KSEG0 entries (pagemask 0x2000, EntryLo 0) |
| 0x8000A628 | Tlb_Map | map vaddr->paddr pair in entry `index`; page size from table D_80038410 (returns 0 if the table index doesn't match); cache mode 3 if `cached` else 2; paddr shifted **signed** (`sra`) |
| 0x8000A6D0 | Task_InitPool | set up the task pool: header, NULL-terminated stack of free blocks (16-aligned after the pointer array), head/tail sentinels |
| 0x8000A76C | Task_Create | `Task_Create(func, prio, argBytes, ...)`: take a block, insert it before the first task with a higher prio, copy the varargs words to the top of the block's stack and again into a0.. of a fresh context frame (0xC0 bytes, ra = Task_Exit, resume pc = func) |
| 0x8000A8D0 | Task_Free | push the block back, unlink (keeps t->next so Task_Exit can carry on); returns t |
| 0x8000A924 | Mtx_ToFixed | guMtxF2L, **destination first** (a0 = Mtx, a1 = float[4][4]) |
| 0x8000A9E8 | Mtx_FromFixed | guMtxL2F, destination first (unused) |
| 0x8000AAE0 | Math_AbsF | abs.s (unused) |
| 0x8000AAF0 | Uart_Init | 8251-style init: ctrl 0x0B0B, 0,0,0, 0x4040 (reset), 0x4E (mode), 0x25 (command), ctrl 0x0F04, with delays |
| 0x8000ABA0 | Uart_PutByte | wait for status bit 0 (1001 polls max), write byte; 0 or -1 |
| 0x8000AC24 | Uart_GetByte | wait for status bit 1 (1001 polls max), read byte; byte or -1 |
| 0x8000AC88 | Uart_ReadData | data register & 0xFF |
| 0x8000AC98 | Uart_ReadStatus | status register |
| 0x8000ACA4 | Uart_Delay | busy-wait n+1 short delays |
| 0x8000B210 | Main_Boot | osInitialize, TLB (0x01000000->0xC0000000 with 1M pages, 0x02000000->0xC0800000, 0x02010000->0xC0C00000, 4K pages, uncached), Main_ParseArgs(NULL), start the idle thread |
| 0x8000B2D0 | Main_IdleThread | PI manager (pri 150), fault thread (id 5, pri 250), gfx thread (id 6, pri 11; not started if gArgDebug), then pri 0 and spin |
| 0x8000B3C8 | Input_Thread | thread id 8 (scheduler client on D_800B04E0). On every retrace: controller housekeeping, then builds button words for 2 players (D_800B0720: 0 copy, 1 clear, 2 remap pad bits; when both U+D or L+R are held it uses the other set of inputs), coin/credit handling (max 99), retrace counter. Pre-NMI: frames-in-flight += 2 |
| 0x8000BD28 | Main_GfxThread | thread id 6: init everything (Main_InitSystems, ObjList_Init), start the controller and input threads; on retrace builds a frame (func_8000FF64 into one of 2 gfx buffers of 0x2180 bytes) when < 2 frames are in flight; message type 2 retires one |
| 0x8000BFE8 | Rom_Read | blocking PI DMA in 1 KB pieces (osInvalDCache first) |
| 0x8000C0D4 | Overlay_Load | clear 0x800E0000..+0x4D000, flush caches, DMA the overlay (gOverlayTable {vram, ?, romStart, romEnd}) |
| 0x8000C218 | Main_ForceLink | never called; calls 8 libultra functions with zero arguments (so they get linked) |
| 0x8000C2A8 | Main_InitSystems | queues, camera/light, heap (0x52B000 at 0x80173600), AllocList, Scene_Init, scheduler (pri 15, VI mode 2, 1 field) + 2 clients, audio, task pool (0x114000 bytes at 0x8069E600, 0x4000-byte blocks), func_8001BF08 |
| 0x8000C424 | Cont_Thread | thread id 7: on SI message type 5, either starts the next read (if D_800AF254) or waits on D_800B0520; D_800C51D0 = where it is blocked |
| 0x8000C510 | Fault_Thread | thread id 5: osSetEventMesg(OS_EVENT_FAULT), wait, D_8003E130 = faulted thread, show it (func_8000D150) |
| 0x8000C590 | Sched_Create | osScCreateScheduler (doAudio not cleared, as in the SDK), plus osViBlack(1) |
| 0x8000C6D8 | Sched_AddClient | osScAddClient |
| 0x8000C730 | Sched_RemoveClient | osScRemoveClient |
| 0x8000C7C0 | Sched_GetCmdQ | osScGetCmdQ |
| 0x8000C7C8 | Sched_Main | __scMain plus profiler hooks; on pre-NMI waits 8 retraces then, for the next 8 messages (of any kind), wipes every 8th column of both frame buffers (on retraces), then idles |
| 0x8000C9D8 | Sched_HandleRetrace | __scHandleRetrace |
| 0x8000CADC | Sched_HandleRSP | __scHandleRSP |
| 0x8000CBCC | Sched_HandleRDP | __scHandleRDP |
| 0x8000CC5C | Sched_TaskReady | __scTaskReady |
| 0x8000CCB0 | Sched_TaskComplete | __scTaskComplete + un-blank the screen on the first swap (D_80038478), RCP busy counter D_80043320, profiler markers |
| 0x8000CD8C | Sched_AppendList | __scAppendList |
| 0x8000CDE4 | Sched_Exec | __scExec, but **always** osWritebackDCacheAll; sets D_80043320 = 2 for gfx tasks; D_80038470 = 1 / D_80038474 = 0 when the RDP starts |
| 0x8000CEF8 | Sched_Yield | __scYield |
| 0x8000CF30 | Sched_Schedule | __scSchedule; task type = flags & 7: 3 = XBUS (RSP+RDP together), 2/6/7 = RSP then RDP |

## Functions remaining (hand-written assembly, not turned into C on purpose)
These only make sense as assembly (register save/restore, coprocessor 0); the Dreamcast
port will need its own versions (e.g. a small context-switch routine, or KOS threads).
| Address | Proposed name | What it does |
|---|---|---|
| 0x8000A910 | Va_SpillArgs | stores a0-a3 into the caller's argument home slots (0..0xC(sp)); no callers |
| 0x8000ACD0 | Tlb_Read | `Tlb_Read(index, TlbEntry *out)`: tlbr, out = {EntryHi, EntryLo0, EntryLo1, PageMask}; returns out; no callers |
| 0x8000ADA4 | Tlb_Write | `Tlb_Write(index, TlbEntry *e)`: tlbwi with e's 4 words; returns e |
| 0x8000AE90 | Task_Exit | where a task goes when its function returns: Task_Free(current task), then switch to current->next (Task_SwitchNext) |
| 0x8000AEAC | Task_Yield | (33 callers) saves all registers incl. f0-f31 in a 0xC0 frame on the stack; if the current task (D_800AF22C) has a next, stores sp in current->sp and falls into Task_SwitchNext; otherwise returns at once |
| 0x8000AF94 | Task_SwitchNext | `t2` = task: current = t2->next, restores its frame from current->sp and jumps to frame[0x3C] with ra = frame[0x38] |
| 0x8000B06C | Task_RunAll | called once per frame by Main_GameLoop: saves the caller's context in the tail sentinel (D_800C53FC->sp) and switches to head->next. Every task runs until it yields; reaching the tail sentinel restores the saved context, so Task_RunAll returns |
| 0x8000B150 | Task_SaveContext | stores a0-a3, s0-s7, gp, fp, ra and f0-f31 into a 0xC0-byte frame at a0 (used by Task_Create for the initial frame) |

Context frame layout (0xC0 bytes): 0x00 a0-a3, 0x10 s0-s7, 0x30 gp, 0x34 fp, 0x38 ra,
0x3C resume pc, 0x40-0xBC f0-f31.

## Data symbols
```
Tlb_ClearAll = 0x8000A5A0; // type:func
Tlb_Map = 0x8000A628; // type:func
Task_InitPool = 0x8000A6D0; // type:func
Task_Create = 0x8000A76C; // type:func
Task_Free = 0x8000A8D0; // type:func
Va_SpillArgs = 0x8000A910; // type:func
Mtx_ToFixed = 0x8000A924; // type:func
Mtx_FromFixed = 0x8000A9E8; // type:func
Math_AbsF = 0x8000AAE0; // type:func
Uart_Init = 0x8000AAF0; // type:func
Uart_PutByte = 0x8000ABA0; // type:func
Uart_GetByte = 0x8000AC24; // type:func
Uart_ReadData = 0x8000AC88; // type:func
Uart_ReadStatus = 0x8000AC98; // type:func
Uart_Delay = 0x8000ACA4; // type:func
Tlb_Read = 0x8000ACD0; // type:func
Tlb_Write = 0x8000ADA4; // type:func
Task_Exit = 0x8000AE90; // type:func
Task_Yield = 0x8000AEAC; // type:func
Task_SwitchNext = 0x8000AF94; // type:func
Task_RunAll = 0x8000B06C; // type:func
Task_SaveContext = 0x8000B150; // type:func
Main_IdleThread = 0x8000B2D0; // type:func
Input_Thread = 0x8000B3C8; // type:func
Main_GfxThread = 0x8000BD28; // type:func
Main_ForceLink = 0x8000C218; // type:func
Main_InitSystems = 0x8000C2A8; // type:func
Cont_Thread = 0x8000C424; // type:func
Fault_Thread = 0x8000C510; // type:func
Sched_Create = 0x8000C590; // type:func
Sched_AddClient = 0x8000C6D8; // type:func
Sched_RemoveClient = 0x8000C730; // type:func
Sched_GetCmdQ = 0x8000C7C0; // type:func
Sched_Main = 0x8000C7C8; // type:func
Sched_HandleRetrace = 0x8000C9D8; // type:func
Sched_HandleRSP = 0x8000CADC; // type:func
Sched_HandleRDP = 0x8000CBCC; // type:func
Sched_TaskReady = 0x8000CC5C; // type:func
Sched_TaskComplete = 0x8000CCB0; // type:func
Sched_AppendList = 0x8000CD8C; // type:func
Sched_Exec = 0x8000CDE4; // type:func
Sched_Yield = 0x8000CEF8; // type:func
Sched_Schedule = 0x8000CF30; // type:func
osWritebackDCacheAll = 0x8001C990; // type:func
osPiStartDma = 0x80025950; // type:func
gTlbPageSizes = 0x80038410; // type:TlbPageSize size:0x60
gSchedRCPBusy = 0x80043320; // type:u32 size:0x4
gTaskPool = 0x800C51DC; // type:TaskPool* size:0x4
gTaskHead = 0x800B05A4; // type:Task* size:0x4
gTaskTail = 0x800C53FC; // type:Task* size:0x4
gTaskCurrent = 0x800AF22C; // type:Task* size:0x4
gScheduler = 0x800C0AC0; // type:OSSched size:0x284
gSchedCmdQ = 0x800B0568; // type:OSMesgQueue* size:0x4
gFramesInFlight = 0x800C53F8; // type:u32 size:0x4
gGfxBufIndex = 0x800C0AB8; // type:s32 size:0x4
gGfxBuffers = 0x800B0730; // type:u8 size:0x4300
gRetraceCount = 0x80043314; // type:u32 size:0x4
gRetracesPerFrame = 0x800C51CC; // type:u32 size:0x4
gCredits = 0x800C30C8; // type:u32 size:0x4
gFrameBuffer0 = 0x807B2600; // type:u16 size:0x25800
gFrameBuffer1 = 0x807D7E00; // type:u16 size:0x25800
gTaskPoolMem = 0x8069E600; // type:u8 size:0x114000
gFaultedThread = 0x8003E130; // type:OSThread* size:0x4
gPiMsgQueue = 0x80041918; // type:OSMesgQueue size:0x18
gRetraceMsgQueueInput = 0x800B04E0; // type:OSMesgQueue size:0x18
gRetraceMsgQueueGfx = 0x800C30E0; // type:OSMesgQueue size:0x18
gSiMsgQueue = 0x800C30A8; // type:OSMesgQueue size:0x18
```
(The function names Main_Boot, Rom_Read and Overlay_Load were already in symbol_addrs.txt.)

## Structs and globals learnt
- **Task pool** (`TaskPool` in system.h, at *D_800C51DC; created by Main_InitSystems in
  0x8069E600 with 0x4000-byte blocks): 0x00 blockSize, 0x08 freeTop (stack of free
  blocks), 0x0C head sentinel {next, prev, sp, prio}, 0x1C tail sentinel, 0x2C free-block
  pointers. Task: 0x0 next, 0x4 prev, 0x8 saved sp (context frame), 0xC prio. The list is
  sorted by prio (ascending, FIFO among equals). The arguments are at the top of the block
  and the first context frame is 0xC0 bytes below them. D_800AF22C = current task.
- **OSSched** at D_800C0AC0 has exactly the N64 SDK layout (see system.h). Clients:
  D_800B054C -> queue D_800C30E0 (gfx thread), D_800C165C -> queue D_800B04E0 (input
  thread). Scheduler messages: 0x29A retrace, 0x29B SP done, 0x29C DP done, 0x29D pre-NMI.
- func_8001BE88/BE90/BE98 are empty profiler hooks; BE98 gets a marker (0x08/0x09 task
  start, 0x48/0x49/0x4A task done, 0x81/0xC1 retrace, 0x82/0xC2 end of frame).
- func_8000F750 just returns the u16 at D_800A6840 (a controller status flag).
- Aleck64 UART registers (TLB entry 2): 0x02000040 data, 0x02000042 status/command,
  0x02000044 control. The UART functions have no callers (left-over debug code).
- Pad remap (Input_Thread mode 2): U->1, D->2, L->4, R->8, A->0x10, B->0x20, Start->0x100.
  The source word arrays (2 players each, u16): D_800B058C / D_800AF222 / D_800AF238
  normally, or D_800C5334 / D_800B0596 / D_800B043A when opposite directions are held;
  outputs D_800B060C, D_800C0D48, D_800AF258. Mode 0 copies D_800B0498, D_800C532C, D_800B4A80.
- `__osContRamRead` (0x800225F0) is called with 4 arguments (mq, 0, 0, buf) by
  Cont_Thread, so the name in symbol_addrs.txt is probably wrong (maybe a controller read
  start function).
- Overlay table entries (gOverlayTable, 16 bytes): {vram, unused, romStart, romEnd}.
