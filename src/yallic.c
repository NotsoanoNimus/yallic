/**
 * @file yallic.c
 * @author Suriel (Zachary) Puhl
 * @date August 2022
 *
 * Yet Another Linked list Implementation in C.
 *
 */

#include "yallic.h"

#include <stdlib.h>
#include <string.h>

/**
 * A simple, internally-used macro to allocate a new linked list node item.
 *
 * @see ListNode_t*
 */
#define LIST_NODE_INITIALIZER (ListNode_t*)calloc( 1, sizeof(ListNode_t) )

static const unsigned long long __list_size_max_limit = 0xFFFFFFFFFFFFFFFF;   /**< Linked list maximum allowable count. */



/**
 * Represents individual, generic linked-list elements. These consist of a node pointer
 *   to some resource or data type, and a pointer to the adjacent (next) list node. These
 *   are never intended to be accessed directly outside of the main translation unit of
 *   yallic.
 *
 * @typedef ListNode_t
 * @struct ListNode_t
 */
struct __linked_list_node_t {
    void* data;   /**< Pointer to the underlying node data. */
    struct __linked_list_node_t* next;   /**< Pointer to the next linked list node. */
};

/**
 * Represents individual, generic linked-list elements. These consist of a node pointer
 *   to some resource or data type, and a pointer to the adjacent (next) list node. These
 *   are never intended to be accessed directly outside of the main translation unit of
 *   yallic.
 *
 * @see List_t
 */
typedef struct __linked_list_node_t ListNode_t;

/**
 * The primary, generic linked-list structure.
 *   The LIST maintains a max_count with a HEAD pointer.
 *
 * @typedef List_t
 * @struct List_t
 */
struct __linked_list_t {
    ListNode_t* head;   /**< The list's HEAD pointer. */
    size_t max_size;   /**< The list's maximum size, defined on instantiation. */
};



// Internal function prototypes as needed.
static ListNode_t* __List__get_last_node( List_t* p_list );
static ListNode_t* __List__get_node_at( List_t* p_list, size_t index );
static ListNode_t* __List__get_node_first_occurrence( List_t* p_list, void* p_data );
static ListNode_t* __List__get_node_last_occurrence( List_t* p_list, void* p_data );
static size_t __List__index_of_node( List_t* p_list, ListNode_t* p_node );



// Create a new linked list.
List_t* List__new( size_t max_size ) {
    if ( max_size <= 0 )
        max_size = __list_size_max_limit;

    List_t* p_list = (List_t*)calloc( 1, sizeof(List_t) );
    p_list->head = NULL;
    p_list->max_size = max_size;

    return p_list;
}


// Shallow deletion of list elements and the list allocation itself.
void List__delete_shallow( List_t* p_list ) {
    if ( NULL == p_list )  return;

    List__clear_shallow( p_list );

    free( p_list );
    return;
}


// Deeply delete all list nodes and the list pointer.
void List__delete_deep( List_t* p_list ) {
    if ( NULL == p_list )  return;

    List__clear_deep( p_list );

    free( p_list );
    return;
}


// Reverse a linked-list.
void List__reverse( List_t** pp_list ) {
    if (
           NULL == pp_list
        || NULL == *pp_list
        || List__length( *pp_list ) <= 0
    )  return;

    // New list structure.
    List_t* p_new = List__new( (*pp_list)->max_size );

    // Go from 0-LEN on the old list, PUSHing nodes onto the new list.
    //   0 --> 0, 1 --> 0, 2 --> 0
    //   Using the underlying stack-like mechanism of the list makes this simple and quick.
    ListNode_t* p_node = (*pp_list)->head;
    while ( NULL != p_node ) {
        List__push( p_new, p_node->data );
        p_node = p_node->next;
    }

    // Shallow deletion of old structure.
    List__delete_shallow( *pp_list );

    // Set the passed pointer to the new list, effectively modifying it in-place (by ref).
    *pp_list = p_new;
    return;
}


