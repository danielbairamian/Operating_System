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

int main (){

char *cmd = "ls";
char *arg[1];
arg[0] = "ls";
char output[4096];

int fds[2];
pipe(fds);

    if (fork() == 0){
        //Child : execute ls using execvp
        dup2(fds[1], 1);
        close(fds[0]);
        execvp(cmd , arg);
    }
    else{
        //Parent: print output from ls here
        close(fds[1]);
        read(fds[0], output , sizeof(output));
        printf("%s" , output); 
    }

    return 0;
}