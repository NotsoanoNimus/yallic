/*
 * lists_test.c
 *
 * Unit-testing the yallic library.
 *
 */

#include "../src/yallic.h"
#include "../src/yallic.c"
#include <criterion/criterion.h>

#include <time.h>
#include <stdio.h>
#include <signal.h>



// Dummy test to ensure Criterion is OK. :)
Test(dummytests, dummyequal) {
    int i = 5;
    cr_expect( 5 == i, "i should equal 5" );
}



// Global functions and macros.

static void* dummy_alloc(void) {  return calloc( 1, 1 );  }

static List_t* __create_and_populate( size_t count ) {
    List_t* p_test = List__new( count );
    srand( (unsigned)time(NULL) );   //eh, don't care if multiple times

    for ( size_t i = 0; i < count; i++ ) {
        int* p_data = (int*)calloc( 1, sizeof(int*) );
        *p_data = (int)(i * ((rand()%20)+1));
        cr_assert( -1 != List__add( p_test, p_data ),
            "Items should be add-able up to the limit" );
    }

    return p_test;
}

#define TEST_SETUP \
    List_t* p_test = __create_and_populate( 100 ); \
    cr_expect(  100 == List__length( p_test ), "List should be populated"  ); \
    \
    List_t* p_t1   = __create_and_populate( 100 ); \
    cr_expect(  100 == List__length( p_t1 ), "List should be populated"  ); \
    \
    List_t* p_t2   = __create_and_populate( 100 ); \
    cr_expect(  100 == List__length( p_t2 ), "List should be populated"  );

#define TEST_TEARDOWN \
    List__delete_deep( &p_test ); \
    cr_expect(  0 == List__length( p_test ), "List should be deleted"  ); \
    \
    List__delete_deep( &p_t1 ); \
    cr_expect(  0 == List__length( p_t1 ), "List should be deleted"  ); \
    \
    List__delete_deep( &p_t2 ); \
    cr_expect(  0 == List__length( p_t2 ), "List should be deleted"  );


static void lists_setup(void) {  }
static void lists_teardown(void) {  }
TestSuite( listops, .init = lists_setup, .fini = lists_teardown );

#define TEST_LISTOPS( name, ... ) \
Test( listops, name ) { \
    TEST_SETUP \
    __VA_ARGS__ \
    TEST_TEARDOWN \
}



TEST_LISTOPS( merge,
    // ^ Extend wrapper
    List__pop( p_t1 );
    List__pop( p_t1 );
    cr_assert(  98 == List__length( p_t1 ), "List t1 should be length 98"  );

    List_t* p_t2a = List__new( 2 );
    void* d1 = dummy_alloc();
    void* d2 = dummy_alloc();
    List__push( p_t2a, d2 );
    List__push( p_t2a, d1 );
    cr_assert(  2 == List__length( p_t2a ) && d1 == List__get_first( p_t2a ),
        "List elements should be in order"  );

    cr_assert(  -1 != List__merge( p_t1, p_t2a ), "List should be able to merge"  );
    cr_assert(  0 == List__length( p_t2a ), "Second source list should be shallowly empty"  );
    cr_assert(  100 == List__length( p_t1 ), "First destination list should be 100 elements"  );
    cr_assert(  d2 == List__get_last( p_t1 ), "Final list should be in proper order"  );

    List__delete_deep( &p_t2a );
);

