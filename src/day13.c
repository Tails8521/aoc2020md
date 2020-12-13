#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day13.h>
#include <utils.h>

typedef struct {
    u16 id;
    u16 offset;
} BusLine;

void day13() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    const u8 *cursor = DAY13_INPUT;
    u32 earliest_departure = skip_atoi_u64(&cursor);
    cursor++; // newline
    const u8 *bus_data_start = cursor;
    u16 bus_count = 0;
    u32 lowest_time_to_wait = MAX_U32;
    u32 lowest_time_bus_id = 0;
    while (cursor < DAY13_INPUT + sizeof DAY13_INPUT) {
        if (*cursor == 'x') {
            cursor++;
        } else {
            bus_count++;
            u16 bus_id = skip_atoi(&cursor);
            u32 time_to_wait = bus_id - earliest_departure % bus_id;
            if (time_to_wait < lowest_time_to_wait) {
                lowest_time_to_wait = time_to_wait;
                lowest_time_bus_id = bus_id;
            }
        }
        cursor++;
    }
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %lu", lowest_time_bus_id * lowest_time_to_wait);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    BusLine *lines = MEM_alloc(bus_count * sizeof (BusLine));
    u16 bus_idx = 0;
    u16 bus_offset = 0;
    cursor = bus_data_start;
    while (cursor < DAY13_INPUT + sizeof DAY13_INPUT) {
        if (*cursor == 'x') {
            cursor++;
        } else {
            u16 bus_id = skip_atoi(&cursor);
            lines[bus_idx++] = (BusLine) {
                .id = bus_id,
                .offset = bus_offset
            };
        }
        bus_offset++;
        cursor++;
    }
    unsigned long long timestamp = 0;
    unsigned long long increment = lines[0].id;
    u16 ok_count = 1; // no need to do anything for the first one
    while (ok_count < bus_count) {
        if ((timestamp + lines[ok_count].offset) % lines[ok_count].id == 0) {
            increment *= lines[ok_count].id;
            ok_count++;
        } else {
            timestamp += increment;
        }
    }
    
    sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(timestamp, u64_result);
    sprintf(buf, "Part 2: %s", u64_result);
    drawText(buf, 1, line++);
    MEM_free(lines);
    drawText("Day 13 done, press START to go back", 1, line + 1);
}