#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day3.h>
#include <utils.h>

void day3() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    const u8 *cursor = DAY3_INPUT;
    u16 tree_count = 0;
    u16 line_width = 0;
    u16 pos_x = 0;
    // first, determine line width
    while (TRUE) {
        if (cursor[line_width] == '\n') {
            break;
        }
        line_width++;
    }
    // solve part 1
    while (cursor < DAY3_INPUT + sizeof DAY3_INPUT) {
        if (*cursor == '#') {
            tree_count++;
        }
        pos_x += 3;
        if (pos_x >= line_width) {
            pos_x -= line_width;
            cursor += 4; // 3 + line_width - line_width + 1 ('\n')
        } else {
            cursor += line_width + 4; // line_width + 3 + 1 ('\n')
        }
    }
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u trees encountered", tree_count);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    // solve part 2
    const u16 SLOPES_X[] = {1, 5, 7, 1};
    const u16 SLOPES_Y[] = {1, 1, 1, 2};
    u64 multiplied_trees = tree_count;
    for (u16 i = 0; i < 4; i++) {
        tree_count = 0;
        pos_x = 0;
        u16 slope_x = SLOPES_X[i];
        u16 slope_y = SLOPES_Y[i];
        cursor = DAY3_INPUT;
        while (cursor < DAY3_INPUT + sizeof DAY3_INPUT) {
            if (*cursor == '#') {
                tree_count++;
            }
            pos_x += slope_x;
            if (pos_x >= line_width) {
                pos_x -= line_width;
                cursor += (line_width + 1) * (slope_y - 1) + slope_x + 1;
            } else {
                cursor += (line_width + 1) * slope_y + slope_x;
            }
        }
        multiplied_trees *= tree_count;
    }
    sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(multiplied_trees, u64_result);
    sprintf(buf, "Part 2: %s multiplied trees", u64_result);
    drawText(buf, 1, line++);
    drawText("Day 3 done, press START to go back", 1, line + 1);
}