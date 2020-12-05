#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day1.h>
#include <utils.h>

#define YEAR 2020

void day1() {
    u16 line = 1;
    // parse 
    startTimer(0);
    drawText("Parsing day 1 input...", 1, line++);
    u16 number_count = 0;
    for (u16 i = 0; i < sizeof DAY1_INPUT; i++) {
        char current_char = DAY1_INPUT[i];
        if (current_char == '\n') {
            number_count++;
        }
    }
    u16* parsed_input = MEM_alloc(number_count * sizeof (u16));
    u8* to_find = MEM_alloc(YEAR + 1); // WORST HASHSET EVER
    memset(to_find, 0, YEAR + 1);
    u16 idx = 0;
    const u8 *cursor = DAY1_INPUT;
    while (idx < number_count) {
        while (!isdigit(*cursor)) {
            cursor++;
        }
        u16 parsed_number = skip_atoi(&cursor);
        parsed_input[idx] = parsed_number;
        if (parsed_number <= YEAR) {
            to_find[YEAR - parsed_number] = TRUE;
        }
        idx++;
    }
    char buf[200];
    sprintf(buf, "Parsed day 1 input in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);

    // solve
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    for (u16 i = 0; i < number_count; i++) {
        if (parsed_input[i] <= YEAR && to_find[parsed_input[i]]) {
            sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
            drawText(buf, 1, line++);
            u32 result = parsed_input[i] * (YEAR - parsed_input[i]);
            sprintf(buf, "%u * %u = %lu", parsed_input[i], YEAR - parsed_input[i], result);
            drawText(buf, 1, line++);
            break;
        }
    }
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    for (u16 i = 0; i < number_count; i++) {
        for (u16 j = 0; j < number_count; j++) {
            if (parsed_input[i] + parsed_input[j] <= YEAR && to_find[parsed_input[i] + parsed_input[j]]) {
                sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
                drawText(buf, 1, line++);
                u32 result = parsed_input[i] * parsed_input[j] * (YEAR - parsed_input[i] - parsed_input[j]);
                sprintf(buf, "%u * %u * %u = %lu", parsed_input[i], parsed_input[j], YEAR - parsed_input[i] - parsed_input[j], result);
                drawText(buf, 1, line++);
                goto exit;
            }
        }
    }
exit:
    MEM_free(parsed_input);
    MEM_free(to_find);
    drawText("Day 1 done, press START to go back", 1, line + 1);
}