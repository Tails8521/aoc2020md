#include <genesis.h>
#include <resources.h>
#include <utils.h>
#include <day1.h>
#include <day2.h>
#include <day3.h>
#include <day4.h>
#include <day5.h>
#include <day6.h>
#include <day7.h>
#include <day8.h>
#include <day9.h>
#include <day10.h>
#include <day11.h>
#include <day12.h>
#include <day13.h>
#include <day14.h>
#include <day15.h>
#include <day16.h>
#include <day17.h>
#include <day18.h>
#include <day19.h>
#include <day20.h>

#define PLANE_W 64
#define PLANE_H 32

typedef enum {
    NONE = 0,
    DOWN,
    UP,
    START
} KeyPressed;

typedef enum {
    MENU = 0,
    DAY
} ProgramState;

s16 horizontal_scroll_array[224];
s16 vertical_scroll_array[20];
u16 vertical_scroll_speedup_threshold[20];
s16 current_initial_horizontal_scroll = 8;
s16 bottom_text_horizontal_scroll_array[8];
u16 controller_state;
u16 controller_changed;

s16 selected_item;
KeyPressed key_pressed;
ProgramState program_state;

void joyCallback(u16 joy, u16 changed, u16 state);
void setScroll();
void draw_menu();
void clear_screen();


void (*const days_fcts[])() = {
    &day1,
    &day2,
    &day3,
    &day4,
    &day5,
    &day6,
    &day7,
    &day8,
    &day9,
    &day10,
    &day11,
    &day12,
    &day13,
    &day14,
    &day15,
    &day16,
    &day17,
    &day18,
    &day19,
    &day20,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
};

void joyCallback(u16 joy, u16 changed, u16 state) {
    if (joy == JOY_1) {
        controller_changed = changed;
        if (~controller_state & controller_changed & BUTTON_DOWN) {
            key_pressed = DOWN;
        } else if (~controller_state & controller_changed & BUTTON_UP) {
            key_pressed = UP;
        } else if (~controller_state & controller_changed & BUTTON_START) {
            key_pressed = START;
        }
        controller_state = state;
    }
}

void setScroll() {
    for (u16 i = 223; i >= 1; i--) {
        horizontal_scroll_array[i] = horizontal_scroll_array[i-1];
    }
    u16 rand = random() % 4;
    if (rand == 0) {
        current_initial_horizontal_scroll++;
    } else if (rand == 1) {
        current_initial_horizontal_scroll--;
    }
    if (current_initial_horizontal_scroll < 0) {
        current_initial_horizontal_scroll = 0;
    }
    if (current_initial_horizontal_scroll > 15) {
        current_initial_horizontal_scroll = 15;
    }
    horizontal_scroll_array[0] = current_initial_horizontal_scroll;
    VDP_setHorizontalScrollLine(BG_A, 0, horizontal_scroll_array, 224, DMA);
    for (u16 i = 0; i < 20; i++) {
        if (random() % 2 == 0) {
            vertical_scroll_speedup_threshold[i] += 8192;
        } else {
            vertical_scroll_speedup_threshold[i] -= 8192;
        }
        vertical_scroll_array[i]--;
        if (vertical_scroll_array[i] % 32 == 0) {
            u16 tile_y_to_reload = (vertical_scroll_array[i] + 224) / 8;
            VDP_setTileMapXY(BG_A,TILE_ATTR_FULL(PAL1, 0, 0, 0, random() % 8 < 6), i * 2, tile_y_to_reload);
        }
        if (random() >= vertical_scroll_speedup_threshold[i]) {
            vertical_scroll_array[i]--;
            if (vertical_scroll_array[i] % 32 == 0) {
                u16 tile_y_to_reload = (vertical_scroll_array[i] + 224) / 8;
                VDP_setTileMapXY(BG_A,TILE_ATTR_FULL(PAL1, 0, 0, 0, random() % 8 < 6), i * 2, tile_y_to_reload);
            }
        }
    }
    VDP_setVerticalScrollTile(BG_A, 0, vertical_scroll_array, 20, DMA);
    s16 bottom_text_horizontal_scroll = --bottom_text_horizontal_scroll_array[0];
    for (u16 i = 1; i < 8; i++) {
        bottom_text_horizontal_scroll_array[i] = bottom_text_horizontal_scroll;
    }
    VDP_setHorizontalScrollLine(BG_B, 216, bottom_text_horizontal_scroll_array, 8, DMA);
}