// Shrink or grow a list capacity to the given max_size. If the linked list contains more
//   elements than the new max_size, an error is returned. Otherwise, return the new max.
size_t List__resize( List_t* p_list, size_t new_max_size ) {
    if (
           NULL == p_list
        || new_max_size <= 0
        || (new_max_size < List__length( p_list ))
    )  return 0;

    p_list->max_size = new_max_size;
    return new_max_size;
}


// Wrapper method to get max_size property.
size_t List__get_max_size( List_t* p_list ) {
    return (NULL == p_list) ? 0 : p_list->max_size;
}


// Shallowly delete a linked list structure, but not the underlying resources.
void List__clear_shallow( List_t* p_list ) {
    if (  List__length( p_list ) > 0  ) {
        ListNode_t* p_node = p_list->head;
        while ( NULL != p_node ) {
            ListNode_t* p_node_shadow = p_node->next;
            free( p_node );
            p_node = p_node_shadow;
        }
    }
}


// Deeply free all held list resources.
void List__clear_deep( List_t* p_list ) {
    if (  List__length( p_list ) > 0  ) {
        ListNode_t* p_node = p_list->head;
        while ( NULL != p_node ) {
            // This is the only real difference between shallow and deep clears.
            if ( NULL != p_node->data )
                free( p_node->data );

            ListNode_t* p_node_shadow = p_node->next;
            free( p_node );
            p_node = p_node_shadow;
        }
    }
}


// Add an item onto the tail of a linked list.
int List__add( List_t* p_list, void* p_data ) {
    if (
           NULL == p_list
        || NULL == p_data
        || ((List__length( p_list ) + 1) > p_list->max_size)
    )  return -1;

    // Seek list tail.
    ListNode_t* p_tailnode = __List__get_last_node( p_list );

    // Init the new list node with the referenced data pointer.
    ListNode_t* p_new_node = LIST_NODE_INITIALIZER;
    p_new_node->data = p_data;

    // Put the new node in place.
    if ( NULL == p_tailnode ) {
        p_list->head = p_new_node;
    } else {
        p_tailnode->next = p_new_node;
    }

    // Return the place of the new node, which is just the end (length) of the linked list.
    return List__length( p_list );
}


// Add an item to a linked list somewhere in its chain of nodes.
int List__add_at( List_t* p_list, void* p_data, size_t index ) {
    if (
           NULL == p_list
        || NULL == p_data
        || ((List__length( p_list ) + 1) > p_list->max_size)
        || (index > List__length( p_list ))   // if len == 3, and list has 0,1,2; this is ok
    )  return -1;

    // If the new location is head, just insert it and be done; no looping.
    if ( 0 == index ) {
        List__push( p_list, p_data );
        return index;
    } else if ( List__length( p_list ) == index ) {
        List__add( p_list, p_data );
        return index;
    }

    // Get the preceding list node.
    ListNode_t* p_node_before = __List__get_node_at( p_list, (index-1) );
    if ( NULL == p_node_before )  return -1;

    // Get the following (next) node after the one being added.
    ListNode_t* p_node_after = p_node_before->next;

    // Create the new node.
    ListNode_t* p_new_node = LIST_NODE_INITIALIZER;
    p_new_node->data = p_data;

    // Insert the new node.
    p_node_before->next = p_new_node;
    p_new_node->next = p_node_after;

    // Return the index to indicate success.
    return index;
}

