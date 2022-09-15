#ifndef YALLIC_H
#define YALLIC_H

/**
 * @file yallic.h
 *
 * @author Zachary Puhl
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

#include <stddef.h>
#include <stdbool.h>



/**
 * The primary, generic linked-list structure.
 *   The structure simply maintains a max_count with a HEAD pointer.
 */
typedef struct __linked_list_t List_t;



/**
 * Initialize a new linked list. This initializes the structure and its override-able
 *   method pointers internally.
 *
 * @param max_size Maximum size of the created linked list. If this is set to 0 or NULL,
 *   the library assumes no theoretical limit to the length of the linked list.
 * @return A pointer to the allocated linked list. NULL on error.
 */
List_t* List__new( size_t max_size );

/**
 * Destroy a linked list. If the list hasn't been cleared--meaning a count-check on
 *   the list is greater than 0--then this function will attempt a shallow clear on
 *   the list. <br />__To prevent memory leaks__, use the `List__clean_deep( List_t* p_list )`
 *   method before invoking this function, or simply use the List__delete_deep() call
 *   if it's true that _all_ list data nodes are allocated/free-able heap blocks.<br />It
 *   also uses a double-pointer to automatically set the list reference to NULL so a
 *   dangling pointer isn't referenced later.
 *
 * @param pp_list The address of the the pointer to the target linked list.
 */
void List__delete_shallow( List_t** pp_list );

/**
 * Deeply delete underlying list node data, list nodes, and destroy the list. This method
 *   assumes the caller ran it explicitly and intentionally, and therefore tries to
 *   forcibly deallocate all list data regardless of what's allocated or given (unless the
 *   List_t pointer is NULL).<br />It also uses a double-pointer to automatically set the
 *   list reference to NULL so a dangling pointer isn't referenced later.
 *
 * @param pp_list The address of the the pointer to the target linked list.
 */
void List__delete_deep( List_t** pp_list );

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
 * Change a linked list's maximum capacity. If the new capacity is lower than the current
 *   count of elements in the list, an error is returned and nothing is changed. Otherwise,
 *   the propert is altered and the new maximum capacity is returned to the caller.
 *
 * @param p_list The linked list to resize.
 * @param new_max_size The new maximum list capacity.
 * @return The new maximum list capacity. Returns 0 on error (not -1 since the type is
 *   `size_t`).
 */
size_t List__resize( List_t* p_list, size_t new_max_size );

/**
 * Get the current capacity of a linked list.
 *
 * @param p_list The target linked list.
 * @return The current capacity of the linked list.
 */
size_t List__get_max_size( List_t* p_list );

/**
 * Shallow clear of list nodes. Deletes and frees all list nodes, but does _NOT_ attempt
 *   to free the values to which the nodes point. Any lists which have nodes pointing to
 *   the same underlying data (clones) are _not affected_.
 *
 * @param p_list The target linked list.
 */
void List__clear_shallow( List_t* p_list );

/**
 * Deep clear of list data. Deletes and free all list nodes, _as well as_ their underlying
 *   data pointers. Any clones (shallow copies) of the provided list should be deleted or
 *   attempts to dereference underlying data can result in undefined behavior. Additionally,
 *   attempts to use this on lists containing unallocated node data pointers will also
 *   result in undefined or problematic behavior.
 *
 * @param p_list The target linked list.
 */
void List__clear_deep( List_t* p_list );


/**
 * Add a node to the _tail end_ of the linked list. If the addition of the new node would
 *   cause the linked list to exceed its size limit, the operation is canceled in error.
 *
 * @param p_list The target linked list.
 * @param p_data A pointer to the data to add.
 * @return _-1_ on failure, or the index of the newly-added element on success.
 */
int List__add( List_t* p_list, void* p_data );

/**
 * Add a node to the linked list at the given 0-based index position. If adding the node
 *   to the list causes the tail of the list to go out-of-bounds (beyond the max_size),
 *   the old list tail will be lost.
 *
 * @param p_list The target linked list.
 * @param p_data A pointer to the data to add.
 * @param index A 0-based position at which to add the new node.
 * @return _-1_ on failure or out-of-bounds, or the index value to indicate success.
 */
