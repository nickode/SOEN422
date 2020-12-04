/* tvmstact.c - Test VM Operand Stack functions
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "hal.h"     // hal_Init()
#include "hal_Out.h" // All VMOut_Put*
#include "vmstack.h"

#define Thread_StackCapacity 32

static Stack        runningStack;
static StackEnumtor se;

// VM_Init stub
static void VM_Init(u8* mainAddr) {
    runningStack = Stack_New(Thread_StackCapacity); // Only one thread in cm VM.

    se = Stack_GetEnumerator(runningStack);

    Stack_Print(se);
}

int main(void) {
    i32 val;

    Hal_Init();

    VMOut_PutS("Test VM Operand Stack:\n");
    VMOut_PutS("[ ][ 2 1 ][ 3 ][ 3 3 ][ 9 ][ 8 9 ][ 1 ][ ]\n");

    VM_Init(0);                   // top->[ ]

    Stack_Push(runningStack, 1);
    Stack_Push(runningStack, 2);
    Stack_Print(se);              // top->[ 2 1 ]

    Stack_add(runningStack);
    Stack_Print(se);              // top->[ 3 ]

    Stack_dup(runningStack);
    Stack_Print(se);              // top->[ 3 3 ]

    Stack_mul(runningStack);
    Stack_Print(se);              // top->[ 9 ]

    val = Stack_top(runningStack);
    Stack_Push(runningStack, val-1);
    Stack_Print(se);              // top->[ 8 9 ]

    Stack_sub(runningStack);
    Stack_Print(se);              // top->[ 1 ]

    Stack_Pop(runningStack);
    Stack_Print(se);              // top->[ ]

    VMOut_PutN();
    return 0;
}
