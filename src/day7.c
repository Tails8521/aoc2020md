#include <genesis.h>
#include <resources.h>
#include <string.h>
#include <day7.h>
#include <utils.h>
#include <bstmap.h>

typedef struct BagListElement {
    struct BagListElement *next;
    const u8 *bag_name;
    u8 bag_name_size;
    u8 qty;
} BagListElement;

typedef struct {
    BinarySearchTreeNode* bag_colors;
    u16 part1_count;
    u32 part2_product;
} IterContext;

// This one is way easier to reason with recursion
u32 part2(BinarySearchTreeNode* tree, BinarySearchTreeNode* node) {
    u32 ret = 0;
    BagListElement* elm = node->data;
    while (elm) {
        ret += elm->qty * (part2(tree, search_bstmap(tree, elm->bag_name, elm->bag_name_size)) + 1);
        elm = elm->next;
    }
    return ret;
}

void iterate(void* const context, BinarySearchTreeNode* node) {
    IterContext* iter_context = (IterContext*) context;
    if (memcmp(node->key, (u8*) "shiny gold", 10) == 0) {
        // part 2
        iter_context->part2_product = part2(iter_context->bag_colors, node);
        return;
    }
    // part 1
    BagListElement* stack[100];
    BagListElement** stack_pointer = stack;
    *stack_pointer++ = (BagListElement*) node->data;
    while (stack_pointer != stack) {
        BagListElement* elm = *--stack_pointer;
        while (elm) {
            if (memcmp(elm->bag_name, (u8*) "shiny gold", 10) == 0) {
                iter_context->part1_count++;
                return;
            }
            *stack_pointer++ = search_bstmap(iter_context->bag_colors, elm->bag_name, elm->bag_name_size)->data;
            elm = elm->next;
        }
    }
}

void iterate_free(void* const context, BinarySearchTreeNode* node) {
    BagListElement* elm = (BagListElement*) node->data;
    while (elm) {
        BagListElement* to_free = elm;
        elm = elm->next;
        MEM_free(to_free);
    }
}

void day7() {
    u16 line = 1;
    char buf[200];
    startTimer(0);
    drawText("Solving day 7...", 1, line++);
    sprintf(buf, "MEM_getFree() = %u", MEM_getFree());
    drawText(buf, 1, line++);
    BinarySearchTreeNode* bag_colors = NULL;
    const u8 *cursor = DAY7_INPUT;
    while (cursor < DAY7_INPUT + sizeof DAY7_INPUT) {
        const u8 *current_bag_name = cursor;
        while (cursor[0] != ' ' || cursor[1] != 'b' || cursor[2] != 'a' || cursor[3] != 'g') {
            cursor++;
        }
        u32 current_bag_name_size = cursor - current_bag_name;
        cursor += 14; // " bags contain "
        BagListElement* list_head = NULL;
        if (*cursor == 'n') {
            cursor += 15; // "contain no other bags.\n"
        } else {
            while (isdigit(*cursor)) {
                u8 qty = skip_atoi(&cursor);
                cursor++; // <space>
                const u8 *bag_content_name = cursor;
                while (cursor[0] != ' ' || cursor[1] != 'b' || cursor[2] != 'a' || cursor[3] != 'g') {
                    cursor++;
                }
                u32 bag_content_name_size = cursor - bag_content_name;
                // " bag(s), " or " bag(s).\n"
                cursor += 6;
                if (qty > 1) {
                    cursor++;
                }
                BagListElement* elm = MEM_alloc(sizeof (BagListElement));
                elm->next = list_head;
                elm->bag_name = bag_content_name;
                elm->bag_name_size = bag_content_name_size;
                elm->qty = qty;
                list_head = elm;
            }
        }
        insert_bstmap(&bag_colors, current_bag_name, current_bag_name_size, list_head);
    }
    IterContext iter_context = {
        .bag_colors = bag_colors,
        .part1_count = 0,
        .part2_product = 0
    };
    iterate_bstmap(bag_colors, &iterate, &iter_context);
    sprintf(buf, "Solved day 7 in %lu s", getTimer(0, FALSE) / SUBTICKPERSECOND);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 1: %u type of bags", iter_context.part1_count);
    drawText(buf, 1, line++);
    sprintf(buf, "Part 2: %lu total bags", iter_context.part2_product);
    drawText(buf, 1, line++);
    sprintf(buf, "MEM_getFree() = %u", MEM_getFree());
    drawText(buf, 1, line++);
    drawText("Freeing memory...", 1, line++);
    iterate_bstmap(bag_colors, &iterate_free, &iter_context);
    delete_bstmap(bag_colors);
    sprintf(buf, "MEM_getFree() = %u", MEM_getFree());
    drawText(buf, 1, line++);
    drawText("Day 7 done, press START to go back", 1, line + 1);
}