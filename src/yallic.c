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



List_t* List__new( size_t max_size ) {}
void List__delete( List_t* p_list ) {}
void List__delete_deep( List_t* p_list ) {}
void List__reverse( List_t** pp_list ) {}
void List__clear_shallow( List_t* p_list ) {}
void List__clear_deep( List_t* p_list ) {}
int List__add( List_t* p_list, void* p_data ) {}
int List__add_at( List_t* p_list, void* p_data, size_t index ) {}
int List__concat( List_t* p_list_dest, List_t* p_list_src ) {}
int List__concat_at( List_t* p_list_dest, List_t* p_list_src, size_t index ) {}
List_t* List__clone( List_t* p_list ) {}
int List__contains( List_t* p_list, void* p_data ) {}
void* List__get_first( List_t* p_list ) {}
void* List__get_last( List_t* p_list ) {}
void* List__get_at( List_t* p_list, size_t index ) {}
void* List__index_of( List_t* p_list, void* p_data ) {}
void* List__last_index_of( List_t* p_list, void* p_data ) {}
void* List__pop( List_t* p_list ) {}
void* List__push( List_t* p_list, void* p_data ) {}
void* List__remove_first( List_t* p_list ) {}
void* List__remove_last( List_t* p_list ) {}
void* List__remove_at( List_t* p_list, size_t index ) {}
void* List__remove_first_occurrence( List_t* p_list, void* p_data ) {}
void* List__remove_last_occurrence( List_t* p_list, void* p_data ) {}
void* List__set_at( List_t* p_list, size_t index, void* p_new_data ) {}
size_t List__length( List_t* p_list ) {}
size_t List__count( List_t* p_list ) {}
void* List__to_array( List_t* p_list, size_t element_size ) {}
