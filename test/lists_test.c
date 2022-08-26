/*
 * lists_test.c
 *
 * Unit-testing the yallic library.
 *
 */

#include "../src/yallic.h"
#include <criterion/criterion.h>



Test(dummytests, dummyequal) {
    int i = 5;
    cr_expect( 5 == i, "i should equal 5" );
}



Test( list, overflow1 ) {
    List_t* p_test = List__new( 100 );

    for ( size_t i = 0; i < 100; i++ ) {
        void* p_data = calloc( 1, sizeof(void*) );
        cr_expect( -1 != List__add( p_test, p_data ), "Items should be add-able up to the limit" );
    }

    void* p_data = calloc( 1, sizeof(void*) );

    int res = List__add( p_test, p_data );
    cr_expect( -1 == res, "Lists should not allow addition beyond their max_size properties" );

    free( p_data );

    List__delete_deep( p_test );
}



Test( memory, leak1 ) {
    List_t* p_n = List__new( 0 );
    for ( size_t i = 0; i < 10; i++ ) {
        void* p_d = calloc( 1, sizeof(void*) );
        cr_expect( -1 != List__add( p_n, p_d ), "The list should be add-able" );
    }
//    List__delete_shallow( p_n );
}
