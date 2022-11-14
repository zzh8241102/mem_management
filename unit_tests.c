#include <stdio.h>
#include "memory_management.h"

// add unit tests here

// warm_up test which fix the address
void unit_test_1(){
    printf("sbrk-init%p\n", sbrk(0));
    int * a = _malloc(4);
    printf("sbrk --1st all%p\n", sbrk(0));
    int * b = _malloc(4);
    printf("sbrk --2nd all%p\n", sbrk(0));
    int * c = _malloc(4);
    printf("sbrk --3rd all%p\n", sbrk(0));
    printf("sbrk --advance all%p\n", sbrk(0));
    printf("------------ UNIT TEST OBSERVATION_PLACE ------------ \n");
    printf("%p\n",a);
    printf("%p\n",b);
    printf("%p\n",c);
    printf("----------");
}

// test free a none merge block
void unit_test_2(){
    printf("------------ ENTER UNIT_TEST2------------ \n");
    printf("sbrk-init%p\n", sbrk(0));
    int* a = _malloc(4);
    memset(a,0,8);
    printf("a malloc %p\n",a);
    printf("sbrk --1st all%p\n", sbrk(0));
    int* b = _malloc(4);
    memset(b,0,8);
    int* stored_b = b;
    printf("b malloc %p\n",b);
    printf("sbrk --2nd all%p\n", sbrk(0));
    int* c = _malloc(4);
    memset(c,0,8);
    printf("c malloc %p\n",c);
    printf("sbrk --3rd all%p\n", sbrk(0));
    int* d = _malloc(4);
    memset(d,0,8);
    printf("d malloc %p\n",d);
    printf("sbrk --4rd all%p\n", sbrk(0));


    // free b
    _free(b);
    printf("------------ UNIT TEST2 OBSERVATION_PLACE ------------ \n");
    printf("a malloc %p\n",a);
    printf("c malloc %p\n",c);
    printf("d malloc %p\n",d);
    int* new_b = _malloc(4);
    assert(new_b==stored_b);
    printf("new_b %p",new_b);
    printf("PASTED UNIT_TEST2!\n");

}
// test basic merge front-merge
void unit_test_3(){
    printf("------------ ENTER UNIT_TEST 3------------ \n");
    int* a = _malloc(4);
    memset(a,0,8);
    printf("a malloc %p\n",a);
    printf("sbrk --1st all%p\n", sbrk(0));
    int* b = _malloc(4);
    memset(b,0,8);
    int* stored_b = b;
    printf("b malloc %p\n",b);
    printf("sbrk --2nd all%p\n", sbrk(0));
    int* c = _malloc(4);
    memset(c,0,8);
    printf("c malloc %p\n",c);
    printf("sbrk --3rd all%p\n", sbrk(0));
    int* d = _malloc(4);
    memset(d,0,8);
    printf("d malloc %p\n",d);
    printf("sbrk --4rd all%p\n", sbrk(0));
    printf("------------ UNIT TEST 3 OBSERVATION_PLACE ------------ \n");
    _free(b);
    _free(c);
    // b c previously has
    // [meta][8] [meta][8] meta is 0x20 = 32 32*2+8*2 =80 = 0x50
    // after merge the space available is 80-32=48
    int* new_alloc = _malloc(48);
    assert(new_alloc==stored_b);
    printf("PASTED UNIT_TEST3!\n");
}
// test basic merge back-merge
void unit_test_4(){
    printf("------------ ENTER UNIT_TEST4------------ \n");
    int* a = _malloc(4);
    memset(a,0,8);
    int* b = _malloc(4);
    memset(b,0,8);
    int* stored_b = b;
    int* c = _malloc(4);
    memset(c,0,8);
    int* d = _malloc(4);
    memset(d,0,8);
    printf("------------ UNIT TEST4 OBSERVATION_PLACE ------------ \n");
    _free(c);
    _free(b);
    // b c previously has
    // [meta][8] [meta][8] meta is 0x20 = 32 32*2+8*2 =80 = 0x50
    // after merge the space available is 80-32=48
    int* new_alloc = _malloc(48);
    assert(new_alloc==stored_b);
    printf("PASTED UNIT_TEST4!\n");
}
// double merge
void unit_test_5(){
    printf("------------ ENTER UNIT_TEST5------------ \n");
    int* a = _malloc(4);
    memset(a,0,8);
    int* b = _malloc(4);
    memset(b,0,8);
    int* stored_b = b;
    int* c = _malloc(4);
    memset(c,0,8);
    int* d = _malloc(4);
    memset(d,0,8);
    int* e = _malloc(8); // avoid the handler cut the last block
    memset(d,0,8);
    printf("------------ UNIT TEST5 OBSERVATION_PLACE ------------ \n");
    _free(b);
    _free(d);
    _free(c);
    // b c previously has
    // [meta][8] [meta][8] [meta][8] meta is 0x20 = 32 32*3+8*3 = 120
    // after merge the space available is 120-32=88
    int* new_alloc = _malloc(88);
    assert(new_alloc==stored_b);
    printf("PASTED UNIT_TEST5!\n");
}
int main() {

    unit_test_5();
}

