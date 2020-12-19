#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day18.h>
#include <utils.h>

typedef enum __attribute__ ((__packed__)) {
    NUMBER = 0,
    OPERATOR = 1,
    END = 2
} TokenType;

typedef struct {
    TokenType type;
    union {
        u64 number;
        u8 operator;
    };
} Token;

typedef struct {
    u64 number_stack[100];
    u8 operator_stack[100];
    u16 operator_stack_pointer;
    u16 number_stack_pointer;
} ShuntingYard;

u64 evaluate_expression_part1(const u8 **cursor) {
    u64 acc = 0;
    u16 multiply = FALSE; // add if false, multiply if true
    while (TRUE) {
        if (isdigit(**cursor)) {
            u16 value = skip_atoi(cursor);
            if (multiply) {
                acc *= value;
            } else {
                acc += value;
            }
        } else if (**cursor == '(') {
            (*cursor)++;
            u64 value = evaluate_expression_part1(cursor);
            if (multiply) {
                acc *= value;
            } else {
                acc += value;
            }
        } else if (**cursor == '+') {
            multiply = FALSE;
            (*cursor)++;
        } else if (**cursor == '*') {
            multiply = TRUE;
            (*cursor)++;
        } else if (**cursor == ')' || **cursor == '\n') {
            (*cursor)++;
            return acc;
        } else { // whitespace
            (*cursor)++;
        }
    }
}

u16 precedence(u8 operator) {
    if (operator == '+') {
        return 1;
    } else {
        return 2;
    }
}

Token read_token(const u8 **cursor) {
    while (**cursor == ' ') { // skip whitespace
        (*cursor)++;
    }
    if (**cursor == '\n') {
        (*cursor)++;
        return (Token) {
            .type = END
        };
    } else if (**cursor == '+' || **cursor == '*' || **cursor == '(' || **cursor == ')') {
        u8 operator = **cursor;
        (*cursor)++;
        return (Token) {
            .type = OPERATOR,
            .operator = operator
        };
    } else {
        return (Token) {
            .type = NUMBER,
            .number = skip_atoi(cursor)
        };
    }
}

u64 pop_number(ShuntingYard *sy) {
    return sy->number_stack[--sy->number_stack_pointer];
}

u8 pop_operator(ShuntingYard *sy) {
    if (sy->operator_stack_pointer > 0) {
        return sy->operator_stack[--sy->operator_stack_pointer];
    } else { // we need a way to stop iterating the stack when we reduce
        return '(';
    }
}

void push_number(ShuntingYard *sy, u64 number) {
    sy->number_stack[sy->number_stack_pointer++] = number;
}

u16 reduce(ShuntingYard *sy) {
    u8 operator = pop_operator(sy);
    if (operator == '+') {
        push_number(sy, pop_number(sy) + pop_number(sy));
        return FALSE;
    } else if (operator == '*') {
        push_number(sy, pop_number(sy) * pop_number(sy));
        return FALSE;
    } else { // (
        return TRUE;
    }
}

void push_operator(ShuntingYard *sy, char operator) {
    if (operator != '(') {
        u64 preced = precedence(operator);
        while (sy->operator_stack_pointer) {
            char top = sy->operator_stack[sy->operator_stack_pointer - 1];
            if (top == '(' || precedence(top) > preced || reduce(sy)) {
                break;
            }
        }
    }
    sy->operator_stack[sy->operator_stack_pointer++] = operator;
}

u64 evaluate_expression_part2(const u8 **cursor) {
    ShuntingYard sy;
    sy.number_stack_pointer = 0;
    sy.operator_stack_pointer = 0;
    while (TRUE) {
        Token token = read_token(cursor);
        if (token.type == NUMBER) {
            push_number(&sy, token.number);
        } else if (token.type == OPERATOR && token.operator != ')') {
            push_operator(&sy, token.operator);
        } else { // ) or end of line
            while (!reduce(&sy)) {

            }
            if (token.type == END) {
                return pop_number(&sy);
            }
        }
    }
}

void day18() {
    u16 line = 1;
    char buf[200];
    char u64_result[30];
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    const u8 *cursor = DAY18_INPUT;
    u64 sum = 0;
    while (cursor < DAY18_INPUT + sizeof DAY18_INPUT) {
        sum += evaluate_expression_part1(&cursor);
    }
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(sum, u64_result);
    sprintf(buf, "Part 1: %s", u64_result);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);
    cursor = DAY18_INPUT;
    sum = 0;
    while (cursor < DAY18_INPUT + sizeof DAY18_INPUT) {
        sum += evaluate_expression_part2(&cursor);
    }
    sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    u64ToStr(sum, u64_result);
    sprintf(buf, "Part 2: %s", u64_result);
    drawText(buf, 1, line++);
    drawText("Day 18 done, press START to go back", 1, line + 1);
}