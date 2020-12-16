#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day9.h>
#include <utils.h>
#include <hashmap.h>

#define PREVIOUS_SIZE 25

int u64_compare(const void *a, const void *b, void *udata) {
    const u64 *elm1 = a;
    const u64 *elm2 = b;
    return *elm1 - *elm2;
}

uint64_t u64_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const u64 *elm = item;
    return *elm; // identity is a hash function :p
}

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
    u64 *input = MEM_alloc(input_size * sizeof (u64));
    const u8 *cursor = DAY9_INPUT;
    for (u16 idx = 0; idx < input_size; idx++) {
        while (!isdigit(*cursor)) {
            cursor++;
        }
        u64 parsed_number = skip_atoi_u64(&cursor);
        input[idx] = parsed_number;
    }
    sprintf(buf, "Parsed day 9 input in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);

    // solve
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    u64 previous[PREVIOUS_SIZE];
    u16 previous_offset = 0;
    for (u16 i = 0; i < PREVIOUS_SIZE; i++) {
        previous[i] = input[i];
    }
    
    u64 invalid_number = 0;
    struct hashmap *values_map = hashmap_new(sizeof (u64), 128, 0, 0, u64_hash, u64_compare, NULL);
    for (u16 x = 0; x < PREVIOUS_SIZE; x++) {
        hashmap_set(values_map, &previous[x]);
    }
    for (u16 i = PREVIOUS_SIZE; i < input_size; i++) {
        for (u16 x = 0; x < PREVIOUS_SIZE; x++) {
            u64 value_to_find = input[i] - previous[x];
            if (value_to_find != previous[x] && hashmap_get(values_map, &value_to_find)) {
                hashmap_delete(values_map, &previous[previous_offset]);
                hashmap_set(values_map, &input[i]);
                previous[previous_offset++] = input[i];
                if (previous_offset == PREVIOUS_SIZE) {
                    previous_offset = 0;
                }
                goto valid;
            }
        }
        invalid_number = input[i];
        break;
valid:
        ;
    }
    hashmap_free(values_map);
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(invalid_number, u64_result);
    sprintf(buf, "Part 1: %s", u64_result);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    u16 index_start = 0;
    u16 index_end = 1;
    u64 sum = input[index_start] + input[index_end];
    while (sum != invalid_number) {
        while (sum < invalid_number) {
            sum += input[++index_end];
        }
        while (sum > invalid_number) {
            sum -= input[index_start++];
        }
    }
    u64 min = 0xffffffffffffffff;
    u64 max = 0;
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
    drawText("Day 9 done, press START to go back", 1, line + 1);
}