// Staple the src linked list to the end of the dest linked list.
int List__extend( List_t* p_list_dest, List_t* p_list_src ) {
    if (
           NULL == p_list_dest
        || ((List__length(p_list_dest) + List__length(p_list_src)) > p_list_dest->max_size)
    )  return -1;

    // Nothing to add.
    if (  NULL == p_list_src || 0 == List__length( p_list_src )  )
        return List__length( p_list_dest );

    // Get destination list tail.
    ListNode_t* p_tail = __List__get_last_node( p_list_dest );
    ListNode_t* p_tail_scroll = p_tail;
    if ( NULL == p_tail )  return -1;

    // Append elements.
    ListNode_t* p_src_scroll = p_list_src->head;
    while ( NULL != p_src_scroll ) {
        ListNode_t* p_new_node = LIST_NODE_INITIALIZER;
        p_new_node->data = p_src_scroll->data;

        // If adding fails, revert all added elements and return a failure.
        if ( -1 == List__add( p_list_dest, p_new_node )  ) {
            free( p_new_node );

            p_tail_scroll = p_tail;
            while ( NULL != p_tail_scroll ) {
                ListNode_t* p_tail_shadow = p_tail_scroll->next;   //save next lnode ptr
                free( p_tail_scroll );   //free lnode
                p_tail_scroll = p_tail_shadow;   //move to save point
            }

            p_tail->next = NULL;   //restore the original tail as the resulting tail
            return -1;
        }

        // Extend the list by setting the current tail's next to the new node, then moving to it.
        p_tail_scroll->next = p_new_node;
        p_tail_scroll = p_new_node;

        // Advance to the next item in the source list.
        p_src_scroll = p_src_scroll->next;
    }

    // Ensure the final tail node points to NULL as an enforcement.
    p_tail_scroll->next = NULL;

    // Return new destination linked list length.
    return List__length( p_list_dest );
}


// Insert the src linked list into the dest linked list at the index.
int List__extend_at( List_t* p_list_dest, List_t* p_list_src, size_t index ) {
    if (
           NULL == p_list_dest
        || ((List__length(p_list_dest) + List__length(p_list_src)) > p_list_dest->max_size)
        || (index > List__length( p_list_dest ))   // can be the new tail index, so 3 from 0,1,2 (len3)
    )  return -1;

    // Nothing to add.
    if (  NULL == p_list_src || 0 == List__length( p_list_src )  )
        return List__length( p_list_dest );

    // Get the preceding list node.
    ListNode_t* p_node_before = __List__get_node_at( p_list_dest, (index-1) );
    ListNode_t* p_before_link_save = p_node_before->next;   // preserve the old 'next' in case trouble
    if ( NULL == p_node_before )  return -1;

    // Append the list.
    ListNode_t* p_scroll = p_list_src->head;
    while ( NULL != p_scroll ) {
        // Create and add the new node.
        ListNode_t* p_new_node = LIST_NODE_INITIALIZER;
        p_new_node->data = p_scroll->data;

        // If the list being copied is about to end (tail), restore the save point as the 'next' ptr.
        //   This should re-establish the linked list chain properly.
        p_new_node->next = (NULL == p_scroll->next) ? p_before_link_save : NULL;

        p_node_before->next = p_new_node;
        p_scroll = p_scroll->next;
    }

    // Return new destination linked list length.
    return List__length( p_list_dest );
}


// Shallow clone of a linked list's structure. This does not copy underlying data.
List_t* List__clone( List_t* p_list ) {
    if ( NULL == p_list )  return NULL;

    List_t* p_new = List__new( p_list->max_size );

    ListNode_t* p_scroll = p_list->head;
    while ( NULL != p_scroll ) {
        if (  -1 == List__add( p_new, p_scroll->data )  ) {
            // If something goes wrong, revert the list changes and destroy the clone.
            List__delete_shallow( p_new );
            return NULL;
        }

        p_scroll = p_scroll->next;
    }

    return p_new;
}


// Slice a given linked list according to two indices.
List_t* List__slice( List_t* p_list, size_t from_index, size_t to_index ) {
    if ( from_index >= to_index || NULL == p_list )  return NULL;

    ListNode_t* p_start = __List__get_node_at( p_list, from_index );
    ListNode_t* p_end   = __List__get_node_at( p_list, to_index );
    if ( NULL == p_start || NULL == p_end )  return NULL;

    List_t* p_new = List__new( p_list->max_size );

    // From start to end of the slice, build up the new list sequentially.
    while ( p_start != p_end->next ) {
        if (  -1 == List__add( p_new, p_start->data )  ) {
            List__delete_shallow( p_new );
            return NULL;
        }

        p_start = p_start->next;
    }

    return p_new;
}


