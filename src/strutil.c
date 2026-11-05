/* Vivid Dolls - string helpers. Verified against the original MIPS code. */
#include "strutil.h"

/* Skip the rest of the current keyword, leaving list on the next one. */
#define KW_SKIP(list) do { while (*(list)++ != 0) {} } while (0)

/* @ 0x800007A0 */
s32 Kw_MatchTable(u8 **pp, const u8 *list, const u8 *tbl) {
    u8 *s = *pp;
    s32 idx = 0;

    if (*list == 0) {
        return -1;
    }
    for (;;) {
        u32 k = *list++;
        if (k == 0) {
            /* whole keyword matched: must end on a word boundary */
            if (tbl[s[0]] == 0 || tbl[s[-1]] == 0) {
                *pp = s;
                return idx;
            }
        } else {
            u32 c = *s++;
            if (k == c || k == tbl[c]) {
                continue;
            }
            KW_SKIP(list);
        }
        if (*list == 0) {
            return -1;
        }
        idx++;
        s = *pp;
    }
}

/* @ 0x80000858 */
s32 Kw_MatchLower(u8 **pp, const u8 *list) {
    return Kw_MatchTable(pp, list, gCharTableLower);
}

/* @ 0x80000918 */
s32 Kw_MatchUpper(u8 **pp, const u8 *list) {
    return Kw_MatchTable(pp, list, gCharTableUpper);
}

/* @ 0x800009D8 - exact prefix match, no boundary check */
s32 Kw_MatchExact(u8 **pp, const u8 *list) {
    u8 *s = *pp;
    s32 idx = 0;

    if (*list == 0) {
        return -1;
    }
    for (;;) {
        u32 k = *list++;
        if (k == 0) {
            *pp = s;
            return idx;
        }
        if (k == *s++) {
            continue;
        }
        KW_SKIP(list);
        if (*list == 0) {
            return -1;
        }
        idx++;
        s = *pp;
    }
}

/* @ 0x80000A40 */
u8 *Kw_MatchLowerAt(s32 *idx, u8 *s, const u8 *list) {
    *idx = Kw_MatchTable(&s, list, gCharTableLower);
    return s;
}

/* @ 0x80000B04 */
u8 *Kw_MatchUpperAt(s32 *idx, u8 *s, const u8 *list) {
    *idx = Kw_MatchTable(&s, list, gCharTableUpper);
    return s;
}

/* @ 0x80000BC8 */
u8 *Kw_MatchExactAt(s32 *idx, u8 *s, const u8 *list) {
    *idx = Kw_MatchExact(&s, list);
    return s;
}

/* @ 0x80000C40 - CRC-16/CCITT, initial 0xFFFF, result inverted.
 * len is a do-while count: 0 means 2^32 bytes, as in the original. */
u16 Crc16_Ccitt(const u8 *data, s32 len) {
    u32 crc = 0xFFFF;
    s32 bit;

    do {
        crc ^= (u32)*data++ << 8;
        for (bit = 7; bit >= 0; bit--) {
            if (crc & 0x8000) {
                crc = 0x1021 ^ (crc * 2);
            } else {
                crc *= 2;
            }
        }
        len--;
    } while (len != 0);
    return ~crc & 0xFFFF;
}

/* @ 0x80000CA0 */
void Str_Stub0(void) {
}

static const char sHexLower[16] = "0123456789abcdef";   /* 0x80038350 */
static const char sHexUpper[16] = "0123456789ABCDEF";   /* 0x80038360 */
static const u32 sDecPowers[] = {                       /* 0x80038370 */
    1000000000, 100000000, 10000000, 1000000, 100000, 10000, 1000, 100, 10, 1, 0,
};

/* Write v in hex into buf with leading zeros suppressed; return the end. */
static char *fmt_hex(char *buf, u32 v, const char *digits) {
    char *p = buf;
    s32 started = 0;
    s32 shift;

    for (shift = 28; shift >= 0; shift -= 4) {
        u32 d = (v >> shift) & 0xF;
        if (d != 0) {
            started = -1;
        }
        *p = digits[d];
        p -= started;
    }
    p[started + 1] = 0;
    return p + (started + 1);
}

/* @ 0x80000CB0
 * Minimal sprintf. Supports %s %c %d %x %X %b with '0' and '-' flags and a
 * width. "\%" prints a literal '%'. Returns a pointer to the trailing NUL.
 * Quirks kept from the original:
 *  - width parsing accepts any character below '0' as a "digit";
 *  - an unknown conversion prints '%' and restarts after it;
 *  - '-' (left justify) with '0' pads with spaces. */
