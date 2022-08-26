/**
 * @file yallic.c
 * @author Suriel (Zachary) Puhl
 * @date August 2022
 *
 */

#include "yallic.h"

#include <stdlib.h>
#include <string.h>

#define NEW_LIST(name,size) \
    List_t* name = (List_t*)calloc( 1, sizeof(List_t) ); \
    name->head = NULL; name->max_size = size;
#define LIST_NODE_INITIALIZER (ListNode_t*)calloc( 1, sizeof(ListNode_t) )
#define NEW_LIST_NODE(name) ListNode_t* name = LIST_NODE_INITIALIZER;

static const unsigned long long __list_size_max_limit = 0xFFFFFFFFFFFFFFFF;   /**< Linked list maximum allowable count. */



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
 * @typedef List_t yallic.h
 * @see ListNode_t
 */
struct __linked_list_t {
    ListNode_t* head;   /**< The list's HEAD pointer. */
    size_t max_size;   /**< The list's maximum size, defined on instantiation. */
};



// Internal function prototypes as needed.
static ListNode_t* __List__get_last_node( List_t* p_list );



// Create a new linked list.
List_t* List__new( size_t max_size ) {
    NEW_LIST(p_list,max_size);
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
        List__push( p_new, p_node );
        p_node = p_node->next;
    }

    // Shallow deletion of old structure.
    List__delete_shallow( *pp_list );

    // Set the passed pointer to the new list, effectively modifying it in-place (by ref).
    *pp_list = p_new;
    return;
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

    // Create the new node.
    ListNode_t* p_new_node = LIST_NODE_INITIALIZER;
    p_new_node->data = p_data;

    // If the new location is head, just insert it and be done; no looping.
    if ( 0 == index ) {
        p_new_node->next = p_list->head;
        p_list->head = p_new_node;
    }

    size_t i = 1;   // important: starts at 1
    ListNode_t* p_node_before = p_list->head;

    // Get up to the node before where the new one is being inserted.
    while ( NULL != p_node_before && i < index ) {
        p_node_before = p_node_before->next;
        i++;
    }

    // Shouldn't happen, but if it does, need to bail here.
    if ( NULL == p_node_before ) {
        free( p_new_node );
        return -1;
    }

    // Insert the new node.
    ListNode_t* p_node_after = p_node_before->next;
    p_node_before->next = p_new_node;
    p_new_node->next = p_node_after;

    // Return the index to indicate success.
    return index;
}

// Staple the src linked list to the end of the dest linked list.
int List__concat( List_t* p_list_dest, List_t* p_list_src ) {
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

    // Return new linked list length.
    return List__length( p_list_dest );
}

// Insert the src linked list into the dest linked list at the index.
int List__insert_at( List_t* p_list_dest, List_t* p_list_src, size_t index ) {}

List_t* List__clone( List_t* p_list ) {}
int List__contains( List_t* p_list, void* p_data ) {}
void* List__get_first( List_t* p_list ) {}
void* List__get_last( List_t* p_list ) {}
void* List__get_at( List_t* p_list, size_t index ) {}
int List__index_of( List_t* p_list, void* p_data ) {}
void* List__last_index_of( List_t* p_list, void* p_data ) {}
void* List__pop( List_t* p_list ) {}
int List__push( List_t* p_list, void* p_data ) {}
void* List__remove_first( List_t* p_list ) {}
void* List__remove_last( List_t* p_list ) {}
void* List__remove_at( List_t* p_list, size_t index ) {}
void* List__remove_first_occurrence( List_t* p_list, void* p_data ) {}
void* List__remove_last_occurrence( List_t* p_list, void* p_data ) {}


// Set the node data pointer at the selected location and return the old pointer.
void* List__set_at( List_t* p_list, size_t index, void* p_new_data ) {
    if (
           NULL == p_list
        || NULL == p_new_data
        || (index >= List__length( p_list ))   //index is 0-based, len is not, thus >=
    )  return NULL;

    // Get the node at the index.
    size_t i = 0;
    ListNode_t* p_node = p_list->head;
    while ( NULL != p_node && i < index ) {
        p_node = p_node->next;
        i++;
    }

    // Just in case.
    if ( NULL == p_node )  return NULL;

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




//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////
// Internal functions.
static ListNode_t* __List__get_last_node( List_t* p_list ) {
    if ( NULL == p_list )  return NULL;

    ListNode_t* p_last = p_list->head;
    while ( NULL != p_last ) {
        if ( NULL == p_last->next )  return p_last;
        p_last = p_last->next;
    }

    return NULL;
}
