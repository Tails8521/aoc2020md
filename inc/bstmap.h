#ifndef _BSTMAP_H_
#define _BSTMAP_H_

#include <genesis.h>
#include <utils.h>

typedef struct BinarySearchTreeNode {
    struct BinarySearchTreeNode* left;
    struct BinarySearchTreeNode* right;
    void* data;
    const u8 *key;
    u16 key_length;
} BinarySearchTreeNode;

BinarySearchTreeNode* search_bstmap(BinarySearchTreeNode* node, const u8 *key, u16 key_length) {
    while (node != NULL) {
        u16 len = key_length < node->key_length ? key_length : node->key_length;
        s16 order = memcmp(key, node->key, len);
        if (order < 0) {
            node = node->left;
        } else if (order > 0) {
            node = node->right;
        } else {
            return node;
        }
    }
    return node;
}

void insert_bstmap(BinarySearchTreeNode** node_input, const u8 *key, u16 key_length, void* data) {
    BinarySearchTreeNode** node = node_input;
    while (*node) {
        u16 len = key_length < (*node)->key_length ? key_length : (*node)->key_length;
        s16 order = memcmp(key, (*node)->key, len);
        if (order < 0) {
            node = &(*node)->left;
        } else if (order > 0) {
            node = &(*node)->right;
        } else {
            (*node)->data = data;
            return;
        }
    }
    *node = MEM_alloc(sizeof (BinarySearchTreeNode));
    (*node)->left = NULL;
    (*node)->right = NULL;
    (*node)->key = key;
    (*node)->key_length = key_length;
    (*node)->data = data;
}

void iterate_bstmap(BinarySearchTreeNode* node, void (callback)(void * const context,  BinarySearchTreeNode *node), void* context) {
    BinarySearchTreeNode* stack[100];
    BinarySearchTreeNode** stack_pointer = stack;
    while (stack != stack_pointer || node != NULL) {
        if (node != NULL) {
            *stack_pointer++ = node;
            node = node->left;
        } else {
            node = *--stack_pointer;
            // visit
            callback(context, node);
            node = node->right;
        }
    }
}

void delete_bstmap(BinarySearchTreeNode* node) {
    BinarySearchTreeNode* stack[100];
    BinarySearchTreeNode** stack_pointer = stack;
    BinarySearchTreeNode* last_visited = NULL;
    while (stack != stack_pointer || node != NULL) {
        if (node != NULL) {
            *stack_pointer++ = node;
            node = node->left;
        } else {
            BinarySearchTreeNode* peek_node = *(stack_pointer - 1);
            if (peek_node->right != NULL && last_visited != peek_node->right) {
                node = peek_node->right;
            } else {
                MEM_free(peek_node);
                last_visited = *--stack_pointer;
            }
        }
    }
}


#endif // _BSTMAP_H_