int List__add_at( List_t* p_list, void* p_data, size_t index );

/**
 * Concatenate two lists in sequence. If the list concatenation causes the destination
 *   list length to go out-of-bounds, the operation will fail and is reverted. The source
 *   list is __not__ freed or otherwise altered in its structure or its data.
 *
 * @param p_list_dest The destination list onto which the source list is added.
 * @param p_list_src The list being added onto the destination list.
 * @return _-1_ on failure, or the new length of the destination list on success.
 */
int List__extend( List_t* p_list_dest, List_t* p_list_src );

/**
 * Concatenate two lists in sequence. If the list concatenation causes the destination
 *   list length to go out-of-bounds, the operation will fail and is reverted. The source
 *   list is __shallowly freed__ upon success.
 *
 * @param p_list_dest The destination list onto which the source list is added.
 * @param p_list_src The list being added onto the destination list.
 * @return _-1_ on failure, or the new length of the destination list on success.
 */
int List__merge( List_t* p_list_dest, List_t* p_list_src );

/**
 * Insert a list into another linked list at the given destination index. If the list
 *   concatenation causes the destiation list length to go out-of-bounds, the operation
 *   will fail and is reverted. The source list is __not__ freed or otherwise altered
 *   in its structure or its data.
 *
 * @param p_list_dest The destination list into which the source list is added.
 * @param p_list_src The list being added into the destination list.
 * @param index The index into the destination list at which to add the source list.
 * @return _-1_ on failure, or the new length of the destination list on success.
 */
int List__extend_at( List_t* p_list_dest, List_t* p_list_src, size_t index );

/**
 * Insert a list into another linked list at the given destination index. If the list
 *   concatenation causes the destiation list length to go out-of-bounds, the operation
 *   will fail and is reverted. The source list is __shallowly freed__ upon success.
 *
 * @param p_list_dest The destination list into which the source list is added.
 * @param p_list_src The list being added into the destination list.
 * @param index The index into the destination list at which to add the source list.
 * @return _-1_ on failure, or the new length of the destination list on success.
 */
int List__merge_at( List_t* p_list_dest, List_t* p_list_src, size_t index );


/**
 * Create a cloned (shallow) linked list from a subset of a larger source list. The
 *   two given indices are _inclusive_, meaning a range of 1-2 will include _both_
 *   elements 1 and 2 in the resulting subset clone, 2-5 will include elements 2, 3,
 *   4, & 5 in it, etc. etc.
 *
 * @param p_list The list to slice.
 * @param from_index The starting index to slice from *p_list*.
 * @param to_index The ending index to slice to from *p_list*.
 * @return Pointer to a new list containing shallow copies from the original linked
 *   within the specified range. _NULL_ on error.
 */
List_t* List__slice( List_t* p_list, size_t from_index, size_t to_index );


/**
 * Create a shallow copy of the given linked list and return a new linked list
 *   pointer. Shallow copies do not independently copy the underlying node data,
 *   only the structure of the linked list itself.
 *
 * @param p_list The list to copy.
 * @return A pointer to the list shallow copy. NULL on failure.
 */
List_t* List__clone( List_t* p_list );


/**
 * Fully and deeply copy a linked list. The new linked list returned is fully
 *   independent from the original list, and can be deeply freed/destroyed without
 *   affecting it (and vice-versa).
 *
 * @param p_list The target linked list to copy.
 * @param element_size The size of each of the underlying list elements.
 * @return Pointer to the newly-copied, independent linked list. _NULL_ on error.
 */
List_t* List__copy( List_t* p_list, size_t element_size );


/**
 * Search a list for the presence of a data pointer. If the data pointer is found in
 *   the linked list, its first occurrence index is returned.
 *
 * @param p_list The target linked list.
 * @param p_data The object/reference to seek inside the linked list.
 * @return _0_ if the value is not found in the list, _1_ on success.
 */
bool List__contains( List_t* p_list, void* p_data );