TEST_LISTOPS( merge_at,
    // ^ Extend-At wrapper
    size_t count = 40;

    int* p[count];
    int  i[count];

    List_t* p_src = List__new( count );
    for ( size_t x = 0; x < count; x++ ) {
        p[x] = (int*)calloc( 1, sizeof(int) );
        i[x] = (rand() % 200) + 1;
        *((int*)(p[x])) = i[x];

        cr_assert(  -1 != List__add( p_src, p[x] ), "List src should be growable"  );
        free(  List__pop( p_test )  );
    }

    cr_assert(  (100-count) == List__count( p_test ), "Improper dest size"  );
    cr_assert(  count == List__count( p_src ), "Improper src size"  );


    // Brief aside to test extend-at
    List_t* p_dummy = List__new( 0 );
    for ( size_t x = 1; x < 6; x++ )
        List__add( p_dummy, (void*)x );

    cr_assert(  5 == List__length( p_dummy ),
        "List p_dummy is not sized 5; got '%lu'", List__length( p_dummy )  );
    cr_assert(  -1 != List__extend_at( p_dummy, p_src, 2 ), "List extension for p_dummy failed"  );
    cr_expect(  (count+5) == List__length( p_dummy ),
        "List p_dummy should extend properly; length is '%lu' but wanted '%lu'",
        List__length( p_dummy ), (count+5)  );

    cr_expect(  (void*)0x2 == List__get_at( p_dummy, 1 ),
        "Dummy pointer should be 0x1 in place 1; got '%p'", List__get_at( p_dummy, 1 )  );

    cr_expect(  p[0] == List__get_at( p_dummy, 2 ),
        "Dummy pointer should be '%p' at place 2; got '%p'", p[0], List__get_at( p_dummy, 2 )  );

    List__delete_shallow( &p_dummy );


    // Resume merge testing
    cr_assert(  -1 != List__merge_at( p_test, p_src, 19 ), "Merge-At failure"  );
    cr_assert(  100 == List__length( p_test ) && 0 == List__length( p_src ),
        "Merge-At should add onto the dest while shallowly freeing the src"
        " -- p_test (%lu), p_src (%lu)", List__length( p_test ), List__length( p_src )  );

    for ( size_t x = 19; x < count; x++ ) {
        cr_assert(  p[x-19] == List__get_at( p_test, x ),
            "Data pointer at position '%lu' is not correct", (x-19)  );
        cr_assert(  i[x-19] == *((int*)(List__get_at( p_test, x ))),
            "Underlying data at position '%lu' is not correct", (x-19)  );
    }

    List__delete_shallow( &p_src );
);

TEST_LISTOPS( extend_at_head_or_tail,
    List_t* p_new = List__new( 0 );
    List_t* p_new_tail = List__new( 0 );

    List_t* p_localtest = List__new( 100 );
    for ( size_t x = 0; x < 100; x++ ) {
        int* p_i = (int*)calloc( 1, sizeof(int) );
        *p_i = (rand() % 10000) + 1;
        cr_assert(  -1 != List__push( p_localtest, p_i ), "List should be growable"  );
    }

    int* i1 = (int*)calloc( 1, sizeof(int) );
    int* i2 = (int*)calloc( 1, sizeof(int) );
    *i1 = 7;
    *i2 = 9;

    List__push( p_new, i2 );
    List__push( p_new, i1 );
    List__push( p_new_tail, i2 );
    List__push( p_new_tail, i1 );

    cr_assert(  2 == List__count( p_new ), "Head list count invalid"  );
    cr_assert(  2 == List__count( p_new_tail ), "Tail list count invalid"  );

    cr_assert(  -1 != List__extend_at( p_new, p_localtest, 0 ), "extend_at HEAD failed"  );
    cr_assert(  -1 != List__extend_at( p_new_tail, p_localtest, 2 ), "extend_at TAIL failed"  );

    // The extend doesn't clear the src like merge does; this is a double-check.
    cr_assert(  100 == List__length( p_localtest ),
        "Invalid length of p_localtest (%lu/%lu)", List__length( p_localtest ), 100  );

    cr_assert(  102 == List__length( p_new ),
        "Invalid length of p_new (%lu/%lu)", List__length( p_new ), 102  );
    cr_assert(  102 == List__length( p_new_tail ),
        "Invalid length of p_new_tail (%lu/%lu)", List__length( p_new_tail ), 102  );

    int val1 = *((int*)(List__get_at( p_new, 100 )));
    cr_assert(  7 == val1, "p_new index 100 expected 7 but got '%d'", val1  );
    int val2 = *((int*)(List__get_at( p_new, 101 )));
    cr_assert(  9 == val2, "p_new index 100 expected 7 but got '%d'", val2  );

    int valt1 = *((int*)(List__get_at( p_new_tail, 0 )));
    cr_assert(  7 == valt1, "p_new_tail index 0 expected 7 but got '%d'", valt1  );
    int valt2 = *((int*)(List__get_at( p_new_tail, 1 )));
    cr_assert(  9 == valt2, "p_new_tail index 1 expected 7 but got '%d'", valt2  );

    List__delete_deep( &p_new );
);

