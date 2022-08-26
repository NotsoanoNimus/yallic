# yallic - Yet Another Linked List Implementation in C

No dependencies, no problems. Simple, maintainable, flexible, and ready to use.

Pronounced as `y'all-ee-see` in your thickest American Southern twang.

Another one out in the wild can't hurt, _right_?


## Using

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
        List__push( p_linkedlist, &values[y] );
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
    List__clear( p_linkedlist );

}

// Discard the list and (shallowly) free all list nodes (but not underlying ptrs).
List__delete( p_linkedlist );

// [...]
```

## Compiling & Installing

It's intended to be as simple as `make; make install` if you intend to use the shared library.

If for some reason the shared object (`.so`) file isn't playing nicely, manually place it in the
typical library location (ex: `/usr/local/lib/`) and change file permissions accordingly.

If you only want to compile the _static library_ for linkage in your own projects, use `make slib`
to create the `lib/libyallic.a` file and use that with the `yallic.h` file in your project.


## Documentation

Most function names are understandable and self-descriptive.
A list of available function for lists can be viewed by simply scrolling through the header (`yallic.h`) file.

Additionally, function declarations in the header file have helpful descriptions for those in need!