/**
 * Returns whether a linked list is empty or not.
 *
 * @param p_list The target linked list.
 * @return _0_ if the list contains one or more elements; _1_ if the list is empty.
 */
bool List__is_empty( List_t* p_list );


/**
 * Return the data pointer from the first list element (HEAD). If the linked list is empty,
 *   this will return _NULL_.
 *
 * @param p_list The target linked list.
 * @return The list's first data pointer. NULL on an empty or invalid linked list.
 */
void* List__get_first( List_t* p_list );

/**
 * Return the data pointer from the last list element (TAIL). If the linked list is empty,
 *   this will return _NULL_.
 *
 * @param p_list The target linked list.
 * @return The list's last data pointer. NULL on an empty or invalid linked list.
 */
void* List__get_last( List_t* p_list );

/**
 * Return the data pointer from the selected list element. If the linked list is empty,
 *   or the index is not valid, this will return _NULL_.
 *
 * @param p_list The target linked list.
 * @param index The 0-based index into the array to select.
 * @return The data pointer at the selected index, or NULL on an invalid index.
 */
void* List__get_at( List_t* p_list, size_t index );


/**
 * Searches the linked list for the provided data pointer and returns its index if found.
 *   Returns -1 if the data pointer was not found in the list.
 *
 * @param p_list The target linked list.
 * @param p_data The data pointer to search for in the list of nodes.
 * @return The index into the linked list where the first occurrence of the pointer is
 *   found. Returns _-1_ if the pointer was not found.
 */
int List__index_of( List_t* p_list, void* p_data );

/**
 * Searches the linked list for the final occurrence of the provided data pointer and
 *   returns its index if found. Returns -1 if the data pointer was not found in the list.
 *
 * @param p_list The target linked list.
 * @param p_data The data pointer to search for in the list of nodes.
 * @return The index into the linked list where the last occurrence of the pointer is
 *   found. Returns _-1_ if the pointer was not found.
 */
int List__last_index_of( List_t* p_list, void* p_data );


/**
 * Pop the first (HEAD) element off the list's stack and return its data pointer. This is
 *   a convenient way to use the linked list as a stack structure in tandem with the 'push'
 *   operation.
 *
 * @param p_list The target linked list.
 * @return The data pointer of the popped list element. _NULL_ if the list has no elements
 *   to pop.
 */
void* List__pop( List_t* p_list );

/**
 * Push a new element onto the first (HEAD) element of a list's node stack. This is a
 *   convenient way to use the linked list as a stack structure in tandem with the 'pop'
 *   operation.
 *
 * @param p_list The target linked list.
 * @param p_data The data pointer to add as the new list HEAD pointer (first element).
 * @return The new list length on success, _-1_ on failure (such as an out-of-bounds error).
 */
int List__push( List_t* p_list, void* p_data );


/**
 * Remove the first list node element from the list and returns its data pointer. If the
 *   list is empty, _NULL_ is returned instead.
 *
 * @param p_list The target linked list.
 * @return The data pointer of the removed list element, or _NULL_ on error.
 */
void* List__remove_first( List_t* p_list );

/**
 * Remove the last list node element from the list and returns its data pointer. If the
 *   list is empty, _NULL_ is returned instead.
 *
 * @param p_list The target linked list.
 * @return The data pointer of the removed list element, or _NULL_ on error.
 */
void* List__remove_last( List_t* p_list );

/**
 * Remove the selected list node element from the list and returns its data pointer. If
 *   the list is empty or if the selected index is invalid, _NULL_ is returned instead.
 *
 * @param p_list The target linked list.
 * @param index The index of the node to remove from the list.
 * @return The data pointer of the removed list element, or _NULL_ on error.
 */
void* List__remove_at( List_t* p_list, size_t index );

/**
 * Remove the first occurrence of the selected list node element and returns its data
 *   pointer. If the list is empty or if there is no occurrence of the provided data
 *   pointer, _NULL_ is returned.
 *
 * @param p_list The target linked list.
 * @param p_data The data pointer to search for in the list.
 * @return The data pointer of the removed list node.
 */
void* List__remove_first_occurrence( List_t* p_list, void* p_data );

