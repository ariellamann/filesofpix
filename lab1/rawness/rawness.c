#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include "pnmrdr.h"

// #define T Pnmrdr_T

 
//void read(FILE *filename);
char convert(int i);


int main(int argc, char *argv[])
{
    if (argc < 2) {
        printf("requires more args\n");
    } else {
        FILE *fp = fopen(argv[1], "r");
        if (fp == NULL) {
            fprintf(stderr, "%s: %s %s %s\n", argv[0], 
            "Could not open file", argv[1], "for reading.");
            exit(1);
        }
        //This calls the read function and stores the P5 file to newFile var
        //read(fp);

        Pnmrdr_T tee = Pnmrdr_new(fp);
    
        Pnmrdr_mapdata picInfo = Pnmrdr_data(tee);
        

        //Make a new file to store the p5 contents in
        //CHECK SYNTAX ON THIS STUFF
        //Does this even put P5 on the top of the p5file?
        //This is printing the hesader stuff for the P5 file 
        fprintf(stdout, "%s\n %s\n %u %u\n %u\n", "P5", "# piccy.pgm", picInfo.width, picInfo.height, picInfo.denominator);

        //USING THE FILE
        // //Skips the first line of p2 file?
        // getline(filename);
        // //For the next 3 lines, copy over the contents
        // for (int i = 0; i < 3; i++) {
        //     p5file >> getline(filename);
        // }
        // printf("here3\n");

        //for loop for the table
        for (unsigned int i = 0; i < picInfo.height; i++) {
            for (unsigned int i = 0; i < picInfo.width; i++) {
                //get int
                int num = Pnmrdr_get(tee);

                //convert int
                // char c = convert(num);

                //store char
                fprintf(stdout, "%c", num);
            }
        }
        
        //Opening the newFile????
        // while (fscanf(newFile, "%s %s %s %s %s", )) {

        // }
        Pnmrdr_free(&tee);
        fclose(fp);
    }
    
    //exit failsure
    return EXIT_SUCCESS;
	// printf("Hello, world!\n");

}

// void read(FILE *filename) {
//     // printf("here\n");
//     //This is a function call within another function call; the larger function 
//     //returns a map data, and the inside function call returns a T (which is 
//     //the Pmn reader) 
//     //After this we have all the picutre info in a struct named picInfo
//     printf("before\n");
//     Pnmrdr_T tee = Pnmrdr_new(filename);
//     printf("after\n");
//     Pnmrdr_mapdata picInfo = Pnmrdr_data(tee);
//     printf("here1\n");

//     //Make a new file to store the p5 contents in
//     //CHECK SYNTAX ON THIS STUFF
//     //Does this even put P5 on the top of the p5file?
//     //This is printing the hesader stuff for the P5 file 
//     fprintf(stdout, "%s\n %s\n %u %s %u\n %u\n", "P5", "# piccy.pgm", picInfo.width, " ", picInfo.height, picInfo.denominator);

//     //USING THE FILE
//     // //Skips the first line of p2 file?
//     // getline(filename);
//     // //For the next 3 lines, copy over the contents
//     // for (int i = 0; i < 3; i++) {
//     //     p5file >> getline(filename);
//     // }
//     // printf("here3\n");

//     //for loop for the table
//     for (unsigned int i = 0; i < picInfo.height; i++) {
//         for (unsigned int i = 0; i < picInfo.width; i++) {
//             //get int
//             int num = Pnmrdr_get(tee);

//             //convert int
//             char c = convert(num);

//             //store char
//             fprintf(stdout, "%d", c);
//         }
//     }
//     printf("here4\n");
// }

char convert(int i) {
    return '0' + i;
}



// //Open the file
// //Read through the file
// //Make a new file
// //convert ascii to char function 
//     //for the first line of the file, add P5
//     //for the next 3 lines of the old file, copy over to new file
//     //then for the and store these characters into a new file

