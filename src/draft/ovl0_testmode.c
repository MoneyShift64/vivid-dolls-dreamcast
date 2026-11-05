M2C_UNK func_8000AEAC();                            /* extern */
M2C_UNK func_8000D160();                            /* extern */
M2C_UNK func_8000D1CC(M2C_UNK);                     /* extern */
M2C_UNK *func_800116B8(M2C_UNK);                    /* extern */
M2C_UNK func_800117BC(M2C_UNK *);                   /* extern */
M2C_UNK func_80015524(u8 *);                        /* extern */
M2C_UNK func_8001556C(u8 *);                        /* extern */
s32 func_80015AA0(M2C_UNK, s8 *, M2C_UNK, M2C_UNK, s32, s32); /* extern */
M2C_UNK func_80015BE4(s32);                         /* extern */
M2C_UNK func_80015E14(M2C_UNK *, s32, M2C_UNK, M2C_UNK, f32, f32); /* extern */
M2C_UNK func_80015EA4(s32, M2C_UNK, M2C_UNK, M2C_UNK, s32); /* extern */
M2C_UNK func_80016094(s32, M2C_UNK, M2C_UNK, M2C_UNK, s32); /* extern */
M2C_UNK func_800161E8(s32, s8 *);                   /* extern */
s32 func_800184E4(s32, void *);                     /* extern */
M2C_UNK func_80018E20(s32);                         /* extern */
M2C_UNK func_80018F88(s32);                         /* extern */
M2C_UNK func_80025DC0(s8 *, M2C_UNK *, s16);        /* extern */
extern M2C_UNK D_611F0;
extern u16 D_800AF220;
extern u16 D_800B0498;
extern u16 D_800B049A;
extern void *D_800B04A0;
extern M2C_UNK D_800B04AC;
extern s16 D_800B0574;
extern s16 D_800B0576;
extern u16 D_800B058C;
extern u16 D_800B058E;
extern u16 D_800B060C;
extern s32 D_800B061C;
extern u16 D_800C0AB0;
extern u16 D_800C0D48;
extern u16 D_800C0D4C;
extern u16 D_800C0D4E;
extern s16 D_800C178C;
extern u16 D_800C30C4;
extern M2C_UNK D_800C5298;
extern u16 D_800C52A0;

/*
Decompilation failure in function ovl0_func_800E0000:

Unable to determine jump table for jr instruction at 3ED30.s line 65.

There must be a read of a variable before the instruction
which has a name starting with with "jtbl"/"jpt_"/"lbl_"/"jumptable_".
*/

