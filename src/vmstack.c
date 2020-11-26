/* vmstack.c - Cm VM Operand Stack - simplified to have one instance only (no threads).
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "out.h" 
#include "vmstack.h"

typedef struct VMStackDesc {
    i32         si[32];    // Simplification for Cm VM - one instance only.
    i8          bp;        /* base (or frame) pointer */
    i8          sp;        /* stack pointer (sp+1 == number of elements) : to avoid size ptr calculation (sp-base) */
    u8          capacity;  /* maximum number of elements: to avoid overflow calculation (limit-base) and sp/limit */
#ifdef STACK_CHECK
    i8          waterMark; /* used to estimate the effective stack space used for a specific thread */
#endif
} VMStackDesc;

#ifdef STACK_CHECK
static void printMsg(Stack s, char* msg) {
        VMOut_PutS("\nStack: "); VMOut_PutS(msg);
        VMOut_PutS(" ** limit["); VMOut_PutU( Stack_Capacity(s) );
        VMOut_PutS("] used(watermark)["); VMOut_PutI((i32)s->waterMark);
        VMOut_PutS("] sp["); VMOut_PutI((i32)s->sp);
        VMOut_PutS("] bp["); VMOut_PutI((i32)s->bp);
        VMOut_PutS("]\n");
}
#endif

static VMStackDesc vmStackDesc; // One instance.

Stack Stack_New(u8 capacity) {
    Stack  s = &vmStackDesc;    // One instance.
    if (s   &&   capacity != 0) {
        s->bp = s->sp = -1;
        s->capacity = capacity;
#ifdef STACK_CHECK
        s->limit = s->stack + capacity;
        s->waterMark = -1;
        printMsg(s, "init");
#endif
    }
    return s;
}

void Stack_Delete(Stack s)   { } // One instance.

i32  Stack_Count   (Stack s) { return s->sp+1;     }
u8   Stack_Capacity(Stack s) { return s->capacity; }

void Stack_Push(Stack s, i32 value) {
//t    VMOut_PutS("\nsp = "); VMOut_PutI((i32)s->sp); VMOut_PutS(", capacity = "); VMOut_PutI((i32)s->capacity); VMOut_PutN();
#ifdef STACK_CHECK
    if (s->sp >= Stack_Capacity(s)-1) { printMsg(s, "overflow"); return; }
#endif
    s->si[++s->sp] = value;
#ifdef STACK_CHECK
    if (s->sp > s->waterMark) {
        s->waterMark = s->sp;
    }
    VMOut_PutS("\nsp = "); VMOut_PutI((i32)s->sp); VMOut_PutS(", waterMark = "); VMOut_PutI((i32)s->waterMark); VMOut_PutN();
#endif
//t    VMOut_PutS("\nPush: val = "); VMOut_PutI(value); VMOut_PutN();
}

i32 Stack_Pop(Stack s) {
#ifdef STACK_CHECK
    if (s->sp < 0)
        printMsg(s, "underflow");
//    else
#endif
    return s->si[s->sp--];
}

bool Stack_IsEmpty(Stack s) { return s->sp == -1; }

#ifdef STACK_CHECK
i8 Stack_WaterMark(Stack s) { return s->waterMark; }
#endif

typedef struct StackEnumtorDesc {
    Stack  stack;
    i8     index;       
} StackEnumtorDesc;

static StackEnumtorDesc stackEnumtorDesc; // One instance.

StackEnumtor StackEnumtor_New(Stack s) {
    StackEnumtor e = &stackEnumtorDesc;   // One instance.
    e->stack = s;
    StackEnumtor_Reset(e);
    return e;
}
StackEnumtor Stack_GetEnumerator(Stack s) { 
    return StackEnumtor_New(s); 
}
void StackEnumtor_Delete(StackEnumtor s) {  } // One instance.

bool StackEnumtor_MoveNext(StackEnumtor s) { return --s->index >= 0; }
u32  StackEnumtor_Current(StackEnumtor s)  { return (u32)s->stack->si[s->index]; }
void StackEnumtor_Reset(StackEnumtor s)    { s->index = s->stack->sp+1; }

void Stack_Print(StackEnumtor e) {
    StackEnumtor_Reset(e);
    VMOut_PutC('[');
    while ( StackEnumtor_MoveNext(e) ) {
        u32 n = StackEnumtor_Current(e);
        VMOut_PutS(" ");
        VMOut_PutX(n);
    }
    VMOut_PutS(" ]");
}

// ----------------------- Inherent Mode Support

i32  Stack_top (Stack s) { return s->si[s->sp]; }
void Stack_dup(Stack s)  { Stack_Push(s, s->si[s->sp]); }
void Stack_not(Stack s)  { s->si[s->sp] = ~s->si[s->sp]; }

