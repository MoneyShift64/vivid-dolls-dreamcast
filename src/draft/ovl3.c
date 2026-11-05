M2C_UNK func_8000A76C(void (*)(s32 *), M2C_UNK, M2C_UNK, s32 *, s32); /* extern */
M2C_UNK func_8000AEAC();                            /* extern */
M2C_UNK func_80011E60();                            /* extern */
M2C_UNK func_80012090(M2C_UNK *);                   /* extern */
M2C_UNK func_8001860C(void *);                      /* extern */
M2C_UNK func_80018E34(void *, s32, s32);            /* extern */
M2C_UNK func_80018EB8(void *, M2C_UNK, M2C_UNK, s32, s32); /* extern */
M2C_UNK func_80018EF4(void *, s32, s32, M2C_UNK);   /* extern */
M2C_UNK func_80018F74(f32, f32);                    /* extern */
M2C_UNK func_80018F88(void *);                      /* extern */
M2C_UNK func_80018FA0(void *, s32);                 /* extern */
M2C_UNK func_80018FB8(void *, void (*)(void *), M2C_UNK); /* extern */
M2C_UNK func_80018FC8(void *);                      /* extern */
M2C_UNK func_80019800(M2C_UNK *, M2C_UNK *);        /* extern */
M2C_UNK func_80019C9C(M2C_UNK *, M2C_UNK *);        /* extern */
M2C_UNK func_8001BA90(M2C_UNK);                     /* extern */
f32 func_8001C450(f32);                             /* extern */
f32 func_8001C5A0(f32);                             /* extern */
void ovl3_func_800E0278(s32 *arg0);                 /* static */
void ovl3_func_800E09C4(void *arg0);                /* static */
void ovl3_func_800E0A5C(void *arg0);                /* static */
void ovl3_func_800E0AB8(void *arg0);                /* static */
void ovl3_func_800E0AFC(void *arg0);                /* static */
extern M2C_UNK D_800B04D4;
extern M2C_UNK D_800C0D48;
extern u16 D_800C0D4A;
extern s32 D_800C1674;
extern M2C_UNK D_800C1730;
extern M2C_UNK D_800C178C;
extern u16 D_800C30C4;
extern s32 D_800C51CC;
static M2C_UNK ovl3_D_800E0B40;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl3_D_800E0DA4;                     /* unable to generate initializer: unknown type */
static M2C_UNK ovl3_D_800E0F38;
static void *ovl3_D_800E0F3C;
static void *ovl3_D_800E0F40;
static void *ovl3_D_800E0F60;
static void *ovl3_D_800E0F64;
static void *ovl3_D_800E0F68;
static void *ovl3_D_800E0F6C;
static M2C_UNK ovl3_D_800E0F70;
static M2C_UNK ovl3_D_800E0F78;
static s32 ovl3_D_800E0F88[1];
static s32 ovl3_D_800E0FA8[1];
static s32 ovl3_D_800E0FD0[1];

s32 ovl3_func_800E0000(s32 *arg0) {
    s32 sp18;

    sp18 = 0;
    func_80011E60();
    func_80012090(&D_800C1730);
    D_800C51CC = 1;
    if (*arg0 == 0) {
        func_8000A76C(ovl3_func_800E0278, 0x14, 8, &sp18, 0);
    }
loop_3:
    if (sp18 == 0) {
        func_8000AEAC();
        goto loop_3;
    }
    D_800C51CC = 1;
    return sp18 & 0xFFFF;
}

void ovl3_func_800E0094(s32 arg0) {
    f32 temp_f20;
    f32 var_f22;
    s32 temp_a2;
    s32 var_s1;
    void **var_s0;
    void *temp_a0;

    var_s1 = 0;
    var_s0 = &ovl3_D_800E0F40;
    var_f22 = ((f32) arg0 / 256.0f) * 3.1415927f;
    do {
        temp_f20 = (func_8001C450(var_f22) * 120.0f) + 160.0f;
        var_s1 += 1;
        temp_a0 = *var_s0;
        var_s0 += 4;
        temp_a2 = (s32) ((func_8001C5A0(var_f22) * 50.0f) + 120.0f);
        var_f22 += 0.7853982f;
        func_80018EF4(temp_a0, (s32) temp_f20, temp_a2, 0);
    } while (var_s1 < 8);
}

