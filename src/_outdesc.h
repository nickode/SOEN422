/* _outdesc.h - Interface for VM Output Descriptor
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVM_OutDesc_h
#define __CmVM_OutDesc_h

#include "_out.h"

// Private VM Output Function Pointer Types:
typedef void (*VMPutB)(bool);
typedef void (*VMPutC)(char);
typedef void (*VMPutI)(i32);
typedef void (*VMPutU)(u32);
typedef void (*VMPutS)(const char*);
typedef void (*VMPutX)(u32);
typedef void (*VMPutN)(void);

// Private Interface Output Descriptor
typedef struct IVMOutDesc {
    VMPutB pb;
    VMPutC pc;
    VMPutI pi;
    VMPutU pu;
    VMPutS ps;
    VMPutX px;
    VMPutN pn;
} IVMOutDesc;

#endif
