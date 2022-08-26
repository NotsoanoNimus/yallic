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
