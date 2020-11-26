/* vmstack.h - Cm VM Operand Stack - simplified to have one instance only (no threads).
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVMStack_h
#define __CmVMStack_h

#include "_stdtype.h"

        struct VMStackDesc;
typedef struct VMStackDesc*  Stack;

Stack     Stack_New     (u8 initialCapacity);
void      Stack_Delete  (Stack s);
i32       Stack_Count   (Stack s);
u8        Stack_Capacity(Stack s);
void      Stack_Push    (Stack s, i32 item);
i32       Stack_Pop     (Stack s);

        struct StackEnumtorDesc;
typedef struct StackEnumtorDesc*  StackEnumtor;

StackEnumtor Stack_GetEnumerator(Stack s);

void     StackEnumtor_Delete  (StackEnumtor s);
bool     StackEnumtor_MoveNext(StackEnumtor s);
u32      StackEnumtor_Current (StackEnumtor s);
void     StackEnumtor_Reset   (StackEnumtor s);
//--------------------------------------------------- VMSTACK_BASIC

bool   Stack_IsEmpty(Stack s);

#ifdef STACK_CHECK
i8     Stack_WaterMark(Stack s);
#endif

StackEnumtor Stack_GetEnumerator(Stack s);

void   Stack_Print  (StackEnumtor e);

// ------------------------------------- VMSTACK_INHERENT Instruction

i32    Stack_top(Stack s);
void   Stack_dup(Stack s);
void   Stack_not(Stack s);
void   Stack_and(Stack s);
void   Stack_or (Stack s);
void   Stack_xor(Stack s);
void   Stack_neg(Stack s);
void   Stack_inc(Stack s);
void   Stack_dec(Stack s);
void   Stack_add(Stack s);
void   Stack_sub(Stack s);
void   Stack_mul(Stack s);
void   Stack_div(Stack s);
void   Stack_rem(Stack s);
void   Stack_shl(Stack s);
void   Stack_shr(Stack s);
void   Stack_teq(Stack s);
void   Stack_tne(Stack s);
void   Stack_tlt(Stack s);
void   Stack_tgt(Stack s);
void   Stack_tle(Stack s);
void   Stack_tge(Stack s);

void   Stack_enterU5(Stack s, u8 funcInfo);
void   Stack_enterU8(Stack s, u8 funcInfo);
void   Stack_exit   (Stack s);

void   Stack_addVariable(Stack s, u8 var);
void   Stack_loadVariable (Stack s, u8 var);
void   Stack_storeVariable(Stack s, u8 var);

#endif // __CmVMStack_h
