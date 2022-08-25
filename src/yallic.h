#ifndef _YALLIC_H
#define _YALLIC_H

/**
 * @file yallic.h
 *
 * @author Suriel (Zachary) Puhl
 * @date August 2022
 * @version 1.0
 *
 * @section DESCRIPTION
 *
 * Header file to define/access/link linked-list operations.<br />
 * __Repository__: https://github.com/NotsoanoNimus/yallic
 *
 *   Implementations of the linked-list collections concept here are inspired
 *   directly by (and in fact mirror) the Java 'LinkedList<E>' resources from:
 *   https://docs.oracle.com/javase/7/docs/api/java/util/LinkedList.html
 *
 * @section LICENSE
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.
 *
 */

#define YALLIC_LIST_SIZE_MAX 0xFFFFFFFFFFFFFFFF   /**< Linked list maximum allowable count. */
#include <stddef.h>
//typedef long long unsigned int size_t;



/**
 * Represents individual, generic linked-list elements. These consist of a node pointer
 *   to some resource or data type, and a pointer to the adjacent (next) list node.
 *
 * @see List_t
 */
typedef struct __linked_list_node_t ListNode_t;

/**
 * The primary, generic linked-list structure.
 *   The LIST maintains a max_count with a HEAD pointer.
 *
 * @see ListNode_t
 */
typedef struct __linked_list_t List_t;



/**
 * Initialize a new linked list. This initializes the structure and its override-able
 *   method pointers internally.
 *
 * @param max_size Maximum size of the created linked list.
 * @return A pointer to the allocated linked list. NULL on error.
 */
List_t* List__new( size_t max_size );

/**
 * Destroys a linked list. If the list hasn't been cleared--meaning a count-check on
 *   the list is greater than 0--then this function will attempt a shallow clear on
 *   the list. __To prevent memory leaks__, use the @{List__clean_deep(List_t* p_list)}
 *   method before invoking this function.
 *
 * @see {List__clean_deep(List_t* p_list)}
 */
void List__delete( List_t* p_list );

/**
 * Deeply delete underlying list node data, list nodes, and destroy the list. This method
 *   assumes the caller ran it explicitly and intentionally, and therefore tries to
 *   forcibly deallocate all list data regardless of what's allocated or given (unless the
 *   List_t pointer is NULL).
 */
void List__delete_deep( List_t* p_list );

/**
 * Reverse the order of a linked list object. This function takes a double-pointer since
 *   it effectly does a shallow deletion of the old list and creates a new one. Therefore,
 *   it needs to modify a passed pointer by reference.
 *
 * @param pp_list Double-pointer to a valid linked list to reverse.
 * @return Nothing. However, the value of the pointer from the param dereference is set
 *   to the newly-formed, reversed list.
 */
void List__reverse( List_t** pp_list );

/**
 * Shallow clear of list nodes. Deletes and frees all list nodes, but does _NOT_ attempt
 *   to free the values to which the nodes point. Any lists which have nodes pointing to
 *   the same underlying data (clones) are _not affected_.
 */
void List__clear_shallow( List_t* p_list );

/**
 * Deep clear of list data. Deletes and free all list nodes, _as well as_ their underlying
 *   data pointers. Any clones (shallow copies) of the provided list should be deleted or
 *   attempts to dereference underlying data can result in undefined behavior. Additionally,
 *   attempts to use this on lists containing unallocated node data pointers will also
 *   result in undefined or problematic behavior.
 */
void List__clear_deep( List_t* p_list );



int List__add( List_t* p_list, void* p_data );
int List__add_at( List_t* p_list, void* p_data, size_t index );
int List__concat( List_t* p_list_dest, List_t* p_list_src );
int List__concat_at( List_t* p_list_dest, List_t* p_list_src, size_t index );

List_t* List__clone( List_t* p_list );

int List__contains( List_t* p_list, void* p_data );

void* List__get_first( List_t* p_list );
void* List__get_last( List_t* p_list );
void* List__get_at( List_t* p_list, size_t index );

void* List__index_of( List_t* p_list, void* p_data );
void* List__last_index_of( List_t* p_list, void* p_data );

void* List__pop( List_t* p_list );
void* List__push( List_t* p_list, void* p_data );

void* List__remove_first( List_t* p_list );
void* List__remove_last( List_t* p_list );
void* List__remove_at( List_t* p_list, size_t index );
void* List__remove_first_occurrence( List_t* p_list, void* p_data );
void* List__remove_last_occurrence( List_t* p_list, void* p_data );

void* List__set_at( List_t* p_list, size_t index, void* p_new_data );


size_t List__length( List_t* p_list );
size_t List__count( List_t* p_list );


void* List__to_array( List_t* p_list, size_t element_size );



#endif   /* _YALLIC_H */