TEST_LISTOPS( extend_at_empty_dest,
    List_t* p_new    = List__new( 0 );
    List_t* p_new_at = List__new( 0 );

    free(  List__pop( p_test )  );
    void* d1 = dummy_alloc();
    cr_assert(  -1 != List__push( p_test, d1 ), "List should be growable"  );

    cr_assert(  -1 != List__extend( p_new, p_test ), "List extend failed"  );
    cr_assert(  -1 != List__extend( p_new_at, p_test ), "List extend_at failed"  );

    cr_expect(  100 == List__length( p_new ),
        "List extend result should be 100 but got '%lu'", List__length( p_new )  );
    cr_expect(  d1 == List__get_first( p_new ), "Pointer '%p' should be first in p_new"  );

    cr_expect(  100 == List__length( p_new_at ),
        "List extend result should be 100 but got '%lu'", List__length( p_new_at )  );
    cr_expect(  d1 == List__get_first( p_new_at ), "Pointer '%p' should be first in p_new_at"  );

    List__delete_shallow( &p_new );
    List__delete_shallow( &p_new_at );
);

TEST_LISTOPS( clear_deep,
    for ( size_t x = 0; x < 10; x++ )  List__remove_last( p_test );
    cr_assert(  90 == List__length( p_test ), "List should be trimmed by 10 elements"  );

    for ( size_t x = 0; x < 5; x++ )  List__add(  p_test, dummy_alloc()  );
    cr_assert(  95 == List__length( p_test ), "List should be trimmed by 10 elements"  );

    List__clear_deep( p_test );
    cr_assert(  0 == List__length( p_test ), "List clear_deep should empty the list"  );
);

TEST_LISTOPS( remove_last_to_end,
    for ( size_t x = 0; x < 100; x++ )  free(  List__remove_last( p_test )  );

    cr_assert(  0 == List__length( p_test ),
        "List should be empty; is '%lu' elements instead", List__length( p_test )  );

    void* d1 = dummy_alloc();
    List__add( p_test, d1 );
    cr_assert(  d1 == List__get_last( p_test ), "Last item should be '%p' (dummy)", d1  );
);

TEST_LISTOPS( pop,
    free(  List__pop( p_test )  );
    cr_assert(  99 == List__length( p_test ), "List pop should remove one element"  );

    free(  List__remove_first( p_test )  );
    cr_assert(  98 == List__length( p_test ), "List remove_first should remove one element"  );
);

TEST_LISTOPS( add_remove_replace_reverse,
    free(  List__pop( p_test )  );
    free(  List__pop( p_test )  );

    void* d1 = dummy_alloc();
    void* d2 = dummy_alloc();

    List__add( p_test, d1 );
    List__add( p_test, d2 );

    free(  List__remove_last( p_test )  );
    cr_assert(  d1 == List__get_last( p_test ),
        "List remove_last should properly remove old items"  );

    void* d3 = dummy_alloc();
    List__add( p_test, d3 );
    cr_assert(  d3 == List__get_last( p_test ),
        "List add should add onto a list dynamically"  );

    List__pop( p_test );
    List__pop( p_test );

    void* d4 = dummy_alloc();
    void* d5 = dummy_alloc();

    List__push( p_test, d4 );
    List__push( p_test, d5 );
    cr_assert(  d5 == List__get_first( p_test ),
        "List push should add element onto list beginning"  );

    List__reverse( &p_test );
    cr_assert(  d5 == List__get_last( p_test ),
        "List reverse should place first element last"  );

    free(  List__remove_last( p_test )  );
    cr_assert(  d4 == List__get_last( p_test ),
        "List remove_last should properly remove old items"  );
);

