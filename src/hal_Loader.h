// File added by Nicolas Samaha
#ifndef __CmVM_VMLoader_h
#define __CmVM_VMLoader_h

#include "_stdtype.h"

#define ACK 0xCC
#define NAK 0x33

enum Status { Success = 0x40, UnknownCmd, InvalidCmd, InvalidAddr, ChecksumInvalid };
u8 memWritePos = 0;
bool readingPacket = false;

void VMIn_Init();

char VMIn_GetByte();

static bool hal_Loader(u8 mem);

#endif