/*
 *     filename
 *     by name(s), date
 *     assignment
 *
 *     summary
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

void parse(char **datapp, int size, Seq_T *DigitSeq, 
          Seq_T *NondigSeq, Seq_T *Temp);
int *HundredthsStore(Seq_T *Temp);
int *TenthStore(Seq_T *Temp);
void createMutliDigit(Seq_T *Temp, Seq_T *DigitSeq);
const char *turnToAtom(Seq_T *NondigSeq);
void storeTable(Seq_T *DigitSeq, const char *NondigKey, Table_T *LinesTable, 
              Seq_T *OGLines, const char **thekey);
              /* const char *thekey*/
void checkMalloc(void *allocated);
void makeP2(int width, int height, Seq_T *OGLines);
void addToTable(Seq_T *LastSequence, Seq_T *OrignialLines);
void runProgram(FILE *fp);
void firstCall(char *datapp, int lineSize, Table_T *LinesTable, 
              Seq_T *OGLines, const char *key);
static void vfree(const void *key, void **value, void *cl);

/********** main ********
 *
 * Returns EXIT_SUCCESS OR 0?? after running the restoration program 
 *
 * Parameters:
 *      int argc:     The number of arguments on the command line
 *      char *argv[]: An array of arguments from the command line
 *
 * Return: EXIT_SUCCESS after running the restoration program (or 0??)
 *
 * Expects: Two arguments on the command line
 * 
 * Notes: After main is run, the corrupted pgm given in from the command line 
 *        should be un corrupted, and a user should be able to view this 
 *        image if the output of this program is put into a pgm file
 ************************/
int main(int argc, char *argv[]) {
    // /* Create datapp pointer */
    // char *datapp = NULL;
    FILE *fp;

    /* Open the file and call necessary functions */
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
    } 

    // if (argc < 2) {
    //     runProgram(stdin);
    //     RAISE(NoInput);
    // } else if (argc > 2) {
    //     RAISE(TooManyArgs);
    // } else {
    //     FILE *fp = fopen(argv[1], "r");
    //     if (fp == NULL) {
    //         RAISE(CannotOpen);
    //     }
    //     runProgram(fp);       
    // }
    return 0; /* EXIT SUCCESS NEEDED HERE*/
}

/********** parse ********
 *
 * Parses through a given line of characters, splitting up the non digits and
 * digits into two seperate Hanson Sequences
 *
 * Parameters:
 *      char **datapp:    The address of the first byte in a line
 *      int size:         The size of a line in bytes
 *      Seq_T *DigitSeq:  A pointer to the Hanson Sequence that will hold the 
 *                        digits in a line
 *      Seq_T *NondigSeq: A pointer to the Hanson Sequence that will hold the 
 *                        non digits in a line
 *      Seq_T *Temp:      A pointer to the Hanson Sequence acts as a temporary 
 *                        sequence while parsing a line
 *
 * Return: None
 *
 * Expects: NOTHING???? --> test on empty file...
 *
 * Notes: NONE?
 *     
 ************************/
void parse(char **datapp, int size, Seq_T *DigitSeq, 
          Seq_T *NondigSeq, Seq_T *Temp) {
    for (int i = 0; i < size - 1; i++) {
        /* Check if the character is a digit */
        if (((*datapp)[i] >= 48) && ((*datapp)[i]<= 57)) {
            int *intptr = malloc(sizeof(int));
            checkMalloc(intptr);
            *intptr = (*datapp)[i];
            Seq_addhi(*Temp, intptr);
        } else {
            char *charptr = malloc(sizeof(char));
            checkMalloc(charptr);
            *charptr = (*datapp)[i];
            Seq_addhi(*NondigSeq, charptr);
            //This made it 10 errs, but fucked up first picture
            //Seq_addhi(*NondigSeq, (*datapp));
            createMutliDigit(Temp, DigitSeq);
        }
    }  
    /* Checking if the line ends with a number, adding numbers if needed */
    createMutliDigit(Temp, DigitSeq);
}

/********** TenthStore ********
 *
 * Converts numbers in a Hanson Sequence into a two digit number.
 *
 * Parameters:
 *      Seq_T *Temp: A pointer to a Hanson Sequence that holds digits to be 
 *                   converted into a two digit number
 *
 * Return: An integer that is the converted two digit number
 *
 * Expects: Temp has a size of 2
 *
 * Notes: None
 *     
 ************************/
