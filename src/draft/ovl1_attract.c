s16 *Heap_AllocZeroed(M2C_UNK *, M2C_UNK);          /* extern */
M2C_UNK Heap_Free(M2C_UNK *, s16 *);                /* extern */
M2C_UNK Jpeg64_Decode(void *, s16 *, M2C_UNK, M2C_UNK, s32 *, s32 *); /* extern */
M2C_UNK func_8000A76C(void (*)(s32 *), M2C_UNK, M2C_UNK, s32 *); /* extern */
M2C_UNK func_8000AEAC();                            /* extern */
M2C_UNK func_80011E60();                            /* extern */
M2C_UNK func_80012090(M2C_UNK *);                   /* extern */
void *func_800145F0(M2C_UNK, s16 *, s32, s32, s32, s32); /* extern */
M2C_UNK func_800146B8(void *);                      /* extern */
M2C_UNK func_80014DD8(s16 *, s32, s32, M2C_UNK, s32, s32, s32, s16 *, s32, s32, s32, s32); /* extern */
void *func_80018350(s32, s32, s32, s32);            /* extern */
M2C_UNK func_80018E20(void *);                      /* extern */
M2C_UNK func_80018E34(void *, s32, s32);            /* extern */
M2C_UNK func_80018EF4(void *, s32, s32, M2C_UNK);   /* extern */
M2C_UNK func_80018F74(void *);                      /* extern */
M2C_UNK func_80018F88(void *);                      /* extern */
M2C_UNK func_80018FA0(void *, s32);                 /* extern */
M2C_UNK func_80019800(M2C_UNK *, M2C_UNK *);        /* extern */
M2C_UNK func_80019C9C(M2C_UNK *, M2C_UNK *);        /* extern */
void ovl1_func_800E0288(s32 *arg0);                 /* static */
void ovl1_func_800E0970(s32 *arg0);                 /* static */
void ovl1_func_800E0C80(s32 *arg0);                 /* static */
void ovl1_func_800E0F40(s32 *arg0);                 /* static */
extern M2C_UNK D_3EB4;
extern M2C_UNK D_611F0;
extern s32 D_800AF260;
extern u32 D_800AF264;
extern s32 D_800B04CC;
extern u32 D_800B056C;
extern s32 D_800B0608;
extern s32 D_800B061C;
extern s32 D_800B4A30;
extern M2C_UNK D_800C0D48;
extern s32 D_800C1674;
extern M2C_UNK D_800C1730;
extern u16 D_800C30C4;
extern s32 D_800C30C8;
extern s32 D_800C51CC;
extern s32 D_800C52F4;
extern M2C_UNK D_80173600;
static M2C_UNK ovl1_D_800E0FC0;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E114C;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1200;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1204;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1240;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1244;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1280;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E13E8;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E14C0;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E14C4;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1510;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1514;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1560;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1710;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1720;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl1_D_800E1768;                     /* unable to generate initializer: unknown type */
static f64 ovl1_D_800E1780 = 0.9;
static f64 ovl1_D_800E1790 = 0.9;
static M2C_UNK ovl1_D_800E17A0;
static M2C_UNK ovl1_D_800E17A4;
static void *ovl1_D_800E17A8;
static void *ovl1_D_800E17AC;
static void *ovl1_D_800E17B0;
static void *ovl1_D_800E17C4;
static void *ovl1_D_800E17C8;
static void *ovl1_D_800E17CC;
static s32 ovl1_D_800E17F0[1];
static s32 ovl1_D_800E1818;
static void *ovl1_D_800E1820;
static void *ovl1_D_800E1834;
static void *ovl1_D_800E183C;
static void *ovl1_D_800E1840;
static s32 ovl1_D_800E186C[1];
static void *ovl1_D_800E18A0;
static void *ovl1_D_800E18C8;

s32 ovl1_func_800E0000(s32 *arg0) {
    s32 sp18;
    s32 temp_s0;
    void (*var_a0)(s32 *);

    sp18 = 0;
    func_80011E60();
    func_80012090(&D_800C1730);
    D_800C51CC = 1;
    temp_s0 = *arg0;
    switch (temp_s0) {                              /* irregular */
    case 0:
        var_a0 = ovl1_func_800E0288;
block_11:
        func_8000A76C(var_a0, 0x14, 8, &sp18);
        break;
    case 1:
        var_a0 = ovl1_func_800E0970;
        goto block_11;
    case 2:
        var_a0 = ovl1_func_800E0C80;
        goto block_11;
    case 3:
        var_a0 = ovl1_func_800E0F40;
        goto block_11;
    }
loop_13:
    if (sp18 == 0) {
        func_8000AEAC();
        goto loop_13;
    }
    D_800C51CC = 1;
    return sp18 & 0xFFFF;
}

