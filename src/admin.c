/* admin.c - admin for the Cm Embedded Virtual Machine which:
//         - isolates the <stdio.h> with all put* in the VM
//         - defines _CRT_SECURE_NO_WARNINGS to avoid all MS secure crap on **_s
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include <stdio.h>  /* for FILE   */
#include <string.h> /* for strtok */

#include "hal.h"
#include "hal_Out.h"
#include "vm.h"

#include "hal_Loader.h"

#ifdef Dos16
#define Target      "(Dos16)"
#elif defined(Arm7)
#define Target      "(Arm7)"
#else
#define Target      "(Win32)"
#endif

#if LaterForSoen422SerialLoader
#include "IStream.h"
#include "Stream.h"

#include "ILoader.h"
#include "Loader.h"
#endif

#define VMName      "Cm Virtual Machine "
#define AppSuffix   ""
#define AppName     "cm"
#define Version     " v0.1.00.1101a "    
#define Copyright   "Copyright (c) 2001-2020  Michel de Champlain"

enum Status { Success = 0x40, UnknownCmd, InvalidCmd, InvalidAddr, ChecksumInvalid  };

// Banner = VMname AppSuffix Version Copyright
static void DisplayBanner() {
    VMOut_PutS(VMName); VMOut_PutS(AppSuffix); VMOut_PutS(Version); VMOut_PutS(Target); VMOut_PutN();
    VMOut_PutS(Copyright); VMOut_PutN();
}
static void Usage() {
    VMOut_PutS("\nUsage: "); VMOut_PutS(AppName); VMOut_PutS(" Options? <file.exe>\n");
    VMOut_PutS("\n         -v          Display the version and exit.");
    VMOut_PutS("\n         -? -help    Display options and exit.\n");
}

#define MemMax        4096
#define MemAllocated  (4096+1024)
/*public*/  u8*    mem;
/*public*/  u8     memAllocated[MemAllocated];
bool status;

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
    buf[1] = (u8)fgetc(f);             // Read size.msb
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
    System_putc('\n'); System_putu(size); System_puts(" bytes loaded.\n");
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

int main(int argc, char* argv[]) {
    char  filename[200]; // On Win32, you need a big buffer because in VS IDE filenames are passed with full path.
    const char* name;
    const char* ext;
    int   i = 1;

//t VMOut_PutS("argv[0] = [%s]\n", argv[0]);
//t VMOut_PutS("argv[1] = [%s]\n", argv[1]);

    COut_Init();
    // Do Hal_Init() before any option messages.
    Hal_Init();
    
    // ********* Important to adjust memory before loading the file in memory.
//t    VMOut_PutS("GetBaseAddr(): sizeof u8* = "); VMOut_PutI((i32)sizeof(u8*)); VMOut_PutN();
//t    VMOut_PutS("GetBaseAddr(): sizeof u32 = "); VMOut_PutI((i32)sizeof(u32)); VMOut_PutN();

    mem = GetBaseAddr(memAllocated, (u32)1024UL);
//t    VMOut_PutS("Admin: memAllocated = "); VMOut_PutX((u32)memAllocated); VMOut_PutN();
//t    VMOut_PutS("Admin: mem          = "); VMOut_PutX((u32)mem); VMOut_PutN();

    /* Parse options */
    for (; i < argc; i++) {
        if ( (strcmp(argv[i], "-?") == 0) || (strcmp(argv[i], "-help") == 0) ) {
            Usage();
            return 0;
        } else if (strcmp(argv[i], "-v") == 0) {
            DisplayBanner();
            return 0;
        } else {
            break;
        }
    }

    /* Parse file */
    if (i == argc-1) {
        char *pfile;

        strcpy(filename, argv[i]);   /* save name and extension */
//t        VMOut_PutS("Parse file: Filename: '%s'\n", filename);

        name = GetFileName(filename);
        ext  = GetFilenameExt(filename);
        strcpy(filename, name);

//t        VMOut_PutS("Filename: '%s' Name: '%s' Ext: '%s':\n", filename, name, ext);

        if (ext && (strcmp(ext, "exe") == 0)) {  /* 3 characters extension maximum */
            char pb[50];

            strcpy(pb, "");
            pfile = strcat(pb, filename);

//t            VMOut_PutS("fopen: Filename: '%s'\n", pfile);

            file = fopen(pfile, "rb" );
            if (file == NULL) {
                VMOut_PutS(filename); VMOut_PutS(" does not exist.\n");
                return -1;
            }

            if (!loadObjFile(file, MemMax)) { // not a success because too big
                return -2;
            }
        } else {
            VMOut_PutS("Error: Must have a file with '.exe' extension.\n");
            Usage();
            return -3;
        }

    } else {
        VMOut_PutS("Error: Must have a file to load.\n");
        Usage();
        return -4;
    }

    while (1) {
        if ((status = hal_Loader(mem)) == Success) {
            DisplayBanner();
            VM_Init(mem);
            VM_execute(mem);

            // Send an Ack to tell the Host that program's execution is done.
            VMOut_PutC((char)ACK);
            VMOut_PutC((char)0);
        }
        else {
            VMOut_PutS("Loader error: "); VMOut_PutX(status); VMOut_PutN();
            return -1;
        }
    }

    return 0;
}
