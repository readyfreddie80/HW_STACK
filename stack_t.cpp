//
// Created by ready on 20.10.2019.
//

#include "stack_t.h"
#include "stdfuncs_wrappers.h"
#include <iostream>
#include <assert.h>
#include <string.h>


#ifdef DEBUG

const canary_t CANARY = 0xDED0BED; /// canary value
const elem_t   POISON = 123;       /// poisonous value

const char* ERR_MSG[10] = {
        "stack is OK",
        "stack is pointed by NULL",
        "data is pointed by NULL",
        "size is invalid",
        "maxSize is invalid",
        "first stack canary is modified",
        "second stack canary is modified",
        "first data canary is modified",
        "second data canary is modified",
        "hash-sum has changed unexpectedly"
};



elem_t * _StackGetIthPointer (
        size_t       i,
        const elem_t *data) {
    assert (data);

    return (elem_t *)((canary_t *)data + 1) +  i;
}

hash_t _StackGetHash (const Stack_t *stk) {
    assert (stk);
    assert (stk->data);

    hash_t hash = 0;
    for (size_t i = 1; i <= stk->maxSize; i++) {
        hash += (hash_t)*_StackGetIthPointer(i - 1, stk->data) * i;
    }
    return hash;
}


canary_t _GetFirsDataCanary(const Stack_t *stk) {
    return *((canary_t *)(stk->data));
}

canary_t _GetSecondDataCanary(const Stack_t *stk) {
    return *((canary_t *)_StackGetIthPointer(stk->maxSize, stk->data));
}

int _StackOK (Stack_t *stk) {
    if (stk == nullptr) {
        return E_NULL_PTR_STACK;
    }

    if (stk->data == nullptr) {
        return stk->errNo = E_NULL_PTR_DATA;
    }

    if (stk->maxSize == 0) {
        return stk->errNo = E_INVALID_MAXSIZE;
    }

    if (stk->size > stk->maxSize) {
        return stk->errNo = E_INVALID_SIZE;
    }

    if (stk->canary1 != CANARY) {
        return stk->errNo = E_WRONG_CANARY1;
    }

    if (stk->canary2 != CANARY) {
        return stk->errNo = E_WRONG_CANARY2;
    }

    if (_GetFirsDataCanary(stk)!= CANARY) {
        return stk->errNo = E_WRONG_DATA_CANARY1;
    }

    if (_GetSecondDataCanary(stk)!= CANARY) {
        return stk->errNo = E_WRONG_DATA_CANARY2;
    }

    if (_StackGetHash(stk) != stk->hash) {
        return stk->errNo = E_INVALID_HASH;
    }

    return OK;
}


void _StackDump (const Stack_t *stk) {

    const char *filename = "dump.log";

    FILE *fdump = fopen_wrapper (filename, "a+");
    EXIT_IF_ERROR_OCCURRED();

    if (stk == nullptr)
        fprintf (
                fdump,
                "!!!ERROR <%d>: %s !!!\n",
                 E_NULL_PTR_STACK,
                 ERR_MSG[E_NULL_PTR_STACK]);
    else {
        if (stk->errNo == OK) {
            fprintf(
                    fdump,
                    "Stack is OK\n");
        }
        else {
            fprintf(
                    fdump,
                    "!!!ERROR <%d>: %s !!!\n",
                    stk->errNo,
                    ERR_MSG[stk->errNo]);
        }
        if (stk->name == nullptr) {
            fprintf(
                    fdump,
                    "Stack_t \"!!!INVALID NAME!!!\"");
        }
        else {
            fprintf(
                    fdump,
                    "Stack_t \"%s\"",
                    stk->name);
        }

        fprintf (
                fdump,
                " [%p] {\n"
                "  canary1          = %llx\n"
                "  current size     = %zu\n"
                "  maximum capacity = %zu\n"
                "  errno            = %d\n"
                "  hash             = %lld\n"
                "  canary2          = %llx\n"
                "  data[%zu]         = [%p]:\n",
                stk,
                stk->canary1,
                stk->size,
                stk->maxSize,
                stk->errNo,
                stk->hash,
                stk->canary2,
                stk->size,
                stk->data);

        const char *indent = "   ";
        if (stk->data != nullptr) {
            fprintf(
                    fdump,
                    "canary1 = %llx\n",
                    _GetFirsDataCanary(stk));
            for (size_t i = 0; i < stk->maxSize; i++) {
                if (i < stk->size) {
                    fprintf(
                            fdump,
                            "%s*[%zu] = %d\n",
                            indent,
                            i,
                            *_StackGetIthPointer(i, stk->data));
                }
                else {
                    fprintf(
                            fdump,
                            "%s [%zu] = %d\n",
                            indent,
                            i,
                            *_StackGetIthPointer(i, stk->data));
                }
            }
            fprintf(
                    fdump,
                    "canary2 = %llx\n",
                    _GetSecondDataCanary(stk));
        }
        else {
            fprintf (
                    fdump,
                    "%sNO DATA\n",
                    indent);
        }
        fprintf (fdump, "}\n");
    }

    fclose_wrapper (fdump);
}


