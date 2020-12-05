#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day4.h>
#include <utils.h>

#define FLAG_BYR (1 << 0)
#define FLAG_IYR (1 << 1)
#define FLAG_EYR (1 << 2)
#define FLAG_HGT (1 << 3)
#define FLAG_HCL (1 << 4)
#define FLAG_ECL (1 << 5)
#define FLAG_PID (1 << 6)
#define FLAG_CID (1 << 7)
#define VALID_PASSPORT_MASK (FLAG_BYR | FLAG_IYR | FLAG_EYR | FLAG_HGT | FLAG_HCL | FLAG_ECL | FLAG_PID) // no FLAG_CID required

typedef enum {
    READ_FIELD_NAME,
    READ_FIELD_DATA,
} ParsingState;

u16 update_current_passport_bitfield(u16 *current_passport_bitfield, const u8 *start_of_current_field, const u8 *cursor) {
    if (cursor - start_of_current_field != 3) {
        return 0;
    }
    char field[] = {start_of_current_field[0], start_of_current_field[1], start_of_current_field[2], '\0'};
    if (strcmp(field, "byr") == 0) {
        *current_passport_bitfield |= FLAG_BYR;
        return FLAG_BYR;
    }
    if (strcmp(field, "iyr") == 0) {
        *current_passport_bitfield |= FLAG_IYR;
        return FLAG_IYR;
    }
    if (strcmp(field, "eyr") == 0) {
        *current_passport_bitfield |= FLAG_EYR;
        return FLAG_EYR;
    }
    if (strcmp(field, "hgt") == 0) {
        *current_passport_bitfield |= FLAG_HGT;
        return FLAG_HGT;
    }
    if (strcmp(field, "hcl") == 0) {
        *current_passport_bitfield |= FLAG_HCL;
        return FLAG_HCL;
    }
    if (strcmp(field, "ecl") == 0) {
        *current_passport_bitfield |= FLAG_ECL;
        return FLAG_ECL;
    }
    if (strcmp(field, "pid") == 0) {
        *current_passport_bitfield |= FLAG_PID;
        return FLAG_PID;
    }
    if (strcmp(field, "cid") == 0) {
        *current_passport_bitfield |= FLAG_CID;
        return FLAG_CID;
    }
    return 0;
}

void update_current_passport_validated_bitfield(u16 *current_passport_validated_bitfield, const u8 *current_field, const u8 *cursor, u16 current_validated_item) {
    switch (current_validated_item) {
    case FLAG_BYR: {
        u16 byr = skip_atoi(&current_field);
        if (byr >= 1920 && byr <= 2002) {
            *current_passport_validated_bitfield |= current_validated_item;
        }
        break;
    }
        
    case FLAG_IYR: {
        u16 iyr = skip_atoi(&current_field);
        if (iyr >= 2010 && iyr <= 2020) {
            *current_passport_validated_bitfield |= current_validated_item;
        }
        break;
    }
        
    case FLAG_EYR: {
        u16 eyr = skip_atoi(&current_field);
        if (eyr >= 2020 && eyr <= 2030) {
            *current_passport_validated_bitfield |= current_validated_item;
        }
        break;
    }
        
    case FLAG_HGT: {
        u16 hgt = skip_atoi(&current_field);
        if ((current_field[0] == 'c' && current_field[1] == 'm' && hgt >= 150 && hgt <= 193) || (current_field[0] == 'i' && current_field[1] == 'n' && hgt >= 59 && hgt <= 76)) {
            *current_passport_validated_bitfield |= current_validated_item;
        }
        break;
    }
        
    case FLAG_HCL: {
        if (cursor - current_field != 7) {
            return;
        }
        if (*current_field++ == '#') {
            for (u16 i = 0; i < 6; i++) {
                if (!ishexdigit(current_field[i])) {
                    return;
                }
                *current_passport_validated_bitfield |= current_validated_item;
            }
        }
        break;
    }
        
    case FLAG_ECL: {
        if (cursor - current_field != 3) {
            return;
        }
        char ecl[] = {current_field[0], current_field[1], current_field[2], '\0'};
        if (strcmp(ecl, "amb") == 0 || strcmp(ecl, "blu") == 0 || strcmp(ecl, "brn") == 0 || strcmp(ecl, "gry") == 0 || strcmp(ecl, "grn") == 0 || strcmp(ecl, "hzl") == 0 || strcmp(ecl, "oth") == 0) {
            *current_passport_validated_bitfield |= current_validated_item;
        }
        break;
    }
        
    case FLAG_PID: {
        if (cursor - current_field != 9) {
            return;
        }
        for (u16 i = 0; i < 9; i++) {
            if (!isdigit(current_field[i])) {
                return;
            }
            *current_passport_validated_bitfield |= current_validated_item;
        }
        break;
    }
        
    case FLAG_CID:
        break;
    default:
        break;
    }
}

void day4() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving day 4...", 1, line++);
    u16 valid_passports_part1 = 0;
    u16 valid_passports_part2 = 0;
    u16 current_passport_bitfield = 0;
    u16 current_passport_validated_bitfield = 0;
    u16 current_validated_item = 0;
    const u8 *start_of_current_field = DAY4_INPUT;
    ParsingState parsing_state = READ_FIELD_NAME;
    for (const u8 *cursor = DAY4_INPUT; cursor < DAY4_INPUT + sizeof DAY4_INPUT; cursor++) {
        switch (parsing_state) {
        case READ_FIELD_NAME:
            if (*cursor == ':') {
                current_validated_item = update_current_passport_bitfield(&current_passport_bitfield, start_of_current_field, cursor);
                parsing_state = READ_FIELD_DATA;
                start_of_current_field = cursor + 1;

            } else if (*cursor == '\n') {
                // next passport
                if ((current_passport_bitfield & VALID_PASSPORT_MASK) == VALID_PASSPORT_MASK) {
                    valid_passports_part1++;
                    if ((current_passport_validated_bitfield & VALID_PASSPORT_MASK) == VALID_PASSPORT_MASK) {
                        valid_passports_part2++;
                    }
                }
                current_passport_bitfield = 0;
                current_passport_validated_bitfield = 0;
                current_validated_item = 0;
                start_of_current_field = cursor + 1;
            }
            break;
        case READ_FIELD_DATA:
            if (*cursor == ' ' || *cursor == '\n') {
                parsing_state = READ_FIELD_NAME;
                update_current_passport_validated_bitfield(&current_passport_validated_bitfield, start_of_current_field, cursor, current_validated_item);
                start_of_current_field = cursor + 1;
            }
            break;
        default:
            break;
        }
    }
    sprintf(buf, "Solved day 4 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u valid passports", valid_passports_part1);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: %u valid passports", valid_passports_part2);
    drawText(buf, 1, line++);
    drawText("Day 4 done, press START to go back", 1, line + 1);
}