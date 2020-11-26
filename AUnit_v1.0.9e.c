/* AUnit.c - A Simple Language-agnostic Tool for Automated Unit Testing
//
// Copyright (C) 2006-2020 by Michel de Champlain
//
*/

#include <stdio.h>
#include <string.h>

#define bool    int
#define true    1
#define false   0

#define Usage                  1
#define FileDoesNotExist       2
#define FileFormat             3
#define MoreThanMaxLinesToTest 4

#define MaxTests             500
#define MaxCharactersInLine  256
#define MaxLinesToTest       9

typedef struct Test {
    int    number;
    char   name[MaxCharactersInLine];
    bool   failed;
} Test;

Test failedTests[MaxTests];
char linesToTest[MaxLinesToTest][MaxCharactersInLine];

int GetNumberOfFailedTests(void) {
    int nFailedTests = 0;
    int n;
    for (n = 0; n < MaxTests; n++) {
        if (failedTests[n].failed) ++nFailedTests;
    }
    return nFailedTests;
}

void ResetLinesToTest(void) {
    int n;
    for (n = 0; n < MaxLinesToTest; n++) {
        linesToTest[n][0] = '\0';
    }
}

bool StartsWith(const char* line, const char* with) {
    int withLength = strlen(with);
    return strncmp(line, with, withLength) == 0;
}

int main(int argc, char** argv) {
    char   line[MaxCharactersInLine];
    char*  filename;
    FILE*  file = NULL;
    int    testNumber = 0;

    printf("AUnit v1.0.9e.20201103\n");
    printf("Copyright (C) Michel de Champlain 2006-2020.  All Right Reserved.\n\n");

    if (argc != 2) {
        printf("Usage: AUnit <textFile>\n");
        return Usage;
    }
    filename = argv[1];
    file = fopen(filename, "r" );
    if (file == NULL) {
        printf("File '%s' does not exist.\n", filename);
        return FileDoesNotExist;
    }

    // Read all tests
    while (fgets(line, MaxCharactersInLine, file) != NULL) {
        char expectedOutput[MaxCharactersInLine];
        char status = '.';
        int n;

        if (StartsWith(line, "Test")) {
            int   nLinesToTest = 1;
            char* lessThanPtr = NULL;

            if ((lessThanPtr = strchr(line, '<')) != NULL) { // More than one line to test.
                nLinesToTest = *(lessThanPtr+1) - '0';
                // If not within [1..MaxLinesToTest], then forced it to 1.
                if (nLinesToTest < 1  ||  nLinesToTest > MaxLinesToTest) {
                    printf("\n\nError on line: %s\nTry to specify <'%c' lines to test (should be within [1..%d]).\n",
                           line, *(lessThanPtr+1), MaxLinesToTest);
                    return MoreThanMaxLinesToTest;
                }
                strncpy(failedTests[testNumber].name, line, lessThanPtr-line);
            } else {
                strncpy(failedTests[testNumber].name, line, strlen(line)-1); // -1 to remove the extra newline
            }

            failedTests[testNumber].number = testNumber;
            failedTests[testNumber].failed = false;

            ResetLinesToTest();
            for (n = 0; n < nLinesToTest; n++) {
                 if (fgets(linesToTest[n], MaxCharactersInLine, file) == NULL) {
                     printf("File '%s' is corrupted or have the wrong number of lines to test.\n", filename);
                     return FileFormat;
                 }
            }
            for (n = 0; n < nLinesToTest; n++) {
                 if (fgets(expectedOutput, MaxCharactersInLine, file) == NULL) {
                     printf("File '%s' is corrupted or have the wrong number of lines to test.\n", filename);
                     return FileFormat;
                 }
                 if (strncmp(expectedOutput, linesToTest[n], strlen(expectedOutput)) != 0) {
                     status = 'F';
                 }
            }
            if (status == 'F') {
                failedTests[testNumber].failed = true;
            }
            printf("%c", status);
        } else {
            printf("\nTest #%d  should start with \"Test ...\" (see file 'runTests.txt' for details)\n", testNumber+1);
            return 0;
        }
        ++testNumber;
    }

    printf("\n");

    // Report only if at least one test failed.
    if (GetNumberOfFailedTests() == 0) {
        printf("%d tests OK\n", testNumber);
    } else {
        int n;
        printf("\nFailed Tests:\n");
        for (n = 0; n < MaxTests; n++) {
            if (failedTests[n].failed) {
                printf("- %s\n", failedTests[n].name);
            }
        }
    }
    fclose(file);
}