void ovl1_func_800E00E4(void) {
    s32 temp_hi;
    s32 temp_v1;
    s32 var_s0;
    u32 var_v0;

    var_s0 = D_800C30C8;
    if (var_s0 >= 0x64) {
        var_s0 = 0x63;
    }
    if ((var_s0 == 0) && ((u32) D_800B056C >= 2U)) {
        if (D_800B4A30 == 0) {
            func_80018E34(ovl1_D_800E17C4, ovl1_D_800E17F0[D_800C52F4]);
            func_80018E34(ovl1_D_800E17C8, ovl1_D_800E1818);
            var_v0 = D_800B056C;
        } else {
            func_80018E34(ovl1_D_800E17C4, ovl1_D_800E17F0[D_800C52F4]);
            func_80018E34(ovl1_D_800E17C8, ovl1_D_800E1818);
            var_v0 = D_800AF264;
        }
        func_80018E34(ovl1_D_800E17CC, ovl1_D_800E17F0[var_v0]);
        return;
    }
    temp_hi = MULT_HI(var_s0, 0x66666667);
    temp_v1 = var_s0 / 10;
    if (temp_v1 != 0) {
        func_80018E34(ovl1_D_800E17C4, ovl1_D_800E17F0[temp_v1], temp_hi);
    } else {
        func_80018FA0(ovl1_D_800E17C4, temp_hi);
    }
    func_80018E34(ovl1_D_800E17C8, ovl1_D_800E17F0[var_s0 % 10], MULT_HI(var_s0, 0x66666667));
    func_80018FA0(ovl1_D_800E17CC);
}