void ovl3_func_800E01A0(s32 arg0) {
    s32 temp_hi;
    s32 temp_s0;
    s32 temp_v0;

    temp_s0 = arg0 / 60;
    temp_hi = MULT_HI(temp_s0, 0x66666667);
    temp_v0 = (arg0 / 60) / 10;
    if (temp_v0 != 0) {
        func_80018E34(ovl3_D_800E0F68, ovl3_D_800E0FD0[temp_v0], temp_hi);
    } else {
        func_80018FA0(ovl3_D_800E0F68, temp_hi);
    }
    func_80018E34(ovl3_D_800E0F6C, ovl3_D_800E0FD0[(arg0 / 60) % 10], MULT_HI(temp_s0, 0x66666667));
}

void ovl3_func_800E0278(s32 *arg0) {
    s32 *sp1C;
    f32 temp_f20;
    f32 temp_f20_2;
    f32 var_f22;
    f32 var_f22_2;
    s32 temp_a2;
    s32 temp_a2_2;
    s32 temp_s0;
    s32 temp_v0;
    s32 temp_v0_2;
    s32 var_hi;
    s32 var_s0;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s1_2;
    s32 var_s1_6;
    s32 var_s2;
    s32 var_s2_2;
    s32 var_s2_3;
    s32 var_s3;
    s32 var_s4;
    s32 var_s5;
    s32 var_v0;
    s32 var_v0_2;
    s32 var_v0_3;
    s32 var_v0_4;
    s32 var_v0_5;
    s32 var_v0_6;
    s32 var_v1;
    s32 var_v1_2;
    u16 var_s1_3;
    u16 var_s1_5;
    void **var_s0_2;
    void **var_s0_5;
    void **var_s1;
    void **var_s1_4;
    void *temp_a0;
    void *temp_a0_2;

    var_s5 = 0x80;
    var_s4 = 0x80;
    sp1C = arg0;
    var_s3 = 6;
    func_80019800(&ovl3_D_800E0B40, &ovl3_D_800E0DA4);
    var_s0 = 0;
    var_s1 = &ovl3_D_800E0F40;
    do {
        var_s0 += 1;
        if (*(&D_800C178C + 2 + var_s0) != 0) {
            func_8001860C(*var_s1);
            func_80018EB8(*var_s1, 0, 0, 0, 0x80);
        }
        var_s1 += 4;
    } while (var_s0 < 8);
    var_s1_2 = 0;
    var_s0_2 = &ovl3_D_800E0F40;
    var_f22 = 0.0f;
    do {
        temp_f20 = (func_8001C450(var_f22) * 120.0f) + 160.0f;
        var_s1_2 += 1;
        temp_a0 = *var_s0_2;
        var_s0_2 += 4;
        temp_a2 = (s32) ((func_8001C5A0(var_f22) * 50.0f) + 120.0f);
        var_f22 += 0.7853982f;
        func_80018EF4(temp_a0, (s32) temp_f20, temp_a2, 0);
    } while (var_s1_2 < 8);
    var_s2 = 0x293;
    func_80018FB8(ovl3_D_800E0F60, ovl3_func_800E0AB8, 0);
    M2C_FIELD(ovl3_D_800E0F60, s32 *, 0xA8) = 0;
    func_80018FB8(ovl3_D_800E0F64, ovl3_func_800E0AFC, 0);
    M2C_FIELD(ovl3_D_800E0F64, s32 *, 0xA8) = 0;
loop_7:
    func_8000AEAC();
    if (D_800C30C4 != 0) {
        D_800C1674 = 1;
    } else {
        var_s2 -= 1;
        if (var_s2 == 0) {
            var_s0_3 = 0;
            if (*(&D_800C178C + 2 + ((var_s3 + 2) & 7)) != 0) {
                var_v1 = -2;
loop_12:
                var_s0_3 += 1;
                if (*(&D_800C178C + 4 + var_v1) == 0) {
                    var_s3 = var_v1 & 7;
                } else {
                    var_v1 += 1;
                    if (var_s0_3 >= 8) {

                    } else {
                        goto loop_12;
                    }
                }
            }
            goto block_54;
        }
        if (M2C_FIELD(&D_800B04D4, s32 *, 0) != 0) {
            if (M2C_FIELD(&D_800B04D4, s32 *, 4) != 0) {
                var_s1_3 = M2C_FIELD(&D_800C0D48, u16 *, 0) | D_800C0D4A;
            } else {
                var_s1_3 = M2C_FIELD(&D_800C0D48, u16 *, 0);
            }
        } else {
            var_s1_3 = D_800C0D4A;
        }
        temp_s0 = var_s2 / 60;
        temp_v0 = (var_s2 / 60) / 10;
        if (temp_v0 != 0) {
            func_80018E34(ovl3_D_800E0F68, ovl3_D_800E0FD0[temp_v0]);
            var_hi = MULT_HI(temp_s0, 0x66666667);
        } else {
            func_80018FA0(ovl3_D_800E0F68);
            var_hi = MULT_HI(temp_s0, 0x66666667);
        }
        func_80018E34(ovl3_D_800E0F6C, ovl3_D_800E0FD0[temp_s0 - (((var_hi >> 2) - (temp_s0 >> 0x1F)) * 0xA)]);
        if (var_s1_3 & 0x10) {
            var_v0 = var_s1_3 & 8;
            if (*(&D_800C178C + 2 + ((var_s3 + 2) & 7)) == 0) {
                func_8001BA90(1);
block_54:
                var_s0_4 = 0;
                func_80018FA0(ovl3_D_800E0F68);
                func_80018FA0(ovl3_D_800E0F6C);
                var_s3 = (var_s3 + 2) & 7;
                func_80018FC8(ovl3_D_800E0F60);
                func_80018FC8(ovl3_D_800E0F64);
                func_80018FA0(ovl3_D_800E0F60);
                func_80018FA0(ovl3_D_800E0F64);
                var_s1_4 = &ovl3_D_800E0F40;
                do {
                    if (var_s3 == var_s0_4) {
                        func_80018FB8((&ovl3_D_800E0F40)[var_s3], ovl3_func_800E09C4, 0);
                        var_s0_4 += 1;
                    } else {
                        func_80018FB8(*var_s1_4, ovl3_func_800E0A5C, 0);
                        M2C_FIELD(*var_s1_4, s32 *, 0xA8) = 0xFF;
                        var_s0_4 += 1;
                        func_8001860C(*var_s1_4);
                    }
                    var_s1_4 += 4;
                } while (var_s0_4 < 8);
                var_s2_2 = 0x3C;
                do {
                    var_s2_2 -= 1;
                    func_8000AEAC();
                } while (var_s2_2 != 0);
                func_80018E34(M2C_FIELD(&ovl3_D_800E0F38, void **, 0), ovl3_D_800E0F88[var_s3]);
                func_80018E34(ovl3_D_800E0F3C, ovl3_D_800E0FA8[var_s3]);
                func_8001860C(M2C_FIELD(&ovl3_D_800E0F38, void **, 0));
                func_80018EB8(M2C_FIELD(&ovl3_D_800E0F38, void **, 0), 0, 0, 0, 0);
                func_80018EB8(ovl3_D_800E0F3C, 0, 0, 0, 0);
                func_80018F88(M2C_FIELD(&ovl3_D_800E0F38, void **, 0));
                func_80018F88(ovl3_D_800E0F3C);
                do {
                    var_s2_2 += 0x10;
                    func_8000AEAC();
                    if (var_s2_2 >= 0x100) {
                        var_s2_2 = 0xFF;
                    }
                    func_80018EB8(M2C_FIELD(&ovl3_D_800E0F38, void **, 0), 0, 0, 0, var_s2_2);
                    func_80018EB8(M2C_FIELD(&ovl3_D_800E0F38, void **, 4), 0, 0, 0, var_s2_2);
                } while (var_s2_2 < 0xFF);
                var_s2_3 = 0;
loop_66:
                var_s2_3 += 1;
                func_8000AEAC();
                if (var_s2_3 < 0x79) {
                    if (M2C_FIELD(&D_800B04D4, s32 *, 0) != 0) {
                        if (M2C_FIELD(&D_800B04D4, s32 *, 4) != 0) {
                            var_s1_5 = M2C_FIELD(&D_800C0D48, u16 *, 0) | M2C_FIELD(&D_800C0D48, u16 *, 2);
                            goto block_72;
                        }
                        var_v0_2 = M2C_FIELD(&D_800C0D48, u16 *, 0) & 0x10;
                    } else {
                        var_s1_5 = D_800C0D4A;
block_72:
                        var_v0_2 = var_s1_5 & 0x10;
                    }
                    if (var_v0_2 == 0) {
                        goto loop_66;
                    }
                }
            } else {
                goto block_29;
            }
        } else {
            var_v0 = var_s1_3 & 8;
block_29:
            if (var_v0 != 0) {
                var_s4 = (var_s4 + 0x40) & 0x1FF;
                var_v0_3 = var_s3 - 1;
                goto block_33;
            }
            if (var_s1_3 & 4) {
                var_s4 = (var_s4 - 0x40) & 0x1FF;
                var_v0_3 = var_s3 + 1;
block_33:
                var_s3 = var_v0_3 & 7;
                func_8001BA90(2);
            }
            var_v1_2 = (var_s4 - var_s5) & 0x1FF;
            if (var_v1_2 >= 0x100) {
                var_v1_2 -= 0x200;
            }
            if (var_v1_2 == 0) {
                var_v0_4 = 0;
                goto block_50;
            }
            var_v0_5 = var_v1_2;
            if (var_v1_2 < 0) {
                var_v0_5 = var_v1_2 + 3;
            }
            temp_v0_2 = var_v0_5 >> 2;
            if (var_v1_2 >= 0) {
                if (temp_v0_2 == 0) {
                    var_v0_4 = 1;
                    goto block_50;
                }
                var_v0_6 = var_s5 + temp_v0_2;
            } else if (temp_v0_2 == 0) {
                var_v0_4 = -1;
block_50:
                var_v0_6 = var_s5 + var_v0_4;
            } else {
                var_v0_6 = var_s5 + temp_v0_2;
            }
            var_s5 = var_v0_6 & 0x1FF;
            var_s1_6 = 0;
            var_s0_5 = &ovl3_D_800E0F40;
            var_f22_2 = ((f32) var_s5 / 256.0f) * 3.1415927f;
            do {
                temp_f20_2 = (func_8001C450(var_f22_2) * 120.0f) + 160.0f;
                var_s1_6 += 1;
                temp_a0_2 = *var_s0_5;
                var_s0_5 += 4;
                temp_a2_2 = (s32) ((func_8001C5A0(var_f22_2) * 50.0f) + 120.0f);
                var_f22_2 += 0.7853982f;
                func_80018EF4(temp_a0_2, (s32) temp_f20_2, temp_a2_2, 0);
            } while (var_s1_6 < 8);
            goto loop_7;
        }
    }
    func_80019C9C(&ovl3_D_800E0B40, &ovl3_D_800E0DA4);
    func_8000AEAC();
    func_8000AEAC();
    func_8000AEAC();
    *sp1C = var_s3 | 0x10000;
}