void StackFree (Stack_t *stk) {
    STACK_OK(stk);

    _StackFree(stk);
}
#endif




int _StackConstruct (
        Stack_t    *stk,
        const char *name,
        size_t      max) {
    assert (stk  != nullptr);
    assert (name != nullptr);
    assert (max  != 0);

#ifdef DEBUG
    stk->canary1 = CANARY;
#endif

    size_t nameLen = strlen (name) + 1;

    stk->name = (char *) calloc_wrapper(nameLen, sizeof(*(stk->name)));

    if (stk->name == nullptr) {
        FPRINTF_ERROR ("The attempt to allocate memory for the stack name failed. "
                       "Please, try shorter name\n");
        return 1;
    }

    memcpy (stk->name, name, nameLen);

    stk->maxSize = max;
    stk->size    = 0;

#ifdef DEBUG
    stk->data = (elem_t *)calloc (1, 2 * sizeof (CANARY) + max * sizeof (*(stk->data)));
#else
    stk->data = (elem_t *)calloc (max, sizeof (*(stk->data)));
#endif

    if (stk->data == nullptr) {
        FPRINTF_ERROR ("The attempt to allocate memory for the stack buf failed. "
                       "Please, try smaller maximum size\n");
        return 1;
    }

#ifdef DEBUG
    *((canary_t *)(stk->data))                         = CANARY;
    *((canary_t *)_StackGetIthPointer(max, stk->data)) = CANARY;

    for (size_t i = 0; i < max; i++)
        *_StackGetIthPointer (i, stk->data) = POISON;


    stk->errNo     = OK;
    stk->canary2   = CANARY;
    stk->hash      = _StackGetHash(stk);

    STACK_OK (stk);
#endif

    return 0;
}


void _StackFree (Stack_t *stk) {

    assert (stk);

    if (stk->name != nullptr)
        free (stk->name);

    if (stk->data != nullptr)
        free (stk->data);

    free (stk);
}

int _StackResize(
        Stack_t *stk,
        size_t   newSize) {
#ifdef DEBUG
    STACK_OK (stk);
#else
    assert (stk);
#endif


    if (newSize < stk->size) {
        FPRINTF_ERROR ("Invalid newSize>: %zu\n", newSize);
        return 1;
    }

#ifdef DEBUG
    stk->data = (elem_t *)realloc (stk->data, 2 * sizeof (CANARY) + newSize * sizeof (*(stk->data)));
#else
    stk->data = (elem_t *)realloc (stk->data, newSize * sizeof (*(stk->data)));
#endif


    if (stk->data == nullptr) {
        FPRINTF_ERROR ("The attempt to reallocate memory for the stack buf failed.\n");
        return 1;
    }

    stk->maxSize = newSize;

#ifdef DEBUG
    for (size_t i = stk->size; i < stk->maxSize; i++)
        *_StackGetIthPointer(i, stk->data) = POISON;

    *((canary_t *)_StackGetIthPointer(stk->maxSize, stk->data)) = CANARY;

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

    assert (elem);

    if (stk->size == 0) {
        FPRINTF_ERROR("Unable to pop: stack is empty\n");
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




