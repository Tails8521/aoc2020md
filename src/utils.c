#include <genesis.h>
#include <utils.h>
#define U64_DIGITS 40

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

u16 skip_atoi(const u8 **s) {
    u16 i = 0;
    while(isdigit(**s)) {
        i = (i * 10) + *((*s)++) - '0';
    }
    return i;
}

u64 skip_atoi_u64(const u8 **s) {
    u64 i = 0;
    while(isdigit(**s)) {
        i = (i * 10) + *((*s)++) - '0';
    }
    return i;
}

void u64ToStr(u64 value, char *buffer) {
    char *cursor = buffer;
    do {
        *cursor++ = '0' + (value % 10);
        value /= 10;
    } while (value != 0);
    *cursor-- = '\0';
    // now reverse it
    char *front_cursor = buffer;
    while (front_cursor < cursor) {
        char temp = *front_cursor;
        *front_cursor++ = *cursor;
        *cursor-- = temp;
    }
}

s16 memcmp(const u8 *elm1, const u8 *elm2, u16 len) {
    while (len-- > 0) {
        if (*elm1++ != *elm2++) {
            return elm1[-1] < elm2[-1] ? -1 : 1;
        }
    }
    return 0;
}

void *calloc(u16 elm_count, u16 elm_size) {
    u16 size = elm_count * elm_size;
    void *ret = MEM_alloc(size);
    if (ret) {
        memset(ret, 0, size);
    }
    return ret;
}