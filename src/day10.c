#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day10.h>
#include <utils.h>
#include <hashmap.h>

typedef struct {
    s16 jolt;
    u64 value;
} Path;

int path_compare(const void *a, const void *b, void *udata) {
    const Path *elm1 = a;
    const Path *elm2 = b;
    return elm1->jolt - elm2->jolt;
}

uint64_t path_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const Path *elm = item;
    return (u16)elm->jolt; // cast to avoid sign extension
}

void day10() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Parsing day 10 input...", 1, line++);
    u16 input_size = 0;
    for (const u8 *cursor = DAY10_INPUT; cursor < DAY10_INPUT + sizeof DAY10_INPUT; cursor++) {
        if (*cursor == '\n') {
            input_size++;
        }
    }
    u16 *input = MEM_alloc(input_size * sizeof (u16));
    const u8 *cursor = DAY10_INPUT;
    for (u16 idx = 0; idx < input_size; idx++) {
        while (!isdigit(*cursor)) {
            cursor++;
        }
        u16 parsed_number = skip_atoi(&cursor);
        input[idx] = parsed_number;
    }
    sprintf(buf, "Parsed day 10 input in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Sorting 10 input...", 1, line++);
    qsort_u16(input, 0, input_size - 1);
    sprintf(buf, "Sorted day 10 input in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    
    // solve
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    u16 count_1 = 0;
    u16 count_3 = 0;
    u16 previous = 0;
    for (u16 i = 0; i < input_size; i++) {
        u16 diff = input[i] - previous;
        previous = input[i];
        if (diff == 1) {
            count_1++;
        } else if (diff == 3) {
            count_3++;
        }
    }
    count_3++; // device's built-in adapter
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u", count_1 * count_3);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    struct hashmap *paths = hashmap_new(sizeof (Path), input_size * 2, 0, 0, path_hash, path_compare, NULL);
    hashmap_set(paths, &(Path) {
        .jolt = -1,
        .value = 1,
    });
    u64 last_sum = 0;
    for (u16 jolt_index = 0; jolt_index < input_size; jolt_index++) {
        u16 jolt = input[jolt_index];
        u64 sum = 0;
        Path* len_1_path = hashmap_get(paths, &(Path) {
            .jolt = jolt - 1,
        });
        if (len_1_path != NULL) {
            sum += len_1_path->value;
        }
        Path* len_2_path = hashmap_get(paths, &(Path) {
            .jolt = jolt - 2,
        });
        if (len_2_path != NULL) {
            sum += len_2_path->value;
        }
        Path* len_3_path = hashmap_get(paths, &(Path) {
            .jolt = jolt - 3,
        });
        if (len_3_path != NULL) {
            sum += len_3_path->value;
        }
        hashmap_set(paths, &(Path) {
            .jolt = jolt,
            .value = sum,
        });
        last_sum = sum;
    }
    sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(last_sum, u64_result);
    sprintf(buf, "Part 2: %s", u64_result);
    drawText(buf, 1, line++);
    hashmap_free(paths);
    MEM_free(input);
    drawText("Day 10 done, press START to go back", 1, line + 1);
}