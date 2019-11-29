#include <iostream>
#include <cstdlib>

#include "stack_t.h"

int unitTest() {

    int maxSize = 1000;
    Stack_t *stk = (Stack_t *)calloc (1, sizeof (Stack_t));

    StackConstruct (stk, 1);

    for(int i = 0; i < maxSize; ++i) {
        StackPush(stk, i);
    }

    for(int i = maxSize - 1; i >= 0; --i) {
        elem_t elem;
        StackPop(stk, &elem);

        if (i != elem)
            printf("Wrong pop result: %d. Expected %d ", elem,  i);
    }

    StackFree(stk);

    return 0;
}

#ifdef DEBUG
int unitTestsVerify(int nCase) {
    int *a = (int *)calloc (1, sizeof (int));
    Stack_t *stk = (Stack_t *)calloc (1, sizeof (Stack_t));
    StackConstruct (stk, 10);
    int *b = (int *)calloc (1, sizeof (int));

    for(int i = 0; i < 3; ++i) {
        StackPush(stk, i);
    }

    Stack_t *stk1 = NULL;

    switch (nCase) {
        case 1:
            StackConstruct (stk1, 1);
            break;
        case 2:
            stk->data = NULL;
            StackPush(stk, 1);
            break;
        case 3:
            stk->maxSize = -1;
            StackPush(stk, 1);
            break;
        case 4:
            stk->size = -1;
            StackPush(stk, 1);
            break;
        case 5:
            *((long *) (stk->data)) = 123;
            StackPush(stk, 1);
            break;
        case 6:
            stk->canary1 = -1;
            StackPush(stk, 1);
            break;
        case 7:
            stk->canary2 = -1;
            StackPush(stk, 1);
            break;
        case 8:
            for (int i = 0; i < 10; ++i)
                *(a + i) = i;
            StackPush(stk, 1);
            break;
        case 9:
            for (int i = 0; i < 10; ++i)
                *(b - i) = i;
            StackPush(stk, 1);
            break;
        case 10:
            stk->data[5] = -1;
            StackPush(stk, 1);
            break;
        default:
            printf("Wrong nCase");
    }
    StackFree(stk);

    return 0;
}

#endif

int main() {

    unitTest();

    //unitTestsVerify(10);

    return 0;
}











