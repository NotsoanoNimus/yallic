# yallic - Yet Another Linked List Implementation in C

No dependencies, no problems. Simple, maintainable, flexible, extensible, and ready to use.

The `yallic` design is heavily inspired by
[Java's implementation](https://docs.oracle.com/javase/7/docs/api/java/util/LinkedList.html)
(from JDK 7) of generic linked list types, with some other functions pepper in.

See the [project documentation](https://xmit.xyz/yallic/yallic_8h.html) for usage information,
or just thumb through the `yallic.h` header file. This is quite a tiny and portable library.

Pronounced as `y'all-ee-see` in your thickest American Southern twang.
Another one out in the wild can't hurt, _right_?


## Features

- [X] Simple, flexible, extensible`(, [xyz]ible)*`.
- [X] Generic.
- [X] Well-documented.
- [X] Intuitive (I hope).
- [X] Tiny.


### Why?

Since I've been ramping up my exploration of C, I need a comfortable way to use linked lists
in C projects with _minimal bloat or dependencies_, so I figured why not roll a small library
to use as needed? This library:

1. Wraps in everything necessary to manipulate dynamic lists.
2. Uses a single header file.
3. Self-contains its documentation.

The project can act as a great beginner's guide to linked list operation and design too!


## Usage

Using the library is fairly straightforward for most simple operations:
```c
#include <yallic.h>

#define CALLS 1000
// [...]

// Static list of values.
static size_t values[CALLS];

// Create a new linked list with a theoretically-unlimited size (UINT64_MAX).
List_t* p_linkedlist = List__new( 0 );

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
    List__clear_shallow( p_linkedlist );

}

// Discard the list and (shallowly) free all list nodes (but not underlying ptrs).
List__delete_shallow( p_linkedlist );

// [...]
```

## Compiling & Installing

It's intended to be as simple as `make release; make install` if you intend to use the library in
multiple different projects. This will place the _static library_ in `/usr/local/lib/` and the
header file in `/usr/local/include/`.

If you only want to compile the _static library_ for linkage in a single project, just `make` and
use the `src/yallic.h` and `lib/libyallic.a` as you wish in the target projects.


## Minutiae

- Yes, I am aware that `List` is technically different from a `LinkedList` -- however, I've chosen
to keep the function names as they are using the `List__` prefix. If someone is using this project
with the intent to have dynamically-sized lists, I don't think the distinction will be necessary,
and it's really just pedantic at that point.
- I do plan to maintain and update the project and its documentation as I need to. Though I do
foresee this being a set-and-forget repository: if I haven't updated in a long time, rest assured
the project will still work at the very least just as well as it did when I initially wrote it.
