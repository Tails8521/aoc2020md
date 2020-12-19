#ifndef _UTILS_H_
#define _UTILS_H_

#include <genesis.h>

#define SUBTICKPERMILLISECOND    77
#define ishexdigit(c)      (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f'))

typedef unsigned long long u64;

void drawText(const char *str, u16 x, u16 y);
void clearText(u16 x, u16 y, u16 w);
u16 skip_atoi(const u8 **s);
u64 skip_atoi_u64(const u8 **s);
void u64ToStr(u64 value, char *buffer);
s16 memcmp(const u8 *elm1, const u8 *elm2, u16 len);
void *calloc(u16 elm_count, u16 elm_size);

#endif // _UTILS_H_