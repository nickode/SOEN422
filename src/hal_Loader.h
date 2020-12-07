// File added by Nicolas Samaha
#ifndef __CmVM_VMLoader_h
#define __CmVM_VMLoader_h

#include "_stdtype.h"

#define ACK 0xCC
#define NAK 0x33

enum Command { Ping = 0x20, Download = 0x21, GetStatus = 0x23, SendData = 0x24, Run = 0x22, Reset = 0x25 };
enum Status { Success = 0x40, UnknownCmd, InvalidCmd, InvalidAddr, ChecksumInvalid };

static bool downloading = false;
static u8 memWritePos = 0;
static u8 packetReadPos = 0;

void VMIn_Init();

char VMIn_GetByte();

unsigned long hal_Loader(u8* mem);

#endif