// Deep copy of a linked list. This creates a fully-independent copy of the provided list.
List_t* List__copy( List_t* p_list, size_t element_size ) {
    if ( NULL == p_list || element_size <= 0 )  return NULL;

    List_t* p_new = List__new( p_list->max_size );

    ListNode_t* p_scroll = p_list->head;
    while ( NULL != p_scroll ) {
        // Allocate a copy of the scroll node's data.
        void* p_new_data = calloc( 1, element_size );
        memcpy( p_new_data, p_scroll->data, element_size );

        if (  -1 == List__add( p_new, p_new_data )  ) {
            // If something goes wrong, revert the list changes and nuke the copied data.
            free( p_new_data );
            List__delete_deep( p_new );
            return NULL;
        }

        p_scroll = p_scroll->next;
    }

    return p_new;
}


// Gets whether the data pointer exists somewhere within the linked list.
int List__contains( List_t* p_list, void* p_data ) {
    ListNode_t* p_occ = __List__get_node_first_occurrence( p_list, p_data );

    return ( NULL == p_occ ) ? -1 : (__List__index_of_node( p_list, p_occ ));
}


// Gets the first data element (HEAD) of the linked list.
void* List__get_first( List_t* p_list ) {
    if ( NULL == p_list || NULL == p_list->head )  return NULL;

    return p_list->head->data;
}


// Gets the final data element (TAIL) of the linked list.
void* List__get_last( List_t* p_list ) {
    ListNode_t* p_last = __List__get_last_node( p_list );

    return ( NULL == p_last ) ? NULL : p_last->data;
}


// Gets the data element at the selected index from the linked list.
void* List__get_at( List_t* p_list, size_t index ) {
    ListNode_t* p_node = __List__get_node_at( p_list, index );

    return ( NULL == p_node ) ? NULL : p_node->data;
}


// Returns the 0-based array index of the data pointer.
int List__index_of( List_t* p_list, void* p_data ) {
    ListNode_t* p_node = __List__get_node_first_occurrence( p_list, p_data );

    return __List__index_of_node( p_list, p_node );
}


// Returns the final 0-based array index of the data pointer.
int List__last_index_of( List_t* p_list, void* p_data ) {
    ListNode_t* p_node = __List__get_node_last_occurrence( p_list, p_data );

    return __List__index_of_node( p_list, p_node );
}


// Pop off the current first element (HEAD) of the linked list, free the
//    node (NOT the data), and return its data pointer.
void* List__pop( List_t* p_list ) {
    if (
           NULL == p_list
        || NULL == p_list->head
        || (0 == List__length( p_list ))
    )  return NULL;

    // Save head node information.
    void* p_save = p_list->head->data;
    ListNode_t* p_next = p_list->head->next;

    // Free the old head and set it the the next/saved stack item.
    free( p_list->head );
    p_list->head = p_next;

    // Return the saved data pointer from the old head node.
    return p_save;
}


// Push a new HEAD element/node onto the linked list.
int List__push( List_t* p_list, void* p_data ) {
    if (
           NULL == p_list
        || NULL == p_data
        || ((List__length( p_list ) + 1) > p_list->max_size)
    )  return -1;

    // New linked list node.
    ListNode_t* p_node = LIST_NODE_INITIALIZER;
    p_node->data = p_data;

    // Swap in the new list head.
    p_node->next = p_list->head;
    p_list->head = p_node;

    return List__length( p_list );
}


// Remove the first list item (HEAD) and return its data pointer. Wrapper for 'pop'.
void* List__remove_first( List_t* p_list ) {
    return List__pop( p_list );
}