void ovl1_func_800E0288(s32 *arg0) {
    f32 sp30;
    f32 sp34;
    f32 sp38;
    f32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 *sp4C;
    s16 *temp_v0;
    s16 *temp_v0_2;
    s16 *var_v1;
    s16 *var_v1_2;
    s32 var_a0;
    s32 var_a0_2;
    s32 var_s0;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s1_3;
    s32 var_s1_4;
    s32 var_s3;
    void *temp_v0_3;
    void *temp_v0_4;

    var_s3 = saved_reg_s3;
    sp4C = arg0;
    temp_v0 = Heap_AllocZeroed(&D_80173600, 0x96000);
    var_a0 = 0;
    var_v1 = temp_v0;
    do {
        *var_v1 = 0;
        var_a0 += 1;
        var_v1 += 2;
    } while (var_a0 <= 0x4AFFF);
    temp_v0_2 = Heap_AllocZeroed(&D_80173600, 0x96000);
    var_a0_2 = 0;
    var_v1_2 = temp_v0_2;
    do {
        *var_v1_2 = 0;
        var_a0_2 += 1;
        var_v1_2 += 2;
    } while (var_a0_2 <= 0x4AFFF);
    Jpeg64_Decode(((s32) &D_3EB4 & 0xFFFFFF) + &D_611F0, temp_v0, 0, 0, &sp40, &sp44);
    var_s1 = 0;
    func_80014DD8(temp_v0, sp40, sp44, 0, 0, sp40, sp44, temp_v0_2, sp40, sp44, 0, 0);
    temp_v0_3 = func_800145F0(0, temp_v0_2, sp40, sp44, 0, 0);
    temp_v0_4 = func_80018350(D_800B061C, M2C_FIELD(temp_v0_3, s32 *, 4), M2C_FIELD(temp_v0_3, s32 *, 0), M2C_FIELD(temp_v0_3, s32 *, 8));
    func_80018F88(temp_v0_4);
    func_80019800(&ovl1_D_800E0FC0, &ovl1_D_800E114C);
    func_8000AEAC();
    sp30 = -256.5f;
    sp38 = 71.5f;
    sp34 = 576.5f;
    sp3C = 71.5f;
loop_5:
    var_s1 += 1;
    func_8000AEAC();
    if (var_s1 >= 0x1F) {
        var_s1_2 = 0;
loop_11:
        var_s1_2 += 1;
        func_8000AEAC();
        if (var_s1_2 >= 0x1F) {
            var_s1_3 = 0;
            func_80018F88(M2C_FIELD(&ovl1_D_800E17A0, void **, 0));
loop_17:
            var_s1_3 += 1;
            func_8000AEAC();
            if (var_s1_3 < 0x1F) {
                ovl1_func_800E00E4();
                if (D_800C30C8 == 0) {
                    func_80018EF4(M2C_FIELD(&ovl1_D_800E17A0, void **, 4), (s32) sp34, (s32) sp3C, 0);
                    func_80018EF4(M2C_FIELD((&ovl1_D_800E17A0 + 4), void **, 4), (s32) sp30, (s32) sp38, 0);
                    sp30 += (94.5f - sp30) / 4.0f;
                    sp38 += (50.5f - sp38) / 4.0f;
                    sp34 += (216.5f - sp34) / 4.0f;
                    sp3C += (92.5f - sp3C) / 4.0f;
                    goto loop_17;
                }
            }
        } else {
            ovl1_func_800E00E4();
            if (D_800C30C8 == 0) {
                func_80018EF4(M2C_FIELD(&ovl1_D_800E17A4, void **, 0), (s32) sp34, (s32) sp3C, 0);
                func_80018EF4(M2C_FIELD(&ovl1_D_800E17A4, void **, 4), (s32) sp30, (s32) sp38, 0);
                sp30 += (94.5f - sp30) / 4.0f;
                sp38 += (54.5f - sp38) / 4.0f;
                sp34 += (216.5f - sp34) / 4.0f;
                sp3C += (96.5f - sp3C) / 4.0f;
                goto loop_11;
            }
        }
    } else {
        ovl1_func_800E00E4();
        if (D_800C30C8 == 0) {
            func_80018EF4(M2C_FIELD(&ovl1_D_800E17A4, void **, 0), (s32) sp34, (s32) sp3C, 0);
            func_80018EF4(M2C_FIELD(&ovl1_D_800E17A4, void **, 4), (s32) sp30, (s32) sp38, 0);
            sp30 += (94.5f - sp30) / 8.0f;
            sp38 += (71.5f - sp38) / 8.0f;
            sp34 += (216.5f - sp34) / 8.0f;
            sp3C += (71.5f - sp3C) / 8.0f;
            goto loop_5;
        }
    }
    var_s1_4 = 0;
    var_s0 = 0;
    func_80018EF4(ovl1_D_800E17A8, 0x5E, 0x32, 0);
    func_80018EF4(M2C_FIELD(&ovl1_D_800E17A4, void **, 0), 0xD8, 0x5C, 0);
loop_21:
    var_s1_4 += 1;
    func_8000AEAC();
    if (D_800C30C8 != 0) {
        var_s0 = 0;
    } else {
        var_s0 += 1;
    }
    if (var_s0 >= 0x12D) {
        var_s3 = 4;
    } else if (D_800C30C4 != 0) {
        D_800C1674 = 1;
    } else if (D_800AF260 == 0) {
        ovl1_func_800E00E4();
        if (var_s1_4 & 0x10) {
            func_80018F88(ovl1_D_800E17AC);
        } else {
            func_80018FA0(ovl1_D_800E17AC);
        }
        if (D_800C30C8 != 0) {
            if (var_s1_4 & 0x10) {
                func_80018F88(ovl1_D_800E17B0);
            } else {
                func_80018FA0(ovl1_D_800E17B0);
            }
        }
        var_s3 = 0;
        if (D_800C30C8 != 0) {
            if (M2C_FIELD(&D_800C0D48, u16 *, 0) & 0x100) {
                var_s3 = 1;
                goto block_42;
            }
            if (M2C_FIELD(&D_800C0D48, u16 *, 2) & 0x100) {
                var_s3 = 2;
block_42:
                D_800C30C8 -= 1;
            } else {
                goto loop_21;
            }
        } else {
            goto loop_21;
        }
    }
    func_80018E20(temp_v0_4);
    func_80019C9C(&ovl1_D_800E0FC0, &ovl1_D_800E114C);
    func_8000AEAC();
    func_8000AEAC();
    func_800146B8(temp_v0_3);
    Heap_Free(&D_80173600, temp_v0_2);
    Heap_Free(&D_80173600, temp_v0);
    func_8000AEAC();
    *sp4C = var_s3 | 0x10000;
}

