#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
//#include "readaline.h"

// #define SIZE 1000

size_t readaline(FILE *inputfd, char **datapp);

int main(int argc, char *argv[]) {

    char **datapp;
    if (argc < 2) {
        printf("requires more args\n");
    } else {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: %s %s %s\n", argv[0], 
            "Could not open file", argv[1], "for reading.");
            exit(1);
        }
        int size = readaline(fp, datapp);
        printf("%d\n", size);
    }

    return 0;
}

size_t readaline(FILE *inputfd, char **datapp) {
    // FILE *counter = inputfd;
    // int lineLength = 0;
    // while (*counter != "\n") {
    //     lineLength++;
    //     counter 
    // }
    
    //ALLOCATING MEM
    
    int size = 500; 
    char *p = malloc(size * sizeof(char));
    //Above should return NULL if succesful, because the mem isn't used yet

    if (p == NULL) {
		fprintf(stderr, "Failed to malloc 500 bytes of memory\n");
		return EXIT_FAILURE;
	} 

	/* Print message showing address of allocated memory */
	fprintf(stderr, "Successfully allocated %d bytes of memory at address %p\n", size * sizeof(char), p);

    //should this be initialized? it was giving us an error when we set it to " "
    char c;
    int i = 0;
    while (c != '\n') {
        c = getc(inputfd);
        if (i > size) {
            size = size * 2;
            p = realloc(p, size * sizeof(char));
        } 
        p[i] = c;
        i++;
        printf("%c", c);
    }
    //SETTING THE PTR TO BE DATAPP -- NEEDS TO BE DEREFERENCED?
    // datapp = p;

    return i * sizeof(char);
    
    //returns the number of bytes in a line
    //sets inputfd to the address of the first byte in the next line
    //sets datapp to the address of the first byte in the line just read
}

/*
We made our readaline function skeleton, and now we are trying to figure out 
how to actually get all the characters in the line into one cocurrent array.
We dont understand how to add characters as we go down the line to a c arr
if it cannot change size, so maybe there is another way to do it.
Issue rn is with line 64 datapp += c






*/
