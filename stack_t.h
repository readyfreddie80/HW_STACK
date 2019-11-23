//
// Created by ready on 06.10.2019.
//

#ifndef TASK3_STACK_STACK_T_H
#define TASK3_STACK_STACK_T_H

#define DEBUG
#define StackConstruct(stack, max) _StackConstruct(stack, #stack, max)
#define STACK_OK(stack) if (_StackOK(stack) != 0) {_StackDump(stack); _StackFree(stack); assert(!"ok");}


typedef int elem_t;                //stack's data type


#ifdef DEBUG
// stack errors.
enum Err {
    OK,                   // stack is valid;
    E_NULL_PTR_STACK,     // stack is pointed by NULL;
    E_NULL_PTR_DATA,      // data is pointed by NULL;
    E_INVALID_SIZE,       // size is invalid;
    E_INVALID_MAXSIZE,    // maxSize is invalid;
    E_WRONG_CANARY1,      // canary1 is modified;
    E_WRONG_CANARY2,      // canary2 is modified;
    E_WRONG_DATA_CANARY1, // first data canary is modified;
    E_WRONG_DATA_CANARY2, // second data canary is modified;
    E_INVALID_HASH,       // hash-sum has changed unexpectedly.
};

#endif

struct Stack_t {
#ifdef DEBUG
    long canary1; // first canary
#endif

    char *name;   // stack's name
    int maxSize;  // maximum capacity
    int size;     // current size
    elem_t *data; // buffer

#ifdef DEBUG
    enum Err errNo; // error occured number
    long int hash;  // hash sum
    long canary2;   // second canary
#endif
};


elem_t * _StackGetIthPointer (int i, const elem_t * const data);

#ifdef DEBUG


int _StackGetHash (const Stack_t *stk);

int _StackOK (Stack_t *stk);

void _StackDump (const Stack_t *stk);

int StackFree (Stack_t *stk);

#else
#define StackFree(stack) _StackFree(stack)
#endif


int _StackConstruct (Stack_t *stk, const char *name, int max);

int _StackFree (Stack_t *stk);

int _StackResize (Stack_t *stk, int newSize);

int StackPush (Stack_t *stk, elem_t data);

int StackPop (Stack_t *stk, elem_t *elem);






#endif //TASK3_STACK_STACK_T_H