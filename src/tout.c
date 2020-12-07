/* tout.c - Test VMOut_Put* functions - was tout.c
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "hal.h"     // hal_Init()
#include "hal_Out.h" // All VMOut_Put*

int main(void) {
    Hal_Init();

    VMOut_PutS("Test Out:<2\n");
    VMOut_PutS("Bools: [T|F|T|T]\n");
    VMOut_PutS("Ints:  [-1|-2147483648|2147483647|4294967295|FFFFFFFF]\n");

    VMOut_PutS("Bools: [");
    VMOut_PutB(-1); VMOut_PutC('|');
    VMOut_PutB(0);  VMOut_PutC('|');
    VMOut_PutB(1);  VMOut_PutC('|');
    VMOut_PutB(2);  VMOut_PutS("]\n");

    VMOut_PutS("Ints:  [");
    VMOut_PutI(0xFFFFFFFFL); VMOut_PutC('|');
    VMOut_PutI(0x80000000L); VMOut_PutC('|');
    VMOut_PutI(0x7FFFFFFFL); VMOut_PutC('|');
    VMOut_PutU(0xFFFFFFFFL); VMOut_PutC('|');
    VMOut_PutX(0xFFFFFFFFL);
    VMOut_PutS("]\n");
    return 0;
}
