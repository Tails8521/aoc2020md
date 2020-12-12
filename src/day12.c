#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day12.h>
#include <utils.h>

typedef enum __attribute__ ((__packed__)) {
    N,
    S,
    E,
    W
} Direction;

void left_part1(Direction *direction) {
    switch (*direction) {
    case N:
        *direction = W;
        break;
    case S:
        *direction = E;
        break;
    case E:
        *direction = N;
        break;
    case W:
        *direction = S;
        break;
    default:
        break;
    }
}

void right_part1(Direction *direction) {
    switch (*direction) {
    case N:
        *direction = E;
        break;
    case S:
        *direction = W;
        break;
    case E:
        *direction = S;
        break;
    case W:
        *direction = N;
        break;
    default:
        break;
    }
}

void left_part2(s32 *x_waypoint, s32 *y_waypoint) {
    s32 temp = *x_waypoint;
    *x_waypoint = -*y_waypoint;
    *y_waypoint = temp;
}

void right_part2(s32 *x_waypoint, s32 *y_waypoint) {
    s32 temp = *x_waypoint;
    *x_waypoint = *y_waypoint;
    *y_waypoint = -temp;
}

void day12() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving day 12...", 1, line++);
    const u8 *cursor = DAY12_INPUT;
    s32 x_part1 = 0;
    s32 y_part1 = 0;
    s32 x_part2 = 0;
    s32 y_part2 = 0;
    s32 x_waypoint = 10;
    s32 y_waypoint = 1;
    Direction direction = E;

    while ( cursor < DAY12_INPUT + sizeof DAY12_INPUT) {
        u8 action = *cursor++;
        u16 amount = skip_atoi(&cursor);
        switch (action) {
        case 'N':
            y_part1 += amount;
            y_waypoint += amount;
            break;
        case 'S':
            y_part1 -= amount;
            y_waypoint -= amount;
            break;
        case 'E':
            x_part1 += amount;
            x_waypoint += amount;
            break;
        case 'W':
            x_part1 -= amount;
            x_waypoint -= amount;
            break;
        case 'L':
            for (u16 i = amount; i > 0; i -= 90) {
                left_part1(&direction);
                left_part2(&x_waypoint, &y_waypoint);
            }
            break;
        case 'R':
            for (u16 i = amount; i > 0; i -= 90) {
                right_part1(&direction);
                right_part2(&x_waypoint, &y_waypoint);
            }
            break;
        case 'F':
            x_part2 += x_waypoint * amount;
            y_part2 += y_waypoint * amount;
            switch (direction) {
            case N:
                y_part1 += amount;
                break;
            case S:
                y_part1 -= amount;
                break;
            case E:
                x_part1 += amount;
                break;
            case W:
                x_part1 -= amount;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        cursor++; // newline
    }
    s32 x_abs_part1 = x_part1;
    if (x_part1 < 0) {
        x_abs_part1 = -x_abs_part1;
    }
    s32 y_abs_part1 = y_part1;
    if (y_part1 < 0) {
        y_abs_part1 = -y_abs_part1;
    }
    s32 distance_part1 = x_abs_part1 + y_abs_part1;
    s32 x_abs_part2 = x_part2;
    if (x_part2 < 0) {
        x_abs_part2 = -x_abs_part2;
    }
    s32 y_abs_part2 = y_part2;
    if (y_part2 < 0) {
        y_abs_part2 = -y_abs_part2;
    }
    s32 distance_part2 = x_abs_part2 + y_abs_part2;
    sprintf(buf, "Solved day 12 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: The distance is %ld", distance_part1);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: The distance is %ld", distance_part2);
    drawText(buf, 1, line++);
    drawText("Day 12 done, press START to go back", 1, line + 1);
}