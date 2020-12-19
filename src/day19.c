#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day19.h>
#include <utils.h>

#define UNROLL_DEPTH 10

typedef enum __attribute__ ((__packed__)) {
    SINGLE_CHARACTER,
    SUBLISTS
} RuleType;

typedef enum __attribute__ ((__packed__)) {
    NONE,
    PARTIAL,
    FULL
} Match;

typedef struct {
    union {
        u8 character;
        u8 subrules_lists[2][3];
    };
    u8 sublists_sizes[2];
    u8 sublists_count;
    RuleType type;
} Rule;

Match solution_match(Rule *rules, const u8 *word_to_match, u16 word_to_match_len, u8 *solution, u16 solution_len) {
    u8 rules_stack[256];
    u16 rules_stack_ptr = 0;
    u16 match_idx = 0;
    u16 solution_idx = 0;
    // push the first rule
    rules_stack[rules_stack_ptr++] = 0;
    while (rules_stack_ptr) {
        u8 rule_idx = rules_stack[--rules_stack_ptr];
        if (rules[rule_idx].type == SUBLISTS) {
            u8 selected = 0;
            if (rules[rule_idx].sublists_count > 1) {
                if (solution_idx == solution_len) {
                    return PARTIAL;
                }
                selected = solution[solution_idx++];
            }
            for (s16 i = rules[rule_idx].sublists_sizes[selected] - 1; i >= 0; i--) {
                // push all the rules of the selected sublist
                rules_stack[rules_stack_ptr++] = rules[rule_idx].subrules_lists[selected][i];
            }
        } else { // single character
            if (word_to_match[match_idx++] != rules[rule_idx].character) {
                return NONE;
            }
        }
    }
    if (match_idx == word_to_match_len) {
        return FULL;
    } else {
        // Despite using a full solution, we still have characters to match
        return NONE;
    }
}

u16 backtrack(Rule *rules, const u8 *word_to_match, u16 word_to_match_len, u8 *solution, u16 solution_len) {
    Match match = solution_match(rules, word_to_match, word_to_match_len, solution, solution_len);
    if (match == NONE) { // dead end, don't recurse
        return FALSE;
    } else if (match == FULL) {
        return TRUE;
    }
    solution[solution_len] = 0;
    if (backtrack(rules, word_to_match, word_to_match_len, solution, solution_len + 1)) {
        return TRUE;
    };
    solution[solution_len] = 1;
    if (backtrack(rules, word_to_match, word_to_match_len, solution, solution_len + 1)) {
        return TRUE;
    };
    return FALSE;
}

void day19() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    Rule *rules = MEM_alloc(256 * sizeof (Rule));
    const u8 *cursor = DAY19_INPUT;
    while (*cursor != '\n') {
        u8 rule_index = skip_atoi(&cursor);
        cursor += 2; // ": "
        if (*cursor == '"') {
            cursor++;
            rules[rule_index].character = *cursor;
            rules[rule_index].type = SINGLE_CHARACTER;
            cursor += 3; // "<char>\"\n"
            continue;
        }
        rules[rule_index].type = SUBLISTS;
        u16 sublist_idx = 0;
        while (TRUE) {
            u16 sublist_elm_idx = 0;
            while (isdigit(*cursor)) {
                rules[rule_index].subrules_lists[sublist_idx][sublist_elm_idx] = skip_atoi(&cursor);
                sublist_elm_idx++;
                if (*cursor++ == '\n') { // " " or the newline
                    break;
                } 
            }
            rules[rule_index].sublists_sizes[sublist_idx] = sublist_elm_idx;
            sublist_idx++;
            if (*cursor == '|') {
                cursor += 2; // "| "
            } else {
                rules[rule_index].sublists_count = sublist_idx;
                break;
            }
        }
    }
    cursor++; // newline
    const u8 *cursor_backup = cursor;
    u8 solution_buf[256];
    u16 valid_messages = 0;
    while (cursor < DAY19_INPUT + sizeof DAY19_INPUT) {
        const u8 *message = cursor;
        u16 message_len = 0;
        while (*cursor++ != '\n') {
            message_len++;
        }
        valid_messages += backtrack(rules, message, message_len, solution_buf, 0);
    }
    sprintf(buf, "Solved part 1 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u valid messages", valid_messages);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);

    // Rather than making the code handle the infinite loop,
    // I "pre-unroll" the loop, up to a certain depth, by making additional rules
    u8 additional_rule_idx = 255;
    rules[8] = (Rule) {
        .type = SUBLISTS,
        .sublists_count = 2,
        .sublists_sizes = {1, 2},
        .subrules_lists = {{42}, {42, additional_rule_idx}}
    };
    for (u16 i = 0; i < UNROLL_DEPTH; i++){
        rules[additional_rule_idx] = (Rule) {
            .type = SUBLISTS,
            .sublists_count = 2,
            .sublists_sizes = {1, 2},
            .subrules_lists = {{42}, {42, additional_rule_idx - 1}}
        };
        additional_rule_idx--;
    }
    rules[additional_rule_idx] = (Rule) {
        .type = SUBLISTS,
        .sublists_count = 1,
        .sublists_sizes = {1},
        .subrules_lists = {{42}}
    };
    additional_rule_idx--;
    rules[11] = (Rule) {
        .type = SUBLISTS,
        .sublists_count = 2,
        .sublists_sizes = {2, 3},
        .subrules_lists = {{42, 31}, {42, additional_rule_idx, 31}}
    };
    for (u16 i = 0; i < UNROLL_DEPTH; i++){
        rules[additional_rule_idx] = (Rule) {
            .type = SUBLISTS,
            .sublists_count = 2,
            .sublists_sizes = {2, 3},
            .subrules_lists = {{42, 31}, {42, additional_rule_idx - 1, 31}}
        };
        additional_rule_idx--;
    }
    rules[additional_rule_idx] = (Rule) {
        .type = SUBLISTS,
        .sublists_count = 2,
        .sublists_sizes = {2},
        .subrules_lists = {{42, 31}}
    };

    valid_messages = 0;
    cursor = cursor_backup;
    while (cursor < DAY19_INPUT + sizeof DAY19_INPUT) {
        const u8 *message = cursor;
        u16 message_len = 0;
        while (*cursor++ != '\n') {
            message_len++;
        }
        valid_messages += backtrack(rules, message, message_len, solution_buf, 0);
    }
    sprintf(buf, "Solved part 2 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: %u valid messages", valid_messages);
    drawText(buf, 1, line++);
    MEM_free(rules);
    drawText("Day 19 done, press START to go back", 1, line + 1);
}