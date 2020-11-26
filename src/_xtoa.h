/* _xtoa.h - 'to ascii' functions (xtoa): itoa, utoa, and ftoa (used only as private functions)
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVM_xtoa_h
#define __CmVM_xtoa_h

#include "_stdtype.h"

void _utoa(u32 n, char* buf, int next, u8 base);

void System_itoa(i32 i, char* buf);

#define     System_utoa(n,buf,base)          _utoa(n,buf,0,base)

#endif
