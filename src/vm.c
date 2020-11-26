/* vm.c - Cm Virtual Machine implementation
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "out.h"
#include "ioreg.h"
#include <string.h> /* strtok */
#include <stdlib.h> /* exit */

/* Cm EVM Error messages */
#define RESERVED_FOR_FUTURE_USED       " is reserved for future used."
#define FATAL_ERROR                    "Fatal error("
#define CANNOT_BE_ALLOCATED            "cannot be allocated.\n"

#include "opcode.h"
#include "vmstack.h"
#include "vm.h"

#ifdef IORegOn
#include "ioreg.h"
#endif

extern  u8*   mem; // Need the memory address for the loader.

#define SpInitial  200

static u32    dp;
static u32    vt;
static Stack  runningStack;

static void checkFlags() {
//t  if (flags & Flags_Stack) { Stack_print(runningStack); }
}

static StackEnumtor se;

void VM_Init(u8* mainAddr) {
    u8 mainId;
    dp = 0UL;

#define Thread_StackCapacity 32

    mainId = 0;
    runningStack = Stack_New(Thread_StackCapacity); // Only one thread in cm VM.
    se = Stack_GetEnumerator(runningStack);

#ifdef MONITOR
    Stack_print(runningStack); putln();
#endif
}

#ifdef InterruptManagerOn
//-----------------------------------------------------------------------
// Interrupt Manager - first parameter is pushed first
//-----------------------------------------------------------------------
static void InterruptManager(int op) {
    u32 handlerAddr;
    u8  number;

    switch( op & 0x0F ) {
        case 0: Interrupt_Disable(); break;
        case 1: Interrupt_Enable(); break;
        case 2: Stack_Push(runningStack, (u32)Interrupt_SaveAndDisable()); break;
        case 3: Interrupt_Restore( Stack_Pop(runningStack) ); break;

        case 4: /* void Interrupt_SetVector(u8 number, u32 handlerAddr) */
            handlerAddr = (u32)Stack_Pop(runningStack);
            number      =  (u8)Stack_Pop(runningStack);
            Interrupt_SetVector(number, handlerAddr);
            break;

        case 5: /* u32  Interrupt_GetVector(u8 number) */
            number = (u8)Stack_Pop(runningStack);
            Stack_Push(runningStack, (u32)Interrupt_GetVector(number));
            break;
    }
}
#endif
//-----------------------------------------------------------------------
// Kernel I/O Manager
//-----------------------------------------------------------------------
static void Kernel_IO(int op) {
//t    VMOut_PutS("Kernel I/O: op = "); VMOut_PutX((i32)op & 0x0F); VMOut_PutN();
//t    VMOut_PutS("top = "); VMOut_PutX((int)Stack_top(runningStack)); VMOut_PutN();
    switch( op & 0x0F ) {
        case 0: /* Put(bool)    */ VMOut_PutB(      Stack_Pop(runningStack)); break;
        case 1: /* Put(char)    */ VMOut_PutC((char)Stack_Pop(runningStack)); break;
        case 2: /* Put(int)     */ VMOut_PutI(Stack_Pop(runningStack)); break;
        case 3: /* Put(uint)    */ VMOut_PutU(Stack_Pop(runningStack)); break;
//      case 4: /*                 VMOut_PutF(): No support for float in Cm VM      */
        case 5: /* Put(cstring) */ VMOut_PutS( (char*)Stack_Pop(runningStack) ); break;
        case 6: /* PutHex(uint) */ VMOut_PutX(       Stack_Pop(runningStack));  break;
        case 7: /* PutLine()    */ VMOut_PutN(); break;
    }
}

static void exitOnInvalidOpcode(u8 opcode) {
    VMOut_PutS("Opcode: "); VMOut_PutX(opcode); VMOut_PutS(RESERVED_FOR_FUTURE_USED); VMOut_PutN();
    exit(0);
}
static void exitThreadNotSupported(void) {
    VMOut_PutS("This version does not support threads.\n");
    exit(0);
}

