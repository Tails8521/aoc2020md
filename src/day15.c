#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day15.h>
#include <utils.h>
#include <hashmap.h>

#define YEAR 2020

typedef struct {
    u16 number;
    u16 turn_difference;
    u16 last_turn_spoken;
} SpokenNumber;

int spoken_number_compare(const void *a, const void *b, void *udata) {
    const SpokenNumber *elm1 = a;
    const SpokenNumber *elm2 = b;
    return elm1->number - elm2->number;
}

uint64_t spoken_number_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const SpokenNumber *elm = item;
    return elm->number;
}

void day15() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving day 15 part 1...", 1, line++);
    u16 starting_numbers_count = 1;
    for (const u8 *cursor = DAY15_INPUT; cursor < DAY15_INPUT + sizeof DAY15_INPUT; cursor++) { // the input is ended by a newline
        if (*cursor == ',') {
            starting_numbers_count++;
        }
    }
    struct hashmap *spoken_numbers = hashmap_new(sizeof (SpokenNumber), 2048, 0, 0, spoken_number_hash, spoken_number_compare, NULL);
    u16 *starting_numbers = MEM_alloc(starting_numbers_count * sizeof (u16));
    const u8 *cursor = DAY15_INPUT;
    u16 *starting_number = starting_numbers;
    while (cursor < DAY15_INPUT + sizeof DAY15_INPUT) {
        *starting_number++ = skip_atoi(&cursor);
        cursor++;
    }
    u16 spoken_number;
    for (u16 turn = 0; turn < YEAR; turn++) {
        if (turn < starting_numbers_count) {
            spoken_number = starting_numbers[turn];
        } else {
            SpokenNumber *previously_spoken_number = hashmap_get(spoken_numbers, &(SpokenNumber) {
                .number = spoken_number,
            });
            spoken_number = previously_spoken_number->turn_difference;
        }
        SpokenNumber *previously_spoken_number = hashmap_get(spoken_numbers, &(SpokenNumber) {
            .number = spoken_number,
        });
        if (previously_spoken_number) {
            hashmap_set(spoken_numbers, &(SpokenNumber) {
                .number = spoken_number,
                .turn_difference = turn - previously_spoken_number->last_turn_spoken,
                .last_turn_spoken = turn,
            });
        } else {
            hashmap_set(spoken_numbers, &(SpokenNumber) {
                .number = spoken_number,
                .turn_difference = 0,
                .last_turn_spoken = turn,
            });
        }
    }
    sprintf(buf, "Solved day 15 part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: The number is %u", spoken_number);
    drawText(buf, 1, line++);
    drawText("Part 2: Elves are demanding too much", 1, line++);
    drawText("This machine only has 64KB of RAM", 1, line++);
    hashmap_free(spoken_numbers);
    MEM_free(starting_numbers);
    drawText("Day 15 done, press START to go back", 1, line + 1);
}