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
    E_INVALID_HASH        // hash-sum has changed unexpectedly.
};

#endif
/*!

	Stack of elem_t built on array

    Has operations Pop and Push
*/
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


#ifdef DEBUG

/*!
    Gives the pointer at the i-th (i = 0, ..., size - 1) data element of data array for the case
    if there's a canary of type long in the beginning of data array
    Has to be used only in DEBUG mode
    \param[in] i index of element (i = 0, ..., size - 1)
    \param[in] data pointer at the beginning of the data array
    \return pointer at the i-th (i = 0, ..., size - 1) data element of data array
 */
elem_t * _StackGetIthPointer (int i, const elem_t *data);

/*!
    Calculates hash-sum of data elements of data array,
    Has to be used only in DEBUG mode
    \param[in] stk pointer at stack
    \return hash-sum of data elements of data array
 */
int _StackGetHash (const Stack_t *stk);

/*!
    Stack verificator
    Has to be used only in DEBUG mode
    \param[in] stk pointer at stack
    \return number of error from enum Err
 */
int _StackOK (Stack_t *stk);

/*!
    Writes stack information into the dump file
    If there is no dump file creates it
    Used as a part of STACK_OK
    Has to be used only in DEBUG mode
    \param[in] stk pointer at stack
 */
void _StackDump (const Stack_t *stk);

/*!
IN DEBUG MODE
    Distracts stack calling _StackFree after checking stack by STACK_OK
IN RELEASE MODE
    Redefinition of _StackFree

   \param[in] stk pointer at stack
 */
void StackFree (Stack_t *stk);

#else
#define StackFree(stack) _StackFree(stack)
#endif

/*!
  Constructs stack

   \param[in] stk pointer at stack
   \param[in] name name of stack variable
   \param[in] max max size of data array

   \retuen 1 if there was an error
           0 if the construction succeeded
 */
int _StackConstruct (Stack_t *stk, const char *name, int max);

/*!
  Distracts stack

   \param[in] stk pointer at stack
 */
void _StackFree (Stack_t *stk);


/*!
  Makes size of the new array bigger
  newSize has to ba more then current size

   \param[in] stk pointer at stack
   \param[in] newSize new size which is bigger then current size

   \retuen 1 if there was an error
           0 if the resize succeeded
 */
int _StackResize (Stack_t *stk, int newSize);

/*!
  Push a data element on the top of the stack

   \param[in] stk pointer at stack
   \param[in] data data element to be pushed on the top of stack

   \retuen 1 if there was an error
           0 if the push succeeded
 */
int StackPush (Stack_t *stk, elem_t data);


/*!
  Pop data element from the top of the stack

   \param[in] stk pointer at stack
   \param[in] elem pointer at elem_t, which popped element will be witten in

   \retuen 1 if there was an error
           0 if the pop succeeded
 */
int StackPop (Stack_t *stk, elem_t *elem);


#endif //TASK3_STACK_STACK_T_H