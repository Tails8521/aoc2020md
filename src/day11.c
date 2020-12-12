#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day11.h>
#include <utils.h>

typedef enum __attribute__ ((__packed__)) {
    EMPTY,
    OCCUPIED,
    FLOOR
} Cell;

// unused, see why below
typedef struct {
    s16 index;
    s16 neighbour_indexes[8];
} Seat;

void day11() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Parsing day 11 input...", 1, line++);
    u16 layout_width = 0;
    for (const u8 *cursor = DAY11_INPUT; *cursor != '\n'; cursor++) {
        layout_width++;
    }
    u16 layout_height = sizeof DAY11_INPUT / (layout_width + 1); // + 1 for newlines
    u16 layout_size = sizeof DAY11_INPUT - layout_height;
    // double buffering
    Cell *buffer1 = MEM_alloc(layout_size * sizeof (Cell));
    Cell *buffer2 = MEM_alloc(layout_size * sizeof (Cell));
    u16 index = 0;
    for (const u8 *cursor = DAY11_INPUT; cursor < DAY11_INPUT + sizeof DAY11_INPUT; cursor++) {
        if (*cursor == '\n') {
            continue;
        }
        if (*cursor == 'L') {
            buffer1[index] = EMPTY;
            buffer2[index] = EMPTY;
        } else {
            buffer1[index] = FLOOR;
            buffer2[index] = FLOOR;
        }
        index++;
    }
    sprintf(buf, "Parsed day 11 input in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    Cell *front = buffer1;
    Cell *back = buffer2;
    u16 has_changed;
    u16 occupied_seats_count;
    u16 iteration = 1;
    do {
        sprintf(buf, "Iterations: %u", iteration++);
        drawText(buf, 1, line);
        has_changed = FALSE;
        occupied_seats_count = 0;
        u16 i = 0;
        for (s16 y = 0; y < layout_height; y++) {
            for (s16 x = 0; x < layout_width; x++) {
                if (front[i] == EMPTY) {
                    u16 neighbour_count = 0;
                    for (s16 yn= -1; yn <= 1; yn++) {
                        for (s16 xn= -1; xn <= 1; xn++) {
                            if ((yn == 0 && xn == 0) || x + xn < 0 || y + yn < 0 || x + xn >= layout_width || y + yn >= layout_height) {
                                continue;
                            }
                            if (front[i + xn + yn * layout_width] == OCCUPIED) {
                                neighbour_count++;
                                goto neighbour_detected;
                            }
                        }
                    }
neighbour_detected:
                    if (neighbour_count > 0) {
                        back[i] = EMPTY;
                    } else {
                        back[i] = OCCUPIED;
                        has_changed = TRUE;
                    }
                } else if (front[i] == OCCUPIED) {
                    occupied_seats_count++;
                    u16 neighbour_count = 0;
                    for (s16 yn= -1; yn <= 1; yn++) {
                        for (s16 xn= -1; xn <= 1; xn++) {
                            if ((yn == 0 && xn == 0) || x + xn < 0 || y + yn < 0 || x + xn >= layout_width || y + yn >= layout_height) {
                                continue;
                            }
                            if (front[i + xn + yn * layout_width] == OCCUPIED) {
                                neighbour_count++;
                            }
                        }
                    }
                    if (neighbour_count >= 4) {
                        back[i] = EMPTY;
                        has_changed = TRUE;
                    } else {
                        back[i] = OCCUPIED;
                    }
                }
                i++;
            }
        }
        // flip buffers
        Cell *temp = front;
        front = back;
        back = temp;
    } while (has_changed);
    line++;
    sprintf(buf, "Solved part 1 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: There are %u occupied seats", occupied_seats_count);
    drawText(buf, 1, line++);

    // Part 2
    index = 0;
    // u16 seat_count = 0;
    for (const u8 *cursor = DAY11_INPUT; cursor < DAY11_INPUT + sizeof DAY11_INPUT; cursor++) {
        if (*cursor == '\n') {
            continue;
        }
        if (*cursor == 'L') {
            buffer1[index] = EMPTY;
            buffer2[index] = EMPTY;
            // seat_count++;
        } else {
            buffer1[index] = FLOOR;
            buffer2[index] = FLOOR;
        }
        index++;
    }
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    front = buffer1;
    back = buffer2;
    iteration = 1;

    // Optimization which works on the example, but the Genesis has not enough memory for it to work on the actual input :(

    // pre-process seats and their neighbours
    // Seat *seats = MEM_alloc(seat_count * sizeof (Seat));
    // s16 current_index = 0;
    // s16 seat_index = 0;
    // for (s16 y = 0; y < layout_height; y++) {
    //     for (s16 x = 0; x < layout_width; x++) {
    //         if (front[current_index] == EMPTY) {
    //             Seat seat;
    //             seat.index = current_index;
    //             u16 neighbour = 0;
    //             for (s16 yn= -1; yn <= 1; yn++) {
    //                 for (s16 xn= -1; xn <= 1; xn++) {
    //                     if (yn == 0 && xn == 0) {
    //                         continue;
    //                     }
    //                     seat.neighbour_indexes[neighbour] = -1;
    //                     s16 neighbour_x = x + xn;
    //                     s16 neighbour_y = y + yn;
    //                     s16 neighbour_i = current_index + yn * layout_width + xn;
    //                     while (neighbour_x >= 0 && neighbour_y >= 0 && neighbour_x < layout_width && neighbour_y < layout_height) {
    //                         if (front[neighbour_i] == EMPTY) {
    //                             seat.neighbour_indexes[neighbour] = neighbour_i;
    //                             break;
    //                         }
    //                         neighbour_x += xn;
    //                         neighbour_y += yn;
    //                         neighbour_i += yn * layout_width + xn;
    //                     }
    //                     neighbour++;
    //                 }
    //             }
    //             KLog_S1("Seat: ", seat_index);
    //             KLog_S1("Index: ", seat.index);
    //             KLog_S2("N0: x: ", seat.neighbour_indexes[0] % layout_width, ", y: ", seat.neighbour_indexes[0] / layout_width);
    //             KLog_S2("N1: x: ", seat.neighbour_indexes[1] % layout_width, ", y: ", seat.neighbour_indexes[1] / layout_width);
    //             KLog_S2("N2: x: ", seat.neighbour_indexes[2] % layout_width, ", y: ", seat.neighbour_indexes[2] / layout_width);
    //             KLog_S2("N3: x: ", seat.neighbour_indexes[3] % layout_width, ", y: ", seat.neighbour_indexes[3] / layout_width);
    //             KLog_S2("N4: x: ", seat.neighbour_indexes[4] % layout_width, ", y: ", seat.neighbour_indexes[4] / layout_width);
    //             KLog_S2("N5: x: ", seat.neighbour_indexes[5] % layout_width, ", y: ", seat.neighbour_indexes[5] / layout_width);
    //             KLog_S2("N6: x: ", seat.neighbour_indexes[6] % layout_width, ", y: ", seat.neighbour_indexes[6] / layout_width);
    //             KLog_S2("N7: x: ", seat.neighbour_indexes[7] % layout_width, ", y: ", seat.neighbour_indexes[7] / layout_width);
    //             seats[seat_index++] = seat;
    //         }
    //         current_index++;
    //     }
    // }     
    // do {
    //     has_changed = FALSE;
    //     occupied_seats_count = 0;
    //     for (u16 s = 0; s < seat_count; s++) {
    //         if (front[seats[s].index] == EMPTY) {
    //             u16 occupied_neighbour_count = 0;
    //             for (u16 n = 0; n < 8; n++) {
    //                 s16 neighbour_index = seats[s].neighbour_indexes[n];
    //                 // KLog_S2("index: ", neighbour_index, ", value: ", front[neighbour_index]);
    //                 if (neighbour_index != -1 && front[neighbour_index] == OCCUPIED) {
    //                     occupied_neighbour_count++;
    //                     break;
    //                 }
    //             }
    //             if (occupied_neighbour_count) {
    //                 back[seats[s].index] = EMPTY;
    //             } else {
    //                 back[seats[s].index] = OCCUPIED;
    //                 has_changed = TRUE;
    //             }
    //         } else {
    //             occupied_seats_count++;
    //             u16 occupied_neighbour_count = 0;
    //             for (u16 n = 0; n < 8; n++) {
    //                 s16 neighbour_index = seats[s].neighbour_indexes[n];
    //                 if (neighbour_index != -1 && front[neighbour_index] == OCCUPIED) {
    //                     occupied_neighbour_count++;
    //                 }
    //             }
    //             // KLog_S1("Occupied: ", occupied_neighbour_count);
    //             if (occupied_neighbour_count >= 5) {
    //                 back[seats[s].index] = EMPTY;
    //                 has_changed = TRUE;
    //             } else {
    //                 back[seats[s].index] = OCCUPIED;
    //             }
    //         }
    //     }
        do {
            sprintf(buf, "Iterations: %u", iteration++);
            drawText(buf, 1, line);
            has_changed = FALSE;
            occupied_seats_count = 0;
            u16 i = 0;
            for (s16 y = 0; y < layout_height; y++) {
                for (s16 x = 0; x < layout_width; x++) {
                    if (front[i] == EMPTY) {
                        u16 neighbour_count = 0;
                        for (s16 yn= -1; yn <= 1; yn++) {
                            for (s16 xn= -1; xn <= 1; xn++) {
                                if (yn == 0 && xn == 0) {
                                    continue;
                                }
                                s16 neighbour_x = x + xn;
                                s16 neighbour_y = y + yn;
                                s16 neighbour_i = i + yn * layout_width + xn;
                                while (neighbour_x >= 0 && neighbour_y >= 0 && neighbour_x < layout_width && neighbour_y < layout_height) {
                                    if (front[neighbour_i] == EMPTY) {
                                        break;
                                    } else if (front[neighbour_i] == OCCUPIED) {
                                        neighbour_count++;
                                        goto neighbour_detected_part2;
                                    }
                                    neighbour_x += xn;
                                    neighbour_y += yn;
                                    neighbour_i += yn * layout_width + xn;
                                }
                            }
                        }
neighbour_detected_part2:
                        if (neighbour_count > 0) {
                            back[i] = EMPTY;
                        } else {
                            back[i] = OCCUPIED;
                            has_changed = TRUE;
                        }
                    } else if (front[i] == OCCUPIED) {
                        occupied_seats_count++;
                        u16 neighbour_count = 0;
                        for (s16 yn= -1; yn <= 1; yn++) {
                            for (s16 xn= -1; xn <= 1; xn++) {
                                if (yn == 0 && xn == 0) {
                                    continue;
                                }
                                s16 neighbour_x = x + xn;
                                s16 neighbour_y = y + yn;
                                s16 neighbour_i = i + yn * layout_width + xn;
                                while (neighbour_x >= 0 && neighbour_y >= 0 && neighbour_x < layout_width && neighbour_y < layout_height) {
                                    if (front[neighbour_i] == EMPTY) {
                                        break;
                                    } else if (front[neighbour_i] == OCCUPIED) {
                                        neighbour_count++;
                                        break;
                                    }
                                    neighbour_x += xn;
                                    neighbour_y += yn;
                                    neighbour_i += yn * layout_width + xn;
                                }
                            }
                        }
                        if (neighbour_count >= 5) {
                            back[i] = EMPTY;
                            has_changed = TRUE;
                        } else {
                            back[i] = OCCUPIED;
                        }
                    }
                    i++;
                }
            }

        // Cell* cell = back;
        // for (u16 i = 0; i < layout_height; i++) {
        //     char b[layout_width + 1];
        //     for (u16 j = 0; j < layout_height; j++) {
        //         if (*cell == FLOOR) {
        //             b[j] = '.';
        //         } else if (*cell == EMPTY) {
        //             b[j] = 'L';
        //         } else {
        //              b[j] = '#';
        //         }
        //         cell++;
        //     }
        //     b[layout_width] = '\0';
        //     KLog(b);
        // }
        // KLog("");
        
        // flip buffers
        Cell *temp = front;
        front = back;
        back = temp;
    } while (has_changed);
    line++;
    sprintf(buf, "Solved part 2 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: There are %u occupied seats", occupied_seats_count);
    drawText(buf, 1, line++);
    MEM_free(buffer1);
    MEM_free(buffer2);
    // MEM_free(seats);
    drawText("Day 11 done, press START to go back", 1, line + 1);
}