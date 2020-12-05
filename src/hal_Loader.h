// Added by Nicolas Samaha

#ifndef __CmVM_VMLoader_h
#define __CmVM_VMLoader_h

#define ACK 0xCC
#define NAK 0x33

#include "_out.h"

bool hal_Loader(u8 mem); // If true, loading was successful.

#endif