// Remove the final list item (TAIL) and return its data pointer.
void* List__remove_last( List_t* p_list ) {
    ListNode_t* p_tail = __List__get_last_node( p_list );
    if ( NULL == p_tail )  return NULL;

    ListNode_t* p_head = p_list->head;
    while ( NULL != p_head ) {
        if ( p_head->next == p_tail )
            p_head->next = NULL;   //sever list chain before the last node, cutting it out

        p_head = p_head->next;
    }

    // Save the data pointer, free the ListNode_t object, and return the old data pointer.
    void* p_save = p_tail->data;
    free( p_tail );

    return p_save;
}


// Remove the list node at the specified index and return its data pointer.
void* List__remove_at( List_t* p_list, size_t index ) {
    if (
           NULL == p_list
        || (0 == List__length( p_list ))
        || index >= List__length( p_list )
    )  return NULL;

    // Knock out easy cases first as applicable.
    if ( 0 == index )
        return List__pop( p_list );
    else if (  (List__length( p_list )-1) == index  )
        return List__remove_last( p_list );

    // If the element is in the middle somewhere, pop it out and bridge.
    ListNode_t* p_before = __List__get_node_at( p_list, (index-1) );
    ListNode_t* p_target = p_before->next; //__List__get_node_at( p_list,  index    );
    ListNode_t* p_after  = p_target->next; //__List__get_node_at( p_list, (index+1) );

    // A NULL 'after' is OK since it's probably in some way the list tail.
    if ( NULL == p_before || NULL == p_target )  return NULL;

    // Remove the node, saving its data pointer first, then bridge the gap.
    void* p_save = p_target->data;
    free( p_target );

    p_before->next = p_after;
    return p_save;
}


// Remove the first occurrence of the node data pointer.
void* List__remove_first_occurrence( List_t* p_list, void* p_data ) {
    int index = List__index_of( p_list, p_data );
    if ( -1 == index )  return NULL;

    return List__remove_at( p_list, (size_t)index );
}


// Remove the final occurrence of the node data pointer.
void* List__remove_last_occurrence( List_t* p_list, void* p_data ) {
    int index = List__last_index_of( p_list, p_data );
    if ( -1 == index )  return NULL;

    return List__remove_at( p_list, (size_t)index );
}


// Set the node data pointer at the selected location and return the old pointer.
void* List__set_at( List_t* p_list, size_t index, void* p_new_data ) {
    ListNode_t* p_node = __List__get_node_at( p_list, index );

    // Swap data pointer and return the old pointer in case caller wants to free.
    void* p_save = p_node->data;
    p_node->data = p_new_data;

    return p_save;
}


// Return the length of a linked list.
size_t List__length( List_t* p_list ) {
    if ( NULL == p_list )  return 0;

    size_t count = 0;
    ListNode_t* p_node = p_list->head;

    while ( NULL != p_node ) {
        count++;
        p_node = p_node->next;
    }

    return count;
}


// Wrapper/Alias function.
size_t List__count( List_t* p_list ) {
    return List__length( p_list );
}


// Copy all linked list elements to a contiguous chunk of memory. NULL on error.
void* List__to_array( List_t* p_list, size_t element_size ) {
    if (
           NULL == p_list
        || List__length( p_list ) <= 0
        || element_size <= 0
    )  return NULL;

    // Calculate the necessary space.
    size_t dest_size = (List__length( p_list ) * element_size);

    // Allocate the array space.
    void* const p_dest = calloc( 1, dest_size );
    void* p_dest_scroll = p_dest;   //walking/scrolling pointer.
    if ( NULL == p_dest )  return NULL;

    // Iterate all list data. If a null pointer exists, return error.
    ListNode_t* p_node = p_list->head;
    while ( NULL != p_node ) {
        if ( NULL == p_node->data ) {
            free( p_dest );
            return NULL;
        }

        memcpy( p_dest_scroll, p_node->data, element_size );

        p_node = p_node->next;
        p_dest_scroll += element_size;
    }

    // Return the pointer to the populated array space.
    return (void*)p_dest;
}


