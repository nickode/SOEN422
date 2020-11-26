/* ioreg.h - IO Register Interface
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVM_IOReg_h
#define __CmVM_IOReg_h

#include "_stdtype.h"

u32  IOReg_Read (u32 ioreg);
void IOReg_Write(u32 ioreg, u32 value);

#endif