void ovl1_func_800E0970(s32 *arg0) {
    M2C_UNK sp10;
    M2C_UNK *var_s0;
    M2C_UNK *var_s0_2;
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 var_f20;
    f64 temp_f24;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s2;
    s32 var_s2_2;
    s32 var_s4;
    s32 var_s4_2;
    void **var_s3;
    void **var_s3_2;
    void *temp_a0;
    void *temp_a0_2;

    var_s2 = 0;
    func_80019800(&ovl1_D_800E1280, &ovl1_D_800E13E8);
    var_s3 = &ovl1_D_800E1820;
    var_s0 = &sp10;
    var_s1 = 0;
    do {
        M2C_FIELD(var_s0, f32 *, 0) = (f32) *(&ovl1_D_800E1240 + var_s1);
        temp_f0 = *(&ovl1_D_800E1244 + var_s1);
        var_s2 += 1;
        var_s1 += 8;
        M2C_FIELD(var_s0, f32 *, 4) = temp_f0;
        temp_a0 = *var_s3;
        temp_a1 = (s32) M2C_FIELD(var_s0, f32 *, 0);
        var_s3 += 4;
        var_s0 += 8;
        func_80018EF4(temp_a0, temp_a1, (s32) temp_f0, 0);
        var_s4 = 0;
    } while (var_s2 < 8);
    func_80018E34(ovl1_D_800E1834, ovl1_D_800E186C[D_800B0608]);
    func_80018E34(ovl1_D_800E183C, ovl1_D_800E186C[D_800B04CC]);
loop_3:
    var_s4 += 1;
    func_8000AEAC();
    if (D_800C30C4 != 0) {
        D_800C1674 = 1;
    } else {
        var_s2_2 = 0;
        if (D_800AF260 == 0) {
            var_s3_2 = &ovl1_D_800E1834 - 0x14;
            var_s0_2 = &sp10;
            var_s1_2 = 0;
            do {
                temp_f2 = M2C_FIELD(var_s0_2, f32 *, 0);
                M2C_FIELD(var_s0_2, f32 *, 0) = (f32) (temp_f2 + ((*(&ovl1_D_800E1200 + var_s1_2) - temp_f2) / 8.0f));
                temp_f2_2 = M2C_FIELD(var_s0_2, f32 *, 4);
                temp_f2_3 = temp_f2_2 + ((*(&ovl1_D_800E1204 + var_s1_2) - temp_f2_2) / 8.0f);
                var_s2_2 += 1;
                var_s1_2 += 8;
                temp_a1_2 = (s32) M2C_FIELD(var_s0_2, f32 *, 0);
                M2C_FIELD(var_s0_2, f32 *, 4) = temp_f2_3;
                temp_a0_2 = *var_s3_2;
                var_s3_2 += 4;
                var_s0_2 += 8;
                func_80018EF4(temp_a0_2, temp_a1_2, (s32) temp_f2_3, 0);
            } while (var_s2_2 < 8);
            if (var_s4 >= 0x3D) {
                var_s4_2 = 0;
                var_f20 = 4.0f;
                temp_f24 = ovl1_D_800E1780;
loop_10:
                var_s4_2 += 1;
                func_8000AEAC();
                if (D_800C30C4 != 0) {
                    D_800C1674 = 1;
                } else if (D_800AF260 == 0) {
                    M2C_FIELD(ovl1_D_800E1840, f32 *, 0x9C) = var_f20;
                    M2C_FIELD(ovl1_D_800E1840, f32 *, 0xA0) = var_f20;
                    func_80018F74(ovl1_D_800E1840);
                    func_80018F88(ovl1_D_800E1840);
                    var_f20 = (f32) ((f64) var_f20 * temp_f24);
                    if (var_f20 < 1.0f) {
                        var_f20 = 1.0f;
                    }
                    if (var_s4_2 < 0x3D) {
                        goto loop_10;
                    }
                }
            } else {
                goto loop_3;
            }
        }
    }
    func_80019C9C(&ovl1_D_800E1280, &ovl1_D_800E13E8);
    func_8000AEAC();
    func_8000AEAC();
    *arg0 = 0x10000;
}

