#include <stdio.h>
#include <unistd.h>

/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/
//Name: Daniel Bairamian
//McGill ID: 260669560


int main(){

    // create the pointer to the output file
    FILE *outputFile;
    // variable that is going to hold the stdout value for later
    int saved_stdout;
    // point our file pointer to the desired file with write access
    outputFile = fopen("redirect_out.txt" , "w");
    // save the stdout to our integer
    saved_stdout = dup(1);
    printf("First : Print to stdout \n");
    // redirect our output to the file
    dup2(fileno(outputFile), 1);
    fprintf(outputFile , "Second :  Print to redirect_out.txt \n");
    // redirect the output back to the terminal
    dup2(saved_stdout, 1);
    printf("Third : Print to stdout \n");
    return 1;
}