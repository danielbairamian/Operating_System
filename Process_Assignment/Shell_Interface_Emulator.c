/*
----------------- COMP 310/ECSE 427 Winter 2018 -----------------
I declare that the awesomeness below is a genuine piece of work
and falls under the McGill code of conduct, to the best of my knowledge.
-----------------------------------------------------------------
*/ 

//Name: Daniel Bairamian
//McGill ID: 260669560

//all the header files you would require
#include <stdio.h>  //for standard IO
#include <unistd.h> //for execvp/dup/dup2
#include <string.h> //for string manipulation
#include <stdlib.h> //for fork  
#include <ctype.h>  //for character type check (isnum,isalpha,isupper etc)
#include <sys/wait.h>//for waitpid
#include <fcntl.h>  //open function to open a file. type "man 2 open" in terminal
#include <time.h>   //to handle time

//pointer to Linked list head
struct node *head_job = NULL;

//pointer to
struct node *current_job = NULL;

//global variable used to store process id of process
//that has been just created
//can be used while adding a job to linked list
pid_t process_id;

//flag variable to check if redirection of output is required
int isred = 0;

//currentWordDirectory
char cwd [1024];

//structure of a single node
//donot modify this structure
struct node
{
    int number;        //the job number
    int pid;           //the process id of the process
    char *cmd;         //string to store the command name
    time_t spawn;      //time to store the time it was spawned
    struct node *next; //when another process is called you add to the end of the linked list
};

// Add a job to the linked list
void addToJobList(char *args[])
{
    //allocate memory for the new job
    struct node *job = malloc(sizeof(struct node));
    //If the job list is empty, create a new head
    if (head_job == NULL)
    {
        //printf("Add to job list but head job is null :) \n");

        //init the job number with 1
        job->number = 1;
        
        //set its pid from the global variable process_id
        job->pid = process_id;
        
        //cmd can be set to arg[0]
        job->cmd = args[0];
        
        //set the job->next to point to NULL.
        job->next = NULL;
        
        //set the job->spawn using time function
        job->spawn = (unsigned int)time(NULL);

        //set head_job to be the job
        head_job = job;
        
        //set current_job to be head_job
        current_job = head_job;
        
    }

    //Otherwise create a new job node and link the current node to it
    else
    {
        //point current_job to head_job
        current_job = head_job;
        
        int jobNumber = (current_job->number) + 1;

        //traverse the linked list to reach the last job
        while(current_job->next != NULL){
            //printf("NEXT YOB PLs \n");
            current_job = current_job->next;
            jobNumber++;           
        }

        //init all values of the job like above num,pid,cmd.spawn
        //set the next of job to be NULL
        //make next of current_job point to job
        //make job to be current_job
        
        job->number = jobNumber;
        job->cmd = args[0];
        job->pid = process_id;
        job->spawn = (unsigned int)time(NULL);
        job->next = NULL;
        current_job->next = job;
        current_job = job;      
        
    }
}

//Function to refresh job list
//Run through jobs in linked list and check
//if they are done executing then remove it
void refreshJobList()
{
    //pointer require to perform operation 
    //on linked list
    struct node *current_job;
    struct node *prev_job;
    
    struct node *temp_node;

    //variable to store returned pid 
    pid_t ret_pid;

    //perform init for pointers
    current_job = head_job;
    prev_job = head_job;
    temp_node = head_job;

    //traverse through the linked list
    while (current_job != NULL)
    {
        //use waitpid to init ret_pid variable
        ret_pid = waitpid(current_job->pid, NULL, WNOHANG);
        //one of the below needs node removal from linked list
        if (ret_pid == 0)
        {
            //the returned pid is equal to 0 , this means that the passed pid job has not
            //finished from executing

            //iterate to the next job , and set the previous job to be the current job
            temp_node=current_job;
            prev_job = temp_node;
            current_job = current_job->next;    
        }
        else
        {
            //if the returned pid is not equal to 0 , then this means that the process has terminated
           //need to remove the process from the joblist

            //special case for the head job , it terminated and it doesn't have a job after it
            if(current_job==head_job && head_job->next == NULL){
                temp_node=current_job->next;
                prev_job->next=temp_node;
                free(current_job);
                head_job = NULL;
                break;
            }
            // head job is done but it has a next job
            else if(current_job==head_job && head_job->next != NULL){
                temp_node=current_job->next;
                prev_job->next=temp_node;
                free(current_job);
                current_job = temp_node;
                head_job = temp_node;
            }
            //non head job terminated
            else{
                temp_node=current_job->next;
                prev_job->next=temp_node;
                free(current_job);
                current_job = temp_node;    
            } 
        }
    }
    return;
}

