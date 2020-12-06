#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day6.h>
#include <utils.h>

void day6() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving day 6...", 1, line++);
    u32 part1_current_group_replies_bitset = 0;
    u32 part2_current_group_replies_bitset = 0b00000011111111111111111111111111; // 26 letters
    u32 part2_current_person_replies_bitset = 0;
    u16 part1_sum = 0;
    u16 part2_sum = 0;
    u16 previous_char_was_newline = FALSE;
    for (const u8 *cursor = DAY6_INPUT; cursor < DAY6_INPUT + sizeof DAY6_INPUT; cursor++) {
        if (*cursor == '\n') {
            if (previous_char_was_newline) {
                // new group
                part1_current_group_replies_bitset = 0;
                part2_sum += __builtin_popcount(part2_current_group_replies_bitset);
                part2_current_group_replies_bitset = 0b00000011111111111111111111111111; // 26 letters
            } else {
                // new person
                part2_current_group_replies_bitset &= part2_current_person_replies_bitset;
                part2_current_person_replies_bitset = 0;
            }
            previous_char_was_newline = TRUE;
        } else {
            u8 char_index = *cursor - 'a';
            u32 mask = 1 << char_index;
            if (!(part1_current_group_replies_bitset & mask)) {
                part1_current_group_replies_bitset |= mask;
                part1_sum++;
            }
            part2_current_person_replies_bitset |= mask;
            previous_char_was_newline = FALSE;
        }
    }
    // treat the last group for part 2
    part2_sum += __builtin_popcount(part2_current_group_replies_bitset);
    sprintf(buf, "Solved day 6 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: The sum is %u", part1_sum);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: The sum is %u", part2_sum);
    drawText(buf, 1, line++);
    drawText("Day 6 done, press START to go back", 1, line + 1);
}