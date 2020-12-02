#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day2.h>
#include <utils.h>

void day2() {
    u16 line = 1;
    char buf[200];
    char result[20];
    startTimer(0);
    drawText("Solving day 2...", 1, line++);
    const char *cursor = DAY2_INPUT;
    u16 valid_password_count_part1 = 0;
    u16 valid_password_count_part2 = 0;
    while (cursor < DAY2_INPUT + sizeof DAY2_INPUT) {
        u16 occurences = 0;
        u16 lower_bound = skip_atoi(&cursor);
        cursor++; // -
        u16 upper_bound = skip_atoi(&cursor);
        cursor++; // <space>
        u8 letter = *cursor++;
        cursor += 2; // :<space>
        if ((cursor[lower_bound - 1] == letter) ^ (cursor[upper_bound - 1] == letter)) {
            valid_password_count_part2++;
        }
        while (TRUE) {
            u8 current_letter = *cursor++;
            if (current_letter == letter) {
                occurences++;
            } else if (current_letter == '\n') {
                if (occurences >= lower_bound && occurences <= upper_bound) {
                    valid_password_count_part1++;
                }
                break;
            }
        }
    }
    uintToStr(getTimer(0, FALSE) / SUBTICKPERMILLISECOND, result, 1);
    sprintf(buf, "Solved day 2 in %s ms", result);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u valid passwords", valid_password_count_part1);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: %u valid passwords", valid_password_count_part2);
    drawText(buf, 1, line++);
    drawText("Day 2 done, press START to go back", 1, line + 1);
}