TEST_LISTOPS( overflow_add,
    void* d1 = dummy_alloc();
    int res = List__add( p_test, d1 );
    cr_assert( -1 == res, "Lists should not allow addition beyond their max_size properties" );
    free( d1 );

    free(  List__pop( p_test )  );

    int* p_idata = (int*)calloc( 1, sizeof(int) );  *p_idata = 600;

    cr_assert(  -1 != List__add_at( p_test, p_idata, 50 ),
        "Adding at index 50 should be possible"  );
    cr_assert(  p_idata == List__get_at( p_test, 50 ),
        "Index 50 should equal the int ptr '%p'", p_idata  );

    cr_assert(  -1 == List__add_at( p_test, p_idata, 50 ),
        "Trying to add at an index when at max_size should error"  );
);

TEST_LISTOPS( overflow_push,
    void* d1 = dummy_alloc();

    cr_expect(  -1 == List__push( p_test, d1 ),
        "Lists should not allow pushing beyond their max_size properties"  );

    free( d1 );
);

TEST_LISTOPS( overflow_extend,
    List_t* p_t1a = __create_and_populate( 10 );
    cr_assert(  14 == List__resize( p_t1a, 14 ),
        "Linked list should be resizeable"  );

    List_t* p_t2a = __create_and_populate( 5 );
    cr_assert(  -1 == List__extend( p_t1a, p_t2a ),
        "Linked list should not be able to extend out of bounds"  );

    List_t* p_s1a = List__slice( p_t2a, 1, 5 );
    cr_assert(  -1 != List__extend( p_t1a, p_s1a ),
        "Linked list should be able to extend up to its limit"  );

    List__delete_deep( &p_t1a );
    List__delete_deep( &p_t2a );
    List__delete_deep( &p_s1a );
);

TEST_LISTOPS( slice,
    void* p[5] = {0};
    for ( size_t x = 0; x < 5; x++ ) {
        p[x] = dummy_alloc();
        free(  List__pop( p_test )  );
        cr_assert(  -1 != List__add( p_test, p[x] ),
            "List should have space to add" );
    }
    cr_assert(  100 == List__length( p_test ),
        "Test list should be 100 elements; got '%lu'", List__length( p_test )  );

    List_t* p_slice = List__slice( p_test, 95, 99 );
    cr_assert(  NULL != p_slice && 5 == List__length( p_slice ),
        "List slice should yield array of size 5; got '%lu'", List__length( p_slice )  );

    for ( size_t x = 0; x < 5; x++ ) {
        cr_assert(  p[x] == List__get_at( p_slice, x ),
            "List elements are not properly ordered"  );
    }

    List__delete_deep( &p_slice );
);

TEST_LISTOPS( deep_copy_and_clone,
    List_t* p1 = List__new( 5 );

    int* p[5] = {0};
    int i[5]  = {0};
    for ( size_t x = 0; x < 5; x++ ) {
        p[x] = (int*)calloc( 1, sizeof(int) );

        *((int*)(p[x])) = ((rand() % 20) + 1);
        i[x] = *((int*)(p[x]));

        cr_assert(  -1 != List__add( p1, p[x] ), "List p1 should be add-able"  );
    }
    cr_assert(  5 == List__length( p1 ), "List p1 should be populated"  );

    List_t* p1_copy = List__copy( p1, sizeof(int) );
    cr_assert(  5 == List__length( p1_copy ),
        "List p1 should be copied properly; got (%lu/5)", List__length( p1_copy )  );

    List__delete_deep( &p1 );
    cr_expect(  NULL == p1 && 0 == List__length( p1 ),
        "List p1 was not properly deeply freed"  );

    List_t* p1_clone = List__clone( p1_copy );

    for ( size_t x = 0; x < 5; x++ ) {
        // Ensure (1) the pointers in the data are not the same, and (2) the underlying integers match.
        cr_assert(  p[x] != List__get_at( p1_copy, x ) && i[x] == *((int*)(List__get_at( p1_copy, x ))),
            "List p1_copy should be ordered properly with separate pointers but same underlying value"  );
        // The clone should have the same data pointers as its source array.
        cr_assert(  List__get_at( p1_copy, x ) == List__get_at( p1_clone, x ),
            "Underlying clone pointers must match the parent list, mismatched at '%lu'", x  );
    }

    List__delete_shallow( &p1_copy );
    cr_assert(  5 == List__length( p1_clone ),
        "The clone should remain even if the parent shallowly deletes"  );

    for ( size_t x = 0; x < 5; x++ ) {
        cr_assert(  i[x] == *((int*)(List__get_at( p1_clone, x ))),
            "Clone pointers should deref to the original int values"  );
    }

    List__delete_deep( &p1_clone );
);

