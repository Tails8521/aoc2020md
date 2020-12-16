#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day16.h>
#include <utils.h>

typedef struct {
    u16 is_departure;
    u16 range1_lower;
    u16 range1_upper;
    u16 range2_lower;
    u16 range2_upper;
} Rule;

void day16() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Solving day 16...", 1, line++);
    const u8 *cursor = DAY16_INPUT;
    u16 prev_char_was_newline = FALSE;
    u16 rule_count = 0;
    // count the rules
    while (TRUE) {
        if (*cursor++ == '\n') {
            if (prev_char_was_newline) {
                break;
            }
            rule_count++;
            prev_char_was_newline = TRUE;
        } else {
            prev_char_was_newline = FALSE;
        }
    }
    // dynamic allocations
    Rule *rules = MEM_alloc(rule_count * sizeof (Rule));
    u16 *my_ticket = MEM_alloc(rule_count * sizeof (u16));
    u16 *ticket_buffer = MEM_alloc(rule_count * sizeof (u16));
    s16 *rule_index_to_ticket_index = MEM_alloc(rule_count * sizeof (u16));
    memsetU16((u16*)rule_index_to_ticket_index, -1, rule_count);
    u8 *candidates = MEM_alloc(rule_count * rule_count); // boolean 2D array where each line is a field, and each column is a position on the ticket, FALSE means it has been ruled out
    memset(candidates, TRUE, rule_count * rule_count);
    u8 *ticket_candidates = MEM_alloc(rule_count * rule_count); // same but for the current ticket
    // read the rules
    Rule *current_rule = rules;
    cursor = DAY16_INPUT;
    while (TRUE) {
        if (*cursor == '\n') {
            break;
        }
        current_rule->is_departure = cursor[0] == 'd' && cursor[1] == 'e' && cursor[2] == 'p' && cursor[3] == 'a' && cursor[4] == 'r' && cursor[5] == 't' && cursor[6] == 'u' && cursor[7] == 'r' && cursor[8] == 'e';
        while (*cursor++ != ':') {
            // skip the name
        }
        cursor++; // space
        current_rule->range1_lower = skip_atoi(&cursor);
        cursor++; // -
        current_rule->range1_upper = skip_atoi(&cursor);
        cursor += 4; // " or "
        current_rule->range2_lower = skip_atoi(&cursor);
        cursor++; // -
        current_rule->range2_upper = skip_atoi(&cursor);
        current_rule++;
        cursor++; // newline
    }
    cursor++; // newline
    while (*cursor++ != '\n') {
        // your ticket:
    }
    for (u16 i = 0; i < rule_count; i++) {
        my_ticket[i] = skip_atoi(&cursor);
        cursor++; // , or the newline
    }
    cursor++; // newline
    while (*cursor++ != '\n') {
        // nearby tickets:
    }
    // ticket processing
    u32 error_rate = 0;
    while (cursor < DAY16_INPUT + sizeof DAY16_INPUT) {
        u16 valid_ticket = TRUE;
        memset(ticket_candidates, TRUE, rule_count * rule_count);
        for (u16 i = 0; i < rule_count; i++) {
            u16 value = skip_atoi(&cursor);
            cursor++; // newline or comma,
            u16 valid_field = FALSE;
            for (u16 r = 0; r < rule_count; r++) {
                if ((rules[r].range1_lower <= value && value <= rules[r].range1_upper) || (rules[r].range2_lower <= value && value <= rules[r].range2_upper)) {
                    valid_field = TRUE;
                } else {
                    ticket_candidates[r * rule_count + i] = FALSE;
                }
            }
            if (valid_field) {
                ticket_buffer[i] = value;
            } else {
                error_rate += value;
                valid_ticket = FALSE;
            }
        }
        if (valid_ticket) {
            for (u16 i = 0; i < rule_count * rule_count; i++) {
                candidates[i] &= ticket_candidates[i];
            }
        }
    }
    // Process of elimination
    u16 deduced_fields = 0;
    while (deduced_fields < rule_count) {
        for (u16 r = 0; r < rule_count; r++) {
            if (rule_index_to_ticket_index[r] != -1) {
                continue; // this one has already been deduced
            }
            u16 possible_count = 0;
            u16 possible_position;
            for (u16 pos = 0; pos < rule_count; pos++) {
                if (candidates[r * rule_count + pos]) {
                    possible_position = pos;
                    possible_count++;
                    if (possible_count > 1) {
                        break;
                    }
                }
            }
            if (possible_count == 1) {
                // 1 and only 1 possible position for this rule
                rule_index_to_ticket_index[r] = possible_position;
                // other rules can't use this position anymore
                for (u16 i = 0; i < rule_count; i++) {
                    candidates[i * rule_count + possible_position] = FALSE;
                }
                deduced_fields++;
            }
        }
    }
    unsigned long long product = 1;
    for (u16 i = 0; i < rule_count; i++) {
        if (rules[i].is_departure) {
            product *= my_ticket[rule_index_to_ticket_index[i]];
        }
    }
    sprintf(buf, "Solved day 16 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: The error rate is %lu", error_rate);
    drawText(buf, 1, line++);
    u64ToStr(product, u64_result);
    sprintf(buf, "Part 2: The product is %s", u64_result);
    drawText(buf, 1, line++);
    MEM_free(rules);
    MEM_free(my_ticket);
    MEM_free(rule_index_to_ticket_index);
    MEM_free(ticket_buffer);
    MEM_free(candidates);
    MEM_free(ticket_candidates);
    drawText("Day 16 done, press START to go back", 1, line + 1);
}