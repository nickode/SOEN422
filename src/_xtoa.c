/* _xtoa.c - 'to ascii' functions (xtoa): itoa, utoa, and ftoa (used only as private functions)
//
// Copyright (C) 1999-2020 by Michel de Champlain
//
*/

#include "_xtoa.h"

/* Converts 32-bit unsigned integer to a buffer. Base is 16 by default. */
void _utoa(u32 n, char* buf, int next, u8 base) {
    u32  r, f;
    bool foundNonzero = false;

    if (base == 10)
        f = 1000000000L;
    else if (base == 16)
        f = 0x10000000L;
    else {
        f = 0x10000000L;
        base = 16;
    }

    if (n == 0) {
        buf[next++] = '0';
    } else {
        while (f > 0) {
            r = n / f;
            if (foundNonzero || r > 0) {
                if (base == 10)
                    buf[next++] = (char)(r+'0');
                else
                    buf[next++] = (char)(r >= 10 ? r-10+'A': r+'0');
                foundNonzero = true;
            }
            n -= r * f;
            f /= base;
        }
    }
    buf[next] = '\0';
}

/* Converts 32-bit signed integer to a buffer. Base is 10 by default. */
void System_itoa(i32 i, char* buf) {
    int  next = 0;
    if (i < 0L) {
        buf[next++] = '-';
        i = -i;
    }
    _utoa(i, buf, next, 10);
}
