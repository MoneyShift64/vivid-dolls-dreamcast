# Area "rotate" (RAM 0x80006C10 - 0x800088E0)

Source: `src/game/rotate.c` / `rotate.h`. Test: `python3 tools/difftest/test_rotate.py`
(400 random trials per case, incl. aliasing out==in, zero-diagonal and singular matrices,
parallel/anti-parallel vectors). All 27 functions PASS bit-exact.

## Functions done

| Address | Name | What it does |
|---|---|---|
| 0x80006C10 | Mtx_LookAt | camera matrix from eye/at/up (side vector s = u x f is NOT normalised) |
| 0x80006EA8 | Mtx_Perspective | perspective projection (fovy in degrees, aspect, near, far), like guPerspectiveF w/o scale |
| 0x80006F74 | Mtx_Ortho | orthographic projection, all 16 elements times scale (guOrthoF) |
| 0x800070A4 | Mtx_RotateAxisSinCos | rotation about unit axis given sin and cos (axis re-read between stores) |
| 0x800071CC | Mtx_RotateAxisDeg | rotation by degrees about (x,y,z), axis normalised via Vec3_NormalizeXYZ |
| 0x800073B0 | Mtx_Invert | 4x4 inverse: Gauss-Jordan if diagonal non-zero (returns 1), else permuted version |
| 0x80007420 | Mtx_InvertGaussJordan | in-place Gauss-Jordan without pivoting (copies in->out first); zero pivot skipped |
| 0x80007558 | Mtx_InvertPermuted | picks first row permutation with non-zero diagonal from table, LU + back-substitution; 0 if none |
| 0x8000793C | Vec3_RotateFromTo | out = v * RotateBetween(from, v); ignores failure (uses uninitialised stack matrix then) |
| 0x80007A08 | Mtx_RotateBetweenUnit | rotation turning unit a onto unit b; 0 if |a x b|^2 < 1e-16; clamps cos <= -1 |
| 0x80007D28 | Mtx_RotateBetween | normalises a, b then Mtx_RotateBetweenUnit |
| 0x80007D84 | Mtx_BasisChange | 3x3 frame-change matrix from 4 vectors (a, b, c, d); 1/det replaced by 0 if det < 1e-16 (also negative det) |
| 0x80008190 | Mtx_RotateAxisRad | rotation by radians about unit axis (fills whole matrix) |
| 0x800082DC | Mtx_PreRotateX | out = RotX(deg) * m |
| 0x800083A8 | Mtx_PreRotateY | out = RotY(deg) * m |
| 0x80008474 | Mtx_PreRotateZ | out = RotZ(deg) * m |
| 0x80008540 | Mtx_PreRotateYXZ | out = RotZ(z) * RotX(x) * RotY(y) * m (args order y, x, z) |
| 0x800085B4 | Mtx_PreTranslate | out = Translate(x,y,z) * m |
| 0x80008624 | Mtx_PreTranslateX | out = Translate(x,0,0) * m |
| 0x80008688 | Mtx_PreTranslateY | out = Translate(0,y,0) * m |
| 0x800086EC | Mtx_PreTranslateZ | out = Translate(0,0,z) * m |
| 0x80008750 | Mtx_PreScale | out = Scale(x,y,z) * m |
| 0x800087BC | Mtx_PreScaleX | row 0 scaled |
| 0x80008818 | Mtx_PreScaleY | row 1 scaled |
| 0x80008874 | Mtx_PreScaleZ | row 2 scaled |
| 0x800088D0 | Rotate_EmptyA | empty function (jr ra), no known callers |
| 0x800088D8 | Rotate_EmptyB | empty function (jr ra), no known callers |

Symbol lines:
```
Mtx_LookAt = 0x80006C10; // type:func
Mtx_Perspective = 0x80006EA8; // type:func
Mtx_Ortho = 0x80006F74; // type:func
Mtx_RotateAxisSinCos = 0x800070A4; // type:func
Mtx_RotateAxisDeg = 0x800071CC; // type:func
Mtx_Invert = 0x800073B0; // type:func
Mtx_InvertGaussJordan = 0x80007420; // type:func
Mtx_InvertPermuted = 0x80007558; // type:func
Vec3_RotateFromTo = 0x8000793C; // type:func
Mtx_RotateBetweenUnit = 0x80007A08; // type:func
Mtx_RotateBetween = 0x80007D28; // type:func
Mtx_BasisChange = 0x80007D84; // type:func
Mtx_RotateAxisRad = 0x80008190; // type:func
Mtx_PreRotateX = 0x800082DC; // type:func
Mtx_PreRotateY = 0x800083A8; // type:func
Mtx_PreRotateZ = 0x80008474; // type:func
Mtx_PreRotateYXZ = 0x80008540; // type:func
Mtx_PreTranslate = 0x800085B4; // type:func
Mtx_PreTranslateX = 0x80008624; // type:func
Mtx_PreTranslateY = 0x80008688; // type:func
Mtx_PreTranslateZ = 0x800086EC; // type:func
Mtx_PreScale = 0x80008750; // type:func
Mtx_PreScaleX = 0x800087BC; // type:func
Mtx_PreScaleY = 0x80008818; // type:func
Mtx_PreScaleZ = 0x80008874; // type:func
Rotate_EmptyA = 0x800088D0; // type:func
Rotate_EmptyB = 0x800088D8; // type:func
```

## Data symbols
```
D_800383A0 = 0x800383A0; // type:u8 size:0x60  (24 permutations of {0,1,2,3}, lexicographic; used by Mtx_InvertPermuted)
D_8003D650 = 0x8003D650; // type:f64 size:0x8  (-1.0, Mtx_LookAt; written as a literal in C)
D_8003D658 = 0x8003D658; // type:f64 size:0x8  (1.0, Mtx_LookAt)
D_8003D660 = 0x8003D660; // type:f64 size:0x8  (1.0, Mtx_InvertGaussJordan)
D_8003D668 = 0x8003D668; // type:f64 size:0x8  (1.0, Mtx_InvertPermuted)
D_8003D670 = 0x8003D670; // type:f64 size:0x8  (1.0, Mtx_InvertPermuted)
```

## Notes
- Angle convention here: degrees, converted with the float constant 0x3C8EFA35 (pi/180).
- The reciprocals in LookAt and the inverters are done as `(f32)(1.0 / (f64)x)`.
- The "Pre" functions work column by column and re-read m after writing out, so with
  out == m the rotations use the already-updated row (kept; tested with aliasing).
- No functions remaining in this range.
