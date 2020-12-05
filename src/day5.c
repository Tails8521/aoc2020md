#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day5.h>
#include <utils.h>

void day5() {
    u16 line = 1;
    char buf[200];
    char result[30];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    const u8 *cursor = DAY5_INPUT;
    u16 current = 0;
    u16 highest = 0;
    u8 listed_seats[1024] = {0};
    while (cursor < DAY5_INPUT + sizeof DAY5_INPUT) {
        for (s16 shift = 9; shift >= 3; shift--) {
            if (*cursor++ == 'B') {
                current |= 1 << shift;
            }
        }
        for (s16 shift = 2; shift >= 0; shift--) {
            if (*cursor++ == 'R') {
                current |= 1 << shift;
            }
        }
        listed_seats[current] = TRUE;
        if (current > highest) {
            highest = current;
        }
        current = 0;
        cursor++; // newline
    }
    uintToStr(getTimer(0, FALSE) / SUBTICKPERMILLISECOND, result, 1);
    sprintf(buf, "Solved part 1 in %s ms", result);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: Highest seat ID is %u", highest);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part2...", 1, line++);
    s16 my_seat = -1;
    for (u16 i = 8; i < 1024 - 8; i++) { // ignore the first and last row of seats
        if (! listed_seats[i] && listed_seats[i + 1] && listed_seats[i - 1]) {
            my_seat = i;
            break;
        }
    }
    uintToStr(getTimer(0, FALSE) / SUBTICKPERMILLISECOND, result, 1);
    sprintf(buf, "Solved part 2 in %s ms", result);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: My seat has ID %d", my_seat);
    drawText(buf, 1, line++);
    drawText("Day 5 done, press START to go back", 1, line + 1);
}