void Stack_and(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (i32)(v1 & v2);
}
void Stack_or(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (i32)(v1 | v2);
}
void Stack_xor(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (i32)(v1 ^ v2);
}
void Stack_neg(Stack s) { s->si[s->sp] = -s->si[s->sp]; }
void Stack_inc(Stack s) { s->si[s->sp]++; } /* ++*s->sp; */
void Stack_dec(Stack s) { s->si[s->sp]--; } /* --*s->sp; */
void Stack_add(Stack s) {    
    i32 v2 = s->si[s->sp--]; /* pop */
    s->si[s->sp] += v2;
}
void Stack_sub(Stack s) {    
    i32 v2 = s->si[s->sp--]; /* pop */
    s->si[s->sp] -= v2;
}
void Stack_mul(Stack s) {    
    i32 v2 = s->si[s->sp--]; /* pop */
    s->si[s->sp] *= v2;
}
void Stack_div(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    if (v2 == 0) {
        VMOut_PutS("Division by zero\n");
        return;
    }
    s->si[s->sp] /= v2;
}
void Stack_rem(Stack s) {     
    i32 v2 = s->si[s->sp--]; /* pop */
    s->si[s->sp] %= v2;
}
void Stack_shl(Stack s) {    
    i32 v2 = s->si[s->sp--]; /* pop */
    s->si[s->sp] <<= v2;
}
void Stack_shr(Stack s) {     
    i32 v2 = s->si[s->sp--]; /* pop */
    s->si[s->sp] >>= v2;
}
void Stack_teq(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (v1 == v2)? 1 : 0;
}
void Stack_tne(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (v1 != v2)? 1 : 0;
}
void Stack_tlt(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (v1 < v2);
}
void Stack_tgt(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (v1 > v2);
}
void Stack_tle(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (v1 <= v2);
}
void Stack_tge(Stack s) {
    i32 v2 = s->si[s->sp--]; /* pop */
    i32 v1 = s->si[s->sp];
    s->si[s->sp] = (v1 >= v2);
}
//---------------------------------------------- Support for Functions
void Stack_enterU5(Stack s, u8 funcInfo) {
//t    VMOut_PutS("enter.u5: ENTRY sp = "); VMOut_PutX((u32)s->sp); VMOut_PutS("; bp = "); VMOut_PutX((u32)s->bp); VMOut_PutN();
    u8  v  = (funcInfo >> 4) & 0x01;
    u8  np = (funcInfo >> 2) & 0x03;
    u8  nl =  funcInfo       & 0x03;
    u8  fi = (v << 6) | (np << 3) | nl;
    i32 retAddr = s->si[s->sp--];   /* pop (save) caller's return address    */
#ifdef MONITOR
   VMOut_PutS("enter.u5: v = "); VMOut_PutU((u32)v);
   VMOut_PutS("; np = ");     VMOut_PutX((u32)np);  VMOut_PutS("; nl = ");     VMOut_PutX((u32)nl);
   VMOut_PutS("; fi = ");     VMOut_PutX((u32)fi);  VMOut_PutS("; oldbp = ");  VMOut_PutI((u32)s->bp); VMOut_PutN();
#endif
    s->sp  += nl;                 /* allocate space for local variables         */
    s->si[++s->sp] = fi;       /* push function information                  */
    s->si[++s->sp] = retAddr;  /* push back the caller's return address      */
    s->si[++s->sp] = s->bp; /* push (save) caller's bp (frame context)    */
    s->bp = s->sp;             /* set frame context for the current function */
//t    VMOut_PutS("enter.u5: EXIT  sp = "); VMOut_PutX((u32)s->sp); VMOut_PutS("; bp = "); VMOut_PutX((u32)s->bp); VMOut_PutN();
}
void Stack_enterU8(Stack s, u8 funcInfo) {
    u8  v  = (funcInfo >> 6) & 0x01;
    u8  np = (funcInfo >> 3) & 0x07;
    u8  nl =  funcInfo       & 0x07;
    u8  fi = (v << 6) | (np << 3) | nl;
    i32 retAddr = s->si[s->sp--];   /* pop (save) caller's return address    */
#ifdef MONITOR
   VMOut_PutS("enter.u8: v = "); VMOut_PutU((u32)v);
   VMOut_PutS("; np = ");        VMOut_PutX((u32)np);  VMOut_PutS("; nl = ");     VMOut_PutX((u32)nl);
   VMOut_PutS("; fi = ");        VMOut_PutX((u32)fi);  VMOut_PutS("; oldbp = ");  VMOut_PutI((u32)s->bp); VMOut_PutN();
#endif
    s->sp  += nl;                 /* allocate space for local variables         */
    s->si[++s->sp] = fi;       /* push function information                  */
    s->si[++s->sp] = retAddr;  /* push back the caller's return address      */
    s->si[++s->sp] = s->bp; /* push (save) caller's bp (frame context)    */
    s->bp    = s->sp;          /* set frame context for the current function */
}
void Stack_exit(Stack s) {
//t    VMOut_PutS("exit: ENTRY sp = "); VMOut_PutX((u32)s->sp); VMOut_PutS("; bp = "); VMOut_PutX((u32)s->bp); VMOut_PutN();
    u8   fi = (u8)s->si[s->bp-2];
    bool v  = (fi >> 6) & 0x01;
    u8   np = (fi >> 3) & 0x07;
    u8   nl =  fi       & 0x07;
    i32   retAddr, retVal = 0L; // to avoid warning not initialized.
#ifdef MONITOR
   VMOut_PutS("exit:  v = "); VMOut_PutU((u32)v);
   VMOut_PutS("; np = ");     VMOut_PutX((u32)np);  VMOut_PutS("; nl = ");     VMOut_PutX((u32)nl);
   VMOut_PutS("; fi = ");     VMOut_PutX((u32)fi);  VMOut_PutS("; oldbp = ");  VMOut_PutI((i32)s->bp); VMOut_PutN();
#endif
    if (v) retVal = s->si[s->sp--];       /* save the return value (if any)            */
    s->bp      = (u8)s->si[s->sp--];   /* pop (restore) caller's bp (frame context) */
    retAddr       = s->si[s->sp--];       /* pop (save) caller's return address        */
    s->sp     -= (np+nl+1);                  /* deallocate space for parameters, local
                                                   variables, and function information       */
    if (np+nl > 7) s->sp -= nl; /* March 8 */

    if (v) s->si[++s->sp] = retVal;       /* push back the return value (if any)       */
    s->si[++s->sp] = retAddr;             /* push back the caller's return address     */
//t    VMOut_PutS("exit: EXIT  sp = "); VMOut_PutX((u32)s->sp); VMOut_PutS("; bp = "); VMOut_PutX((u32)s->bp); VMOut_PutN();
}

