/* Vivid Dolls port - EEPROM (512 bytes) kept in RAM only; controller paks
 * report "no pak".
 *
 * The game itself never saves: the only EEPROM access is the test mode's
 * system check, which writes a test pattern and reads it back. So nothing is
 * stored on the VMU. The RAM copy survives a soft reset, like a real EEPROM.
 */
#include <kos.h>
#include <string.h>
#include "ultra_internal.h"

#define EEPROM_SIZE   512
#define EEPROM_TYPE_4K 1
#define PFS_ERR_NOPACK 1

static u8 vd_eeprom[EEPROM_SIZE];

void vd_eeprom_init(void)
{
}

/* ---- libultra EEPROM ---- */

s32 osEepromProbe(void *mq)
{
    (void)mq;
    return EEPROM_TYPE_4K;
}

s32 osEepromLongRead(void *mq, u8 addr, u8 *buf, s32 n)
{
    u32 off = (u32)addr * 8;

    (void)mq;
    if (n < 0 || off + (u32)n > EEPROM_SIZE)
        return -1;
    memcpy(buf, vd_eeprom + off, n);
    return 0;
}

s32 osEepromLongWrite(void *mq, u8 addr, u8 *buf, s32 n)
{
    u32 off = (u32)addr * 8;

    (void)mq;
    if (n < 0 || off + (u32)n > EEPROM_SIZE)
        return -1;
    memcpy(vd_eeprom + off, buf, n);
    return 0;
}

/* ---- controller pak: never present ---- */

s32 osPfsIsPlug(void *mq, u8 *pattern)
{
    (void)mq;
    if (pattern != NULL)
        *pattern = 0;
    return 0;
}

s32 osPfsInitPak(void *mq, void *pfs, s32 channel)
{
    (void)mq; (void)pfs; (void)channel;
    return PFS_ERR_NOPACK;
}

s32 osPfsAllocateFile(void *pfs, u16 company, u32 game, u8 *name, u8 *ext, s32 length, s32 *file_no)
{
    (void)pfs; (void)company; (void)game; (void)name; (void)ext; (void)length; (void)file_no;
    return PFS_ERR_NOPACK;
}

s32 osPfsFindFile(void *pfs, u16 company, u32 game, u8 *name, u8 *ext, s32 *file_no)
{
    (void)pfs; (void)company; (void)game; (void)name; (void)ext; (void)file_no;
    return PFS_ERR_NOPACK;
}

s32 osPfsReadWriteFile(void *pfs, s32 file_no, u8 flag, s32 offset, s32 size, u8 *buf)
{
    (void)pfs; (void)file_no; (void)flag; (void)offset; (void)size; (void)buf;
    return PFS_ERR_NOPACK;
}

s32 osPfsDeleteFile(void *pfs, u16 company, u32 game, u8 *name, u8 *ext)
{
    (void)pfs; (void)company; (void)game; (void)name; (void)ext;
    return PFS_ERR_NOPACK;
}

s32 osPfsChecker(void *pfs)
{
    (void)pfs;
    return PFS_ERR_NOPACK;
}

VD_ALIAS(func_80024850, osEepromProbe);
VD_ALIAS(func_800249C0, osEepromLongRead);
VD_ALIAS(func_800248D0, osEepromLongWrite);
VD_ALIAS(func_80024360, osPfsIsPlug);
VD_ALIAS(func_80024A30, osPfsInitPak);
VD_ALIAS(func_800230D0, osPfsAllocateFile);
VD_ALIAS(func_80023B60, osPfsReadWriteFile);
VD_ALIAS(func_800236C0, osPfsDeleteFile);
