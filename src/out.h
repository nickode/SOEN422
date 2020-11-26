/* out.h - Cm VM Console Out Interface - no more macros.
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVM_VMOut_h
#define __CmVM_VMOut_h

#include "_out.h"

void VMOut_Init(IOut out);

void VMOut_PutB(bool b);
void VMOut_PutC(char c);
void VMOut_PutI(i32  i);
void VMOut_PutU(u32  u);
void VMOut_PutS(const char* s);
void VMOut_PutX(u32  x);
void VMOut_PutN(void);

#endif