TEST_LISTOPS( set_at,
    void* d1 = dummy_alloc();
    free(  List__set_at( p_test, 45, d1 )  );

    cr_assert(  d1 == List__get_at( p_test, 45 ),
        "List set-at should modify the underlying data pointer"  );
);

TEST_LISTOPS( remove_at,
    void* d1 = dummy_alloc();

    free(  List__set_at( p_test, 46, d1 )  );

    free(  List__remove_at( p_test, 45 )  );

    cr_assert(  d1 == List__get_at( p_test, 45 ) && 99 == List__count( p_test ),
        "List remove-at should properly squash the list"  );
);

TEST_LISTOPS( add_at,
    void* d1 = dummy_alloc();

    free(  List__pop( p_test )  );

    cr_assert(  -1 != List__add_at( p_test, d1, 27 ),
        "List should have space to add"  );

    cr_assert(  d1 == List__get_at( p_test, 27 ),
        "List data pointer at index 27 should match"  );
);

TEST_LISTOPS( contains,
    void* d1 = dummy_alloc();
    void* d2 = dummy_alloc();
    void* d3 = dummy_alloc();

    free(  List__pop( p_test )  );
    free(  List__pop( p_test )  );

    cr_assert(  -1 != List__push( p_test, d1 ), "List should be growable"  );
    cr_assert(  -1 != List__add( p_test, d2 ), "List should be growable"  );

    cr_assert(  List__contains( p_test, d1 ) > 0, "List should contain pointer d1"  );
    cr_assert(  List__contains( p_test, d2 ) > 0, "List should contain pointer d2"  );

    cr_assert(  0 == List__contains( p_test, d3 ), "List should not contain d3"  );
    free( d3 );
);

TEST_LISTOPS( get_max_and_resize,
    cr_assert(  100 == List__get_max_size( p_test ), "Improper max size"  );

    for ( size_t x = 0; x < 15; x++ )
        free(  List__pop( p_test )  );
    cr_assert(  100 == List__get_max_size( p_test ), "Improper max size after list size change"  );

    cr_assert(  0 == List__resize( p_test, 5 ),
        "List should not be resizeable below its element count"  );

    cr_assert(  0 != List__resize( p_test, 553 ),
        "List should be resizeable within element constraints"  );
    cr_assert(  553 == List__get_max_size( p_test ),
        "List max size should match the most recent resize"  );

    List_t* p_new = List__new( 0 );
    cr_assert(  0xFFFFFFFFFFFFFFFF == List__get_max_size( p_new ),
        "Improper max size for unbounded list; got '%lu'", List__get_max_size( p_new )  );
    List__delete_deep( &p_new );
);



