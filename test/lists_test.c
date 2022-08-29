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



void* dummy_alloc(void) {  return calloc( 1, 1 );  }

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



TEST_LISTOPS( extend,
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

Test( speed, clear__pop_vs_iter ) {
    printf( "RUNNING TEST: clear__pop_vs_iter\n" );
    size_t count = 20000;

    List_t* p_t1 = List__new( count );
    List_t* p_t2 = List__new( count );

    printf( "\tPopulating each test list with '%lu' items.\n", count );
    for ( size_t x = 0; x < count; x++ ) {
        ListNode_t* p_n1 = (ListNode_t*)calloc( 1, sizeof(ListNode_t) );
        p_n1->data = (void*)0x01;
        p_n1->next = p_t1->head;
        p_t1->head = p_n1;

        ListNode_t* p_n2 = (ListNode_t*)calloc( 1, sizeof(ListNode_t) );
        p_n2->data = (void*)0x02;
        p_n2->next = p_t2->head;
        p_t2->head = p_n2;
    }
    printf( "\tL1 |%lu| /// L2 |%lu|\n\tClearing lists...\n\n",
        List__length(p_t1), List__length(p_t2) );

    cr_expect(  count == List__length( p_t1 ), "List 1 should be full"  );
    clock_t pop_begin = clock();
    size_t x1 = 0;
    while ( NULL != p_t1->head ) {
        List__pop( p_t1 );
        x1++;
    }
    clock_t pop_end = clock();
    cr_expect(  0 == List__length( p_t1 ), "List 1 should be empty"  );
    double time_spent1 = (double)(pop_end - pop_begin) / CLOCKS_PER_SEC;
    printf( "\tList cleared by POP(%lu): |%f|\n", x1, time_spent1 );

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
    printf( "\tList cleared by ITER(%lu): |%f|\n", x2, time_spent2 );
}