void ovl0_func_800E029C(s32 *arg0) {
    M2C_UNK *temp_v0_2;
    s32 temp_s1;
    s32 temp_v0;

    temp_v0 = func_800184E4(D_800B061C, (0 & 0xFFFFFF) + &D_611F0);
    func_80018F88(temp_v0);
    temp_s1 = func_80015AA0(0, "PUSH TEST SW TO EXIT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_2 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    func_80015E14(temp_v0_2, temp_s1, 0xC2A00000, 0xC2C00000, 1.0f, 1.0f);
    do {
        func_8000AEAC();
    } while (D_800AF220 == 0);
    func_8000AEAC();
    func_80018E20(temp_v0);
    func_800117BC(temp_v0_2);
    func_8000AEAC();
    func_8000AEAC();
    func_80015BE4(temp_s1);
    func_8000AEAC();
    *arg0 = 0x10000;
}

void ovl0_func_800E03F0(s32 *arg0) {
    s32 sp24;
    s32 sp28;
    s32 sp2C;
    s32 sp30;
    s32 sp34;
    s32 sp38;
    s32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 sp4C;
    s32 sp50;
    s32 sp54;
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 sp64;
    s32 sp68;
    s32 sp6C;
    s32 sp8C;
    s32 sp90;
    s32 sp94;
    s32 sp98;
    s32 sp9C;
    s32 spA0;
    s32 spA4;
    s32 spA8;
    s32 spB4;
    s32 spBC;
    s32 spC0;
    s32 spC4;
    s32 spC8;
    s32 spCC;
    s32 spD0;
    s32 spD4;
    s32 spD8;
    s32 spE4;
    s32 spE8;
    s32 spEC;
    s32 spF0;
    s32 spF4;
    s32 spF8;
    s32 spFC;
    s32 sp100;
    s32 sp104;
    s8 sp218;
    s8 sp228;
    s32 *sp23C;
    M2C_UNK *sp244;
    M2C_UNK *sp24C;
    M2C_UNK *sp254;
    M2C_UNK *sp25C;
    M2C_UNK *sp264;
    M2C_UNK *sp26C;
    M2C_UNK *sp274;
    M2C_UNK *sp27C;
    M2C_UNK *sp284;
    M2C_UNK *sp28C;
    M2C_UNK *sp294;
    M2C_UNK *sp29C;
    M2C_UNK *sp2A4;
    M2C_UNK *sp2AC;
    M2C_UNK *sp2B4;
    M2C_UNK *sp2BC;
    M2C_UNK *sp2C4;
    M2C_UNK *sp2CC;
    M2C_UNK *sp2D4;
    M2C_UNK *sp2DC;
    M2C_UNK *sp2E4;
    M2C_UNK *sp2EC;
    M2C_UNK *sp2F4;
    M2C_UNK *sp2FC;
    M2C_UNK *sp304;
    M2C_UNK *sp30C;
    M2C_UNK *sp314;
    M2C_UNK *sp31C;
    M2C_UNK *sp324;
    M2C_UNK *sp32C;
    M2C_UNK *sp334;
    M2C_UNK *sp33C;
    M2C_UNK *sp344;
    M2C_UNK *sp34C;
    M2C_UNK *sp354;
    M2C_UNK *sp35C;
    M2C_UNK *sp364;
    M2C_UNK *sp36C;
    M2C_UNK *sp374;
    M2C_UNK *sp37C;
    M2C_UNK *sp384;
    M2C_UNK *sp38C;
    M2C_UNK *sp394;
    M2C_UNK *sp39C;
    M2C_UNK *sp3A4;
    M2C_UNK *sp3AC;
    M2C_UNK *sp3B4;
    M2C_UNK *sp3BC;
    M2C_UNK *sp3C4;
    M2C_UNK *sp3CC;
    M2C_UNK *sp3D4;
    M2C_UNK *sp3DC;
    M2C_UNK *sp3E4;
    M2C_UNK *sp3EC;
    s32 sp3F4;
    s32 sp3FC;
    s32 sp404;
    s32 sp40C;
    s32 sp414;
    s32 sp41C;
    s32 sp424;
    s32 sp42C;
    s32 sp434;
    s32 sp43C;
    s32 sp444;
    s32 sp44C;
    s32 sp454;
    s32 sp45C;
    s32 sp464;
    s32 sp46C;
    s32 sp474;
    s32 sp47C;
    s32 sp484;
    s32 sp48C;
    s32 sp494;
    s32 sp49C;
    s32 sp4A4;
    s32 sp4AC;
    s32 sp4B4;
    s32 sp4BC;
    s32 sp4C4;
    s32 sp4CC;
    s32 sp4D4;
    s32 sp4DC;
    s32 sp4E4;
    s32 sp4EC;
    s32 sp4F4;
    s32 sp4FC;
    s32 sp504;
    s32 sp50C;
    s32 sp514;
    s32 sp51C;
    s32 sp524;
    s32 sp52C;
    s32 sp534;
    s32 sp53C;
    s32 sp544;
    s32 sp54C;
    s32 sp554;
    s32 sp55C;
    s32 sp564;
    s32 sp56C;
    s32 sp574;
    s32 sp57C;
    s32 sp584;
    s32 sp58C;
    s8 *sp594;
    s8 *sp59C;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_10;
    M2C_UNK *temp_v0_11;
    M2C_UNK *temp_v0_12;
    M2C_UNK *temp_v0_13;
    M2C_UNK *temp_v0_14;
    M2C_UNK *temp_v0_15;
    M2C_UNK *temp_v0_16;
    M2C_UNK *temp_v0_17;
    M2C_UNK *temp_v0_18;
    M2C_UNK *temp_v0_19;
    M2C_UNK *temp_v0_20;
    M2C_UNK *temp_v0_21;
    M2C_UNK *temp_v0_22;
    M2C_UNK *temp_v0_23;
    M2C_UNK *temp_v0_24;
    M2C_UNK *temp_v0_25;
    M2C_UNK *temp_v0_26;
    M2C_UNK *temp_v0_27;
    M2C_UNK *temp_v0_28;
    M2C_UNK *temp_v0_29;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_30;
    M2C_UNK *temp_v0_31;
    M2C_UNK *temp_v0_32;
    M2C_UNK *temp_v0_33;
    M2C_UNK *temp_v0_34;
    M2C_UNK *temp_v0_35;
    M2C_UNK *temp_v0_36;
    M2C_UNK *temp_v0_37;
    M2C_UNK *temp_v0_38;
    M2C_UNK *temp_v0_39;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_40;
    M2C_UNK *temp_v0_41;
    M2C_UNK *temp_v0_42;
    M2C_UNK *temp_v0_43;
    M2C_UNK *temp_v0_44;
    M2C_UNK *temp_v0_45;
    M2C_UNK *temp_v0_46;
    M2C_UNK *temp_v0_47;
    M2C_UNK *temp_v0_48;
    M2C_UNK *temp_v0_49;
    M2C_UNK *temp_v0_4;
    M2C_UNK *temp_v0_50;
    M2C_UNK *temp_v0_51;
    M2C_UNK *temp_v0_52;
    M2C_UNK *temp_v0_53;
    M2C_UNK *temp_v0_54;
    M2C_UNK *temp_v0_55;
    M2C_UNK *temp_v0_56;
    M2C_UNK *temp_v0_5;
    M2C_UNK *temp_v0_6;
    M2C_UNK *temp_v0_7;
    M2C_UNK *temp_v0_8;
    M2C_UNK *temp_v0_9;
    M2C_UNK var_a1;
    M2C_UNK var_a1_10;
    M2C_UNK var_a1_11;
    M2C_UNK var_a1_12;
    M2C_UNK var_a1_13;
    M2C_UNK var_a1_14;
    M2C_UNK var_a1_15;
    M2C_UNK var_a1_16;
    M2C_UNK var_a1_17;
    M2C_UNK var_a1_18;
    M2C_UNK var_a1_19;
    M2C_UNK var_a1_20;
    M2C_UNK var_a1_21;
    M2C_UNK var_a1_22;
    M2C_UNK var_a1_23;
    M2C_UNK var_a1_24;
    M2C_UNK var_a1_25;
    M2C_UNK var_a1_26;
    M2C_UNK var_a1_27;
    M2C_UNK var_a1_28;
    M2C_UNK var_a1_29;
    M2C_UNK var_a1_2;
    M2C_UNK var_a1_30;
    M2C_UNK var_a1_31;
    M2C_UNK var_a1_32;
    M2C_UNK var_a1_33;
    M2C_UNK var_a1_34;
    M2C_UNK var_a1_35;
    M2C_UNK var_a1_36;
    M2C_UNK var_a1_37;
    M2C_UNK var_a1_38;
    M2C_UNK var_a1_39;
    M2C_UNK var_a1_3;
    M2C_UNK var_a1_40;
    M2C_UNK var_a1_4;
    M2C_UNK var_a1_5;
    M2C_UNK var_a1_6;
    M2C_UNK var_a1_7;
    M2C_UNK var_a1_8;
    M2C_UNK var_a1_9;
    M2C_UNK var_a2;
    M2C_UNK var_a2_10;
    M2C_UNK var_a2_11;
    M2C_UNK var_a2_12;
    M2C_UNK var_a2_13;
    M2C_UNK var_a2_14;
    M2C_UNK var_a2_15;
    M2C_UNK var_a2_16;
    M2C_UNK var_a2_17;
    M2C_UNK var_a2_18;
    M2C_UNK var_a2_19;
    M2C_UNK var_a2_20;
    M2C_UNK var_a2_21;
    M2C_UNK var_a2_22;
    M2C_UNK var_a2_23;
    M2C_UNK var_a2_24;
    M2C_UNK var_a2_25;
    M2C_UNK var_a2_26;
    M2C_UNK var_a2_27;
    M2C_UNK var_a2_28;
    M2C_UNK var_a2_29;
    M2C_UNK var_a2_2;
    M2C_UNK var_a2_30;
    M2C_UNK var_a2_31;
    M2C_UNK var_a2_32;
    M2C_UNK var_a2_33;
    M2C_UNK var_a2_34;
    M2C_UNK var_a2_35;
    M2C_UNK var_a2_36;
    M2C_UNK var_a2_37;
    M2C_UNK var_a2_38;
    M2C_UNK var_a2_39;
    M2C_UNK var_a2_3;
    M2C_UNK var_a2_40;
    M2C_UNK var_a2_4;
    M2C_UNK var_a2_5;
    M2C_UNK var_a2_6;
    M2C_UNK var_a2_7;
    M2C_UNK var_a2_8;
    M2C_UNK var_a2_9;
    s32 *var_v0;
    s32 temp_s3;
    s32 temp_s4;
    s32 temp_s5;
    s32 temp_s6;
    s32 var_t0;
    s32 var_t0_2;
    s32 var_t0_3;
    s32 var_t0_4;
    s32 var_t0_5;
    s8 *temp_fp;
    s8 *temp_s2;
    s8 *var_a0;
    s8 *var_a0_2;
    s8 *var_v1;
    s8 *var_v1_2;
    s8 var_t1;
    s8 var_t1_2;

    var_t0 = 0x3A;
    var_v0 = &sp100;
    sp23C = arg0;
    do {
        *var_v0 = 0;
        var_t0 -= 1;
        var_v0 -= 4;
    } while (var_t0 >= 0);
    var_t0_2 = 0;
    var_a0 = &sp218;
    do {
        if (!(((s32) D_800C52A0 >> (var_t0_2 + 8)) & 1)) {
            *var_a0 = 0x4C;
        } else {
            *var_a0 = 0x48;
        }
        var_t0_2 += 1;
        var_a0 += 1;
    } while (var_t0_2 < 8);
    (&sp218)[var_t0_2] = 0;
    var_t0_3 = 0;
    var_v1 = &sp228;
    do {
        if (!(((s32) D_800C52A0 >> var_t0_3) & 1)) {
            *var_v1 = 0x4C;
        } else {
            *var_v1 = 0x48;
        }
        var_t0_3 += 1;
        var_v1 += 1;
    } while (var_t0_3 < 8);
    sp594 = &sp228;
    (&sp228)[var_t0_3] = 0;
    sp3F4 = func_80015AA0(0, "INPUT TEST", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0 = func_800116B8(2);
    sp244 = temp_v0;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(sp244, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp244, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp244;
    func_80015E14(sp244, sp3F4, 0xC2200000, 0x42DC0000, 1.0f, 1.0f);
    sp3FC = func_80015AA0(0, "PUSH TEST SW TO EXIT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_2 = func_800116B8(2);
    sp24C = temp_v0_2;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(sp24C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp24C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp24C;
    func_80015E14(sp24C, sp3FC, 0xC2A00000, 0xC2C00000, 1.0f, 1.0f);
    sp404 = func_80015AA0(0, "1P", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_3 = func_800116B8(2);
    sp254 = temp_v0_3;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    M2C_FIELD(sp254, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp254, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp254;
    func_80015E14(sp254, sp404, 0xC3040000, 0x42B40000, 1.0f, 1.0f);
    sp40C = func_80015AA0(0, "U", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_4 = func_800116B8(2);
    sp25C = temp_v0_4;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    M2C_FIELD(sp25C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp25C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp25C;
    func_80015E14(sp25C, sp40C, 0xC2C80000, 0x42B40000, 1.0f, 1.0f);
    sp414 = func_80015AA0(0, "D", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_5 = func_800116B8(2);
    sp264 = temp_v0_5;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    M2C_FIELD(sp264, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp264, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp264;
    func_80015E14(sp264, sp414, 0xC2A80000, 0x42B40000, 1.0f, 1.0f);
    sp41C = func_80015AA0(0, "L", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_6 = func_800116B8(2);
    sp26C = temp_v0_6;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_6;
    M2C_FIELD(sp26C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp26C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp26C;
    func_80015E14(sp26C, sp41C, 0xC2880000, 0x42B40000, 1.0f, 1.0f);
    sp424 = func_80015AA0(0, "R", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_7 = func_800116B8(2);
    sp274 = temp_v0_7;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_7;
    M2C_FIELD(sp274, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp274, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp274;
    func_80015E14(sp274, sp424, 0xC2500000, 0x42B40000, 1.0f, 1.0f);
    temp_fp = "P1";
    sp42C = func_80015AA0(0, temp_fp, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_8 = func_800116B8(2);
    sp27C = temp_v0_8;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_8;
    M2C_FIELD(sp27C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp27C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp27C;
    func_80015E14(sp27C, sp42C, 0xC2C80000, 0x42A00000, 1.0f, 1.0f);
    sp434 = func_80015AA0(0, "P2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_9 = func_800116B8(2);
    sp284 = temp_v0_9;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_9;
    M2C_FIELD(sp284, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp284, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp284;
    func_80015E14(sp284, sp434, 0xC2A80000, 0x42A00000, 1.0f, 1.0f);
    sp43C = func_80015AA0(0, "P3", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_10 = func_800116B8(2);
    sp28C = temp_v0_10;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_10;
    M2C_FIELD(sp28C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp28C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp28C;
    func_80015E14(sp28C, sp43C, 0xC2880000, 0x42A00000, 1.0f, 1.0f);
    sp444 = func_80015AA0(0, "P4", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_11 = func_800116B8(2);
    sp294 = temp_v0_11;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_11;
    M2C_FIELD(sp294, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp294, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp294;
    func_80015E14(sp294, sp444, 0xC2500000, 0x42A00000, 1.0f, 1.0f);
    sp44C = func_80015AA0(0, "START", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_12 = func_800116B8(2);
    sp29C = temp_v0_12;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_12;
    M2C_FIELD(sp29C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp29C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp29C;
    func_80015E14(sp29C, sp44C, 0xC2C80000, 0x428C0000, 1.0f, 1.0f);
    sp454 = func_80015AA0(0, "2P", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_13 = func_800116B8(2);
    sp2A4 = temp_v0_13;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_13;
    M2C_FIELD(sp2A4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2A4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2A4;
    func_80015E14(sp2A4, sp454, 0xC3040000, 0x42480000, 1.0f, 1.0f);
    sp45C = func_80015AA0(0, "U", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_14 = func_800116B8(2);
    sp2AC = temp_v0_14;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_14;
    M2C_FIELD(sp2AC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2AC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2AC;
    func_80015E14(sp2AC, sp45C, 0xC2C80000, 0x42480000, 1.0f, 1.0f);
    sp464 = func_80015AA0(0, "D", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_15 = func_800116B8(2);
    sp2B4 = temp_v0_15;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_15;
    M2C_FIELD(sp2B4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2B4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2B4;
    func_80015E14(sp2B4, sp464, 0xC2A80000, 0x42480000, 1.0f, 1.0f);
    sp46C = func_80015AA0(0, "L", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_16 = func_800116B8(2);
    sp2BC = temp_v0_16;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_16;
    M2C_FIELD(sp2BC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2BC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2BC;
    func_80015E14(sp2BC, sp46C, 0xC2880000, 0x42480000, 1.0f, 1.0f);
    sp474 = func_80015AA0(0, "R", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_17 = func_800116B8(2);
    sp2C4 = temp_v0_17;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_17;
    M2C_FIELD(sp2C4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2C4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2C4;
    func_80015E14(sp2C4, sp474, 0xC2500000, 0x42480000, 1.0f, 1.0f);
    sp47C = func_80015AA0(0, temp_fp, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_18 = func_800116B8(2);
    sp2CC = temp_v0_18;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_18;
    M2C_FIELD(sp2CC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2CC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2CC;
    func_80015E14(sp2CC, sp47C, 0xC2C80000, 0x42200000, 1.0f, 1.0f);
    sp484 = func_80015AA0(0, "P2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_19 = func_800116B8(2);
    sp2D4 = temp_v0_19;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_19;
    M2C_FIELD(sp2D4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2D4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2D4;
    func_80015E14(sp2D4, sp484, 0xC2A80000, 0x42200000, 1.0f, 1.0f);
    sp48C = func_80015AA0(0, "P3", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_20 = func_800116B8(2);
    sp2DC = temp_v0_20;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_20;
    M2C_FIELD(sp2DC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2DC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2DC;
    func_80015E14(sp2DC, sp48C, 0xC2880000, 0x42200000, 1.0f, 1.0f);
    sp494 = func_80015AA0(0, "P4", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_21 = func_800116B8(2);
    sp2E4 = temp_v0_21;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_21;
    M2C_FIELD(sp2E4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2E4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2E4;
    func_80015E14(sp2E4, sp494, 0xC2500000, 0x42200000, 1.0f, 1.0f);
    sp49C = func_80015AA0(0, "START", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_22 = func_800116B8(2);
    sp2EC = temp_v0_22;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_22;
    M2C_FIELD(sp2EC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2EC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2EC;
    func_80015E14(sp2EC, sp49C, 0xC2C80000, 0x41F00000, 1.0f, 1.0f);
    sp4A4 = func_80015AA0(0, "DIPSW1", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_23 = func_800116B8(2);
    sp2F4 = temp_v0_23;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_23;
    M2C_FIELD(sp2F4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2F4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2F4;
    func_80015E14(sp2F4, sp4A4, 0x42000000, 0x42B40000, 1.0f, 1.0f);
    temp_s2 = "12345678";
    sp4AC = func_80015AA0(0, temp_s2, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_24 = func_800116B8(2);
    sp2FC = temp_v0_24;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_24;
    M2C_FIELD(sp2FC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2FC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp2FC;
    func_80015E14(sp2FC, sp4AC, 0x42000000, 0x42A00000, 1.0f, 1.0f);
    sp59C = &sp218;
    sp4B4 = func_80015AA0(0, sp59C, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_25 = func_800116B8(2);
    sp304 = temp_v0_25;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_25;
    M2C_FIELD(sp304, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp304, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp304;
    func_80015E14(sp304, sp4B4, 0x42000000, 0x428C0000, 1.0f, 1.0f);
    sp4BC = func_80015AA0(0, "DIPSW2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_26 = func_800116B8(2);
    sp30C = temp_v0_26;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_26;
    M2C_FIELD(sp30C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp30C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp30C;
    func_80015E14(sp30C, sp4BC, 0x42000000, 0x42480000, 1.0f, 1.0f);
    sp4C4 = func_80015AA0(0, temp_s2, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_27 = func_800116B8(2);
    sp314 = temp_v0_27;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_27;
    M2C_FIELD(sp314, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp314, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp314;
    func_80015E14(sp314, sp4C4, 0x42000000, 0x42200000, 1.0f, 1.0f);
    sp4CC = func_80015AA0(0, sp594, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_28 = func_800116B8(2);
    sp31C = temp_v0_28;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_28;
    M2C_FIELD(sp31C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp31C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp31C;
    func_80015E14(sp31C, sp4CC, 0x42000000, 0x41F00000, 1.0f, 1.0f);
    sp4D4 = func_80015AA0(0, "BR1", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_29 = func_800116B8(2);
    sp324 = temp_v0_29;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_29;
    M2C_FIELD(sp324, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp324, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp324;
    func_80015E14(sp324, sp4D4, 0xC3040000, 0xC1A00000, 1.0f, 1.0f);
    sp4DC = func_80015AA0(0, "U", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_30 = func_800116B8(2);
    sp32C = temp_v0_30;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_30;
    M2C_FIELD(sp32C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp32C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp32C;
    func_80015E14(sp32C, sp4DC, 0xC2C80000, 0xC1A00000, 1.0f, 1.0f);
    sp4E4 = func_80015AA0(0, "D", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_31 = func_800116B8(2);
    sp334 = temp_v0_31;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_31;
    M2C_FIELD(sp334, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp334, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp334;
    func_80015E14(sp334, sp4E4, 0xC2A80000, 0xC1A00000, 1.0f, 1.0f);
    sp4EC = func_80015AA0(0, "L", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_32 = func_800116B8(2);
    sp33C = temp_v0_32;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_32;
    M2C_FIELD(sp33C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp33C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp33C;
    func_80015E14(sp33C, sp4EC, 0xC2880000, 0xC1A00000, 1.0f, 1.0f);
    sp4F4 = func_80015AA0(0, "R", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_33 = func_800116B8(2);
    sp344 = temp_v0_33;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_33;
    M2C_FIELD(sp344, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp344, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp344;
    func_80015E14(sp344, sp4F4, 0xC2500000, 0xC1A00000, 1.0f, 1.0f);
    sp4FC = func_80015AA0(0, temp_fp, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_34 = func_800116B8(2);
    sp34C = temp_v0_34;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_34;
    M2C_FIELD(sp34C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp34C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp34C;
    func_80015E14(sp34C, sp4FC, 0xC2C80000, 0xC1F00000, 1.0f, 1.0f);
    sp504 = func_80015AA0(0, "P2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_35 = func_800116B8(2);
    sp354 = temp_v0_35;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_35;
    M2C_FIELD(sp354, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp354, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp354;
    func_80015E14(sp354, sp504, 0xC2A80000, 0xC1F00000, 1.0f, 1.0f);
    sp50C = func_80015AA0(0, "P3", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_36 = func_800116B8(2);
    sp35C = temp_v0_36;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_36;
    M2C_FIELD(sp35C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp35C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp35C;
    func_80015E14(sp35C, sp50C, 0xC2880000, 0xC1F00000, 1.0f, 1.0f);
    sp514 = func_80015AA0(0, "P4", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_37 = func_800116B8(2);
    sp364 = temp_v0_37;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_37;
    M2C_FIELD(sp364, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp364, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp364;
    func_80015E14(sp364, sp514, 0xC2500000, 0xC1F00000, 1.0f, 1.0f);
    sp51C = func_80015AA0(0, "START", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_38 = func_800116B8(2);
    sp36C = temp_v0_38;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_38;
    M2C_FIELD(sp36C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp36C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp36C;
    func_80015E14(sp36C, sp51C, 0xC2C80000, 0xC2200000, 1.0f, 1.0f);
    sp524 = func_80015AA0(0, "BR2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_39 = func_800116B8(2);
    sp374 = temp_v0_39;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_39;
    M2C_FIELD(sp374, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp374, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp374;
    func_80015E14(sp374, sp524, 0x41900000, 0xC1A00000, 1.0f, 1.0f);
    sp52C = func_80015AA0(0, "U", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_40 = func_800116B8(2);
    sp37C = temp_v0_40;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_40;
    M2C_FIELD(sp37C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp37C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp37C;
    func_80015E14(sp37C, sp52C, 0x42480000, 0xC1A00000, 1.0f, 1.0f);
    sp534 = func_80015AA0(0, "D", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_41 = func_800116B8(2);
    sp384 = temp_v0_41;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_41;
    M2C_FIELD(sp384, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp384, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp384;
    func_80015E14(sp384, sp534, 0x42840000, 0xC1A00000, 1.0f, 1.0f);
    sp53C = func_80015AA0(0, "L", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_42 = func_800116B8(2);
    sp38C = temp_v0_42;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_42;
    M2C_FIELD(sp38C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp38C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp38C;
    func_80015E14(sp38C, sp53C, 0x42A40000, 0xC1A00000, 1.0f, 1.0f);
    sp544 = func_80015AA0(0, "R", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_43 = func_800116B8(2);
    sp394 = temp_v0_43;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_43;
    M2C_FIELD(sp394, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp394, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp394;
    func_80015E14(sp394, sp544, 0x42C40000, 0xC1A00000, 1.0f, 1.0f);
    sp54C = func_80015AA0(0, temp_fp, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_44 = func_800116B8(2);
    sp39C = temp_v0_44;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_44;
    M2C_FIELD(sp39C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp39C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp39C;
    func_80015E14(sp39C, sp54C, 0x42480000, 0xC1F00000, 1.0f, 1.0f);
    sp554 = func_80015AA0(0, "P2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_45 = func_800116B8(2);
    sp3A4 = temp_v0_45;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_45;
    M2C_FIELD(sp3A4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3A4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3A4;
    func_80015E14(sp3A4, sp554, 0x42840000, 0xC1F00000, 1.0f, 1.0f);
    sp55C = func_80015AA0(0, "P3", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_46 = func_800116B8(2);
    sp3AC = temp_v0_46;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_46;
    M2C_FIELD(sp3AC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3AC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3AC;
    func_80015E14(sp3AC, sp55C, 0x42A40000, 0xC1F00000, 1.0f, 1.0f);
    sp564 = func_80015AA0(0, "P4", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_47 = func_800116B8(2);
    sp3B4 = temp_v0_47;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_47;
    M2C_FIELD(sp3B4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3B4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3B4;
    func_80015E14(sp3B4, sp564, 0x42C40000, 0xC1F00000, 1.0f, 1.0f);
    sp56C = func_80015AA0(0, "START", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_48 = func_800116B8(2);
    sp3BC = temp_v0_48;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_48;
    M2C_FIELD(sp3BC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3BC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3BC;
    func_80015E14(sp3BC, sp56C, 0x42480000, 0xC2200000, 1.0f, 1.0f);
    temp_s6 = func_80015AA0(0, "BR1 X:000", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_49 = func_800116B8(2);
    sp3C4 = temp_v0_49;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_49;
    M2C_FIELD(sp3C4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3C4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3C4;
    func_80015E14(sp3C4, temp_s6, 0xC2C80000, 0xC2700000, 1.0f, 1.0f);
    temp_s5 = func_80015AA0(0, "BR1 Y:000", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_50 = func_800116B8(2);
    sp3D4 = temp_v0_50;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_50;
    M2C_FIELD(sp3D4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3D4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3D4;
    func_80015E14(sp3D4, temp_s5, 0xC2C80000, 0xC28C0000, 1.0f, 1.0f);
    temp_s4 = func_80015AA0(0, "BR2 X:000", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_51 = func_800116B8(2);
    sp3CC = temp_v0_51;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_51;
    M2C_FIELD(sp3CC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3CC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3CC;
    func_80015E14(sp3CC, temp_s4, 0x42480000, 0xC2700000, 1.0f, 1.0f);
    temp_s3 = func_80015AA0(0, "BR2 Y:000", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_52 = func_800116B8(2);
    sp3DC = temp_v0_52;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_52;
    M2C_FIELD(sp3DC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3DC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3DC;
    func_80015E14(sp3DC, temp_s3, 0x42480000, 0xC28C0000, 1.0f, 1.0f);
    sp574 = func_80015AA0(0, "COIN1", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_53 = func_800116B8(2);
    sp3E4 = temp_v0_53;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_53;
    M2C_FIELD(sp3E4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3E4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3E4;
    func_80015E14(sp3E4, sp574, 0xC2C80000, 0x41200000, 1.0f, 1.0f);
    sp57C = func_80015AA0(0, "COIN2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_54 = func_800116B8(2);
    sp3EC = temp_v0_54;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_54;
    M2C_FIELD(sp3EC, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3EC, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = sp3EC;
    func_80015E14(sp3EC, sp57C, 0, 0x41200000, 1.0f, 1.0f);
    sp584 = func_80015AA0(0, "SERVICE SW", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_55 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_55;
    M2C_FIELD(temp_v0_55, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_55, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_55;
    func_80015E14(temp_v0_55, sp584, 0xC2C80000, 0, 1.0f, 1.0f);
    sp58C = func_80015AA0(0, "TEST SW", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_56 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_56;
    M2C_FIELD(temp_v0_56, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_56, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_56;
    func_80015E14(temp_v0_56, sp58C, 0, 0, 1.0f, 1.0f);
    do {
        func_8000AEAC();
        var_a1 = 0xFFC1;
        if (D_800B0498 & 1) {
            var_a2 = 0xFFC1;
            sp24 = 1;
            goto block_19;
        }
        var_a1 = 0xF801;
        if (sp24 != 0) {
            var_a2 = 0xF801;
block_19:
            func_80016094(sp40C, var_a1, var_a2, 1, 1);
        }
        var_a1_2 = 0xFFC1;
        if (D_800B0498 & 2) {
            var_a2_2 = 0xFFC1;
            sp28 = 1;
            goto block_24;
        }
        var_a1_2 = 0xF801;
        if (sp28 != 0) {
            var_a2_2 = 0xF801;
block_24:
            func_80016094(sp414, var_a1_2, var_a2_2, 1, 1);
        }
        var_a1_3 = 0xFFC1;
        if (D_800B0498 & 4) {
            var_a2_3 = 0xFFC1;
            sp2C = 1;
            goto block_29;
        }
        var_a1_3 = 0xF801;
        if (sp2C != 0) {
            var_a2_3 = 0xF801;
block_29:
            func_80016094(sp41C, var_a1_3, var_a2_3, 1, 1);
        }
        var_a1_4 = 0xFFC1;
        if (D_800B0498 & 8) {
            var_a2_4 = 0xFFC1;
            sp30 = 1;
            goto block_34;
        }
        var_a1_4 = 0xF801;
        if (sp30 != 0) {
            var_a2_4 = 0xF801;
block_34:
            func_80016094(sp424, var_a1_4, var_a2_4, 1, 1);
        }
        var_a1_5 = 0xFFC1;
        if (D_800B0498 & 0x10) {
            var_a2_5 = 0xFFC1;
            sp34 = 1;
            goto block_39;
        }
        var_a1_5 = 0xF801;
        if (sp34 != 0) {
            var_a2_5 = 0xF801;
block_39:
            func_80016094(sp42C, var_a1_5, var_a2_5, 1, 1);
        }
        var_a1_6 = 0xFFC1;
        if (D_800B0498 & 0x20) {
            var_a2_6 = 0xFFC1;
            sp38 = 1;
            goto block_44;
        }
        var_a1_6 = 0xF801;
        if (sp38 != 0) {
            var_a2_6 = 0xF801;
block_44:
            func_80016094(sp434, var_a1_6, var_a2_6, 1, 1);
        }
        var_a1_7 = 0xFFC1;
        if (D_800B0498 & 0x40) {
            var_a2_7 = 0xFFC1;
            sp3C = 1;
            goto block_49;
        }
        var_a1_7 = 0xF801;
        if (sp3C != 0) {
            var_a2_7 = 0xF801;
block_49:
            func_80016094(sp43C, var_a1_7, var_a2_7, 1, 1);
        }
        var_a1_8 = 0xFFC1;
        if (D_800B0498 & 0x80) {
            var_a2_8 = 0xFFC1;
            sp40 = 1;
            goto block_54;
        }
        var_a1_8 = 0xF801;
        if (sp40 != 0) {
            var_a2_8 = 0xF801;
block_54:
            func_80016094(sp444, var_a1_8, var_a2_8, 1, 1);
        }
        var_a1_9 = 0xFFC1;
        if (D_800B0498 & 0x100) {
            var_a2_9 = 0xFFC1;
            sp44 = 1;
            goto block_59;
        }
        var_a1_9 = 0xF801;
        if (sp44 != 0) {
            var_a2_9 = 0xF801;
block_59:
            func_80016094(sp44C, var_a1_9, var_a2_9, 1, 1);
        }
        var_a1_10 = 0xFFC1;
        if (D_800B049A & 1) {
            var_a2_10 = 0xFFC1;
            sp4C = 1;
            goto block_64;
        }
        var_a1_10 = 0xF801;
        if (sp4C != 0) {
            var_a2_10 = 0xF801;
block_64:
            func_80016094(sp45C, var_a1_10, var_a2_10, 1, 1);
        }
        var_a1_11 = 0xFFC1;
        if (D_800B049A & 2) {
            var_a2_11 = 0xFFC1;
            sp50 = 1;
            goto block_69;
        }
        var_a1_11 = 0xF801;
        if (sp50 != 0) {
            var_a2_11 = 0xF801;
block_69:
            func_80016094(sp464, var_a1_11, var_a2_11, 1, 1);
        }
        var_a1_12 = 0xFFC1;
        if (D_800B049A & 4) {
            var_a2_12 = 0xFFC1;
            sp54 = 1;
            goto block_74;
        }
        var_a1_12 = 0xF801;
        if (sp54 != 0) {
            var_a2_12 = 0xF801;
block_74:
            func_80016094(sp46C, var_a1_12, var_a2_12, 1, 1);
        }
        var_a1_13 = 0xFFC1;
        if (D_800B049A & 8) {
            var_a2_13 = 0xFFC1;
            sp58 = 1;
            goto block_79;
        }
        var_a1_13 = 0xF801;
        if (sp58 != 0) {
            var_a2_13 = 0xF801;
block_79:
            func_80016094(sp474, var_a1_13, var_a2_13, 1, 1);
        }
        var_a1_14 = 0xFFC1;
        if (D_800B049A & 0x10) {
            var_a2_14 = 0xFFC1;
            sp5C = 1;
            goto block_84;
        }
        var_a1_14 = 0xF801;
        if (sp5C != 0) {
            var_a2_14 = 0xF801;
block_84:
            func_80016094(sp47C, var_a1_14, var_a2_14, 1, 1);
        }
        var_a1_15 = 0xFFC1;
        if (D_800B049A & 0x20) {
            var_a2_15 = 0xFFC1;
            sp60 = 1;
            goto block_89;
        }
        var_a1_15 = 0xF801;
        if (sp60 != 0) {
            var_a2_15 = 0xF801;
block_89:
            func_80016094(sp484, var_a1_15, var_a2_15, 1, 1);
        }
        var_a1_16 = 0xFFC1;
        if (D_800B049A & 0x40) {
            var_a2_16 = 0xFFC1;
            sp64 = 1;
            goto block_94;
        }
        var_a1_16 = 0xF801;
        if (sp64 != 0) {
            var_a2_16 = 0xF801;
block_94:
            func_80016094(sp48C, var_a1_16, var_a2_16, 1, 1);
        }
        var_a1_17 = 0xFFC1;
        if (D_800B049A & 0x80) {
            var_a2_17 = 0xFFC1;
            sp68 = 1;
            goto block_99;
        }
        var_a1_17 = 0xF801;
        if (sp68 != 0) {
            var_a2_17 = 0xF801;
block_99:
            func_80016094(sp494, var_a1_17, var_a2_17, 1, 1);
        }
        var_a1_18 = 0xFFC1;
        if (D_800B049A & 0x100) {
            var_a2_18 = 0xFFC1;
            sp6C = 1;
            goto block_104;
        }
        var_a1_18 = 0xF801;
        if (sp6C != 0) {
            var_a2_18 = 0xF801;
block_104:
            func_80016094(sp49C, var_a1_18, var_a2_18, 1, 1);
        }
        var_a1_19 = 0xFFC1;
        if (D_800B058C & 0x800) {
            var_a2_19 = 0xFFC1;
            sp8C = 1;
            goto block_109;
        }
        var_a1_19 = 0xF801;
        if (sp8C != 0) {
            var_a2_19 = 0xF801;
block_109:
            func_80016094(sp4DC, var_a1_19, var_a2_19, 1, 1);
        }
        var_a1_20 = 0xFFC1;
        if (D_800B058C & 0x400) {
            var_a2_20 = 0xFFC1;
            sp90 = 1;
            goto block_114;
        }
        var_a1_20 = 0xF801;
        if (sp90 != 0) {
            var_a2_20 = 0xF801;
block_114:
            func_80016094(sp4E4, var_a1_20, var_a2_20, 1, 1);
        }
        var_a1_21 = 0xFFC1;
        if (D_800B058C & 0x200) {
            var_a2_21 = 0xFFC1;
            sp94 = 1;
            goto block_119;
        }
        var_a1_21 = 0xF801;
        if (sp94 != 0) {
            var_a2_21 = 0xF801;
block_119:
            func_80016094(sp4EC, var_a1_21, var_a2_21, 1, 1);
        }
        var_a1_22 = 0xFFC1;
        if (D_800B058C & 0x100) {
            var_a2_22 = 0xFFC1;
            sp98 = 1;
            goto block_124;
        }
        var_a1_22 = 0xF801;
        if (sp98 != 0) {
            var_a2_22 = 0xF801;
block_124:
            func_80016094(sp4F4, var_a1_22, var_a2_22, 1, 1);
        }
        var_a1_23 = 0xFFC1;
        if (D_800B058C & 0x8000) {
            var_a2_23 = 0xFFC1;
            sp9C = 1;
            goto block_129;
        }
        var_a1_23 = 0xF801;
        if (sp9C != 0) {
            var_a2_23 = 0xF801;
block_129:
            func_80016094(sp4FC, var_a1_23, var_a2_23, 1, 1);
        }
        var_a1_24 = 0xFFC1;
        if (D_800B058C & 0x4000) {
            var_a2_24 = 0xFFC1;
            spA0 = 1;
            goto block_134;
        }
        var_a1_24 = 0xF801;
        if (spA0 != 0) {
            var_a2_24 = 0xF801;
block_134:
            func_80016094(sp504, var_a1_24, var_a2_24, 1, 1);
        }
        var_a1_25 = 0xFFC1;
        if (D_800B058C & 0x10) {
            var_a2_25 = 0xFFC1;
            spA4 = 1;
            goto block_139;
        }
        var_a1_25 = 0xF801;
        if (spA4 != 0) {
            var_a2_25 = 0xF801;
block_139:
            func_80016094(sp50C, var_a1_25, var_a2_25, 1, 1);
        }
        var_a1_26 = 0xFFC1;
        if (D_800B058C & 1) {
            var_a2_26 = 0xFFC1;
            spA8 = 1;
            goto block_144;
        }
        var_a1_26 = 0xF801;
        if (spA8 != 0) {
            var_a2_26 = 0xF801;
block_144:
            func_80016094(sp514, var_a1_26, var_a2_26, 1, 1);
        }
        var_a1_27 = 0xFFC1;
        if (D_800B058C & 0x1000) {
            var_a2_27 = 0xFFC1;
            spB4 = 1;
            goto block_149;
        }
        var_a1_27 = 0xF801;
        if (spB4 != 0) {
            var_a2_27 = 0xF801;
block_149:
            func_80016094(sp51C, var_a1_27, var_a2_27, 1, 1);
        }
        var_a1_28 = 0xFFC1;
        if (D_800B058E & 0x800) {
            var_a2_28 = 0xFFC1;
            spBC = 1;
            goto block_154;
        }
        var_a1_28 = 0xF801;
        if (spBC != 0) {
            var_a2_28 = 0xF801;
block_154:
            func_80016094(sp52C, var_a1_28, var_a2_28, 1, 1);
        }
        var_a1_29 = 0xFFC1;
        if (D_800B058E & 0x400) {
            var_a2_29 = 0xFFC1;
            spC0 = 1;
            goto block_159;
        }
        var_a1_29 = 0xF801;
        if (spC0 != 0) {
            var_a2_29 = 0xF801;
block_159:
            func_80016094(sp534, var_a1_29, var_a2_29, 1, 1);
        }
        var_a1_30 = 0xFFC1;
        if (D_800B058E & 0x200) {
            var_a2_30 = 0xFFC1;
            spC4 = 1;
            goto block_164;
        }
        var_a1_30 = 0xF801;
        if (spC4 != 0) {
            var_a2_30 = 0xF801;
block_164:
            func_80016094(sp53C, var_a1_30, var_a2_30, 1, 1);
        }
        var_a1_31 = 0xFFC1;
        if (D_800B058E & 0x100) {
            var_a2_31 = 0xFFC1;
            spC8 = 1;
            goto block_169;
        }
        var_a1_31 = 0xF801;
        if (spC8 != 0) {
            var_a2_31 = 0xF801;
block_169:
            func_80016094(sp544, var_a1_31, var_a2_31, 1, 1);
        }
        var_a1_32 = 0xFFC1;
        if (D_800B058E & 0x8000) {
            var_a2_32 = 0xFFC1;
            spCC = 1;
            goto block_174;
        }
        var_a1_32 = 0xF801;
        if (spCC != 0) {
            var_a2_32 = 0xF801;
block_174:
            func_80016094(sp54C, var_a1_32, var_a2_32, 1, 1);
        }
        var_a1_33 = 0xFFC1;
        if (D_800B058E & 0x4000) {
            var_a2_33 = 0xFFC1;
            spD0 = 1;
            goto block_179;
        }
        var_a1_33 = 0xF801;
        if (spD0 != 0) {
            var_a2_33 = 0xF801;
block_179:
            func_80016094(sp554, var_a1_33, var_a2_33, 1, 1);
        }
        var_a1_34 = 0xFFC1;
        if (D_800B058E & 0x10) {
            var_a2_34 = 0xFFC1;
            spD4 = 1;
            goto block_184;
        }
        var_a1_34 = 0xF801;
        if (spD4 != 0) {
            var_a2_34 = 0xF801;
block_184:
            func_80016094(sp55C, var_a1_34, var_a2_34, 1, 1);
        }
        var_a1_35 = 0xFFC1;
        if (D_800B058E & 1) {
            var_a2_35 = 0xFFC1;
            spD8 = 1;
            goto block_189;
        }
        var_a1_35 = 0xF801;
        if (spD8 != 0) {
            var_a2_35 = 0xF801;
block_189:
            func_80016094(sp564, var_a1_35, var_a2_35, 1, 1);
        }
        var_a1_36 = 0xFFC1;
        if (D_800B058E & 0x1000) {
            var_a2_36 = 0xFFC1;
            spE4 = 1;
            goto block_194;
        }
        var_a1_36 = 0xF801;
        if (spE4 != 0) {
            var_a2_36 = 0xF801;
block_194:
            func_80016094(sp56C, var_a1_36, var_a2_36, 1, 1);
        }
        var_a1_37 = 0xFFC1;
        if (D_800C0D4C & 1) {
            var_a2_37 = 0xFFC1;
            spF8 = 1;
            goto block_199;
        }
        var_a1_37 = 0xF801;
        if (spF8 != 0) {
            var_a2_37 = 0xF801;
block_199:
            func_80016094(sp574, var_a1_37, var_a2_37, 1, 1);
        }
        var_a1_38 = 0xFFC1;
        if (D_800C0D4E & 1) {
            var_a2_38 = 0xFFC1;
            spFC = 1;
            goto block_204;
        }
        var_a1_38 = 0xF801;
        if (spFC != 0) {
            var_a2_38 = 0xF801;
block_204:
            func_80016094(sp57C, var_a1_38, var_a2_38, 1, 1);
        }
        var_a1_39 = 0xFFC1;
        if (D_800C0AB0 & 1) {
            var_a2_39 = 0xFFC1;
            sp100 = 1;
            goto block_209;
        }
        var_a1_39 = 0xF801;
        if (sp100 != 0) {
            var_a2_39 = 0xF801;
block_209:
            func_80016094(sp584, var_a1_39, var_a2_39, 1, 1);
        }
        var_a1_40 = 0xFFC1;
        if (D_800C30C4 & 1) {
            var_a2_40 = 0xFFC1;
            sp104 = 1;
            goto block_214;
        }
        var_a1_40 = 0xF801;
        if (sp104 != 0) {
            var_a2_40 = 0xF801;
block_214:
            func_80016094(sp58C, var_a1_40, var_a2_40, 1, 1);
        }
        if ((u32) (M2C_FIELD(&D_800B04AC, s16 *, 0) + 4) < 8U) {
            if (spE8 != 0) {
                func_80025DC0(sp59C, "BR1 X:%d  ", M2C_FIELD(&D_800B04AC, s16 *, 0));
                func_800161E8(temp_s6, sp59C);
                func_80016094(temp_s6, 0xF801, 0xF801, 1, 1);
            }
        } else {
            spE8 = 1;
            func_80025DC0(sp59C, "BR1 X:%d  ", M2C_FIELD(&D_800B04AC, s16 *, 0));
            func_800161E8(temp_s6, sp59C);
            func_80015EA4(temp_s6, 0xFFC1, 0xFFC1, 1, 1);
        }
        if ((u32) (M2C_FIELD(&D_800C5298, s16 *, 0) + 4) < 8U) {
            if (spF0 != 0) {
                func_80025DC0(sp59C, "BR1 Y:%d  ", M2C_FIELD(&D_800C5298, s16 *, 0));
                func_800161E8(temp_s5, sp59C);
                func_80016094(temp_s5, 0xF801, 0xF801, 1, 1);
            }
        } else {
            spF0 = 1;
            func_80025DC0(sp59C, "BR1 Y:%d  ", M2C_FIELD(&D_800C5298, s16 *, 0));
            func_800161E8(temp_s5, sp59C);
            func_80015EA4(temp_s5, 0xFFC1, 0xFFC1, 1, 1);
        }
        if ((u32) (M2C_FIELD(&D_800B04AC, s16 *, 2) + 4) < 8U) {
            if (spEC != 0) {
                func_80025DC0(sp59C, "BR2 X:%d  ", M2C_FIELD(&D_800B04AC, s16 *, 2));
                func_800161E8(temp_s4, sp59C);
                func_80016094(temp_s4, 0xF801, 0xF801, 1, 1);
            }
        } else {
            spEC = 1;
            func_80025DC0(sp59C, "BR2 X:%d  ", M2C_FIELD(&D_800B04AC, s16 *, 2));
            func_800161E8(temp_s4, sp59C);
            func_80015EA4(temp_s4, 0xFFC1, 0xFFC1, 1, 1);
        }
        var_a0_2 = sp59C;
        if ((u32) (M2C_FIELD(&D_800C5298, s16 *, 2) + 4) < 8U) {
            var_t0_4 = 0;
            if (spF4 != 0) {
                func_80025DC0(sp59C, "BR2 Y:%d  ", M2C_FIELD(&D_800C5298, s16 *, 2));
                func_800161E8(temp_s3, sp59C);
                func_80016094(temp_s3, 0xF801, 0xF801, 1, 1);
                var_t0_4 = 0;
                goto block_231;
            }
        } else {
            spF4 = 1;
            func_80025DC0(var_a0_2, "BR2 Y:%d  ", M2C_FIELD(&D_800C5298, s16 *, 2));
            func_800161E8(temp_s3, sp59C);
            func_80015EA4(temp_s3, 0xFFC1, 0xFFC1, 1, 1);
            var_t0_4 = 0;
block_231:
            var_a0_2 = sp59C;
        }
loop_232:
        var_t1 = 0x4C;
        if (((s32) D_800C52A0 >> (var_t0_4 + 8)) & 1) {
            var_t1 = 0x48;
        }
        *var_a0_2 = var_t1;
        var_t0_4 += 1;
        var_a0_2 += 1;
        if (var_t0_4 < 8) {
            goto loop_232;
        }
        sp59C[var_t0_4] = 0;
        var_t0_5 = 0;
        var_v1_2 = &sp228;
loop_236:
        var_t1_2 = 0x4C;
        if (((s32) D_800C52A0 >> var_t0_5) & 1) {
            var_t1_2 = 0x48;
        }
        *var_v1_2 = var_t1_2;
        var_t0_5 += 1;
        var_v1_2 += 1;
        if (var_t0_5 < 8) {
            goto loop_236;
        }
        (&sp228)[var_t0_5] = 0;
        func_800161E8(sp4B4, &sp218);
        func_800161E8(sp4CC, &sp228);
    } while (D_800AF220 == 0);
    func_8000AEAC();
    func_800117BC(sp244);
    func_800117BC(sp24C);
    func_800117BC(sp254);
    func_800117BC(sp25C);
    func_800117BC(sp264);
    func_800117BC(sp26C);
    func_800117BC(sp274);
    func_800117BC(sp27C);
    func_800117BC(sp284);
    func_800117BC(sp28C);
    func_800117BC(sp294);
    func_800117BC(sp29C);
    func_800117BC(sp2A4);
    func_800117BC(sp2AC);
    func_800117BC(sp2B4);
    func_800117BC(sp2BC);
    func_800117BC(sp2C4);
    func_800117BC(sp2CC);
    func_800117BC(sp2D4);
    func_800117BC(sp2DC);
    func_800117BC(sp2E4);
    func_800117BC(sp2EC);
    func_8000AEAC();
    func_800117BC(sp2F4);
    func_800117BC(sp2FC);
    func_800117BC(sp304);
    func_800117BC(sp30C);
    func_800117BC(sp314);
    func_800117BC(sp31C);
    func_800117BC(sp324);
    func_800117BC(sp32C);
    func_800117BC(sp334);
    func_800117BC(sp33C);
    func_800117BC(sp344);
    func_800117BC(sp34C);
    func_800117BC(sp354);
    func_800117BC(sp35C);
    func_800117BC(sp364);
    func_800117BC(sp36C);
    func_8000AEAC();
    func_800117BC(sp374);
    func_800117BC(sp37C);
    func_800117BC(sp384);
    func_800117BC(sp38C);
    func_800117BC(sp394);
    func_800117BC(sp39C);
    func_800117BC(sp3A4);
    func_800117BC(sp3AC);
    func_800117BC(sp3B4);
    func_800117BC(sp3BC);
    func_800117BC(sp3C4);
    func_800117BC(sp3CC);
    func_800117BC(sp3D4);
    func_800117BC(sp3DC);
    func_800117BC(sp3E4);
    func_800117BC(sp3EC);
    func_800117BC(temp_v0_55);
    func_800117BC(temp_v0_56);
    func_8000AEAC();
    func_8000AEAC();
    func_80015BE4(sp3F4);
    func_80015BE4(sp3FC);
    func_80015BE4(sp404);
    func_80015BE4(sp40C);
    func_80015BE4(sp414);
    func_80015BE4(sp41C);
    func_80015BE4(sp424);
    func_80015BE4(sp42C);
    func_80015BE4(sp434);
    func_80015BE4(sp43C);
    func_80015BE4(sp444);
    func_80015BE4(sp44C);
    func_80015BE4(sp454);
    func_80015BE4(sp45C);
    func_80015BE4(sp464);
    func_80015BE4(sp46C);
    func_80015BE4(sp474);
    func_80015BE4(sp47C);
    func_80015BE4(sp484);
    func_80015BE4(sp48C);
    func_80015BE4(sp494);
    func_80015BE4(sp49C);
    func_8000AEAC();
    func_80015BE4(sp4A4);
    func_80015BE4(sp4AC);
    func_80015BE4(sp4B4);
    func_80015BE4(sp4BC);
    func_80015BE4(sp4C4);
    func_80015BE4(sp4CC);
    func_80015BE4(sp4D4);
    func_80015BE4(sp4DC);
    func_80015BE4(sp4E4);
    func_80015BE4(sp4EC);
    func_80015BE4(sp4F4);
    func_80015BE4(sp4FC);
    func_80015BE4(sp504);
    func_80015BE4(sp50C);
    func_80015BE4(sp514);
    func_80015BE4(sp51C);
    func_8000AEAC();
    func_80015BE4(sp524);
    func_80015BE4(sp52C);
    func_80015BE4(sp534);
    func_80015BE4(sp53C);
    func_80015BE4(sp544);
    func_80015BE4(sp54C);
    func_80015BE4(sp554);
    func_80015BE4(sp55C);
    func_80015BE4(sp564);
    func_80015BE4(sp56C);
    func_80015BE4(temp_s6);
    func_80015BE4(temp_s4);
    func_80015BE4(temp_s5);
    func_80015BE4(temp_s3);
    func_80015BE4(sp574);
    func_80015BE4(sp57C);
    func_80015BE4(sp584);
    func_80015BE4(sp58C);
    func_8000AEAC();
    *sp23C = 0x10000;
}

void ovl0_func_800E33C8(s32 *arg0) {
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    s32 temp_s1;
    s32 temp_s3;

    temp_s3 = func_80015AA0(0, "PUSH TEST SW TO EXIT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(temp_v0, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    func_80015E14(temp_v0, temp_s3, 0xC2A00000, 0xC2C00000, 1.0f, 1.0f);
    temp_s1 = func_80015AA0(0, "SOUND TEST", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_2 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    func_80015E14(temp_v0_2, temp_s1, 0xC2200000, 0x42DC0000, 1.0f, 1.0f);
    func_8000D1CC(7);
    do {
        func_8000AEAC();
    } while (D_800AF220 == 0);
    func_8000D160();
    func_8000AEAC();
    func_800117BC(temp_v0);
    func_800117BC(temp_v0_2);
    func_8000AEAC();
    func_8000AEAC();
    func_80015BE4(temp_s3);
    func_80015BE4(temp_s1);
    func_8000AEAC();
    *arg0 = 0x10000;
}

void ovl0_func_800E3588(s32 *arg0) {
    u8 sp18;
    s8 sp217;
    s8 sp218;
    s32 *sp31C;
    s32 sp324;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_4;
    M2C_UNK *temp_v0_5;
    M2C_UNK *var_a1_3;
    M2C_UNK *var_a1_5;
    s32 *var_v1;
    s32 *var_v1_2;
    s32 temp_fp;
    s32 temp_s2;
    s32 temp_s4;
    s32 temp_s7;
    s32 var_a0;
    s32 var_a0_2;
    s32 var_a0_4;
    s32 var_a1;
    s32 var_a1_2;
    s32 var_s0;
    s32 var_s0_2;
    s8 *var_v0;
    s8 var_a0_3;
    u8 *var_a1_4;

    sp31C = arg0;
    sp324 = func_80015AA0(0, "PUSH TEST SW TO EXIT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(temp_v0, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    func_80015E14(temp_v0, sp324, 0xC2A00000, 0xC2C00000, 1.0f, 1.0f);
    temp_fp = func_80015AA0(0, "SYSTEM CHECK", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_2 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(temp_v0_2, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_2, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    func_80015E14(temp_v0_2, temp_fp, 0xC2400000, 0x42DC0000, 1.0f, 1.0f);
    func_8000AEAC();
    var_a1 = 0;
    var_v1 = (s32 *)0x01000000;
    var_a0 = 0;
    do {
        *var_v1 = var_a1;
        var_a0 += 1;
        var_a1 += 1;
    } while (var_a0 <= 0xFFFFF);
    var_a1_2 = 0;
    var_s0 = 0;
    var_v1_2 = (s32 *)0x01000000;
    var_a0_2 = 0;
loop_3:
    if (*var_v1_2 == var_a1_2) {
        var_a0_2 += 1;
        var_a1_2 += 1;
        if (var_a0_2 <= 0xFFFFF) {
            goto loop_3;
        }
    } else {
        var_s0 = 1;
    }
    temp_s7 = func_80015AA0(0, "VERSION 00.09", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_3 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    M2C_FIELD(temp_v0_3, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_3, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    func_80015E14(temp_v0_3, temp_s7, 0xC2C80000, 0x42200000, 1.0f, 1.0f);
    if (var_s0 != 0) {
        var_a1_3 = "SDRAM ERROR";
    } else {
        var_a1_3 = "SDRAM OK";
    }
    func_80025DC0(&sp218, var_a1_3);
    temp_s4 = func_80015AA0(0, &sp218, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_4 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    M2C_FIELD(temp_v0_4, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_4, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    func_80015E14(temp_v0_4, temp_s4, 0xC2C80000, 0x41A00000, 1.0f, 1.0f);
    var_a0_3 = 0x1FF;
    var_v0 = &sp217;
    do {
        *var_v0 = var_a0_3;
        var_a0_3 -= 1;
        var_v0 -= 1;
    } while (var_a0_3 >= 0);
    func_8001556C(&sp18);
    var_s0_2 = 0;
    func_80015524(&sp18);
    var_a0_4 = 0;
    var_a1_4 = &sp18;
loop_13:
    var_a0_4 += 1;
    if (*var_a1_4 == (var_a0_4 & 0xFF)) {
        var_a1_4 += 1;
        if (var_a0_4 < 0x200) {
            goto loop_13;
        }
    } else {
        var_s0_2 = 1;
    }
    if (var_s0_2 != 0) {
        var_a1_5 = "EEPROM ERROR";
    } else {
        var_a1_5 = "EEPROM OK";
    }
    func_80025DC0(&sp218, var_a1_5);
    temp_s2 = func_80015AA0(0, &sp218, 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_5 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    M2C_FIELD(temp_v0_5, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(temp_v0_5, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    func_80015E14(temp_v0_5, temp_s2, 0xC2C80000, 0, 1.0f, 1.0f);
    do {
        func_8000AEAC();
    } while (D_800AF220 == 0);
    func_8000AEAC();
    func_800117BC(temp_v0);
    func_800117BC(temp_v0_2);
    func_800117BC(temp_v0_3);
    func_800117BC(temp_v0_4);
    func_800117BC(temp_v0_5);
    func_8000AEAC();
    func_8000AEAC();
    func_80015BE4(sp324);
    func_80015BE4(temp_fp);
    func_80015BE4(temp_s7);
    func_80015BE4(temp_s4);
    func_80015BE4(temp_s2);
    func_8000AEAC();
    *sp31C = 0x10000;
}

void ovl0_func_800E3A48(s32 *arg0) {
    M2C_UNK *sp18;
    M2C_UNK *sp1C;
    M2C_UNK *sp20;
    M2C_UNK *sp24;
    M2C_UNK *sp28;
    M2C_UNK *sp2C;
    M2C_UNK *sp30;
    M2C_UNK *sp34;
    s32 sp38;
    s32 sp3C;
    s32 sp40;
    s32 sp44;
    s32 sp48;
    s32 sp4C;
    s32 sp50;
    s32 sp54;
    M2C_UNK sp100;
    M2C_UNK **var_s1;
    M2C_UNK **var_s1_2;
    M2C_UNK *temp_a0_3;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_4;
    M2C_UNK *temp_v0_5;
    M2C_UNK *temp_v0_6;
    M2C_UNK *temp_v0_7;
    M2C_UNK *temp_v0_8;
    M2C_UNK *var_v0;
    s16 var_s3;
    s32 *var_s0_2;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a0_4;
    s32 var_s0;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s2;
    s32 phi_a0;

    var_s3 = saved_reg_s3;
    var_s2 = 0;
    var_s0 = 0x3A;
    var_v0 = &sp100;
    do {
        M2C_FIELD(var_v0, s32 *, 0x40) = 0;
        var_s0 -= 1;
        var_v0 -= 4;
    } while (var_s0 >= 0);
    sp38 = func_80015AA0(0, "OUTPUT TEST", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0 = func_800116B8(2);
    sp18 = temp_v0;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(sp18, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp18, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    func_80015E14(sp18, sp38, 0xC2200000, 0x42DC0000, 1.0f, 1.0f);
    sp3C = func_80015AA0(0, "PUSH TEST SW TO EXIT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_2 = func_800116B8(2);
    sp1C = temp_v0_2;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(sp1C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp1C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    func_80015E14(sp1C, sp3C, 0xC2A00000, 0xC2C00000, 1.0f, 1.0f);
    sp40 = func_80015AA0(0, "1P UP/DOWN TO SELECT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_3 = func_800116B8(2);
    sp20 = temp_v0_3;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    M2C_FIELD(sp20, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp20, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    func_80015E14(sp20, sp40, 0xC2A00000, 0xC2980000, 1.0f, 1.0f);
    sp44 = func_80015AA0(0, "PUSH 1P BUTTON TO ENABLE", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_4 = func_800116B8(2);
    sp24 = temp_v0_4;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    M2C_FIELD(sp24, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp24, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    func_80015E14(sp24, sp44, 0xC2C00000, 0xC2840000, 1.0f, 1.0f);
    sp48 = func_80015AA0(0, "LOCKOUT COIL1", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_5 = func_800116B8(2);
    sp28 = temp_v0_5;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    M2C_FIELD(sp28, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp28, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    func_80015E14(sp28, sp48, 0xC2500000, 0x42700000, 1.0f, 1.0f);
    sp4C = func_80015AA0(0, "LOCKOUT COIL2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_6 = func_800116B8(2);
    sp2C = temp_v0_6;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_6;
    M2C_FIELD(sp2C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_6;
    func_80015E14(sp2C, sp4C, 0xC2500000, 0x42200000, 1.0f, 1.0f);
    sp50 = func_80015AA0(0, "COIN COUNTER1", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_7 = func_800116B8(2);
    sp30 = temp_v0_7;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_7;
    M2C_FIELD(sp30, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp30, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_7;
    func_80015E14(sp30, sp50, 0xC2500000, 0x41A00000, 1.0f, 1.0f);
    sp54 = func_80015AA0(0, "COIN COUNTER2", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_8 = func_800116B8(2);
    sp34 = temp_v0_8;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_8;
    M2C_FIELD(sp34, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp34, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_8;
    func_80015E14(sp34, sp54, 0xC2500000, 0, 1.0f, 1.0f);
    var_s0_2 = &(&sp48)[0];
loop_3:
    func_8000AEAC();
    if (D_800AF220 == 0) {
        if (!(D_800C0D48 & 1)) {
            if ((D_800C0D48 & 2) && (var_s2 < 3)) {
                temp_a0 = *var_s0_2;
                var_s0_2 += 4;
                var_s2 += 1;
                phi_a0 = temp_a0;
                goto block_12;
            }
        } else if (var_s2 > 0) {
            temp_a0_2 = *var_s0_2;
            var_s0_2 -= 4;
            var_s2 -= 1;
            phi_a0 = temp_a0_2;
block_12:
            func_80016094(phi_a0, 0xFFFF, 0xFFFF, 1);
        }
        if (!(D_800B060C & 0x10)) {
            var_s3 = 0;
            func_80016094(*var_s0_2, 0xF801, 0xF801, 1, 1);
        } else {
            switch (var_s2) {                       /* irregular */
            case 0:
                var_s3 = 4;
                break;
            case 3:
                var_s3 = 2;
                break;
            case 1:
                var_s3 = 8;
                break;
            case 2:
                var_s3 = 1;
                break;
            }
            func_80015EA4(*var_s0_2, 0xFFC1, 0xFFC1, 1);
        }
        D_800C178C = var_s3;
        goto loop_3;
    }
    var_s0_3 = 0;
    func_8000AEAC();
    var_s1 = &sp18;
    do {
        temp_a0_3 = *var_s1;
        var_s1 += 4;
        var_s0_3 += 1;
        func_800117BC(temp_a0_3);
    } while (var_s0_3 < 8);
    var_s0_4 = 0;
    func_8000AEAC();
    func_8000AEAC();
    var_s1_2 = &sp18;
    do {
        temp_a0_4 = M2C_FIELD(var_s1_2, s32 *, 0x20);
        var_s1_2 += 4;
        var_s0_4 += 1;
        func_80015BE4(temp_a0_4);
    } while (var_s0_4 < 8);
    func_8000AEAC();
    *arg0 = 0x10000;
}

void ovl0_func_800E40D0(s32 *arg0) {
    M2C_UNK *sp18;
    M2C_UNK *sp1C;
    M2C_UNK *sp20;
    M2C_UNK *sp24;
    M2C_UNK *sp28;
    M2C_UNK *sp2C;
    M2C_UNK *sp30;
    M2C_UNK *sp34;
    M2C_UNK *sp38;
    M2C_UNK *sp3C;
    M2C_UNK *sp40;
    s32 sp48;
    s32 sp4C;
    s32 sp50;
    s32 sp54;
    s32 sp58;
    s32 sp5C;
    s32 sp60;
    s32 sp64;
    s32 sp68;
    s32 sp6C;
    s32 sp70;
    M2C_UNK sp100;
    M2C_UNK **var_s1;
    M2C_UNK **var_s1_2;
    M2C_UNK *temp_a0_3;
    M2C_UNK *temp_v0;
    M2C_UNK *temp_v0_10;
    M2C_UNK *temp_v0_11;
    M2C_UNK *temp_v0_2;
    M2C_UNK *temp_v0_3;
    M2C_UNK *temp_v0_4;
    M2C_UNK *temp_v0_5;
    M2C_UNK *temp_v0_6;
    M2C_UNK *temp_v0_7;
    M2C_UNK *temp_v0_8;
    M2C_UNK *temp_v0_9;
    M2C_UNK *var_v0;
    s32 *var_s0_2;
    s32 temp_a0;
    s32 temp_a0_2;
    s32 temp_a0_4;
    s32 var_s0;
    s32 var_s0_3;
    s32 var_s0_4;
    s32 var_s2;
    s8 *var_a1;
    s32 phi_a0;

    var_s2 = 0;
    var_s0 = 0x3A;
    var_v0 = &sp100;
    do {
        M2C_FIELD(var_v0, s32 *, 0x60) = 0;
        var_s0 -= 1;
        var_v0 -= 4;
    } while (var_s0 >= 0);
    sp48 = func_80015AA0(0, "TEST MODE MENU", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0 = func_800116B8(2);
    sp18 = temp_v0;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(sp18, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp18, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    func_80015E14(sp18, sp48, 0xC2600000, 0x42DC0000, 1.0f, 1.0f);
    sp4C = func_80015AA0(0, "                    ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_2 = func_800116B8(2);
    sp1C = temp_v0_2;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    M2C_FIELD(sp1C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp1C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_2;
    func_80015E14(sp1C, sp4C, 0xC2A00000, 0xC2C00000, 1.0f, 1.0f);
    sp50 = func_80015AA0(0, "1P UP/DOWN TO SELECT", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_3 = func_800116B8(2);
    sp20 = temp_v0_3;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    M2C_FIELD(sp20, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp20, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_3;
    func_80015E14(sp20, sp50, 0xC2A00000, 0xC2980000, 1.0f, 1.0f);
    sp54 = func_80015AA0(0, "PUSH TEST SW TO ENABLE", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_4 = func_800116B8(2);
    sp24 = temp_v0_4;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    M2C_FIELD(sp24, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp24, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_4;
    func_80015E14(sp24, sp54, 0xC2C00000, 0xC2840000, 1.0f, 1.0f);
    sp58 = func_80015AA0(0, "SYSTEM TEST  ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_5 = func_800116B8(2);
    sp28 = temp_v0_5;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    M2C_FIELD(sp28, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp28, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_5;
    func_80015E14(sp28, sp58, 0xC2500000, 0x42700000, 1.0f, 1.0f);
    sp5C = func_80015AA0(0, "SCREEN TEST  ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_6 = func_800116B8(2);
    sp2C = temp_v0_6;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_6;
    M2C_FIELD(sp2C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp2C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_6;
    func_80015E14(sp2C, sp5C, 0xC2500000, 0x42200000, 1.0f, 1.0f);
    sp60 = func_80015AA0(0, "INPUT TEST   ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_7 = func_800116B8(2);
    sp30 = temp_v0_7;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_7;
    M2C_FIELD(sp30, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp30, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_7;
    func_80015E14(sp30, sp60, 0xC2500000, 0x41A00000, 1.0f, 1.0f);
    sp64 = func_80015AA0(0, "OUTPUT TEST  ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_8 = func_800116B8(2);
    sp34 = temp_v0_8;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_8;
    M2C_FIELD(sp34, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp34, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_8;
    func_80015E14(sp34, sp64, 0xC2500000, 0, 1.0f, 1.0f);
    sp68 = func_80015AA0(0, "SOUND TEST   ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_9 = func_800116B8(2);
    sp38 = temp_v0_9;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_9;
    M2C_FIELD(sp38, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp38, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_9;
    func_80015E14(sp38, sp68, 0xC2500000, 0xC1A00000, 1.0f, 1.0f);
    sp6C = func_80015AA0(0, "1P:JOYSTICK  ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_10 = func_800116B8(2);
    sp3C = temp_v0_10;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_10;
    M2C_FIELD(sp3C, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp3C, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_10;
    func_80015E14(sp3C, sp6C, 0xC2500000, 0xC2100000, 1.0f, 1.0f);
    sp70 = func_80015AA0(0, "2P:JOYSTICK  ", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0_11 = func_800116B8(2);
    sp40 = temp_v0_11;
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_11;
    M2C_FIELD(sp40, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    M2C_FIELD(sp40, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0_11;
    func_80015E14(sp40, sp70, 0xC2500000, 0xC2380000, 1.0f, 1.0f);
    func_80016094(sp6C, 0x7FF, 0x7FF, 1, 1);
    func_80016094(sp70, 0x7FF, 0x7FF, 1, 1);
    var_s0_2 = &(&sp58)[0];
loop_3:
    func_8000AEAC();
    if (D_800AF220 == 0) {
        if (!(D_800C0D48 & 1)) {
            if ((D_800C0D48 & 2) && (var_s2 < 4)) {
                temp_a0 = *var_s0_2;
                var_s0_2 += 4;
                var_s2 += 1;
                phi_a0 = temp_a0;
                goto block_12;
            }
        } else if (var_s2 > 0) {
            temp_a0_2 = *var_s0_2;
            var_s0_2 -= 4;
            var_s2 -= 1;
            phi_a0 = temp_a0_2;
block_12:
            func_80016094(phi_a0, 0xFFFF, 0xFFFF, 1);
        }
        func_80016094(*var_s0_2, 0xFA11, 0xFA11, 1, 1);
        if (D_800C52A0 & 0x10) {
            func_800161E8(sp6C, "1P:JAMMASTICK");
            func_800161E8(sp70, "2P:JAMMASTICK");
        } else {
            if (((D_800B058C & 0xC00) == 0xC00) || ((D_800B058C & 0x300) == 0x300)) {
                var_a1 = "1P:3DSTICK   ";
            } else {
                var_a1 = "1P:JOYSTICK  ";
            }
            func_800161E8(sp6C, var_a1);
            if (((D_800B058E & 0xC00) == 0xC00) || ((D_800B058E & 0x300) == 0x300)) {
                func_800161E8(sp70, "2P:3DSTICK   ");
            } else {
                func_800161E8(sp70, "2P:JOYSTICK  ");
            }
        }
        goto loop_3;
    }
    var_s0_3 = 0;
    func_8000AEAC();
    var_s1 = &sp18;
    do {
        temp_a0_3 = *var_s1;
        var_s1 += 4;
        var_s0_3 += 1;
        func_800117BC(temp_a0_3);
    } while (var_s0_3 < 0xB);
    var_s0_4 = 0;
    func_8000AEAC();
    func_8000AEAC();
    var_s1_2 = &sp18;
    do {
        temp_a0_4 = M2C_FIELD(var_s1_2, s32 *, 0x30);
        var_s1_2 += 4;
        var_s0_4 += 1;
        func_80015BE4(temp_a0_4);
    } while (var_s0_4 < 0xB);
    func_8000AEAC();
    *arg0 = var_s2 | 0x10000;
}

void ovl0_func_800E4988(s32 *arg0) {
    M2C_UNK *temp_v0;
    s32 temp_s2;
    s32 var_s0;

    temp_s2 = func_80015AA0(0, "COIN ERROR", 0xFFFF, 0xE739, 1, 0x4211);
    temp_v0 = func_800116B8(2);
    *M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    M2C_FIELD(temp_v0, M2C_UNK **, 4) = (M2C_UNK *) M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4);
    var_s0 = 0;
    M2C_FIELD(temp_v0, M2C_UNK **, 0) = M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4);
    M2C_FIELD(M2C_FIELD(D_800B04A0, M2C_UNK **, 0xD4), M2C_UNK **, 4) = temp_v0;
    func_80015E14(temp_v0, temp_s2, 0xC2200000, 0, 1.0f, 1.0f);
    do {
        var_s0 += 1;
        func_8000AEAC();
    } while (var_s0 < 0x12C);
    func_8000AEAC();
    func_800117BC(temp_v0);
    func_8000AEAC();
    func_8000AEAC();
    func_80015BE4(temp_s2);
    func_8000AEAC();
    D_800B0576 = 0;
    D_800B0574 = 0;
    *arg0 = 0x10000;
}