// Callback and for-each testing.
struct __test_iter_t {
    int was_callbackd;
};
struct __test_res_t {
    size_t add_result;
    int stored;
};
void __test_action( void* p_data, void* p_input, void** pp_result ) {
    // Add 4 for each element in the test list.
    if ( NULL != pp_result && NULL != *pp_result ) {
        ((struct __test_res_t*)(*pp_result))->add_result =
            ((struct __test_res_t*)(*pp_result))->add_result + 4;
    }
}
void __test_callback( void* p_input, void** pp_result ) {
    // Set some static numbers to prove a callback can affect out-of-scope variables.
    if ( NULL != pp_result && NULL != *pp_result )
        ((struct __test_res_t*)(*pp_result))->stored = 777;

    ((struct __test_iter_t*)p_input)->was_callbackd = 1;
}
void __test_action_print( void* p_data, void* p_input, void** pp_result ) {
    printf( "Node '%d' ==> |%lu|\n", ((struct __test_iter_t*)p_input)->was_callbackd, *((size_t*)p_data) );
    (((struct __test_iter_t*)p_input)->was_callbackd)++;
}
void __test_callback_print( void* p_input, void** pp_result ) {
    printf( "PRINTING COMPLETE (callback)\n" );
}


TEST_LISTOPS( foreach_print,
    List_t* p_localtest = __create_and_populate( 5 );

    struct __test_res_t* p_res =
        (struct __test_res_t*)calloc( 1, sizeof(struct __test_res_t) );
    struct __test_iter_t* p_iter =
        (struct __test_iter_t*)calloc( 1, sizeof(struct __test_iter_t) );

    printf( "FOR-EACH LOOP TESTING...\n" );
    List__for_each( p_localtest, (void**)&p_res, (void*)p_iter,
        &__test_action_print, &__test_callback_print );

    p_iter->was_callbackd = 0;
    List__reverse( &p_localtest );
    printf( "[REVERSED] FOR-EACH LOOP TESTING...\n" );
    List__for_each( p_localtest, (void**)&p_res, (void*)p_iter,
        &__test_action_print, &__test_callback_print );

    free( p_iter ); free( p_res );
    List__delete_deep( &p_localtest );
);

TEST_LISTOPS( foreach_arithmetic,
    List_t* p_localtest = List__new( 0 );

    int limit = (rand() % 500) + 500;
    for ( size_t i = 0; i < limit; i++ ) {
        size_t* p_d = (size_t*)calloc( 1, sizeof(size_t) );
        cr_expect( -1 != List__add( p_localtest, p_d ), "List should be add-able" );
    }

    struct __test_res_t* p_res =
        (struct __test_res_t*)calloc( 1, sizeof(struct __test_res_t) );
    struct __test_iter_t* p_iter =
        (struct __test_iter_t*)calloc( 1, sizeof(struct __test_iter_t) );

    List__for_each( p_localtest, (void**)&p_res, (void*)p_iter,
        &__test_action, &__test_callback );

    cr_expect( (4*List__length( p_localtest )) == p_res->add_result, "The for-each did not iterate properly" );
    cr_expect( 1 == p_iter->was_callbackd, "The callback for the struct did not issue properly" );
    cr_expect( 777 == p_res->stored, "Did not see a proper result struct storage" );

    free( p_res );
    p_res = (struct __test_res_t*)calloc( 1, sizeof(struct __test_res_t) );

    List__for_each( p_localtest, (void**)&p_res,
        (void*)p_iter, &__test_action, NULL );

    cr_expect( (4*List__length( p_localtest )) == p_res->add_result,
        "The NULL callback on the for-each did not iterate properly" );

    free( p_iter ); free( p_res );
    List__delete_deep( &p_localtest );
);



///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
// SPEED TESTS.



