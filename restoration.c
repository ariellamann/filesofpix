/*
 *     restoration.c
 *     by Nehir Ozden and Ariella Mann, Due: 1/31/23
 *     filesofpix
 *
 *     This file contains the implementation of the restoration program. It 
 *     takes in a corrupted P2 file, uncorrupts it, and returns the uncorrupted 
 *     P2 file in stdout. Users can then see the uncorrupted photo by 
 *     redirecting the stdout into a pgm file! This implementation includes 
 *     readaline.h, a file that helps read in lines from the corrupted file.
 *
 */

#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "readaline.h"
#include "seq.h"
#include "atom.h"
#include "table.h"
#include <except.h>

Except_T TooManyArgs = { "More than one argument is supplied" };
Except_T CannotOpen = { "The named input file cannot be opened" };
Except_T NoInput = { "Input is expected but not supplied" };
Except_T CannotAllocate = { "Memory cannot be allocated using malloc" };

void parse(char **datapp, int size, Seq_T DigitSeq, 
          Seq_T NondigSeq, Seq_T Temp);
int *HundredthsStore(Seq_T Temp);
int *TenthStore(Seq_T Temp);
void createMutliDigit(Seq_T Temp, Seq_T DigitSeq);
const char *turnToAtom(Seq_T NondigSeq);
void storeTable(Seq_T DigitSeq, const char *NondigKey, Table_T LinesTable, 
              Seq_T OGLines, const char **thekey);
void checkMalloc(void *allocated);
void makeP5(int width, int height, Seq_T OGLines);
void runProgram(FILE *fp);
void firstCall(char *datapp, int lineSize, Table_T LinesTable, 
              Seq_T OGLines, const char *key);
static void vfree(const void *key, void **value, void *cl);
void runProgramHelper(Seq_T SeqToBeFreed);

/********** main ********
 *
 * Runs the restoration program and then returns EXIT_SUCCESS
 *
 * Parameters:
 *      int argc:     The number of arguments on the command line
 *      char *argv[]: An array of arguments from the command line
 *
 * Return: EXIT_SUCCESS after successfully running the restoration
           program
 *
 * Expects: Two arguments on the command line
 * 
 * Notes: After main is run, the corrupted pgm given in from the command line 
 *        should be un-corrupted, and a user should be able to view this 
 *        image if the output of this program is directed into a pgm file.
 ************************/
int main(int argc, char *argv[]) {
    FILE *fp;

    /* Opens the file and calls necessary functions */
    if (argc == 1) {
        fp = stdin;
        runProgram(fp);
    } else if (argc > 2) {
        RAISE(TooManyArgs);
    } else if (argc == 2) {
        fp = fopen(argv[1], "r");
        if (fp == NULL) {
            RAISE(CannotOpen);
        }
        runProgram(fp);  
        fclose(fp);
    } 
    
    return EXIT_SUCCESS;
}

/********** parse ********
 *
 * Parses through a given line of characters, splitting up the non-digits and
 * digits into two seperate Hanson Sequences
 *
 * Parameters:
 *      char **datapp:    A pointer to the address of the first byte in a line
 *      int size:         The size of a the given line in bytes
 *      Seq_T *DigitSeq:  A Hanson Sequence that will hold the digits in a line 
 *      Seq_T *NondigSeq: A Hanson Sequence that will hold the non-digits in a
 *                        line
 *      Seq_T *Temp:      A Hanson Sequence that acts as a temporary sequence 
 *                        for digits while parsing through the line 
 *
 * Return: None
 *
 * Notes: None
 *     
 ************************/