void draw_menu() {
    for (u16 i = 0; i < 25; i++) {
        char buf[70];
        if (days_fcts[i]) {
            sprintf(buf, "Day %2u", i + 1);
        } else {
            sprintf(buf, "Day %2u (not yet)", i + 1);
        }
        drawText(buf, 10, i + 1);
        drawText("(Part 1 only)", 17, 15);
    }
}

void clear_screen() {
    SYS_disableInts();
    VDP_clearTextArea(0, 0, 40, 27);
    SYS_enableInts();
}

int main() {
    VDP_setEnable(FALSE);
    SYS_disableInts();
    // SYS_showFrameLoad();
    VDP_setTextPlane(BG_B);
    VDP_setTextPriority(0);
    VDP_setTextPalette(PAL1);
    VDP_setPlaneSize(PLANE_W, PLANE_H, TRUE);
    VDP_clearPlane(BG_A, TRUE);
    VDP_clearPlane(BG_B, TRUE);
    VDP_setScrollingMode(HSCROLL_LINE, VSCROLL_2TILE);
    VDP_setHorizontalScrollLine(BG_A, 0, horizontal_scroll_array, 224, DMA);
    VDP_setVerticalScrollTile(BG_A, 0, vertical_scroll_array, 20, DMA);
    VDP_setHorizontalScrollLine(BG_B, 0, horizontal_scroll_array, 224, DMA);
    VDP_setVerticalScrollTile(BG_B, 0, vertical_scroll_array, 20, DMA);
    VDP_loadTileSet(snowflake_image.tileset, 1, DMA);
    VDP_setPalette(PAL1, snowflake_image.palette->data);
    SPR_init();
    for (s16 y = 0; y < 224; y+= 32) {
        SPR_addSprite(&candycane_sprite, 0, y, TILE_ATTR(PAL1, 0, 0, 0));
        SPR_addSprite(&candycane_sprite, 312, y, TILE_ATTR(PAL1, 0, 0, 1));
    }
    SPR_update();
    for (u16 i = 0; i < PLANE_W * PLANE_H; i++) {
        if (i % 2 == 0 && i / PLANE_W % 4 == 0 && random() % 8 < 6) {
            VDP_setTileMapXY(BG_A,TILE_ATTR_FULL(PAL1, 0, 0, 0, 1), i % PLANE_W, i / PLANE_W);
        }
    }
    for (u16 i = 0; i < 20; i++) {
        vertical_scroll_array[i] = random();
        vertical_scroll_speedup_threshold[i] = random() / 8;
    }
    for (u16 i = 0; i < 224; i++) {
        setScroll();
    }
    VDP_drawText("Advent of Code 2020 on a Sega MegaDrive, by Tails8521 :)", 0, 27);
    SYS_setVIntCallback(&setScroll);
    JOY_setEventHandler(&joyCallback);
    draw_menu();
    VDP_setEnable(TRUE);
    SYS_enableInts();
    while(TRUE) {
        if (program_state == MENU) {
            switch (key_pressed) {
            case DOWN:
                selected_item++;
                if (selected_item > 24) {
                    selected_item = 0;
                }
                break;
            case UP:
                selected_item--;
                if (selected_item < 0) {
                    selected_item = 24;
                }
                break;
            case START:
                if (days_fcts[selected_item]) {
                    program_state = DAY;
                    key_pressed = NONE;
                    clear_screen();
                    days_fcts[selected_item]();
                    MEM_pack();
                    continue;
                }
                break;
            default:
                break;
            }
            key_pressed = NONE;
            for (u16 i = 0; i < 25; i++) {
                if (i == selected_item) {
                    drawText(">", 8, i + 1);
                } else {
                    clearText(8, i + 1, 1);
                }
            }
        }
        else {
            switch (key_pressed) {
            case START:
                program_state = MENU;
                clear_screen();
                draw_menu();
                break;
            default:
                break;
            }
            key_pressed = NONE;
        }
        SYS_doVBlankProcess();
    }
    return 0;
}