void ovl1_func_800E0C80(s32 *arg0) {
    M2C_UNK sp10;
    M2C_UNK *var_s0;
    M2C_UNK *var_s0_2;
    f32 temp_f0;
    f32 temp_f2;
    f32 temp_f2_2;
    f32 temp_f2_3;
    f32 var_f20;
    f64 temp_f24;
    s32 temp_a1;
    s32 temp_a1_2;
    s32 var_s1;
    s32 var_s1_2;
    s32 var_s2;
    s32 var_s2_2;
    s32 var_s4;
    s32 var_s4_2;
    void **var_s3;
    void **var_s3_2;
    void *temp_a0;
    void *temp_a0_2;

    var_s2 = 0;
    func_80019800(&ovl1_D_800E1560, &ovl1_D_800E1710);
    var_s3 = &ovl1_D_800E18A0;
    var_s0 = &sp10;
    var_s1 = 0;
    do {
        M2C_FIELD(var_s0, f32 *, 0) = (f32) *(&ovl1_D_800E1510 + var_s1);
        temp_f0 = *(&ovl1_D_800E1514 + var_s1);
        var_s2 += 1;
        var_s1 += 8;
        M2C_FIELD(var_s0, f32 *, 4) = temp_f0;
        temp_a0 = *var_s3;
        temp_a1 = (s32) M2C_FIELD(var_s0, f32 *, 0);
        var_s3 += 4;
        var_s0 += 8;
        func_80018EF4(temp_a0, temp_a1, (s32) temp_f0, 0);
        var_s4 = 0;
    } while (var_s2 < 0xA);
loop_3:
    var_s4 += 1;
    func_8000AEAC();
    if (D_800C30C4 != 0) {
        D_800C1674 = 1;
    } else {
        var_s2_2 = 0;
        if (D_800AF260 == 0) {
            var_s3_2 = &ovl1_D_800E18A0;
            var_s0_2 = &sp10;
            var_s1_2 = 0;
            do {
                temp_f2 = M2C_FIELD(var_s0_2, f32 *, 0);
                M2C_FIELD(var_s0_2, f32 *, 0) = (f32) (temp_f2 + ((*(&ovl1_D_800E14C0 + var_s1_2) - temp_f2) / 8.0f));
                temp_f2_2 = M2C_FIELD(var_s0_2, f32 *, 4);
                temp_f2_3 = temp_f2_2 + ((*(&ovl1_D_800E14C4 + var_s1_2) - temp_f2_2) / 8.0f);
                var_s2_2 += 1;
                var_s1_2 += 8;
                temp_a1_2 = (s32) M2C_FIELD(var_s0_2, f32 *, 0);
                M2C_FIELD(var_s0_2, f32 *, 4) = temp_f2_3;
                temp_a0_2 = *var_s3_2;
                var_s3_2 += 4;
                var_s0_2 += 8;
                func_80018EF4(temp_a0_2, temp_a1_2, (s32) temp_f2_3, 0);
            } while (var_s2_2 < 0xA);
            if (var_s4 >= 0x3D) {
                var_s4_2 = 0;
                var_f20 = 4.0f;
                temp_f24 = ovl1_D_800E1790;
loop_10:
                var_s4_2 += 1;
                func_8000AEAC();
                if (D_800C30C4 != 0) {
                    D_800C1674 = 1;
                } else if (D_800AF260 == 0) {
                    M2C_FIELD(ovl1_D_800E18C8, f32 *, 0x9C) = var_f20;
                    M2C_FIELD(ovl1_D_800E18C8, f32 *, 0xA0) = var_f20;
                    func_80018F74(ovl1_D_800E18C8);
                    func_80018F88(ovl1_D_800E18C8);
                    var_f20 = (f32) ((f64) var_f20 * temp_f24);
                    if (var_f20 < 1.0f) {
                        var_f20 = 1.0f;
                    }
                    if (var_s4_2 < 0x3D) {
                        goto loop_10;
                    }
                }
            } else {
                goto loop_3;
            }
        }
    }
    func_80019C9C(&ovl1_D_800E1560, &ovl1_D_800E1710);
    func_8000AEAC();
    func_8000AEAC();
    *arg0 = 0x10000;
}

void ovl1_func_800E0F40(s32 *arg0) {
    s32 var_s0;

    var_s0 = 0;
    func_80019800(&ovl1_D_800E1720, &ovl1_D_800E1768);
    do {
        var_s0 += 1;
        func_8000AEAC();
    } while (var_s0 < 0xB5);
    func_80019C9C(&ovl1_D_800E1720, &ovl1_D_800E1768);
    func_8000AEAC();
    func_8000AEAC();
    *arg0 = 0x10000;
}
