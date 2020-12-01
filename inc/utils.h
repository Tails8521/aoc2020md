#ifndef _UTILS_H_
#define _UTILS_H_

#include <genesis.h>

#define SUBTICKPERMILLISECOND    77

void drawText(const char *str, u16 x, u16 y);
void clearText(u16 x, u16 y, u16 w);
u16 skip_atoi(const char **s);

#endif // _UTILS_H_