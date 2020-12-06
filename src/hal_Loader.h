// Added by Nicolas Samaha

#ifndef __CmVM_VMLoader_h
#define __CmVM_VMLoader_h

#define ACK 0xCC
#define NAK 0x33
#define PACKET_SIZE 11

enum Status { SUCCESS = 0x40, UNKNOWN_COMMAND, INVALID_COMMAND, INVALID_ADDRESS, MEMORY_FAILED, CHECKSUM_INVALID };

enum Cmd {CMD_BEGIN = 0x20, PING = CMD_BEGIN, DOWNLOAD, RUN, GET_STATUS, SEND_DATA, RESET, CMD_END};

#include "_out.h"

bool hal_Loader(FILE* file, u8 mem); // If true, loading was successful.

#endif