/**
 * Remove the last occurrence of the selected list node element and returns its data
 *   pointer. If the list is empty or if there is no occurrence of the provided data
 *   pointer, _NULL_ is returned.
 *
 * @param p_list The target linked list.
 * @param p_data The data pointer to search for in the list.
 * @return The data pointer of the removed list node.
 */
void* List__remove_last_occurrence( List_t* p_list, void* p_data );


/**
 * Change the data pointer of the selected linked list node. If the index is out of
 *   bounds or if there's another error, _NULL_ is returned.
 *
 * @param p_list The target linked list.
 * @param index The index to select from the linked list.
 * @param p_new_data The new data pointer for the selected linked list node.
 * @return The previous data pointer of the selected list node. _NULL_ on error.
 */
void* List__set_at( List_t* p_list, size_t index, void* p_new_data );


/**
 * Return the length of the linked list.
 *
 * @param p_list The target linked list.
 * @return The length of the linked list.
 */
size_t List__length( List_t* p_list );

/**
 * Return the length of the linked list.
 *
 * @param p_list The target linked list.
 * @return The length of the linked list.
 */
size_t List__count( List_t* p_list );

/**
 * Return the length of the linked list.
 *
 * @param p_list The target linked list.
 * @return The length of the linked list.
 */
size_t List__size( List_t* p_list );


/**
 * Convert the linked list to a type-agnostic, linear memory space. This simply scrolls
 *   through each list node, copies the raw memory of each data pointer according to the
 *   provided element_size value, and pastes it into a contiguous chunk of memory. This
 *   means the new array of elements is ___separate and independent___ from the original
 *   linked list. Therefore, if the list is destroyed, the array will still be preserved
 *   in its own memory on the heap until freed separately.
 *
 * @param p_list The target linked list.
 * @param element_size The expected size of the underlying linked list type.
 * @param extra_bytes Amount of extra bytes to add onto the calloc memory allocation.
 * @return Pointer to the newly-allocated array on the heap. _NULL_ on error.
 */
void* List__to_array( List_t* p_list, size_t element_size, size_t extra_bytes );

/**
 * Convert an array of data to a linked list. This linearly iterates all memory in the source
 *   array, stepping by the given size up to the given count, and _copies_ data into a
 *   resulting linked list structure. The resulting objects are independent of each other: the
 *   list can be deleted without affecting memory held in the array, and vice-versa.
 *
 * @param p_array The initial contiguous segment of array data to read into a linked list.
 * @param element_size The size of each independent array element.
 * @param count The amount of array elements to copy into the linked list.
 * @param list_max_size The maximum size of the resulting linked list. _Cannot_ be less than
 *   the provided _count_ parameter.
 * @return A new, independent linked list object from the initial data. _NULL_ on error.
 */
List_t* List__from_array( void* p_array, size_t element_size, size_t count, size_t list_max_size );



/**
 * Iterate the elements in a linked list and perform an operation for each. When the
 *   individual operations are finished executing, the ending `callback` is executed
 *   and the result of the operation is stored in the *pp_result* parameter. This iterable
 *   mechanism is _intended_ to be customizable and flexible, allowing an initial input to
 *   be provided to each linked list item (mutable along the way as desired), as well as
 *   providing an in-memory way to return results from everything.
 *
 * @param p_list The list to iterate.
 * @param pp_result A generic double-pointer used to store the result of the iteration(s).
 * @param p_input A generic pointer to some data which is fed into each *action* call, as
 *   well as the callback function.
 * @param action A per-element operation which accepts the node data, the input data, and
 *   the result double-pointer, respectively.
 * @param callback A final, summary operation called after all iterations have finished.
 *   This accepts the input data and the result double-pointer as parameters respectively.
 * @return Nothing. The result of the operations performed by the action and callbacks
 *   are up to the caller, but they are stored in the given pointer reference *pp_result*.
 */
void List__for_each(
    List_t* p_list,
    void**  pp_result,
    void*   p_input,
    void    (*action)(void*, void*, void**),
    void    (*callback)(void*, void**)
);



#endif   /* YALLIC_H */
