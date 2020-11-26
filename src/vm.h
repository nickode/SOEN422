/* vm.h - Cm Virtual Machine - Interface
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __vm_h
#define __vm_h

#include "_stdtype.h"

void VM_Init(u8* mainAddr);
void VM_execute(u8* startAddr);

#endif
