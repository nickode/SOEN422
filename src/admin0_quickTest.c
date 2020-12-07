/* admin0.c - is a quick admin.c prototype to make a first running port for the
//            Cm Embedded Virtual Machine with a "small (4-bytes) hardcoded program"
//            on the Arduino Nana with a Uart tx function setup at 9600, 8N1 (in the BSL).
//            No Uart loader is required for the completion of Task 4.
//
//            As an example (see below) to achieve this quick prototype,
//            I have stripped all unnecessary code required in admin.c for the Host version.
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "hal.h"
#include "hal_Out.h"
#include "vm.h"

#define Target      "(ATMega328P)"
#define VMName      "Cm Virtual Machine "
#define AppSuffix   ""
#define AppName     "cm"
#define Version     " v0.1.00.1101a "
#define Copyright   "Copyright (c) 2001-2020  Michel de Champlain"

// Banner = VMname AppSuffix Version Copyright
static void DisplayBanner() {
    VMOut_PutS(VMName); VMOut_PutS(AppSuffix); VMOut_PutS(Version); VMOut_PutS(Target); VMOut_PutN();
    VMOut_PutS(Copyright); VMOut_PutN();
}

// A "small (4-bytes) hardcoded program" which will
// print '1' on the console output (with PuTTY or HyperTerminal set at 9600 baud and 8N1):
u8 mem[] = { 0x91, 0xFF, 0x82, 0x00 };

int main(int argc, char* argv[]) {
    Hal_Init();
    DisplayBanner();

    VM_Init(mem);
    VM_execute(mem);
    return 0;
}