void parse(char **datapp, int size, Seq_T DigitSeq, 
          Seq_T NondigSeq, Seq_T Temp) {

    for (int i = 0; i < size - 1; i++) {
        /* Check if the character is a digit */
        if (((*datapp)[i] >= 48) && ((*datapp)[i]<= 57)) {
            int *intptr = malloc(sizeof(int));
            checkMalloc(intptr);
            *intptr = (*datapp)[i];
            Seq_addhi(Temp, intptr);
        } else {
            char *charptr = malloc(sizeof(char));
            checkMalloc(charptr);
            *charptr = (*datapp)[i];
            Seq_addhi(NondigSeq, charptr);
            createMutliDigit(Temp, DigitSeq);
        }
    }  
    
    /* Checking if the line ends with a number, adding numbers if needed */
    createMutliDigit(Temp, DigitSeq);
    free(*datapp);
}

/********** TenthStore ********
 *
 * Converts numbers in a Hanson Sequence into a two digit number.
 *
 * Parameters:
 *      Seq_T *Temp: A Hanson Sequence that holds digits to be converted into
 *                   a two digit integer
 *
 * Return: An integer that is the converted two digit number
 *
 * Notes: This is a helper function for our createMultidigit function.
 *     
 ************************/
int *TenthStore(Seq_T Temp) {
    char *tenths = Seq_remlo(Temp);
    int tenths_int = (*tenths - '0') * 10;
    char *ones = Seq_remlo(Temp);
    int ones_int = *ones - '0';
    int store = tenths_int + ones_int;
    int *intptr = malloc(sizeof(int));
    checkMalloc(intptr);
    *intptr = store;
    free(tenths);
    free(ones);
    return intptr;
}

/********** HundredthsStore ********
 *
 * Converts numbers in a Hanson Sequence into a three digit number.
 *
 * Parameters:
 *      Seq_T *Temp: A Hanson Sequence that holds digits to be converted into
 *                   a three digit integer
 *
 * Return: An integer that is the converted three digit number
 *
 * Notes: This is a helper function for our createMultidigit function.
 *     
 ************************/
int *HundredthsStore(Seq_T Temp) {
    char *hundredths = Seq_remlo(Temp);
    int hundredths_int = (*hundredths - '0') * 100;
    char *tenths = Seq_remlo(Temp);
    int tenths_int = (*tenths - '0') * 10;
    char *ones = Seq_remlo(Temp);
    int ones_int = *ones - '0';
    int store = hundredths_int + tenths_int + ones_int;
    int *intptr = malloc(sizeof(int));
    checkMalloc(intptr);
    *intptr = store;
    free(hundredths);
    free(tenths);
    free(ones);
    return intptr;
}

/********** createMutliDigit ********
 *
 * Converts digits in a Hanson Sequence into 3, 2, or 1 digit integers depending
 * one the temp sequence's size.
 *
 * Parameters:
 *      Seq_T *Temp:     A Hanson Sequence that holds digits to be converted
 *                       into a 3, 2, or 1 digit integer
 *      Seq_T *DigitSeq: A Hanson Sequence that the converted integers 
 *                       are stored into 
 *
 * Return: None
 *
 * Notes: This is a helper function for our parse function
 *     
 ************************/
void createMutliDigit(Seq_T Temp, Seq_T DigitSeq) {
    if (Seq_length(Temp) == 3) {
        int *toBeStored = HundredthsStore(Temp);
        Seq_addhi(DigitSeq, toBeStored);
    } else if (Seq_length(Temp) == 2) {
        int *toBeStored = TenthStore(Temp);
        Seq_addhi(DigitSeq, toBeStored);
    } else if (Seq_length(Temp) == 1) {
        char *ones = Seq_remlo(Temp);
        int ones_int = *ones - '0';
        free(ones);
        int *intptr = malloc(sizeof(int));
        checkMalloc(intptr);
        *intptr = ones_int;
        Seq_addhi(DigitSeq, intptr);
    } 
}

/********** turnToAtom ********
 *
 * Turns a Hanson Sequence into an Atom
 *
 * Parameters:
 *      Seq_T *NondigSeq: A Hanson Sequence holding non-digits chars
 *
 * Return: An Atom holding the non-digits from the given sequence
 *
 * Notes: If there is a NULL character, we turn it into a different
 *        character to ensure that it doesn't cut the key off in 
 *        the middle. 
 *     
 ************************/