int *TenthStore(Seq_T *Temp) {
    char *tenths = Seq_remlo(*Temp);
    int tenths_int = (*tenths - '0') * 10;
    char *ones = Seq_remlo(*Temp);
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
 *      Seq_T *Temp: A pointer to a Hanson Sequence that holds digits to be 
 *                   converted into a three digit number
 *
 * Return: An integer that is the converted three digit number
 *
 * Expects: Temp has a size of 3
 *
 * Notes: None
 *     
 ************************/
int *HundredthsStore(Seq_T *Temp) {
    char *hundredths = Seq_remlo(*Temp);
    int hundredths_int = (*hundredths - '0') * 100;
    char *tenths = Seq_remlo(*Temp);
    int tenths_int = (*tenths - '0') * 10;
    char *ones = Seq_remlo(*Temp);
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
 * Converts digits in a Hanson Sequence into 3, 2 or 1 digit integers depending
 * one the sequence's size.
 *
 * Parameters:
 *      Seq_T *Temp:     A pointer to a Hanson Sequence that holds digits to be
 *                       converted into a three digit number
 *      Seq_T *DigitSeq: A pointer to a Hanson Sequence that the converted 3, 2,
 *                       or 1 digit numbers are stored into CHECK HERE!!!!!!!!!
 *
 * Return: None
 *
 * Expects: Temp to have a size 3, 2, or 1, and DigitSeq to be empty
 *
 * Notes: None
 *     
 ************************/
void createMutliDigit(Seq_T *Temp, Seq_T *DigitSeq) {
    if (Seq_length(*Temp) == 3) {
        int *toBeStored = HundredthsStore(Temp);
        Seq_addhi(*DigitSeq, toBeStored);
    } else if (Seq_length(*Temp) == 2) {
        int *toBeStored = TenthStore(Temp);
        Seq_addhi(*DigitSeq, toBeStored);
    } else if (Seq_length(*Temp) == 1) {
        char *ones = Seq_remlo(*Temp);
        int ones_int = *ones - '0';
        free(ones);
        int *intptr = malloc(sizeof(int));
        checkMalloc(intptr);
        *intptr = ones_int;
        Seq_addhi(*DigitSeq, intptr);
    } 
}

/********** turnToAtom ********
 *
 * Turns a Hanson Sequence into an Atom
 *
 * Parameters:
 *      Seq_T *NondigSeq: A pointer to a Hanson Sequence holding non digits
 *
 * Return: A pointer to an Atom holding the non digits from the Sequence
 *
 * Expects: NondigSeq to be not empty
 *
 * Notes: None
 *     
 ************************/
const char *turnToAtom(Seq_T *NondigSeq) {
    char *NondigString = calloc(Seq_length(*NondigSeq) + 1, 1);
    checkMalloc(NondigString);
    int size = Seq_length(*NondigSeq);

    for (int i = 0; i < size; i++) {
        char *c = Seq_remlo(*NondigSeq);
        NondigString[i] = *c;
        // free(c);
    }
    
    const char *toReturn = Atom_string(NondigString);
    free(NondigString);
    return toReturn;
} 

/********** storeTable ********
 *
 * Stores the infused lines in the given Hanson Table, or stores original
 * lines in the given original lines sequence depending using the Hanson Table's
 * functionality??.
 *
 * Parameters:
 *      Seq_T *DigitSeq:       A pointer to a Sequence of digits in a line from
 *                             the file
 *      const char *NondigKey: A pointer to an Atom that holds a key of the 
 *                             table
 *      Table_T *LinesTable:   A pointer to a Table that will hold infused lines
 *                             and original lines
 *      Seq_T *OrignialLines:  A pointer to a Sequence that will hold all the 
 *                             original lines
 *      const char **thekey:   A double pointer?? to 
 *
 * Return: 
 *
 * Expects:  DO THRISSS HERe
 *
 * Notes:  
 *     
 ************************/
void storeTable(Seq_T *DigitSeq, const char *NondigKey, Table_T *LinesTable, 
              Seq_T *OrignialLines, const char **thekey) {
                /* const char *thekey*/
    Seq_T TempSeq = Seq_new(500);
    TempSeq = Table_put(*LinesTable, NondigKey, *DigitSeq);

    if (TempSeq != NULL) {
        int size = Seq_length(TempSeq);
        *thekey = NondigKey;
        for (int i = 0; i < size; i++) {
            Seq_addhi(*OrignialLines, Seq_remlo(TempSeq));
        }
    } 
    (void) thekey;
}

void checkMalloc(void *allocated) {
    //Checks if memory allocation was succesful
    if (allocated == NULL) {
        RAISE(CannotAllocate);
    }
}

void makeP2(int width, int height, Seq_T *OrignialLines) {
    printf("P5\n%d %d\n%d\n", width, height, 255);
    int size = Seq_length(*OrignialLines);
    
    for (int i = 0; i < size; i++) {
        char *forPrint = Seq_remlo(*OrignialLines);
        printf("%c", *forPrint);
        free(forPrint);
    }
}

void runProgram(FILE *fp) {
    char *datapp = NULL;

    /* Create necessary sequences & table */
    Table_T LinesTable = Table_new(4000, NULL, NULL);
    Seq_T OGLines = Seq_new(500);
    const char *key = NULL;
    
    int lineSize = readaline(fp, &datapp);
    if (lineSize == 0) {
        RAISE(NoInput);
    }
    firstCall(datapp, lineSize, &LinesTable, &OGLines, key);
    int width, height = 0;
    while (datapp != NULL) {
        lineSize = readaline(fp, &datapp);
        if (lineSize > 0) {
            Seq_T DigitSeq = Seq_new(500);
            Seq_T NondigSeq = Seq_new(500);
            Seq_T TempDig = Seq_new(500);
            parse(&datapp, lineSize, &DigitSeq, &NondigSeq, &TempDig);
            width = Seq_length(DigitSeq);
            const char *NondigKey = turnToAtom(&NondigSeq);
            storeTable(&DigitSeq, NondigKey, &LinesTable, &OGLines, &key);
            height = Seq_length(OGLines)/width;
            // maybe here do the for loop through the sequences and 
            Seq_free(&TempDig);
            Seq_free(&DigitSeq);
            Seq_free(&NondigSeq);
        } 
        // else if (lineSize == 0 && datapp != NULL) {
        //     RAISE(NoInput);
        // }
        //else {
        //     RAISE(NoInput);
        // }
    }
    Seq_T value = Seq_new(500);
    value = Table_get(LinesTable, key);
    int size = Seq_length(value);
    for (int i = 0; i < size; i++) {
        addToTable(&value, &OGLines);
    }
    makeP2(width, height, &OGLines);

    //Now, technically, all the information from digitSeq and Nondigseq should
    //be in OG lines, or in the values of the table. We should then be able to
    //mao through this and have the values of both the seq an table removed, and
    //then free the hanson structures
    int sizeOG = Seq_length(OGLines);
    for (int i = 0; i < sizeOG; i++) {
        char *toFree = Seq_remlo(OGLines);
        free(toFree);
    }
    Seq_free(&OGLines);
    Table_map(LinesTable, vfree, NULL);
    Table_free(&LinesTable);
}

void addToTable(Seq_T *LastSequence, Seq_T *OrignialLines) {
    // char *charptr = malloc(sizeof(char));
    // checkMalloc(charptr);
    // charptr = Seq_remlo(*LastSequence);
    // Seq_addhi(*OrignialLines, charptr);
    //this made errs down to 15
    Seq_addhi(*OrignialLines, Seq_remlo(*LastSequence));
}

void firstCall(char *datapp, int lineSize,  Table_T *LinesTable, 
              Seq_T *OGLines, const char *key) {
    Seq_T DigitSeq = Seq_new(500);
    Seq_T NondigSeq = Seq_new(500);
    Seq_T TempDig = Seq_new(500);
    if (lineSize > 0) {
        parse(&datapp, lineSize, &DigitSeq, &NondigSeq, &TempDig);
        const char *NondigKey = turnToAtom(&NondigSeq);
        storeTable(&DigitSeq, NondigKey, LinesTable, OGLines, &key);
    } 
    //Trying to check for an empty file here, not catching it
    // else if (lineSize == 0 && datapp != NULL) {
    //     RAISE(NoInput);
    // }
    // else {
    //     RAISE(NoInput);
    // }
}

static void vfree(const void *key, void **value, void *cl) {
    (void) key;
    (void) cl;
    free(*value);
}