//Function that list all the jobs
void listAllJobs()
{
    struct node *current_job;
    int ret_pid;

    //refresh the linked list
    refreshJobList();

    //init current_job with head_job
    if(head_job != NULL){
    current_job=head_job;
    }else{
        current_job = NULL;
    }
    //heading row print only once.
    printf("\nID\tPID\tCmd\tstatus\tspawn-time\n");

        while(current_job != NULL){
        printf("%d\t%d\t%s\tRUNNING\t%s\n", current_job->number, current_job->pid, current_job->cmd, ctime(&(current_job->spawn)));
        current_job = current_job->next;
        }
            
        
    return;
}

// wait till the linked list is empty
// you would have to look for a place 
// where you would call this function.
// donot modify this function
void waitForEmptyLL(int nice, int bg)
{
    if (nice == 1 && bg == 0)
    {
        while (head_job != NULL)
        {
            sleep(1);
            refreshJobList();
        }
    }
    return;
}

//function to perform word count
 int wordCount(char *filename,char* flag)
 {
     int cnt;
     //if flag is l 
     //count the number of lines in the file 
     //set it in cnt

     //if flag is w
     //count the number of words in the file
     //set it in cnt
    // if either of the arguments are null , return with -1 and print an error message
    if(flag==NULL || filename==NULL ){
        printf("One of the argument is not specified \n");
        cnt = -1;
    }
    else if(!strcmp("-l" , flag)){
        //print out the number of lines
        FILE *fp = fopen(filename, "r");
        //if fp is null that means the fopen operation failed
        if(fp == NULL){
            perror("Error: ");
            cnt = -1;
        }else{
            int ch = 0;
            while(ch != EOF){
            ch = fgetc(fp);
            if(ch == '\n'){
                cnt++;
            }
        }
    }     
    }else if(!strcmp("-w", flag)){
        //print out the number of words
        FILE *fp = fopen(filename, "r");
        //if fp is null that means the fopen operation failed
        if(fp == NULL){
            perror("Error: ");
            cnt = -1;
        }else{
        int ch = 0;
        while(ch != EOF){
            ch = fgetc(fp);
            if(ch == ' ' || ch == '\n'){
              cnt++;
            }
        }
    }
    //if both if statements that test for flag fail
    //this means the flag is something other than -l or -w   
    }else{
        printf("Unrecognizable flag \n");
        cnt = -1;
    } 
     return cnt;
 }

// function to augment waiting times for a process
// donot modify this function
void performAugmentedWait()
{
    int w, rem;
    time_t now;
    srand((unsigned int)(time(&now)));
    w = rand() % 3;
    printf("sleeping for %d\n", w);
    rem = sleep(w);
    return;
}

//simulates running process to foreground
//by making the parent process wait for
//a particular process id.
int waitforjob(char *jobnc)
{
    struct node *trv;
    int jobn = (*jobnc) - '0';
    trv = head_job;
    //traverse through linked list and find the corresponding job
    //hint : traversal done in other functions too

    //if correspoding job is found 
    //use its pid to make the parent process wait.
    //waitpid with proper argument needed here
    if(trv->number == jobn){
        waitpid(trv->pid , NULL , WUNTRACED);
    }else{
        while(trv->next != NULL){
            trv = trv->next;
            if(trv->number == jobn){
                waitpid(trv->pid, NULL, WUNTRACED);
                break;
            }
        
        }
    }
    
        
    
    return 0;
}

// splits whatever the user enters and sets the background/nice flag variable
// and returns the number of tokens processed
// donot modify this function
int getcmd(char *prompt, char *args[], int *background, int *nice)
{
    int length, i = 0;
    char *token, *loc;
    char *line = NULL;
    size_t linecap = 0;
    printf("%s", prompt);
    length = getline(&line, &linecap, stdin);
    if (length <= 0)
    {
        exit(-1);
    }
    // Check if background is specified..
    if ((loc = index(line, '&')) != NULL)
    {
        *background = 1;
        *loc = ' ';
    }
    else
        *background = 0;
    while ((token = strsep(&line, " \t\n")) != NULL)
    {
        for (int j = 0; j < strlen(token); j++)
            if (token[j] <= 32)
                token[j] = '\0';
        if (strlen(token) > 0)
        {
            if (!strcmp("nice", token))
            {
                *nice = 1;
            }
            else
            {
                args[i++] = token;
            }
        }
    }
    return i;
}

// this initialises the args to All null.
// donot modify
void initialize(char *args[])
{
    for (int i = 0; i < 20; i++)
    {
        args[i] = NULL;
    }
    return;
}