const char *turnToAtom(Seq_T NondigSeq) {
    char *NondigString = calloc(Seq_length(NondigSeq) + 1, 1);
    checkMalloc(NondigString);
    int size = Seq_length(NondigSeq);

    for (int i = 0; i < size; i++) {
        char *c = Seq_remlo(NondigSeq);
        NondigString[i] = *c;
        free(c);
    }
    
    const char *toReturn = Atom_new(NondigString, size);
    free(NondigString);
    Seq_free(&NondigSeq);
    return toReturn;
} 

/********** storeTable ********
 *
 * Stores the lines into the given Hanson Table, with the non-digit Atoms
 * as the keys and the digit Hanson Sequences as the value. The infused
 * lines are stored normally and the original lines' values will be 
 * returned and added to another Hanson Sequence of all the oringal 
 * lines. 
 *
 * Parameters:
 *      Seq_T *DigitSeq:       A Sequence of digits of a line from the file
 *      const char *NondigKey: A Atom that holds non-digits to be the key
 *      Table_T *LinesTable:   A Table that will hold infused lines and original
 *                             lines
 *      Seq_T *OrignialLines:  A Sequence that will hold all the original lines
 *      const char **thekey:   A pointer pointing to the address that will store
 *                             the non-digits that corresponde to all of the  
 *                             orignal lines.
 *
 * Return: None
 *
 * Notes: None
 *     
 ************************/
void storeTable(Seq_T DigitSeq, const char *NondigKey, Table_T LinesTable, 
              Seq_T OrignialLines, const char **thekey) {
    Seq_T TempSeq = Table_put(LinesTable, NondigKey, DigitSeq);

    if (TempSeq != NULL) {
        int size = Seq_length(TempSeq);
        *thekey = NondigKey;
        for (int i = 0; i < size; i++) {
            Seq_addhi(OrignialLines, Seq_remlo(TempSeq));
        }
        Seq_free(&TempSeq);
    } 
    (void) thekey;
}

/********** checkMalloc ********
 *
 * This raises an exception if memory has not been allocated correctly.
 *
 * Parameters:
 *      void *allocated: A pointer to space that has been (assumed) allocated
 *                       successfully.
 *
 * Return: None
 *
 * Expects: Allocated to not be NULL.
 *          
 * Notes: This is a helper function to help check memory allocation throughout
 *        our program.
 *     
 ************************/
void checkMalloc(void *allocated) {
    if (allocated == NULL) {
        RAISE(CannotAllocate);
    }
}

/********** makeP5 ********
 *
 * This takes the now uncorrupted P2 file and prints the P5 to stdout, 
 * with the correct file information. 
 *
 * Parameters:
 *      int width:           The width of the P2 raster
 *      int height:          The height of the P2 raster
 *      Seq_T OrignialLines: A Sequence that holds all the originals lines of a
 *                           file consecutively
 *
 * Return: None 
 *
 * Notes: After this is outputted to the stdout, the user is able to 
 *        redirect the output into a pgm file to check if the picture has 
 *        been uncorrupted and restored. 
 *     
 ************************/
void makeP5(int width, int height, Seq_T OrignialLines) {
    printf("P5\n%d %d\n%d\n", width, height, 255);
    int size = Seq_length(OrignialLines);

    for (int i = 0; i < size; i++) {
        char *forPrint = Seq_remlo(OrignialLines);
        printf("%c", *forPrint);
        free(forPrint);
    }
}

/********** runProgram ********
 *
 * Runs the restoration program, taking a corrupted file and outputting the 
 * uncorrupted version. 
 *
 * Parameters:
 *      FILE *fp: A pointer to the corrupted file given in by the user
 *
 * Return: None 
 *
 * Expects: fp to be not be NULL, and lineSize to be greater than zero
 *
 * Notes: None
 *     
 ************************/
