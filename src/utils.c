#include <genesis.h>

void drawText(const char *str, u16 x, u16 y) {
    SYS_disableInts();
    VDP_drawText(str, x, y);
    SYS_enableInts();
}

void clearText(u16 x, u16 y, u16 w) {
    SYS_disableInts();
    VDP_clearText(x, y, w);
    SYS_enableInts();
}

u16 skip_atoi(const char **s) {
    u16 i = 0;
    while(isdigit(**s)) {
        i = (i * 10) + *((*s)++) - '0';
    }
    return i;
}
