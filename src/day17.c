#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day17.h>
#include <utils.h>

#define ROUNDS 6

u8 bit_array_get(u8 *array, u32 index) {
    u16 byte_index = index / 8;
    u16 bit_offset = index % 8;
    return array[byte_index] & (1 << bit_offset);
}

void bit_array_set(u8 *array, u32 index, u8 bool_value) {
    u16 byte_index = index / 8;
    u16 bit_offset = index % 8;
    array[byte_index] = (array[byte_index] & ~(1 << bit_offset)) | (bool_value << bit_offset);
}

void day17() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    u16 input_width = 0;
    u16 input_height = 0;
    for (const u8 *cursor = DAY17_INPUT; cursor < DAY17_INPUT + sizeof DAY17_INPUT; cursor++) {
        if (*cursor == '\n') {
            if (input_width == 0) {
                input_width = cursor - DAY17_INPUT;
            }
            input_height++;
        }
    }
    u16 x_size = input_width + ROUNDS * 2;
    u16 y_size = input_height + ROUNDS * 2;
    u16 z_size = 1 + ROUNDS * 2;
    u16 x_y_size = x_size * y_size;
    u16 x_y_z_size = x_y_size * z_size;
    u8 *buffer1 = MEM_alloc(x_y_z_size);
    u8 *buffer2 = MEM_alloc(x_y_z_size);
    u8 *front = buffer1;
    u8 *back = buffer2;
    memset(front, 0, x_y_z_size);
    memset(back, 0, x_y_z_size);
    for (u16 input_y = 0; input_y < input_height; input_y++) {
        for (u16 input_x = 0; input_x < input_width; input_x++) {
            if (DAY17_INPUT[input_y * (input_width + 1) + input_x] == '#') {
                u16 x = input_x + ROUNDS;
                u16 y = input_y + ROUNDS;
                u16 z = ROUNDS;
                front[z * y_size * x_size + y * x_size + x] = TRUE;
            }
        }
    }
    for (u16 iter = 1; iter <= ROUNDS; iter++) {
        sprintf(buf, "Iteration %u/%u", iter, ROUNDS);
        drawText(buf, 1, line);
        u16 i = 0;
        for (s16 z = 0; z < z_size; z++) {
            for (s16 y = 0; y < y_size; y++) {
                for (s16 x = 0; x < x_size; x++) {
                    u16 neighbour_count = 0;
                    for (s16 zn= -1; zn <= 1; zn++) {
                        for (s16 yn= -1; yn <= 1; yn++) {
                            for (s16 xn= -1; xn <= 1; xn++) {
                                if ((xn == 0 && yn == 0 && zn == 0) || x + xn < 0 || y + yn < 0 || z + zn < 0 || x + xn >= x_size || y + yn >= y_size || z + zn >= z_size) {
                                    continue;
                                }
                                if (front[i + zn * x_y_size + yn * x_size + xn]) {
                                    neighbour_count++;
                                }
                            }
                        }
                    }
                    back[i] = (front[i] && neighbour_count == 2) || neighbour_count == 3;
                    i++;
                }
            }
        }
        // flip buffers
        u8 *temp = front;
        front = back;
        back = temp;
    }
    u16 total = 0;
    for (u16 i = 0; i < x_y_z_size; i++) {
        total += front[i];
    }
    sprintf(buf, "Solved part 1 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u active cubes", total);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    u16 w_size = 1 + ROUNDS * 2;
    u16 x_y_z_w_byte_size = x_size * y_size * z_size * w_size / 8;
    if (x_size * y_size * z_size * w_size % 8) {
        x_y_z_w_byte_size++; // need one more byte, even if we don't use all bits of it
    }
    MEM_free(buffer1);
    MEM_free(buffer2);
    buffer1 = MEM_alloc(x_y_z_w_byte_size);
    buffer2 = MEM_alloc(x_y_z_w_byte_size);
    front = buffer1;
    back = buffer2;
    memset(front, 0, x_y_z_w_byte_size);
    memset(back, 0, x_y_z_w_byte_size);
    for (u16 input_y = 0; input_y < input_height; input_y++) {
        for (u16 input_x = 0; input_x < input_width; input_x++) {
            if (DAY17_INPUT[input_y * (input_width + 1) + input_x] == '#') {
                u16 x = input_x + ROUNDS;
                u16 y = input_y + ROUNDS;
                u16 z = ROUNDS;
                u16 w = ROUNDS;
                bit_array_set(front, w * x_y_z_size + z * x_y_size + y * x_size + x, TRUE);
            }
        }
    }
    for (u16 iter = 1; iter <= ROUNDS; iter++) {
        sprintf(buf, "Iteration %u/%u", iter, ROUNDS);
        drawText(buf, 1, line);
        u32 i = 0;
        for (u16 w = 0; w < w_size; w++) {
            for (s16 z = 0; z < z_size; z++) {
                for (s16 y = 0; y < y_size; y++) {
                    for (s16 x = 0; x < x_size; x++) {
                        u16 neighbour_count = 0;
                        for (s16 wn= -1; wn <= 1; wn++) {
                            for (s16 zn= -1; zn <= 1; zn++) {
                                for (s16 yn= -1; yn <= 1; yn++) {
                                    for (s16 xn= -1; xn <= 1; xn++) {
                                        if ((xn == 0 && yn == 0 && zn == 0 && wn == 0) || x + xn < 0 || y + yn < 0 || z + zn < 0 || w + wn < 0 || x + xn >= x_size || y + yn >= y_size || z + zn >= z_size || w + wn >= w_size) {
                                            continue;
                                        }
                                        if (bit_array_get(front, i + wn * x_y_z_size + zn * x_y_size + yn * x_size + xn)) {
                                            neighbour_count++;
                                        }
                                    }
                                }
                            }
                        }
                        bit_array_set(back, i, (bit_array_get(front, i) && neighbour_count == 2) || neighbour_count == 3);
                        i++;
                    }
                }
            }
        }
        // flip buffers
        u8 *temp = front;
        front = back;
        back = temp;
    }
    total = 0;
    for (u16 i = 0; i < x_y_z_w_byte_size; i++) {
        total += __builtin_popcount(front[i]);
    }
    sprintf(buf, "Solved part 2 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: %u active hypercubes", total);
    drawText(buf, 1, line++);
    MEM_free(buffer1);
    MEM_free(buffer2);
    drawText("Day 17 done, press START to go back", 1, line + 1);
}