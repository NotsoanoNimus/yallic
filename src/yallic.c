/**
 * @file yallic.c
 * @author Suriel (Zachary) Puhl
 * @date August 2022
 *
 */

#include "yallic.h"



/**
 * Represents individual, generic linked-list elements. These consist of a node pointer
 *   to some resource or data type, and a pointer to the adjacent (next) list node.
 *
 * @typedef ListNode_t yallic.h
 * @see List_t
 */
struct __linked_list_node_t {
    void* data;   /**< Pointer to the underlying node data. */
    struct __linked_list_node_t* next;   /**< Pointer to the next linked list node. */
};

/**
 * The primary, generic linked-list structure.
 *   The LIST maintains a max_count with a HEAD pointer.
 *
 * @typedef List_t yallic.h
 * @see ListNode_t
 */
struct __linked_list_t {
    ListNode_t* head;   /**< The list's HEAD pointer. */
    size_t max_size;   /**< The list's maximum size, defined on instantiation. */
};
