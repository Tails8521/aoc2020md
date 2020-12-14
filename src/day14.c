#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day14.h>
#include <utils.h>
#include <hashmap.h>

#define BIT_COUNT 36

typedef struct {
    u16 parts[3];
} u48;

unsigned long long unpack_u48(u48 value) {
    unsigned long long ret = 0;
    ret += (unsigned long long)value.parts[0] << 32;
    ret += (unsigned long long)value.parts[1] << 16;
    ret += (unsigned long long)value.parts[2];
    return ret;
}

u48 pack_u48(unsigned long long value) {
    u48 ret;
    ret.parts[0] = value >> 32;
    ret.parts[1] = value >> 16;
    ret.parts[2] = value;
    return ret;
}

typedef struct {
    u48 index;
    u48 value;
} MemoryCell;

int memory_cell_compare(const void *a, const void *b, void *udata) {
    const MemoryCell *elm1 = a;
    const MemoryCell *elm2 = b;
    return unpack_u48(elm1->index) - unpack_u48(elm2->index);
}

uint64_t memory_cell_hash(const void *item, uint64_t seed0, uint64_t seed1) {
    const MemoryCell *elm = item;
    return unpack_u48(elm->index);
}

bool memory_cell_sum(const void *item, void *udata) {
    const MemoryCell *elm = item;
    unsigned long long *total = udata;
    *total += unpack_u48(elm->value);
    return true;
}

void day14() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    const u8 *cursor = DAY14_INPUT;
    unsigned long long and_mask;
    unsigned long long or_mask;
    struct hashmap *memory = hashmap_new(sizeof (MemoryCell), 2048, 0, 0, memory_cell_hash, memory_cell_compare, NULL);
    while (cursor < DAY14_INPUT + sizeof DAY14_INPUT) {
        if (cursor[1] == 'e') { // mem
            cursor += 4; // "mem["
            u16 index = skip_atoi(&cursor);
            cursor += 4; // "] = "
            unsigned long long value = skip_atoi_u64(&cursor);
            value &= and_mask;
            value |= or_mask;
            hashmap_set(memory, &(MemoryCell) {
                .index = pack_u48(index),
                .value = pack_u48(value),
            });
        } else { // mask
            cursor += 7; // "mask = "
            and_mask = 0;
            or_mask = 0;
            for (s16 shift = BIT_COUNT - 1; shift >= 0; shift--) {
                and_mask <<= 1;
                or_mask <<= 1;
                if (*cursor == '1') {
                    and_mask++;
                    or_mask++;
                } else if (*cursor == 'X') {
                    and_mask++;
                }
                cursor++;
            }
        }
        cursor++; // newline
    }
    
    unsigned long long total = 0;
    hashmap_scan(memory, &memory_cell_sum, &total);
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(total, u64_result);
    sprintf(buf, "Part 1: The sum is %s", u64_result);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    total = 0;
    for (u16 partition_index = 0; partition_index < 4096; partition_index++) {
        sprintf(buf, "Partition %u/%u", partition_index + 1, 4096);
        drawText(buf, 1, line);
        hashmap_clear(memory, true);
        // This works for my input, may have to be adjusted for other inputs to avoid OOM when adding to the hashmap
        // Larger masks (and thus larger partition count) are safer, but obviously take way longer to complete
        const unsigned long long partition_mask = 0x0000000000fff000;
        unsigned long long partition = (unsigned long long)partition_index << 12;
        cursor = DAY14_INPUT;
        u16 addr_per_line;
        u16 x_count;
        u16 x_pos[BIT_COUNT];
        while (cursor < DAY14_INPUT + sizeof DAY14_INPUT) {
            if (cursor[1] == 'e') { // mem
                cursor += 4; // "mem["
                unsigned long long index_base = skip_atoi(&cursor);
                cursor += 4; // "] = "
                unsigned long long value = skip_atoi_u64(&cursor);
                index_base |= or_mask;
                for (u16 permutation = 0; permutation < addr_per_line; permutation++) {
                    unsigned long long index = index_base;
                    for (u16 x = 0; x < x_count; x++) {
                        unsigned long long current_bit = permutation & (1 << x);
                        if (current_bit) {
                            index |= (1ULL << x_pos[x]);
                        } else {
                            index &= ~(1ULL << x_pos[x]);
                        }
                    }
                    if ((index & partition_mask) == partition) {
                        hashmap_set(memory, &(MemoryCell) {
                            .index = pack_u48(index),
                            .value = pack_u48(value),
                        });
                        if (hashmap_oom(memory)) {
                            sprintf(buf, "OOM at pos %lu", (u32) (cursor - DAY14_INPUT));
                            SYS_die(buf);
                        }
                    }
                }
            } else { // mask
                cursor += 7; // "mask = "
                or_mask = 0;
                x_count = 0;
                for (s16 shift = BIT_COUNT - 1; shift >= 0; shift--) {
                    and_mask <<= 1;
                    or_mask <<= 1;
                    if (*cursor == '1') {
                        or_mask++;
                    } else if (*cursor == 'X') {
                        x_pos[x_count++] = shift;
                    }
                    cursor++;
                }
                addr_per_line = 1 << x_count;
            }
            cursor++; // newline
        }
        unsigned long long partition_total = 0;
        hashmap_scan(memory, &memory_cell_sum, &partition_total);
        total += partition_total;
        // KLog_U1("", hashmap_count(memory));
    }
    sprintf(buf, "Solved part 2 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    u64ToStr(total, u64_result);
    sprintf(buf, "Part 2: The sum is %s", u64_result);
    drawText(buf, 1, line++);
    hashmap_free(memory);
    drawText("Day 14 done, press START to go back", 1, line + 1);
}