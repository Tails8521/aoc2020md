#ifndef _UTILS_H_
#define _UTILS_H_

#include <genesis.h>

#define SUBTICKPERMILLISECOND    77

#define ishexdigit(c)      (((c) >= '0' && (c) <= '9') || ((c) >= 'a' && (c) <= 'f'))

void drawText(const char *str, u16 x, u16 y);
void clearText(u16 x, u16 y, u16 w);
u16 skip_atoi(const u8 **s);
void u64ToStr(unsigned long long value, char *buffer);

#endif // _UTILS_H_