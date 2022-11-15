/**
 * @FILE_NAME: unit_tests.c
 * @DESCRIPTION: The unit_tests and unit_test interface
 * @MAIN_FUNC: unit_test_xx(),unit_test_selector()
 * @AUTHOR: Zihan Zhou
 * @DATE: 2022-11
 * @PURPOSE: For better test and present the behaviour of _malloc and _free
 */

#include <stdio.h>
#include "memory_management.h"

#define DEMO_PURPOSE 1

// add unit tests here

// warm_up test which fix the address
void unit_test_1() {
    printf("sbrk-init%p\n", sbrk(0));
    int *a = _malloc(4);
    printf("sbrk --1st all%p\n", sbrk(0));
    int *b = _malloc(4);
    printf("sbrk --2nd all%p\n", sbrk(0));
    int *c = _malloc(4);
    printf("sbrk --3rd all%p\n", sbrk(0));
    printf("sbrk --advance all%p\n", sbrk(0));
    printf("------------ UNIT TEST OBSERVATION_PLACE ------------ \n");
    printf("%p\n", a);
    printf("%p\n", b);
    printf("%p\n", c);
    printf("----------");
}

// test free a none merge block
void unit_test_2() {
    int *a = _malloc(4);
    memset(a, 0, 8);
    int *b = _malloc(4);
    memset(b, 0, 8);
    int *stored_b = b;
    int *c = _malloc(4);
    memset(c, 0, 8);
    int *d = _malloc(4);
    memset(d, 0, 8);
    // free b
    _free(b);
    printf("------------ UNIT TEST2 OBSERVATION_PLACE ------------ \n");
//    printf("a malloc %p\n",a);
//    printf("c malloc %p\n",c);
//    printf("d malloc %p\n",d);
    int *new_b = _malloc(4);
    assert(new_b == stored_b);
    printf("PASTED UNIT_TEST2!\n");

}
/**
 * Test basic
 */
// test basic merge front-merge
void unit_test_3() {
    int *a = _malloc(4);
    memset(a, 0, 8);
    int *b = _malloc(4);
    memset(b, 0, 8);
    int *stored_b = b;
    int *c = _malloc(4);
    memset(c, 0, 8);
    int *d = _malloc(4);
    memset(d, 0, 8);
    printf("------------ UNIT TEST 3 OBSERVATION_PLACE ------------ \n");
    _free(b);
    _free(c);
    // b c previously has
    // [meta][8] [meta][8] meta is 0x20 = 32 32*2+8*2 =80 = 0x50
    // after merge the space available is 80-32=48
    int *new_alloc = _malloc(48);
    assert(new_alloc == stored_b);
    printf("PASTED UNIT_TEST3!\n");
}

// test basic merge back-merge
void unit_test_4() {
    int *a = _malloc(4);
    memset(a, 0, 8);
    int *b = _malloc(4);
    memset(b, 0, 8);
    int *stored_b = b;
    int *c = _malloc(4);
    memset(c, 0, 8);
    int *d = _malloc(4);
    memset(d, 0, 8);
    printf("------------ UNIT TEST4 OBSERVATION_PLACE ------------ \n");
    _free(c);
    _free(b);
    // b c previously has
    // [meta][8] [meta][8] meta is 0x20 = 32 32*2+8*2 =80 = 0x50
    // after merge the space available is 80-32=48
    int *new_alloc = _malloc(48);
    assert(new_alloc == stored_b);
    printf("PASTED UNIT_TEST4!\n");
}

// double merge
void unit_test_5() {
    int *a = _malloc(4);
    memset(a, 0, 8);
    int *b = _malloc(4);
    memset(b, 0, 8);
    int *stored_b = b;
    int *c = _malloc(4);
    memset(c, 0, 8);
    int *d = _malloc(4);
    memset(d, 0, 8);
    int *e = _malloc(8); // avoid the handler cut the last block
    memset(d, 0, 8);
    printf("------------ UNIT TEST5 OBSERVATION_PLACE ------------ \n");
    _free(b);
    _free(d);
    _free(c);
    // b c previously has
    // [meta][8] [meta][8] [meta][8] meta is 0x20 = 32 32*3+8*3 = 120
    // after merge the space available is 120-32=88
    int *new_alloc = _malloc(88);
    assert(new_alloc == stored_b);
    printf("PASTED UNIT_TEST5!\n");
}

/** test split cases
 [meta][big-size] =>[meta][need_size] + [meta][new_size] new_size = big-size - (meta_size+need_size)
*/

// must proper suit, no need to split
void unit_test_6() {
    int *a = _malloc(48);
    int *stored_a = a;
    _free(a);
    int *b = _malloc(48);
    assert(stored_a == b);
    printf("PASTED UNIT_TEST6!\n");

}

// test split -> the newly split small block is  attached
void unit_test_7() {
    // [meta][48] -> []
    int *a = _malloc(48);
    int *stored_a = a;
    _free(a);
    int *b = _malloc(6);
    assert(stored_a == b);
    printf("PASTED UNIT_TEST7!\n");

}

void unit_test_8() {
    // [m][size] -> [m][size] -> [m][size]
    // [m][size] -> [m][size1] -> [m][size2] ->[m][size]
    int *a = _malloc(48);
    int *stored_a = a;
    int *b = _malloc(48);
    int *stored_b = b;
    int *c = _malloc(48);
    int *stored_c = c;
    _free(b);
    int size1 = _malloc(4);


    printf("PASTED UNIT_TEST8!\n");

}

// TODO: test the impl of split

void unit_test_demo_selector() {
//TODO: ADD CLEAN OPS
    printf("+----------------------------------------------+\n");
    printf("Enter the unit test number you want to observe:\n");
    printf("+----------------------------------------------+\n");
    char input[10];
    scanf("%2s", input);
    int option = atoi(input);
    switch (option) {
        case 2:
            unit_test_2();
            break;
        case 3:
            unit_test_3();
            break;
        case 4:
            unit_test_4();
            break;
        case 5:
            unit_test_5();
            break;
        case 6:
            unit_test_6();
            break;
        case 7:
            unit_test_7();
            break;
        case 8:
            unit_test_8();
        default:
            printf("No this case, re-prompt the program and check if there are any typos");
    }
}

int main() {
#if DEMO_PURPOSE
    unit_test_demo_selector();
#endif
}