int main(void)
{
    //args is a array of charater pointers
    //where each pointer points to a string
    //which may be command , flag or filename
    char *args[20];

    //flag variables for background, status and nice
    //bg set to 1 if the command is to executed in background
    //nice set to 1 if the command is nice
    //status  
    int bg, status, nice;

    //variable to store the process id.
    pid_t pid;

    //variable to store the file des
    //helpful in output redirection
    int fd1, fd2;

    //your terminal executes endlessly unless 
    //exit command is receivedprocess_id
    while (1)
    {
        //init background to zero
        bg = 0;
        //init nice to zero
        nice = 0;
        //init args to null
        initialize(args);
        //get the user input command
        int cnt = getcmd("\n>> ", args, &bg, &nice);
        //keep asking unless the user enters something
        while (!(cnt >= 1))
            cnt = getcmd("\n>> ", args, &bg, &nice);

        //use the if-else ladder to handle built-in commands
        //built in commands don't need redirection
        //also no need to add them to jobs linked list
        //as they always run in foreground
        if (!strcmp("jobs", args[0]))
        {
            //call the listalljobs function
            listAllJobs();
        }
        else if (!strcmp("exit", args[0]))
        {
            //exit the execution of endless while loop 
            exit(0);
        }
        else if (!strcmp("fg", args[0]))
        {
            //bring a background process to foregrounf
            if(args[1] != NULL){
             waitforjob(args[1]);
            }else{
              printf("Unspecified job to put in foreground \n");  
            }
        }
        else if (!strcmp("cd", args[0]))
        {
            int result = 0;
            // if no destination directory given 
            // change to home directory 

            //if given directory does not exist
            //print directory does not exit

            //if everthing is fine 
            //change to destination directory 
            if(args[1]==NULL){
                chdir(getenv("HOME"));
            }else{
                int ret = chdir (args[1]);
                if (ret == -1){
                    printf("Cannot find the directory %s \n" , args[1]);
                }
            }
        }
        else if (!strcmp("pwd", args[0]))
        {
            //use getcwd and print the current working directory
            printf("Current directory is: %s" , getcwd(cwd, sizeof(cwd)));
            
        }
        else if(!strcmp("wc",args[0]))
        {
            //call the word count function
            int wordC = wordCount(args[2],args[1]);
            char *flagName ;
            if(args[1]==NULL){
                //do nothing
                flagName = "placeholderText";
            }
            else if(!strcmp("-l" , args[1])){
                flagName = "line(s)";
            }else if(!strcmp("-w", args[1])){
                flagName = "word(s)";
            }
            if(wordC >= 0){
                printf("The file %s has %d %s in it \n", args[2] , wordC , flagName);
            }
            

        }
        else
        {
            //Now handle the executable commands here 
            /* the steps can be..:
            (1) fork a child process using fork()
            (2) the child process will invoke execvp()
            (3) if background is not specified, the parent will wait,
                otherwise parent starts the next command... */


            //hint : samosas are nice but often there 
            //is a long waiting line for it.

            //create a child
            waitForEmptyLL(nice , bg);
            pid = fork();
            
            //to check if it is parent
            if (pid > 0)
            {
                //we are inside parent
                //printf("inside the parent\n");
                if (bg == 0)
                {
                    //FOREGROUND
                    // waitpid with proper argument required
                    waitForEmptyLL(nice , bg);
                    waitpid(pid, NULL, WUNTRACED);
                }
                else
                {
                    //BACKGROUND
                    process_id = pid;
                    addToJobList(args);
                    // waitpid with proper argument required
                    waitForEmptyLL(nice , bg);
                    waitpid(process_id, NULL, WNOHANG);
                }
            }
            else
            {
                // we are inside the child

                //introducing augmented delay
                performAugmentedWait();

                //check for redirection
                //now you know what does args store
                //check if args has ">"
                //if yes set isred to 1
                //else set isred to 0
                
                int i = 0;
                for(i ; i < strlen(*args) ; i++){
                    if(args[i] == NULL){
                        isred = 0;
                    }else{
                        if(!strcmp(">", args[i])){
                        isred = 1;
                        break;
                        }else{
                        isred = 0;   
                        }
                    }


                }

                //if redirection is enabled
                if (isred == 1)
                {
                    //open file and change output from stdout to that  
                    //make sure you use all the read write exec authorisation flags
                    //while you use open (man 2 open) to open file
                    FILE *outputFile;
                    // variable that is going to hold the stdout value for later
                    int saved_stdout;
                    // point our file pointer to the desired file with write access
                    outputFile = fopen(args[i+1] , "w");
                    if(outputFile == NULL){
                      perror("Error: ");
                    }else{
                    // save the stdout to our integer
                    saved_stdout = dup(1);
                    // redirect our output to the file
                    dup2(fileno(outputFile), 1);
                    //set ">" and redirected filename to NUL
                    args[i] = NULL;     
                    args[i + 1] = NULL;

                    //run your command
                    waitForEmptyLL(nice , bg);
                    execvp(args[0], args);

                    //restore to stdout
                    dup2(saved_stdout, 1);
                    fflush(stdout);
                    }
                   
                    
                }
                else
                {
                    //simply execute the command.
                    waitForEmptyLL(nice , bg);
                    int successCommand = execvp(args[0], args);
                    if (successCommand == -1){
                        perror("Error: ");
                    }

                }
            }
        }
    }

    return 0;
}