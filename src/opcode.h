/* opcode.h
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/
#if !defined(OPCODE_H)
#    define  OPCODE_H

typedef enum {
// ----------------------------------------- INHERENT [0x00..0x2F]
// RFUn means Reserved for Future Used

    HALT, POP,  DUP,  EXIT, RET, RFU1, RFU2, RFU3, // 0x00 .. 0x07
    RFU4, RFU5, RFU6, RFU7, NOT,  AND,  OR,   XOR, // 0x08 .. 0x0F
    NEG,  INC,  DEC,  ADD,  SUB,  MUL,  DIV,  REM, // 0x10 .. 0x17
    SHL,  SHR,  TEQ,  TNE,  TLT,  TGT,  TLE,  TGE, // 0x18 .. 0x1F

INHERENT_END = 0x2F,

// ---------------------------------------- IMMEDIATE [0x30..0xAF] 
    BR_I5    = 0x30,
    BRF_I5   = 0x50,
    ENTER_U5 = 0x70,
    LDC_I3   = 0x90,
    ADDV_U3  = 0x98,
     LDV_U3  = 0xA0,
     STV_U3  = 0xA8,

// ---------------------------------------- RELATIVE [0xB0..0xFF]
RELATIVE_BEGIN = 0xB0,

    ADDV_U8 = 0xB0,
     LDV_U8 = 0xB1,
     STV_U8 = 0xB2,
    INCV_U8 = 0xB3,
    DECV_U8 = 0xB4,

   ENTER_U8 = 0xBF,

     LDA_I8  = 0xD4,
     LDA_I16 = 0xD5,

     LDC_C8  = 0xD7,
     LDC_C16 = 0xD8,
     LDC_I8  = 0xD9,
     LDC_I16 = 0xDA,
     LDC_I32 = 0xDB,
     LDC_U8  = 0xDC,
     LDC_U16 = 0xDD,
     LDC_U32 = 0xDE,

      BR_I8  = 0xE0,
      BR_I16 = 0xE1,
     BRF_I8  = 0xE3,
     BRF_I16 = 0xE4,

   CALLS_I8  = 0xE6,
   CALLS_I16 = 0xE7,

    TRAP     = 0xFF,
RELATIVE_END = 0xFF,

} Opcode;

#endif /* OPCODE_H */
