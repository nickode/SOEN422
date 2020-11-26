/* _stdtype.h - Cm VM Standard (basic) type definitions (VS2019 Host Version)
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#ifndef __CmVM_stdtype_h
#define __CmVM_stdtype_h

#include <stdint.h>

#define bool               unsigned long
#define false              ((bool)0)
#define true               ((bool)1)

typedef uint8_t   u8;
typedef int8_t    i8;
typedef uint16_t  u16;
typedef int16_t   i16;
typedef uint32_t  u32;
typedef int32_t   i32;
typedef float     f32;

#ifdef Ptr16bits
typedef           u16*     ptr;
#else
typedef           u32*     ptr;
#endif

#endif