void VM_execute(u8* startAddr) {
    u8  opcode;
    u8* ip;

//t VMOut_PutS("sizeof u8* ip = "); VMOut_PutU((u32)sizeof(ip)); VMOut_PutN();

    for (ip = startAddr; opcode = *ip;) {
//t        VMOut_PutS("ip = "); VMOut_PutX((u32)ip); VMOut_PutS("*ip = "); VMOut_PutX((u32)opcode); VMOut_PutN();

        if ( opcode > HALT  &&  opcode <= INHERENT_END ) {
            switch(opcode) {
                case POP:    Stack_Pop(runningStack); ip++; break;
                case DUP:    Stack_dup(runningStack); ip++; break;
                case EXIT:   Stack_exit(runningStack); ip = (u8*)Stack_Pop(runningStack); break;
                case RET:    ip = (u8*)Stack_Pop(runningStack); break;
                case NOT:    Stack_not(runningStack);   ip++; break;
                case AND:    Stack_and(runningStack);   ip++; break;
                case OR:     Stack_or (runningStack);   ip++; break;
                case XOR:    Stack_xor(runningStack);   ip++; break;
                case NEG:    Stack_neg(runningStack);   ip++; break;
                case INC:    Stack_inc(runningStack);   ip++; break;
                case DEC:    Stack_dec(runningStack);   ip++; break;
                case ADD:    Stack_add(runningStack);   ip++; break;
                case SUB:    Stack_sub(runningStack);   ip++; break;
                case MUL:    Stack_mul(runningStack);   ip++; break;
                case DIV:    Stack_div(runningStack);   ip++; break;
                case REM:    Stack_rem(runningStack);   ip++; break;
                case SHL:    Stack_shl(runningStack);   ip++; break;
                case SHR:    Stack_shr(runningStack);   ip++; break;
                case TEQ:    Stack_teq(runningStack);   ip++; break;
                case TNE:    Stack_tne(runningStack);   ip++; break;
                case TLT:    Stack_tlt(runningStack);   ip++; break;
                case TGT:    Stack_tgt(runningStack);   ip++; break;
                case TLE:    Stack_tle(runningStack);   ip++; break;
                case TGE:    Stack_tge(runningStack);   ip++; break;
            }
        } else if ( opcode >= BR_I5     &&  opcode < BRF_I5   ) {
            i8 offset = opcode - BR_I5;
            if (offset >= 0x10) offset = (offset - 0x10) | 0xFFFFFFF0;
            ip += offset;
//t            VMOut_PutS("xBR_I5 at "); VMOut_PutX((u32)ip); VMOut_PutN();
        } else if ( opcode >= BRF_I5    &&  opcode < ENTER_U5 ) {
            i8 offset = opcode - BRF_I5;
            if (offset >= 0x10) offset = (offset - 0x10) | 0xFFFFFFF0;
            if (Stack_Pop(runningStack)) offset = 1; /* no branching if true */
            ip += offset;
//t            VMOut_PutS("xBRF_I5 at "); VMOut_PutX((u32)ip); VMOut_PutN();
        } else if ( opcode >= ENTER_U5  &&  opcode < LDC_I3   ) {
            u8 fctInfo = opcode - ENTER_U5;
            Stack_enterU5(runningStack, fctInfo);
            ip++;
        } else if ( opcode >= LDC_I3    &&  opcode < ADDV_U3  ) {
            short constant = (opcode & 0x07) % 0x08;
            if (constant >= 0x04) constant = (constant - 0x04) | 0xFFFC;
            Stack_Push(runningStack, (i16)constant);
#ifdef ExampleOfStackDump
    se = Stack_GetEnumerator(runningStack);
    Stack_Print(se);
    VMOut_PutS("\n");
#endif
            ip++;
        } else if ( opcode >= ADDV_U3   &&  opcode < LDV_U3   ) {
            Stack_addVariable(runningStack, opcode & 0x07);
            ip++;
        } else if ( opcode >= LDV_U3    &&  opcode < STV_U3   ) {
            Stack_loadVariable(runningStack, opcode & 0x07);
            ip++;
        } else if ( opcode >= STV_U3    &&  opcode < ADDV_U8  ) {
            Stack_storeVariable(runningStack, opcode & 0x07);
            ip++;
        } else if (opcode >= RELATIVE_BEGIN && opcode <= RELATIVE_END) {
            switch(opcode) {
            case ADDV_U8: {
                Stack_addVariable(runningStack, *(ip + 1));
                ip += 2;
                break;
            }
            case LDV_U8: {
                Stack_loadVariable(runningStack, *(ip + 1));
                ip += 2;
                break;
            }
            case STV_U8: {
                Stack_storeVariable(runningStack, *(ip + 1));
                ip += 2;
                break;
            }
            case INCV_U8: {
                Stack_Push(runningStack, 1);
                Stack_addVariable(runningStack, *(ip + 1));
                ip += 2;
                break;
            }
            case DECV_U8: {
                Stack_Push(runningStack, -1);
                Stack_addVariable(runningStack, *(ip + 1));
                ip += 2;
                break;
            }
            case ENTER_U8: {
                Stack_enterU8(runningStack, *(ip + 1));
                ip += 2;
                break;
            }
            case LDA_I8: {
                i8 offset = *(ip+1);
                Stack_Push(runningStack, (i32)(ip+offset));
                ip += 2;
                break;
            }
            case LDA_I16: {
                i16 offset = (*(ip+1) << 8) | *(ip+2);
                Stack_Push(runningStack, (i32)(ip+offset));
                ip += 3;
                break;
            }
            case LDC_C8:
            case LDC_I8:
            case LDC_U8: {
                u8   u8const = 0;
                i8   i8const = 0;

                if (opcode == LDC_I8) i8const = *(ip+1); else u8const = *(ip+1);
                Stack_Push(runningStack, (opcode == LDC_I8) ? (i32)i8const : (u32)u8const);
                ip += 2;
                break;
            }
            case LDC_C16:
            case LDC_I16:
            case LDC_U16: {
                u16  u16const = 0;
                i16  i16const = 0;

                if (opcode == LDC_I16) {
                    i16const = (*(ip + 1) << 8) | *(ip + 2);
                } else {
                    u16const = (*(ip + 1) << 8) | *(ip + 2);
                }
                Stack_Push(runningStack, (opcode == LDC_I16) ? (i32)i16const : (u32)u16const);
                ip += 3;
                break;
            }
            case  LDC_I32:
            case  LDC_U32: {
                u8  msb = *++ip;
                u8  lsb = *++ip;
                u32 lsoffset, msoffset = (u32)( ((msb << 8) & 0x0000FF00L) | (lsb & 0x000000FFL));
                i32 offset;

                msb = *++ip;
                lsb = *++ip;
                lsoffset = (u32)( ((msb << 8) & 0x0000FF00L) | (lsb & 0x000000FFL));

                offset = (i32)( (msoffset << 16) | lsoffset );

                Stack_Push(runningStack, offset);
                ip++;
                break;
            }
            case BR_I8: {
                i8 relAddr = *(ip+1);
                ip += relAddr;
                break;
            }
            case BRF_I8: {
                i8 relAddr;
                if (Stack_Pop(runningStack))
                    relAddr = 2; /* no branching if true */
                else
                    relAddr = *(ip+1); /* false, then branch */

                ip += relAddr;
                break;
            }
            case BR_I16: {
                i16 relAddr = (*(ip+1) << 8) | *(ip+2);
                ip += relAddr;
                break;
            }
            case CALLS_I16: {
                i16 relAddr = (*(ip+1) << 8) | *(ip+2);
                Stack_Push(runningStack, (i32)(ip+3));
                ip += relAddr;
//t                VMOut_PutS("calls_16 (ip = "); VMOut_PutX((i32)ip); VMOut_PutN();
                break;
            }
            case TRAP: {
                u8 op = *(ip+1);
                switch( op ) {
#ifdef InterruptManagerOn
                    // Interrupt Manager
                    case 0x00: case 0x01: case 0x02: case 0x03: case 0x04: case 0x05:
                        InterruptManager(op);
                        break;
#endif
                    /* Kernel I/O */
                    case 0x80: case 0x81: case 0x82: case 0x83:
                    case 0x84: case 0x85: case 0x86: case 0x87:
                        Kernel_IO(op);
                        break;
                }
                ip += 2;
                break;
            }
            default:
                exitOnInvalidOpcode(opcode);
            }
        }
//----------------------------------
//t        Stack_Print(se);
        checkFlags();
    }
}
