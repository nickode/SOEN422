/* admin0.c - admin for the Cm Embedded Virtual Machine which:
//          - isolates the <stdio.h> with all put* in the VM
//          - defines _CRT_SECURE_NO_WARNINGS to avoid all MS secure crap on **_s
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

//----[OnHost - Code for the host platform only to load programs from the command-line]--------
#ifdef OnHost
#include <stdio.h>  /* for FILE   */
#include <string.h> /* for strtok */
#endif

#include "hal.h"
#include "hal_Out.h"
#include "vm.h"

#if LaterForSoen422SerialLoader
#include "IStream.h"
#include "Stream.h"

#include "ILoader.h"
#include "Loader.h"
#endif

#ifdef Dos16
#define Target      "(Dos16)"
#elif defined(ArduinoNano)
#define Target      "(ATMega328P)"
#elif defined(Arm7)
#define Target      "(Arm7)"
#else
#define Target      "(Win32)"
#endif

#define VMName      "Cm Virtual Machine "
#define AppSuffix   ""
#define AppName     "cm"
#define Version     " v0.1.00.1101a "
#define Copyright   "Copyright (c) 2001-2020  Michel de Champlain"
#define OnHost

// Banner = VMname AppSuffix Version Copyright
static void DisplayBanner() {
    VMOut_PutS(VMName); VMOut_PutS(AppSuffix); VMOut_PutS(Version); VMOut_PutS(Target); VMOut_PutN();
    VMOut_PutS(Copyright); VMOut_PutN();
}

#ifdef OnHost
#define MemMax         4096
#define MemAllocated  (4096+1024)
/*public*/  u8*    mem;
/*public*/  u8     memAllocated[MemAllocated];
#else
///*public*/  u8     mem[] = { 0x91, 0xFF, 0x82, 0x00 };

u8 mem[]= { 0xE1, 0x00, 0x25, 0x71, 0xD5, 0x00, 0x2F, 0xFF, 0x85, 0xD5, 0x00, 0x44, 0xFF, 0x85,
0xD9, 0x09, 0xA8, 0xE0, 0x0E, 0xA0, 0x90, 0x1C, 0xE3, 0x04, 0xE0, 0x09, 0xA0, 0xB4, 0x00, 0xFF,
0x82, 0xE0, 0xF4, 0xFF, 0x87, 0x03, 0x04, 0xE7, 0xFF, 0xFF, 0xE7, 0xFF, 0xDB, 0x00, 0x54, 0x2E,
0x53, 0x74, 0x6D, 0x74, 0x00, 0x54, 0x65, 0x73, 0x74, 0x20, 0x31, 0x31, 0x3A, 0x20, 0x62, 0x72,
0x65, 0x61, 0x6B, 0x20, 0x53, 0x74, 0x61, 0x74, 0x65, 0x6D, 0x65, 0x6E, 0x74, 0x0A, 0x00, 0x39,
0x38, 0x37, 0x36, 0x35, 0x34, 0x33, 0x32, 0x31, 0x30, 0x0A, 0x00 };

#endif

//----[OnHost - Code for the host platform only to load programs from the command-line]--------
#ifdef OnHost

static void Usage() {
    VMOut_PutS("\nUsage: "); VMOut_PutS(AppName); VMOut_PutS(" Options? <file.exe>\n");
    VMOut_PutS("\n         -v          Display the version and exit.");
    VMOut_PutS("\n         -? -help    Display options and exit.\n");
}

// To get the base RAM address on a memory segment increment.
static u8* GetBaseAddr(u8* memAddr, u32 memInc) {
    u32 a = (u32)memAddr + memInc;
    u32 m = memInc - 1U;
//t    VMOut_PutS("Admin: a = "); VMOut_PutX((u32)a); VMOut_PutN();
//t    VMOut_PutS("Admin: m = "); VMOut_PutX((u32)m); VMOut_PutN();

    u32 r = a & ~m;
//t    VMOut_PutS("Admin: r = "); VMOut_PutX((u32)r); VMOut_PutN();
    return (u8*)r;
}

FILE* file;

/* 1st two bytes are the size (msb:lsb) */
static bool loadObjFile(FILE* f, u16 maxSize) {
    u16 n, size;
    u8  buf[2];

    buf[0] = (u8)fgetc(f);             // Read size.msb
    buf[1] = (u8)fgetc(f);             // Read size.lsb
    size = (u16)((buf[0] << 8) | buf[1]);

//t VMOut_PutS("loadObjFile of size = %u\n", (u32)size);

    if (size <= maxSize) {
        for (n = 0; n < size; n++) {
            mem[n] = (u8)fgetc(f);
#ifdef MONITOR
            VMOut_PutS(".");
            VMOut_PutS("%02x ", (u8)mem[n]);
#endif
        }
    } else {
        VMOut_PutS("Executable file too big (should be <= "); VMOut_PutU((u32)maxSize); VMOut_PutS(" bytes).\n");
        return false;
    }
    fclose(f);
#ifdef MONITOR
    VMOut_PutC('\n'); VMOut_PutU( (u32)size ); VMOut_PutS(" bytes loaded.\n");
#endif
    return true;
}

// Returns the filename extention.
const char *GetFilenameExt(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

// Returns filename portion of the given path (for Unix or Windows)
// Returns empty string if path is directory
const char *GetFileName(const char *path) {
    const char *pfile = path + strlen(path);
    for (; pfile > path; pfile--) {
        if ((*pfile == '\\') || (*pfile == '/')) {
            pfile++;
            break;
        }
    }
    return pfile;
}
#endif

int main(int argc, char* argv[]) {
    Hal_Init();
    COut_Init(); // Initiliaze UART console
    DisplayBanner();

#ifdef MONITOR
    VMOut_PutS("Admin: mem          = "); VMOut_PutX((u32)mem); VMOut_PutN();
    VMOut_PutS("sizeof(int) = "); VMOut_PutI((u32)sizeof(int));  VMOut_PutN();
    VMOut_PutS("sizeof(ptr) = "); VMOut_PutI((u32)sizeof(int*)); VMOut_PutN();
#endif



    VM_Init(mem);
    VM_execute(mem);
    return 0;
}
