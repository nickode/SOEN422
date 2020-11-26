/* out.c - Out on Console for the VM Host
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "_outdesc.h"
#include "out.h"

static IOut vmOut;

void VMOut_Init(IOut out)      { vmOut = out;  }

void VMOut_PutB(bool b)        { vmOut->pb(b); }
void VMOut_PutC(char c)        { vmOut->pc(c); }
void VMOut_PutI(i32  i)        { vmOut->pi(i); }
void VMOut_PutU(u32  u)        { vmOut->pu(u); }
void VMOut_PutS(const char* s) { vmOut->ps(s); }
void VMOut_PutX(u32  x)        { vmOut->px(x); }
void VMOut_PutN(void)          { vmOut->pn();  }