// Create a new linked list from an array.
List_t* List__from_array(
    void* p_array,
    size_t element_size,
    size_t count,
    size_t list_max_size
) {
    if (
           NULL == p_array
        || element_size <= 0
        || count <= 0
        || list_max_size < count
    )  return NULL;

    // Create the new list.
    List_t* p_list = List__new( list_max_size );

    // Walk the array. If at any point there's a failure, nuke the allocated nodes
    //   to prevent memory leaks.
    for ( size_t walk = 0; walk < count; walk++ ) {
        void* p_new_element = calloc( 1, element_size );
        memcpy( p_new_element, (p_array+(walk*element_size)), element_size );

        if (  NULL == p_new_element || -1 == List__add( p_list, p_new_element )  ) {
            if ( NULL != p_new_element )  free( p_new_element );
            List__delete_deep( p_list );
            return NULL;
        }
    }

    // Return the pointer to the new list.
    return p_list;
}


// For-each iterable functionality  [ or at least an attempt at it :') ].
void List__for_each(
    List_t* p_list,
    void**  pp_result,
    void*   p_input,
    void    (*action)(void*, void*, void**),
    void    (*callback)(void*, void**)
) {
    if (  NULL == p_list || (0 == List__length( p_list ))  )  return;

    ListNode_t* p_scroll = p_list->head;
    while ( NULL != p_scroll ) {
        (*action)( p_scroll->data, p_input, pp_result );

        p_scroll = p_scroll->next;
    }

    (*callback)( p_input, pp_result );
    return;
}




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Internal functions.

// Fetch the tail node of the list. NULL on error condition.
static ListNode_t* __List__get_last_node( List_t* p_list ) {
    if ( NULL == p_list )  return NULL;

    ListNode_t* p_last = p_list->head;
    while ( NULL != p_last ) {
        if ( NULL == p_last->next )  return p_last;
        p_last = p_last->next;
    }

    return NULL;
}


// Fetch the node at the given index. NULL on error condition.
static ListNode_t* __List__get_node_at( List_t* p_list, size_t index ) {
    if (
           NULL == p_list
        || (index >= List__length( p_list ))
     )  return NULL;

    ListNode_t* p_node = p_list->head;
    for ( size_t i = 0; i < index && NULL != p_node; i++ )
        p_node = p_node->next;

    return p_node;
}


// Fetch the first occurrence of the node with the given data pointer.
//   NULL on error condition or pointer not found.
static ListNode_t* __List__get_node_first_occurrence( List_t* p_list, void* p_data ) {
    if (
           NULL == p_list
        || NULL == p_data
        || (0 == List__length( p_list ))
    )  return NULL;

    ListNode_t* p_node = p_list->head;
    while ( NULL != p_node ) {
        if ( p_node->data == p_data )
            return p_node;
    }

    return NULL;
}


// Fetch the last occurrence of the node with the given data pointer.
//   NULL on error condition or pointer not found.
static ListNode_t* __List__get_node_last_occurrence( List_t* p_list, void* p_data ) {
    if (
           NULL == p_list
        || NULL == p_data
        || (0 == List__length( p_list ))
    )  return NULL;

    ListNode_t* p_node = p_list->head;
    ListNode_t* p_node_shadow = NULL;

    while ( NULL != p_node ) {
        if ( p_node->data == p_data )
            p_node_shadow = p_node;

        p_node = p_node->next;
    }

    return p_node_shadow;
}


// Get the index of the node pointer (NOT THE DATA OF THE NODE) from the linked list HEAD.
static size_t __List__index_of_node( List_t* p_list, ListNode_t* p_node ) {
    if (
           NULL == p_list
        || NULL == p_node
        || (0 == List__length( p_list ))
    )  return -1;

    size_t index = 0;
    ListNode_t* p_scroll = p_list->head;
    while (  NULL != p_scroll && p_scroll != p_node && index < List__length( p_list )  ) {
        p_scroll = p_scroll->next;
        index++;
    }

    return (index > List__length( p_list )) ? -1 : index;
}