void ovl3_func_800E09C4(void *arg0) {
    f32 temp_f12;
    f32 temp_f12_2;
    f32 temp_f14;
    f32 temp_f14_2;
    f32 temp_f16;
    f32 temp_f6;

    temp_f16 = M2C_FIELD(arg0, f32 *, 0x78);
    temp_f14 = M2C_FIELD(arg0, f32 *, 0x7C);
    temp_f12 = M2C_FIELD(arg0, f32 *, 0x9C);
    temp_f6 = M2C_FIELD(arg0, f32 *, 0xA0);
    temp_f14_2 = temp_f14 + ((120.0f - temp_f14) / 8.0f);
    temp_f12_2 = temp_f12 + ((2.0f - temp_f12) / 8.0f);
    M2C_FIELD(arg0, f32 *, 0x78) = (f32) (temp_f16 + ((100.0f - temp_f16) / 8.0f));
    M2C_FIELD(arg0, f32 *, 0x7C) = temp_f14_2;
    M2C_FIELD(arg0, f32 *, 0x9C) = temp_f12_2;
    M2C_FIELD(arg0, f32 *, 0xA0) = (f32) (temp_f6 + ((2.0f - temp_f6) / 8.0f));
    func_80018F74(temp_f12_2, temp_f14_2);
}

