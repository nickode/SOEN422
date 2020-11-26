/* _out.h - Interface for Cm VM Output Interface
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVM_Out_h
#define __CmVM_Out_h

#include "_stdtype.h"

        struct IVMOutDesc;
typedef struct IVMOutDesc* IOut;

IOut Out_GetFactory(const char* whichOne);

#endif