static u8 getFrameOffset(u8 v, u8 np, u8 nl) {
    if (np+nl > 7)
        return (u8)2 + np + nl - v + nl;
    else
        return (u8)2 + np + nl - v;
}
//------------------------------------------------------------------------------
void Stack_addVariable(Stack s, u8 var) {
    u8 fi = (u8)s->si[s->bp-2];
    u8 np = (fi >> 3) & 0x07;
    u8 nl =  fi       & 0x07;
    u8 fo = getFrameOffset(var, np, nl);
#ifdef MONITOR
   VMOut_PutS("addv: var = "); VMOut_PutU((u32)var);
   VMOut_PutS("; fi = ");      VMOut_PutX((u32)fi);
   VMOut_PutS("; fo = ");      VMOut_PutX((u32)fo);
   VMOut_PutS("; v = ");       VMOut_PutX((u32)s->si[s->bp-fo].data.ival); VMOut_PutN();
#endif
    s->si[s->bp-fo] += Stack_Pop(s);
}
//------------------------------------------------------------------------------
void Stack_loadVariable(Stack s, u8 var) {
    u8 fi = (u8)s->si[s->bp-2];
    u8 np = (fi >> 3) & 0x07;
    u8 nl =  fi       & 0x07;
    u8 fo = getFrameOffset(var, np, nl);
#ifdef MONITOR
   VMOut_PutS("\r\n");
   VMOut_PutS("ldv: var = "); VMOut_PutU((u32)var);
   VMOut_PutS("; fi = ");     VMOut_PutX((u32)fi);
   VMOut_PutS("; fo = ");     VMOut_PutX((u32)fo);
   VMOut_PutS("; v = ");      VMOut_PutX((u32)s->si[s->bp-fo]); VMOut_PutN();
#endif
    Stack_Push(s, s->si[s->bp-fo]);
}
//------------------------------------------------------------------------------
void Stack_storeVariable(Stack s, u8 var) {
    u8  fi = (u8)s->si[s->bp-2];
    u8  np = (fi >> 3) & 0x07;
    u8  nl =  fi       & 0x07;
    u8  fo = getFrameOffset(var, np, nl);
#ifdef MONITOR
   VMOut_PutS("\r\n");
   VMOut_PutS("stv: var = "); VMOut_PutU((u32)var);
   VMOut_PutS("; fi = ");     VMOut_PutX((u32)fi);
   VMOut_PutS("; fo = ");     VMOut_PutX((u32)fo);
   VMOut_PutS("; v = ");      VMOut_PutX((u32)s->si[s->bp-fo]); VMOut_PutN();
#endif
    s->si[s->bp-fo] = Stack_Pop(s);
}