void ovl3_func_800E0A5C(void *arg0) {
    s32 temp_v0;
    s32 temp_v0_2;

    func_80018EB8(NULL, 0, 0, M2C_FIELD(arg0, s32 *, 0xA8));
    temp_v0 = M2C_FIELD(arg0, s32 *, 0xA8);
    temp_v0_2 = temp_v0 - 4;
    if (temp_v0 != 0) {
        M2C_FIELD(arg0, s32 *, 0xA8) = temp_v0_2;
        if (temp_v0_2 < 0) {
            M2C_FIELD(arg0, s32 *, 0xA8) = 0;
        }
    } else {
        func_80018FC8(arg0);
    }
}

void ovl3_func_800E0AB8(void *arg0) {
    func_80018E34(*(&ovl3_D_800E0F70 + (M2C_FIELD(arg0, s32 *, 0xA8) & 4)));
    M2C_FIELD(arg0, s32 *, 0xA8) = (s32) (M2C_FIELD(arg0, s32 *, 0xA8) + 1);
}

void ovl3_func_800E0AFC(void *arg0) {
    func_80018E34(*(&ovl3_D_800E0F78 + (M2C_FIELD(arg0, s32 *, 0xA8) & 0xC)));
    M2C_FIELD(arg0, s32 *, 0xA8) = (s32) (M2C_FIELD(arg0, s32 *, 0xA8) + 1);
}
