# yallic - Yet Another Linked List Implementation in C

No dependencies, no problems. Simple, maintainable, flexible, and ready to use.

Another one out in the wild can't hurt, _right_?


## Using

`yallic` tends to prefer function pointers where possible to organize linked lists. This
extensibility allows overriding of a struct pointer's predefined functions, for better or
for worse depending on your implementation of it.

Use of the library is fairly straightforward for most simple operations:
```c
#include <yallic.h>

#define CALLS 1000
// [...]

// Static list of values.
static size_t values[CALLS];

// Create a new linked list with a theoretically-unlimited size.
List_t* p_linkedlist = List__new( UINT64_MAX );

// Push some items onto the list, reverse it, call some function, then clear it: 1000x.
for ( size_t x = 0; x < CALLS; x++ ) {

    // Wasting CPU cycles for a joyous occasion.
    memset( &values[0], 0, CALLS*sizeof(size_t) );
    for ( size_t y = 0; y < CALLS; y++ ) {
        values[y] = y;
        p_linkedlist->push( &values[y] );
    }

    // Reverse the list. Flips the most-recently added node to index [0] and
    //   places the first list node at index [count(p_linkedlist)]. This uses a
    //   double-pointer because it technically organizes a wholly new list.
    //
    //   Since 1000 was the most recent push, flip the list so 0 is read up to 1000.
    List__reverse( &p_linkedlist );

    // Do something.
    __some_func_to_read_numbers_ascending( p_linkedlist );

    // Do a _shallow_ clear of the list elements. This only deletes and frees all list
    //   NODES but does NOT free the node resources which are pointed to. A _deep_ clear
    //   will attempt to free list contents and nodes alike, but leave the list ptr intact.
    //
    //   A deep clear would cause a crash here since the nodes do not point to heap data.
    p_linkedlist->clear_shallow();

}

// Discard the list and deeply free all allocated resources.
//List__delete( p_linkedlist );
// ^ The above is not done because the LL doesn't use heap data/resources.
//   Since the LL was shallowly cleared already in the final for-loop iteration, just free
//   the list pointer and be done with it.
free( p_linkedlist );

// [...]
```

## Compiling & Installing

It's intended to be as simple as `make; make install`.

If for some reason the shared object (`.so`) file isn't playing nicely, manually place it in the
typical library location (ex: `/usr/local/lib/`) and change file permissions accordingly.


## Documentation

Most function names are understandable and self-descriptive.
A list of available function for lists can be viewed by simply scrolling through the header (`yallic.h`) file.

Additionally, function declarations in the header file have helpful descriptions for those in need!