static inline List_t* __test__List__clone_forloop( List_t* p_list ) {
    size_t len = List__length( p_list );
    if ( NULL == p_list || 0 == len )  return NULL;

    List_t* p_new = List__new( p_list->max_size );

    p_new->head = LIST_NODE_INITIALIZER;
    p_new->head->data = p_list->head->data;

    ListNode_t* p_scroll = p_new->head;

    // Construct blank list nodes to start.
    for ( size_t x = 1; x < len; x++ ) {
        ListNode_t* p_new_node = LIST_NODE_INITIALIZER;

        p_scroll->next = p_new_node;
        p_scroll = p_new_node;
    }

    // Loop again and fill out the data.
    p_scroll = p_new->head->next;
    ListNode_t* p_src_scroll = p_list->head->next;

    for ( size_t x = 1; x < len; x++ ) {
        p_scroll->data = p_src_scroll->data;

        p_scroll = p_scroll->next;
        p_src_scroll = p_src_scroll->next;
    }

    // Return the new List_t shallow clone.
    return p_new;
}
static inline List_t* __test__List__clone_addcall( List_t* p_list ) {
    size_t len = List__length( p_list );
    if ( NULL == p_list || 0 == len )  return NULL;

    List_t* p_new = List__new( p_list->max_size );
    ListNode_t* p_scroll = p_list->head;

    while ( NULL != p_scroll ) {
        if (  -1 == List__add( p_new, p_scroll->data )  ) {
            // If something goes wrong, revert the list changes and destroy the clone.
            List__delete_shallow( &p_new );
            return NULL;
        }

        p_scroll = p_scroll->next;
    }

    return p_new;
}

Test( speed_a, clone__for_vs_add ) {
    printf( "RUNNING TEST: clone__for_vs_add\n" );
    size_t count = 20000;

    printf( "\tPopulating lists.\n" );
    List_t* p_t1 = __create_and_populate( count );
    List_t* p_t2 = __create_and_populate( count );

    printf( "\tCloning lists.\n" );
    cr_assert(  count == List__length( p_t1 ), "List t1 must be populated"  );
    clock_t for_start = clock();
    List_t* p_t1a = __test__List__clone_forloop( p_t1 );
    clock_t for_end = clock();
    double time_spent1 = (double)(for_end - for_start) / CLOCKS_PER_SEC;
    printf( "\t\tList t1 cloned by FOR in '%f' seconds.\n", time_spent1 );

    cr_assert(  count == List__length( p_t2 ), "List t2 must be populated"  );
    clock_t add_start = clock();
    List_t* p_t2a = __test__List__clone_addcall( p_t2 );
    clock_t add_end = clock();
    double time_spent2 = (double)(add_end - add_start) / CLOCKS_PER_SEC;
    printf( "\t\tList t2 cloned by ADD in '%f' seconds.\n", time_spent2 );

    free( p_t1a );
    free( p_t2a );
    List__delete_deep( &p_t1 );
    List__delete_deep( &p_t2 );
}



Test( speed_b, clear__pop_vs_iter ) {
    printf( "RUNNING TEST: clear__pop_vs_iter\n" );
    size_t count = 20000;

    List_t* p_t1 = __create_and_populate( count );
    List_t* p_t2 = __create_and_populate( count );

    printf( "\tClearing lists...\n" );
    cr_expect(  count == List__length( p_t1 ), "List 1 should be full"  );
    clock_t pop_begin = clock();
    size_t x1 = 0;
    while (  NULL != List__pop( p_t1 )  ) {
        x1++;
    }
    clock_t pop_end = clock();
    cr_expect(  0 == List__length( p_t1 ), "List 1 should be empty"  );
    double time_spent1 = (double)(pop_end - pop_begin) / CLOCKS_PER_SEC;
    printf( "\t\tList cleared by POP(%lu): |%f|\n", x1, time_spent1 );

    cr_expect(  count == List__length( p_t2 ), "List 2 should be full"  );
    clock_t iter_begin = clock();
    ListNode_t* p_node2 = p_t2->head;
    size_t x2 = 0;
    while ( NULL != p_node2 ) {
        ListNode_t* p_shadow = p_node2->next;
        free( p_node2 );
        p_node2 = p_shadow;
        x2++;
    }
    p_t2->head = NULL;
    clock_t iter_end = clock();
    cr_expect(  0 == List__length( p_t2 ), "List 2 should be empty"  );
    double time_spent2 = (double)(iter_end - iter_begin) / CLOCKS_PER_SEC;
    printf( "\t\tList cleared by ITER(%lu): |%f|\n", x2, time_spent2 );

    List__delete_deep( &p_t1 );
    List__delete_deep( &p_t2 );
}
