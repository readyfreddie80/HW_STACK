//
// Created by ready on 20.10.2019.
//

#include "stack_t.h"
#include <iostream>
#include <assert.h>
#include <string.h>


#ifdef DEBUG


const long CANARY = 0xDED0BED;     // canary value
elem_t POISON     = (elem_t)(123); // poisonous value.


const char* errMessage[] = {
        "stack is OK",
        "stack is pointed by NULL",
        "data is pointed by NULL",
        "size is invalid",
        "maxSize is invalid",
        "first stack canary is modified",
        "second stack canary is modified",
        "first data canary is modified",
        "second data canary is modified",
        "hash-sum has changed unexpectedly",
};




elem_t * _StackGetIthPointer (int i, const elem_t * const data) {
    assert (data);
    assert (i >= 0);

    return (elem_t *)((long *)data + 1) +  i;
}

int _StackGetHash (const Stack_t *stk) {
    assert (stk);
    assert (stk->data);

    int hash = 0;
    for (int i = 1; i <= stk->maxSize; i++)
        hash += ((int)*_StackGetIthPointer(i - 1, stk->data)) * i;
    return hash;
}


int _StackOK (Stack_t* stk) {
    if (stk == NULL)
        return E_NULL_PTR_STACK;

    if (stk->data == NULL) {
        stk->errNo = E_NULL_PTR_DATA;
        return E_NULL_PTR_DATA;
    }

    if (stk->maxSize <= 0) {
        stk->errNo = E_INVALID_MAXSIZE;
        return E_INVALID_MAXSIZE;
    }

    if (stk->size < 0 || stk->size > stk->maxSize) {
        stk->errNo = E_INVALID_SIZE;
        return E_INVALID_SIZE;
    }

    if (stk->canary1 != CANARY) {
        stk->errNo = E_WRONG_CANARY1;
        return E_WRONG_CANARY1;
    }

    if (stk->canary2 != CANARY) {
        stk->errNo = E_WRONG_CANARY2;
        return E_WRONG_CANARY2;
    }

    if (*((long*)(stk->data)) != CANARY) {
        stk->errNo = E_WRONG_DATA_CANARY1;
        return E_WRONG_DATA_CANARY1;
    }


    if (*((long*)_StackGetIthPointer(stk->maxSize, stk->data)) != CANARY) {
        stk->errNo = E_WRONG_DATA_CANARY2;
        return E_WRONG_DATA_CANARY2;
    }

    if (_StackGetHash(stk) != stk->hash) {
        stk->errNo = E_INVALID_HASH;
        return E_INVALID_HASH;
    }

    return OK;
}


void _StackDump (Stack_t* stk) {

    const char *filename = "dump.log";

    FILE *fdump = fopen (filename, "a+");

    if (fdump == NULL) {
        printf ("Can't open file %s", filename);
        exit(1);
    }


    if (stk == NULL)
        fprintf (fdump, "!!!ERROR <%d>: %s !!!\n",
                 E_NULL_PTR_STACK,
                 errMessage[E_NULL_PTR_STACK]);
    else {

        if (stk->errNo == OK)
            fprintf (fdump, "Stack is OK\n");
        else
            fprintf (fdump, "!!!ERROR <%d>: %s !!!\n",
                     stk->errNo,
                     errMessage[stk->errNo]);

        if (stk->name == NULL)
            fprintf (fdump, "Stack_t \"!!!INVALID NAME!!!\"");
        else
            fprintf (fdump, "Stack_t \"%s\"",
                     stk->name);


        fprintf (fdump, " [%p] {\n"
                        "  canary1          = %lx\n"
                        "  current size     = %d\n"
                        "  maximum capacity = %d\n"
                        "  errno            = %d\n"
                        "  hash             = %ld\n"
                        "  canary2          = %lx\n"
                        "  data[%d]         = [%p]:\n",
                 stk,
                 stk->canary1,
                 stk->size,
                 stk->maxSize,
                 stk->errNo,
                 stk->hash,
                 stk->canary2,
                 stk->size,
                 stk->data);


        if (stk->data != NULL) {
            for (int i = 0; i < stk->maxSize; i++) {
                if (i < stk->size)
                    fprintf(fdump, "   *[%d] = %d\n", i, *_StackGetIthPointer(i, stk->data));
                else
                    fprintf(fdump, "    [%d] = %d\n", i, *_StackGetIthPointer(i, stk->data));
            }
        }
        else
            fprintf (fdump, "   NO DATA\n");


        fprintf (fdump, "}\n");
    }

    fclose (fdump);
}


