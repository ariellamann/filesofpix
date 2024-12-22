/*
 *     readaline.c
 *     by Nehir Ozden and Ariella Mann, Due: 1/31/23
 *     filesofpix
 * 
 *     This file contains the implementation of the readaline function. It 
 *     takes in a corrupted P2 file, and read a single line from the file. It 
 *     then returns the size of the line in bytes and sets a pointer to the file
 *     to the next line to be read. This implementation is included in the 
 *     restoration implementation, and is a crucial part of uncorrupting a P2 
 *     file. 
 *
 */

#include "readaline.h"
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <except.h>

Except_T NullArgs = { "One or more arguments are NULL" };
Except_T BadReading = { "Unable to read from file" };
Except_T BadMemory = { "Failed to allocate memory" };

/********** readaline ********
 *
 * Reads a single line of input from file given, and returns the size of the
 * line in bytes
 *
 * Parameters:
 *      FILE *inputfd: A file pointer pointing to an opened file
 *      char **datapp: A pointer that will be set to the address of the first 
 *                     byte in a line
 *
 * Return: The size of the line in bytes
 *
 * Expects: inputfd and datapp must not be NULL
 *
 * Notes: 
 *
 ************************/
size_t readaline(FILE *inputfd, char **datapp) {  
    if (datapp == NULL || inputfd == NULL) {
        RAISE(NullArgs);
    } 
 
    int size = 1000; 
    *datapp = malloc(size);

    if (datapp == NULL) {
        RAISE(BadMemory);
    }

    int currCharacter = 0;
    int lineIndex = 0;
    while ((currCharacter = fgetc(inputfd)) != 10) {
        if (currCharacter == EOF) {
            free(*datapp);
            if (ferror(inputfd)) {
                RAISE(BadReading);
            }
            *datapp = NULL;
            return 0;
        } 
        if (lineIndex > size) {
            fprintf(stderr, "%s", "readaline: input line too long\n");
            exit(4);
        } 
        (*datapp)[lineIndex] = currCharacter;
        lineIndex++;
    } 
    (*datapp)[lineIndex] = '\n';
    return lineIndex + 1;
}
