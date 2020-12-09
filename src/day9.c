#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day8.h>
#include <utils.h>

#define PREVIOUS_SIZE 25

void day9() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Parsing day 9 input...", 1, line++);
    u16 input_size = 0;
    for (const u8 *cursor = DAY9_INPUT; cursor < DAY9_INPUT + sizeof DAY9_INPUT; cursor++) {
        if (*cursor == '\n') {
            input_size++;
        }
    }
    unsigned long long *input = MEM_alloc(input_size * sizeof (unsigned long long));
    const u8 *cursor = DAY9_INPUT;
    for (u16 idx = 0; idx < input_size; idx++) {
        while (!isdigit(*cursor)) {
            cursor++;
        }
        unsigned long long parsed_number = skip_atoi_u64(&cursor);
        input[idx] = parsed_number;
    }
    sprintf(buf, "Parsed day 9 input in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);

    // solve
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    unsigned long long previous[PREVIOUS_SIZE];
    u16 previous_offset = 0;
    for (u16 i = 0; i < PREVIOUS_SIZE; i++) {
        previous[i] = input[i];
    }
    
    unsigned long long invalid_number = 0;
    for (u16 i = PREVIOUS_SIZE; i < input_size; i++) {
        for (u16 x = 0; x < PREVIOUS_SIZE; x++) {
            for (u16 y = 0; y < PREVIOUS_SIZE; y++) {
                if (input[i] == previous[x] + previous[y]) {
                    previous[previous_offset++] = input[i];
                    if (previous_offset == PREVIOUS_SIZE) {
                        previous_offset = 0;
                    }
                    goto valid;
                }
            }
        }
        invalid_number = input[i];
        break;
valid:
        ;
    }
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(invalid_number, u64_result);
    sprintf(buf, "Part 1: %s", u64_result);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    u16 index_start = 0;
    u16 index_end = 0; // inclusive
    for (u16 i_sum_start = 0; i_sum_start < input_size; i_sum_start++) {
        unsigned long long acc = input[i_sum_start];
        for (u16 i_sum_end = i_sum_start + 1; i_sum_end < input_size; i_sum_end++) {
            acc += input[i_sum_end];
            if (acc > invalid_number) {
                // overshot it, try with a next start index
                break;
            }
            if (acc == invalid_number) {
                index_start = i_sum_start;
                index_end = i_sum_end;
                goto exit;
            }
        }
    }
exit:
    ;
    unsigned long long min = 0xffffffffffffffff;
    unsigned long long max = 0;
    for (u16 i = index_start; i <= index_end; i++) {
        if (input[i] > max) {
            max = input[i];
        }
        if (input[i] < min) {
            min = input[i];
        }
    }
    sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(min + max, u64_result);
    sprintf(buf, "Part 2: %s", u64_result);
    drawText(buf, 1, line++);
    MEM_free(input);
    drawText("Day 8 done, press START to go back", 1, line + 1);
}