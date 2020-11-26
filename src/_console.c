/* _console.c -- Console Interface for Win32 (with Microsoft Visual C/C++ 2015 - VS14) which isolates 'putchar()' 
//               to avoid including <stdio.h> bringing all sort of conflicts with its kitchen sick of macros.
//
// Copyright (C) 1985-2020 by Michel de Champlain
//
// Jun 24, 2014 - just to wrap putchar() and avoid including <stdio.h> anywhere else.
*/

#include <stdio.h>    /* for only: putchar, getchar, fflush, stdin */

void Console_Putchar(char c) { putchar(c); }