char *vd_vsprintf(char *out, const char *fmt, va_list ap) {
    char buf[40];
    const u8 *f = (const u8 *)fmt;
    u32 c;

    c = *f++;
    while (c != 0) {
        if (c == '\\' && *f == '%') {
            *out++ = '%';
            f++;
        } else if (c != '%') {
            *out++ = (char)c;
        } else {
            const u8 *after = f;
            s32 neg = 0;
            s32 width = 0;
            u32 flags = 0;
            s32 d;
            char *str = buf;
            char *end;

            for (;;) {
                c = *f++;
                if (c == '0') {
                    flags |= 1;
                } else if (c == '-') {
                    flags |= 2;
                } else {
                    f--;
                    break;
                }
            }
            d = (s32)*f++ - '0';
            while (d < 10) {
                width = (s32)((u32)width * 10 + (u32)d);   /* wraps like the original */
                d = (s32)*f++ - '0';
            }

            switch (d + '0') {
            case 's':
                str = va_arg(ap, char *);
                if (str == NULL) {
                    /* The original reads address 0 here; with an empty string
                     * there its end pointer is NULL and nothing is printed. */
                    end = NULL;
                    break;
                }
                end = str;
                while (*end != 0) {
                    end++;
                }
                break;
            case 'x':
                end = fmt_hex(buf, va_arg(ap, u32), sHexLower);
                break;
            case 'X':
                end = fmt_hex(buf, va_arg(ap, u32), sHexUpper);
                break;
            case 'd': {
                u32 v = va_arg(ap, u32);
                const u32 *pw = sDecPowers;
                u32 div;
                char *p = buf;
                s32 started = 0;

                if ((s32)v < 0) {
                    v = -v;
                    neg = 1;
                }
                div = *pw++;
                while (div != 0) {
                    u32 q = v / div;
                    if (q != 0) {
                        started = -1;
                    }
                    *p = (char)(q + '0');
                    p -= started;
                    v -= q * div;
                    div = *pw++;
                }
                p[started + 1] = 0;
                end = p + (started + 1);
                break;
            }
            case 'b': {
                u32 v = va_arg(ap, u32);
                char *p = buf;
                u32 started = 0;
                s32 bit;

                for (bit = 31; bit >= 0; bit--) {
                    u32 b = (v >> bit) & 1;
                    started |= b;
                    *p = (char)(b + '0');
                    p += started;
                }
                end = p - (started - 1);
                *end = 0;
                break;
            }
            case 'c':
                buf[0] = (char)(u8)va_arg(ap, u32);
                buf[1] = 0;
                end = buf + 1;
                break;
            default:
                end = NULL;
                f = after;
                *out++ = '%';
                break;
            }

            if (end != NULL) {
                char fill = ' ';
                s32 zeros = 0;
                s32 right = 0;
                s32 pad = (s32)((u32)width - (u32)((s32)(end - str) + neg));
                char ch;

                if (pad > 0) {
                    if (flags != 0) {
                        if (!(flags & 2)) {
                            zeros = pad;
                            fill = '0';
                        } else {
                            right = pad;
                        }
                    } else {
                        while (pad-- != 0) {
                            *out++ = ' ';
                        }
                    }
                }
                *out = '-';     /* kept only when neg */
                out += neg;
                while (zeros-- != 0) {
                    *out++ = fill;
                }
                do {
                    ch = *str++;
                    *out++ = ch;
                } while (ch != 0);
                out--;
                while (right-- != 0) {
                    *out++ = fill;
                }
            }
        }
        c = *f++;
    }
    *out = 0;
    return out;
}

/* @ 0x80001068 */
char *vd_sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    char *ret;

    va_start(ap, fmt);
    ret = vd_vsprintf(out, fmt, ap);
    va_end(ap);
    return ret;
}

/* @ 0x80005D40 - copy text, keeping EUC-JP pairs (0xA0-0xFE) and printable
 * ASCII; anything else becomes '.'. Returns a pointer to the NUL. */
char *Str_SanitizeEuc(char *out, const u8 *src, s32 len) {
    while (len != 0) {
        u32 c = *src++;
        if (c < 0x80) {
            if ((c < 0x20) | (c == 0x7F)) {
                c = '.';
            }
        } else if (len != 1 && (u8)(c + 0x60) < 0x5F && (u8)(*src + 0x60) < 0x5F) {
            out[0] = (char)c;
            out[1] = (char)*src++;
            out += 2;
            len -= 2;
            continue;
        } else {
            c = '.';
        }
        *out++ = (char)c;
        len--;
    }
    *out = 0;
    return out;
}

/* @ 0x80005DF4 */
void Str_HexDumpGrouped(char *out, const u8 *src, s32 len, s32 group) {
    while (len > 0) {
        s32 n = group;
        if (n == 0) {
            n = 1;
        }
        while (n-- != 0) {
            u32 b = *src++;
            len--;
            out = vd_sprintf(out, "%02x", b);
        }
        out = vd_sprintf(out, " ");
    }
    *out = 0;
}

/* @ 0x80005EB0 */
void Str_Stub1(void) {
}

/* @ 0x80005EC0 */
void Str_SanitizeAscii(char *out, const u8 *src, s32 len) {
    while (len != 0) {
        u32 c = *src++;
        if (c - 0x20 >= 0x5F) {
            c = '.';
        }
        *out++ = (char)c;
        len--;
    }
    *out = 0;
}

/* @ 0x80005EF8 */
void Str_HexDumpBytes(char *out, const u8 *src, s32 len) {
    while (len > 0) {
        u32 b = *src++;
        len--;
        out = vd_sprintf(out, "%02x ", b);
    }
}

/* @ 0x80005F50 */
void Str_HexDumpWords(char *out, const s32 *src, s32 len) {
    while (len > 0) {
        s32 w = *src++;
        len -= 4;
        out = vd_sprintf(out, "%08x ", w);
    }
}

/* @ 0x80005FA8 */
void Str_Stub2(void) {
}

/* @ 0x80005FB0 */
void Str_Stub3(void) {
}
