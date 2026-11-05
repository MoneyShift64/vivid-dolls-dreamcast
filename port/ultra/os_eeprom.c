/* Vivid Dolls port - EEPROM (512 bytes, bookkeeping and settings) kept in RAM
 * and saved to the first VMU found as VIVIDOLL.EEP; controller paks report
 * "no pak".
 *
 * osEepromLongWrite returns at once; a low-priority KOS thread writes the VMU
 * file about half a second after the last change (VMU writes are slow).
 * The RAM copy survives a soft reset, like a real EEPROM.
 */
#include <kos.h>
#include <string.h>
#include <dc/vmu_pkg.h>
#include "ultra_internal.h"

#define EEPROM_SIZE   512
#define EEPROM_TYPE_4K 1
#define PFS_ERR_NOPACK 1
#define VMU_FILE      "VIVIDOLL.EEP"

/* icon: port/assets/vmu_icon.png, converted at build time by tools/port/gen_vmu_icon.py */
extern const unsigned short vd_vmu_icon_pal[16];
extern const unsigned char vd_vmu_icon_data[512];

static u8 vd_eeprom[EEPROM_SIZE];
static volatile int vd_eeprom_dirty;
static semaphore_t vd_eeprom_sem;
static int vd_eeprom_started;

static int vd_vmu_path(char *out, size_t n)
{
    maple_device_t *dev = maple_enum_type(0, MAPLE_FUNC_MEMCARD);

    if (dev == NULL)
        return -1;
    snprintf(out, n, "/vmu/%c%d/" VMU_FILE, 'a' + dev->port, dev->unit);
    return 0;
}

static void vd_eeprom_load(void)
{
    char path[32];
    file_t f;
    int size;
    uint8_t *buf;
    vmu_pkg_t pkg;

    memset(vd_eeprom, 0, sizeof(vd_eeprom));
    if (vd_vmu_path(path, sizeof(path)) < 0)
        return;
    f = fs_open(path, O_RDONLY);
    if (f < 0)
        return;
    size = fs_total(f);
    buf = size > 0 ? malloc(size) : NULL;
    if (buf != NULL && fs_read(f, buf, size) == size &&
        vmu_pkg_parse(buf, size, &pkg) == 0 && pkg.data_len >= EEPROM_SIZE)
        memcpy(vd_eeprom, pkg.data, EEPROM_SIZE);
    free(buf);
    fs_close(f);
}

static void vd_eeprom_save(void)
{
    static uint8_t icon[512];
    char path[32];
    vmu_pkg_t pkg;
    uint8_t *out = NULL;
    int out_size = 0, i;
    u8 data[EEPROM_SIZE];
    file_t f;

    memcpy(data, vd_eeprom, EEPROM_SIZE);
    if (vd_vmu_path(path, sizeof(path)) < 0)
        return;
    memset(&pkg, 0, sizeof(pkg));
    strcpy(pkg.desc_short, "Vivid Dolls");
    strcpy(pkg.desc_long, "Vivid Dolls settings/bookkeeping");
    strcpy(pkg.app_id, "VIVIDDOLLS");
    pkg.icon_cnt = 1;
    pkg.icon_anim_speed = 0;
    for (i = 0; i < 16; i++)
        pkg.icon_pal[i] = vd_vmu_icon_pal[i];
    memcpy(icon, vd_vmu_icon_data, sizeof(icon));
    pkg.icon_data = icon;
    pkg.data_len = EEPROM_SIZE;
    pkg.data = data;
    if (vmu_pkg_build(&pkg, &out, &out_size) < 0 || out == NULL)
        return;
    fs_unlink(path);
    f = fs_open(path, O_WRONLY | O_CREAT | O_TRUNC);
    if (f >= 0) {
        fs_write(f, out, out_size);
        fs_close(f);
    }
    free(out);
}

static void *vd_eeprom_writer(void *arg)
{
    (void)arg;
    for (;;) {
        sem_wait(&vd_eeprom_sem);
        thd_sleep(500);
        while (sem_trywait(&vd_eeprom_sem) == 0)
            ;
        if (vd_eeprom_dirty) {
            vd_eeprom_dirty = 0;
            vd_eeprom_save();
        }
    }
    return NULL;
}

void vd_eeprom_init(void)
{
    kthread_attr_t attr;

    if (vd_eeprom_started)
        return;
    vd_eeprom_started = 1;
    vd_eeprom_load();
    sem_init(&vd_eeprom_sem, 0);
    memset(&attr, 0, sizeof(attr));
    attr.create_detached = true;
    attr.stack_size = 16 * 1024;
    attr.prio = PRIO_MAX - 1;           /* below every game thread except idle */
    attr.label = "eeprom-vmu";
    thd_create_ex(&attr, vd_eeprom_writer, NULL);
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
    if (memcmp(vd_eeprom + off, buf, n) != 0) {
        memcpy(vd_eeprom + off, buf, n);
        vd_eeprom_dirty = 1;
        if (vd_eeprom_started)
            sem_signal(&vd_eeprom_sem);
    }
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
