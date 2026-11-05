/* Vivid Dolls - string helpers: keyword matching, CRC16, sprintf, hex dumps. */
#ifndef VD_STRUTIL_H
#define VD_STRUTIL_H

#include <stdarg.h>
#include "vd_common.h"

/* Identifier character tables in ROM: maps a character to its folded form,
 * or 0 for characters that end a word. */
extern u8 gCharTableLower[256];   /* 0x80038150: A-Z folded to a-z */
extern u8 gCharTableUpper[256];   /* 0x80038250: a-z kept, A-Z kept */

/* Keyword lists are NUL-separated strings ending in an empty string.
 * The matchers return the index of the keyword found at *pp, or -1. */
s32 Kw_MatchTable(u8 **pp, const u8 *list, const u8 *tbl);      /* 0x800007A0 */
s32 Kw_MatchLower(u8 **pp, const u8 *list);                     /* 0x80000858 */
s32 Kw_MatchUpper(u8 **pp, const u8 *list);                     /* 0x80000918 */
s32 Kw_MatchExact(u8 **pp, const u8 *list);                     /* 0x800009D8 */
u8 *Kw_MatchLowerAt(s32 *idx, u8 *s, const u8 *list);           /* 0x80000A40 */
u8 *Kw_MatchUpperAt(s32 *idx, u8 *s, const u8 *list);           /* 0x80000B04 */
u8 *Kw_MatchExactAt(s32 *idx, u8 *s, const u8 *list);           /* 0x80000BC8 */
u16 Crc16_Ccitt(const u8 *data, s32 len);                       /* 0x80000C40 */
void Str_Stub0(void);                                           /* 0x80000CA0 */
char *vd_vsprintf(char *out, const char *fmt, va_list ap);      /* 0x80000CB0 */
char *vd_sprintf(char *out, const char *fmt, ...);              /* 0x80001068 */

char *Str_SanitizeEuc(char *out, const u8 *src, s32 len);       /* 0x80005D40 */
void Str_HexDumpGrouped(char *out, const u8 *src, s32 len, s32 group); /* 0x80005DF4 */
void Str_Stub1(void);                                           /* 0x80005EB0 */
void Str_SanitizeAscii(char *out, const u8 *src, s32 len);      /* 0x80005EC0 */
void Str_HexDumpBytes(char *out, const u8 *src, s32 len);       /* 0x80005EF8 */
void Str_HexDumpWords(char *out, const s32 *src, s32 len);      /* 0x80005F50 */
void Str_Stub2(void);                                           /* 0x80005FA8 */
void Str_Stub3(void);                                           /* 0x80005FB0 */

#endif
