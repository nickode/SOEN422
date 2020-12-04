/* dumpFile.c
//
// Copyright (C) 1985-2020 by Michel de Champlain
//
*/

#include <stdio.h>
#include "_stdtype.h"

#define Usage                  1
#define FileDoesNotExist       2

/*-------------------------------------------------------------------
 * put_hexNibble - print a nibble as an hex digit character
 *-------------------------------------------------------------------*/
void put_hexNibble(u8 n) {
    n &= 0xF;
    putchar( n >= 10 ? n-10+'A': n+'0' );
}
/*-------------------------------------------------------------------
 * put_hexByte - print a byte as two hex digit characters
 *-------------------------------------------------------------------*/
void put_hexByte(u8 b) {
    put_hexNibble( b >> 4 );
    put_hexNibble( b      );
}
/*-------------------------------------------------------------------
 * put_hexWord - print a word as four hex digit characters
 *-------------------------------------------------------------------*/
void put_hexWord(u16 w) {
    put_hexByte( (u8)(w >> 8) );
    put_hexByte( (u8)w        );
}
/*-------------------------------------------------------------------
 * main
 *-------------------------------------------------------------------*/
int main(int argc, char** argv) {
    char* filename;
    FILE* file = NULL;
    int   c;
    u16   i = 16;
    u16   from = 0;

    printf("DumpFile v1.1.20201120\n");
    printf("Copyright (C) Michel de Champlain 1985-2020.  All Right Reserved.\n\n");

    if (argc != 2) {
        printf("Usage: dumpf <file>\n");
        return Usage;
    }
    filename = argv[1];
    file = fopen(filename, "r");
    if (file == NULL) {
        printf("File '%s' does not exist.\n", filename);
        return FileDoesNotExist;
    }

    while ( (c = fgetc(file)) != EOF ) {
        if ( i == 16 ) {
            putchar('\n');
            put_hexWord(from);
            putchar(' ');
            from += 16;
            i = 0;
        }

        put_hexByte( (u8)c );
        putchar(' ');
        i++;
    }
    putchar('\n');
    fclose(file);
    return 0;
}