int StackFree (Stack_t *stk) {
    STACK_OK(stk);

    _StackFree(stk);

    return 0;
}
#endif




int _StackConstruct (Stack_t *stk, const char *name, int max) {
    assert (stk != NULL);
    assert (name != NULL);
    assert (max > 0);

#ifdef DEBUG
    stk->canary1 = CANARY;
#endif

    size_t nameLen = strlen (name);

    stk->name = (char *)calloc (nameLen + 1, sizeof(char));

    if (stk->name == NULL) {
        printf ("The attempt to allocate memory for the stack name failed. "
                "Please, try shorter name\n");
        return  1;
    }

    memcpy (stk->name, name, nameLen);
    stk->name[nameLen] = '\0';


    stk->maxSize = max;
    stk->size    = 0;

#ifdef DEBUG
    stk->data = (elem_t *)calloc (1, 2 * sizeof (CANARY) + max * sizeof (elem_t));
#else
    stk->data = (elem_t *)calloc (max, sizeof (elem_t));
#endif

    if (stk->data == NULL) {
        printf ("The attempt to allocate memory for the stack buf failed. "
                "Please, try smaller maximum size\n");

        free(stk->name);
        return  1;
    }

#ifdef DEBUG
    *((long *)(stk->data)) = CANARY;
    *((long *)_StackGetIthPointer(max, stk->data)) = CANARY;

    for (int i = 0; i < max; i++)
        *_StackGetIthPointer (i, stk->data) = POISON;


    stk->errNo     = OK;
    stk->canary2   = CANARY;
    stk->hash      = _StackGetHash(stk);

    STACK_OK (stk);
#endif

    return 0;

}


int _StackFree (Stack_t *stk) {

    assert (stk);

    if (stk->name != NULL)
        free (stk->name);

    if (stk->data != NULL)
        free (stk->data);

    free (stk);

    return 0;
}

int _StackResize (Stack_t *stk, int newSize) {
#ifdef DEBUG
    STACK_OK (stk);
#else
    assert (stk);
#endif


    if (newSize < stk->size) {
        printf ("Invalid newSize>: %d\n", newSize);
        exit (1);
    }

#ifdef DEBUG
    stk->data = (elem_t *)realloc (stk->data, 2 * sizeof (CANARY) + newSize * sizeof (elem_t));
#else
    stk->data = (elem_t *)realloc (stk->data, newSize * sizeof (elem_t));
#endif


    if (stk->data == NULL) {
        printf ("The attempt to reallocate memory for the stack buf failed.\n");
        return 1;
    }

    stk->maxSize = newSize;

#ifdef DEBUG
    for (int i = stk->size; i < stk->maxSize; i++)
        *_StackGetIthPointer(i, stk->data) = POISON;

    *((long *)_StackGetIthPointer(stk->maxSize, stk->data)) = CANARY;

    stk->hash = _StackGetHash(stk);

    STACK_OK (stk);
#endif

    return 0;
}



int StackPush(Stack_t *stk, elem_t data) {
#ifdef DEBUG
    STACK_OK (stk);
#else
    assert (stk);
#endif

    if (stk->size == stk->maxSize) {
        if (_StackResize(stk, stk->maxSize * 2) != 0)
            return 1;
    }

#ifdef DEBUG
    *_StackGetIthPointer(stk->size, stk->data) = data;
#else
    *(stk->data + stk->size) = data;
#endif

    stk->size++;

#ifdef DEBUG
    stk->hash = _StackGetHash(stk);

    STACK_OK (stk);
#endif

    return 0;
}


int StackPop(Stack_t *stk, elem_t *elem) {
#ifdef DEBUG
    STACK_OK(stk);
#else
    assert(stk);
#endif

    assert(elem);

    if (stk->size == 0) {
        printf("Unable to pop: stack is empty\n");
        return 1;
    }

    stk->size--;

#ifdef DEBUG
    *elem = *_StackGetIthPointer(stk->size, stk->data);
#else
    *elem = stk->data[stk->size];
#endif

#ifdef DEBUG
    stk->hash = _StackGetHash(stk);

    STACK_OK(stk);
#endif

    return 0;
}