void runProgram(FILE *fp) {
    char *datapp = NULL;
    Table_T LinesTable = Table_new(4000, NULL, NULL);
    Seq_T OGLines = Seq_new(500);
    const char *key = NULL;
    
    int lineSize = readaline(fp, &datapp);
    if (lineSize == 0) {
        RAISE(NoInput);
    }
    firstCall(datapp, lineSize, LinesTable, OGLines, key);

    int width, height = 0;
    while (datapp != NULL) {
        lineSize = readaline(fp, &datapp);
        if (lineSize > 0) {
            Seq_T DigitSeq = Seq_new(500);
            Seq_T NondigSeq = Seq_new(500);
            Seq_T TempDig = Seq_new(500);
            parse(&datapp, lineSize, DigitSeq, NondigSeq, TempDig);
            width = Seq_length(DigitSeq);
            const char *NondigKey = turnToAtom(NondigSeq);
            storeTable(DigitSeq, NondigKey, LinesTable, OGLines, &key);
            runProgramHelper(TempDig);
        } 
    }

    Seq_T value = Table_get(LinesTable, key);
    if (value != NULL) {
        int size = Seq_length(value);
        for (int i = 0; i < size; i++) {
            Seq_addhi(OGLines, Seq_remlo(value));
        }
    }
    height = Seq_length(OGLines)/width;
    makeP5(width, height, OGLines);
    Seq_free(&OGLines);
    Table_map(LinesTable, vfree, NULL);
    Table_free(&LinesTable);
}

/********** runProgramHelper ********
 *
 * A helper function for the runProgram function to free a Sequence
 *
 * Parameters:
 *      Seq_T SeqToBeFreed: A Sequence to be freed.
 *
 * Return: None 
 *
 * Notes: Helps shorten the runProgram function
 *     
 ************************/
void runProgramHelper(Seq_T SeqToBeFreed) {
    int size = Seq_length(SeqToBeFreed);
    for (int i = 0; i < size; i++) {
        free(Seq_remlo(SeqToBeFreed));
    }
    Seq_free(&SeqToBeFreed);
}

/********** firstCall ********
 *
 * Runs the program for the first time before looping later on in the program
 *
 * Parameters:
 *      char *datapp:       A pointer to the address of the first byte in a line
 *      int lineSize:       The size of a line in bytes
 *      Table_T LinesTable: A Hanson Table that holds the 
 *      Seq_T OGLines:      A Hanson Sequence of original lines
 *      const char *key:    A pointer pointing to the address that will store
 *                          the non digit sequence corresponding to an 
 *                          original line.
 *
 * Return: None 
 *
 * Notes: This runs the program once so that in the runProgram function, we can
 *        ensure that datapp will be poitning to something, whether it be NULL
 *        or a byte on a line. This then allows runProgram to function correctly
 *     
 ************************/
void firstCall(char *datapp, int lineSize,  Table_T LinesTable, 
              Seq_T OGLines, const char *key) {
    Seq_T DigitSeq = Seq_new(500);
    Seq_T NondigSeq = Seq_new(500);
    Seq_T TempDig = Seq_new(500);
    if (lineSize > 0) {
        parse(&datapp, lineSize, DigitSeq, NondigSeq, TempDig);
        const char *NondigKey = turnToAtom(NondigSeq);
        storeTable(DigitSeq, NondigKey, LinesTable, OGLines, &key);
    } 
    Seq_free(&TempDig);
}

/********** vfree ********
 *
 * Frees the information in a Hanson Table.
 *
 * Parameters:
 *      const void *key: A pointer to the key in the Hanson table
 *      void **value:    A pointer to the address of a value in the Hanson table
 *      void *cl:        A pointer to a closure variable
 *
 * Return: None 
 *
 * Notes: This goes through the Hanson table and frees all the information 
 *        inside of it. This is referenced from Hanson's Table implemenation.
 *     
 ************************/
static void vfree(const void *key, void **value, void *cl) {
    (void) key;
    (void) cl;
    int size = Seq_length(*value);
    for (int i = 0; i < size; i++) {
        free(Seq_remlo(*value));
    }
    Seq_T toFree = *value;
    Seq_free(&toFree);
}
