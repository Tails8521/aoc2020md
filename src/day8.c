#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day8.h>
#include <utils.h>

typedef enum __attribute__ ((__packed__)) {
    NOP = 0,
    ACC = 1,
    JMP = 2
} Opcode;

typedef struct {
    Opcode opcode;
    s16 arg;
} Instruction;

typedef struct {
    s16 pc;
    s16 acc;
    Instruction* instructions;
} State;

void execute_instruction(State* state) {
    Instruction instruction = state->instructions[state->pc];
    switch (instruction.opcode) {
    case NOP:
        state->pc++;
        break;
    case ACC:
        state->acc += instruction.arg;
        state->pc++;
        break;
    case JMP:
        state->pc += instruction.arg;
        break;
    default:
        break;
    }
}

void day8() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Assembling the program...", 1, line++);
    u16 instruction_count = 0;
    for (const u8 *cursor = DAY8_INPUT; cursor < DAY8_INPUT + sizeof DAY8_INPUT; cursor++) {
        if (*cursor == '\n') {
            instruction_count++;
        }
    }
    Instruction* instructions = MEM_alloc(instruction_count * sizeof (Instruction));
    u8* has_already_visited = MEM_alloc(instruction_count);
    memset(has_already_visited, 0, instruction_count);
    const u8 *cursor = DAY8_INPUT;
    u16 current_instruction = 0;
    // Assemble
    while (cursor < DAY8_INPUT + sizeof DAY8_INPUT) {
        Opcode opcode;
        if (cursor[0] == 'n' && cursor[1] == 'o' && cursor[2] == 'p') {
            opcode = NOP;
        } else if (cursor[0] == 'a' && cursor[1] == 'c' && cursor[2] == 'c') {
            opcode = ACC;
        } else /* if (cursor[0] == 'j' && cursor[1] == 'm' && cursor[2] == 'p') */ {
            opcode = JMP;
        }
        cursor+= 4;
        u16 is_negative = FALSE;
        if (*cursor++ == '-') {
            is_negative = TRUE;
        }
        u16 arg_positive_part = skip_atoi(&cursor);
        s16 arg;
        if (is_negative) {
            arg = -arg_positive_part;
        } else {
            arg = arg_positive_part;
        }
        Instruction instruction = {
            .opcode = opcode,
            .arg = arg
        };
        instructions[current_instruction++] = instruction;
        cursor++; // newline
    }
    sprintf(buf, "Assembled the program in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 1...", 1, line++);
    State state = {
        .pc = 0,
        .acc = 0,
        .instructions = instructions
    };

    // part 1
    while (! has_already_visited[state.pc]) {
        has_already_visited[state.pc] = TRUE;
        execute_instruction(&state);
    }
    sprintf(buf, "Solved part 1 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: Content of acc is %d", state.acc);
    drawText(buf, 1, line++);
    startTimer(0);
    drawText("Solving part 2...", 1, line++);

    // part 2
    for (u16 repair_index = 0; repair_index < instruction_count; repair_index++) {
        switch (instructions[repair_index].opcode) {
        case NOP:
            instructions[repair_index].opcode = JMP;
            break;
        case ACC:
            continue; // ignore these ones
        case JMP:
            instructions[repair_index].opcode = NOP;
            break;
        default:
            break;
        }
        // reset the state
        memset(has_already_visited, 0, instruction_count);
        state.pc = 0;
        state.acc = 0;
        while (state.pc < instruction_count && ! has_already_visited[state.pc]) {
            has_already_visited[state.pc] = TRUE;
            execute_instruction(&state);
        }
        if (state.pc == instruction_count) { // exactly one after the last instruction
            sprintf(buf, "Solved part 2 in %lu ms", getTimer(0, FALSE) / SUBTICKPERMILLISECOND);
            drawText(buf, 1, line++);
            sprintf(buf, "Part 2: Content of acc is %d", state.acc);
            drawText(buf, 1, line++);
            break;
        }
        // restore back the original program
        switch (instructions[repair_index].opcode) {
        case NOP:
            instructions[repair_index].opcode = JMP;
            break;
        case JMP:
            instructions[repair_index].opcode = NOP;
            break;
        default:
            break;
        }
    }
    MEM_free(instructions);
    MEM_free(has_already_visited);
    drawText("Day 8 done, press START to go back", 1, line + 1);
}