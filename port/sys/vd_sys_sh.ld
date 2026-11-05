/* Vivid Dolls port - symbols only the port's own assembly needs (sh-elf, leading _).
 * gTaskCurrent (0x800AF22C) is used by the task switcher (task_sh4.S) but by no
 * game C file, so port/gen/vd_blocks_sh.ld does not define it. */
_D_800AF22C = _vd_blk_main_bss + (0x800AF22C - 0x8003E130);
