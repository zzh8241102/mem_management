/**
 * @FILE_NAME: unit_tests.c
 * @DESCRIPTION: The unit_tests and unit_test interface
 * @MAIN_FUNC: unit_test_xx(),unit_test_selector()
 * @AUTHOR: Zihan Zhou
 * @DATE: 2022-11
 * @PURPOSE: For better test and present the behaviour of _malloc and _free
 * @NOTICE!!!: THIS IS NOT A PART OF THE CW, JUST FOR DEMO AND TEST
 */

#include <stdio.h>
#include "memory_management.h"

#define DEMO_PURPOSE 1

// add unit tests here

// warm_up test which fix the address
void warm_up() {
    printf("sbrk-init%p\n", sbrk(0));
    int *a = _malloc(4);
    printf("sbrk --1st all%p\n", sbrk(0));
    int *b = _malloc(4);
    printf("sbrk --2nd all%p\n", sbrk(0));
    int *c = _malloc(4);
    printf("sbrk --3rd all%p\n", sbrk(0));
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
    int *new_b = _malloc(4);
    printf("The new address is %p\n", new_b);
    printf("The freed perfect address is %p, which should be same as the new address\n", b);
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
    _free(b);
    _free(c);
    // b c previously has
    // [meta][8] [meta][8] meta is 0x20 = 32 32*2+8*2 =80 = 0x50
    // after merge the space available is 80-32=48
    int *new_alloc = _malloc(48);
    printf("Since after merge,there will be a 32*2+8*2 = 80(0x50) hole,remain80-32=48,\n we allocate 48, and the new address is %p\n",
           new_alloc);
    printf("which should be the same as the address of prev b %p\n", stored_b);
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
    _free(c);
    _free(b);
    // b c previously has
    // [meta][8] [meta][8] meta is 0x20 = 32 32*2+8*2 =80 = 0x50
    // after merge the space available is 80-32=48
    int *new_alloc = _malloc(48);
    printf("Since after back merge,there will be a 32*2+8*2 = 80(0x50) hole,remain80-32=48,\n we allocate 48, and the new address is %p\n",
           new_alloc);
    printf("which should be the same as the address of prev b %p\n", stored_b);
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
    printf("Since after back merge,there will be a 32*3+8*3 = 120 hole,\n we allocate 120-32 = 88, and the new address is %p\n",
           new_alloc);
    printf("which should be the same as the address of prev b %p\n", stored_b);
    assert(new_alloc == stored_b);
    printf("PASTED UNIT_TEST5!\n");
}

/**
 * test split cases
 * [meta][big-size] =>[meta][need_size] + [meta][new_size] new_size = big-size - (meta_size+need_size)
*/

// must proper suit, no need to split
void unit_test_6() {
    int *a = _malloc(48);
    int *stored_a = a;
    int *b = _malloc(48);
    _free(a);
    int *c = _malloc(48);
    assert(stored_a == c);
    printf("PASTED UNIT_TEST6!\n");

}

// test split -> the newly split small block is attached
void unit_test_7() {
    // [meta][48] -> []
    int *a = _malloc(48);
    int *stored_a = a;
    int *b = _malloc(48);
    _free(a);
    int *c = _malloc(6);
    assert(stored_a == c);
    printf("PASTED UNIT_TEST7!\n");

}

// split and test the older hole and newly added hole
void unit_test_8() {
    // [m][size] -> [m][size] -> [m][size]
    // [m][size] -> [m][size1] -> [m][size2] ->[m][size]
    int *a = _malloc(48);
    memset(a, 0, 48);
    int *stored_a = a;
    int *b = _malloc(48);
    memset(a, 0, 48);
    int *stored_b = b;
    int *c = _malloc(48);
    memset(a, 0, 48);
    int *stored_c = c;
    _free(b);
    // after that there are 48 remain and can be split
    printf("After that there are 48 remain and can be split\n");
    printf("we allocate a small 4 size block and remain 32+12 spaces,it will take the 4 previous space\n");
    int *size1 = _malloc(4);
    memset(a, 0, 8);
    // give 4 and b will take the previous place
    // also a new block sized 48-4==44 [32][12]
    // after that the
    printf("The new address should be same as previous address b, which is %p\n", size1);
    assert(b == size1);
    int *size2 = _malloc(4);
    memset(a, 0, 8);
    printf("Again we allocated a size 4 block, and the address of this is %p\n", size2);
    printf("So from b, the size 2 should be b+8+Metasize, which is %p\n",
           (void *) b + round_align(4, 8) + META_BLOCK_SIZE);
    assert(size2 == (void *) b + round_align(4, 8) + META_BLOCK_SIZE);
    printf("PASTED UNIT_TEST8!\n");
}

/** Let's scale it up
 *  and test whether the algo is robust
 */
// robust malloc and free
void unit_test_9() {
#if __linux__
    for (int i = 0; i < 10; i++) {
        int *a = _malloc(4096*1000*20);
        memset(a, 100, 4096*1000*20);
        system("free -m");
        _free(a);
        printf("Enter...\n");
        getchar();
        int *smaller_chunk = _malloc(1);
        _free(smaller_chunk);
        system("free -m");
        printf("Enter...\n");
        getchar();
    }
#else
    for (int i = 0; i < 10; i++) {
        int *a = _malloc(4096 * 100);
        memset(a, 100, 4096 * 100);
        _free(a);
        printf("Enter...\n");
        getchar();
        int *smaller_chunk = _malloc(1);
        _free(smaller_chunk);
        printf("Enter...\n");
        getchar();
    }
#endif
    printf("PASTED UNIT_TEST9!\n");
}

// different type for malloc
typedef struct test_ {
    int a;
    short b;
    struct test_ *ptr;
} test_block;

void unit_test_10() {
    printf("This is a test used to ensure every data type is usable\n");
    int *a = _malloc(4 * sizeof(int));
    memset(a, 0, 4 * sizeof(int));
    _free(a);
    short *b = _malloc(4 * sizeof(short));
    memset(a, 0, 4 * sizeof(short));
    _free(b);
    long *c = _malloc(4 * sizeof(long));
    memset(a, 0, 4 * sizeof(long));
    _free(c);
    float *d = _malloc((4 * sizeof(float)));
    memset(a, 0, 4 * sizeof(float));
    _free(d);
    test_block *p = (test_block *) _malloc(100 * sizeof(test_block));
    memset(a, 0, 100 * sizeof(test_block));
    _free(p);
    printf("PASTED UNIT_TEST10!\n");

}

void unit_test_11() {
    int *b = _malloc(0);
    assert(b == NULL);
    int *c = _malloc(-1);
    assert(c == NULL);
    _free(b);
    _free(c);
    printf("Passed the test if free says two invalid address!\n");
}

void unit_test_demo_selector() {
//TODO: ADD CLEAN OPS
    printf("+----------------------------------------------+\n");
    printf("Enter the unit test number(2-11) you want to observe:\n");
    printf("+----------------------------------------------+\n");
    printf("You can only select one test at one time and observe its behaviour\n");
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
            break;
        case 9:
            unit_test_9();
            break;
        case 10:
            unit_test_10();
            break;
        case 11:
            unit_test_11();
            break;
        default:
            printf("No this case, re-prompt the program and check if there are any typos");
    }
}

int main() {
#if DEMO_PURPOSE
    unit_test_demo_selector();

#endif

}

