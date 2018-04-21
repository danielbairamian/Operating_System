#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>

int numProcess;
int numResource;

int *avail;
int *max;
int *hold;
int *need;
// binary semaphore that will ensure that only 1 process will be allowed to get allocation granted
sem_t mutex;


void printer(){
    printf("Number of process: %d \n", numProcess);
    printf("Number of resources: %d \n" , numResource);
    printf("Avail array: \n");
    for(int i = 0 ; i < numResource; i++){
        printf("%d ", avail[i]);
    }
    printf("\nMax array");
    for(int i = 0 ; i < numProcess; i ++){
        printf("\n");
        for(int j = 0 ; j < numResource; j++){
            printf("%d ", max[i*numResource+j]);
        }
    }
    printf("\nNeed array");
    for(int i = 0 ; i < numProcess; i ++){
        printf("\n");
        for(int j = 0 ; j < numResource; j++){
            printf("%d ", need[i*numResource+j]);
        }
    }
    printf("\nHold array");
    for(int i = 0 ; i < numProcess; i ++){
        printf("\n");
        for(int j = 0 ; j < numResource; j++){
            printf("%d ", hold[i*numResource+j]);
        }
    }
    printf("\n");
}

//check if a process is finished
int checkProcessFinish(int pid){
    for(int i = 0 ; i < numResource ; i++){
        if(need[pid*numResource + i] > 0){
            return 1;
        }
    }
    return 0;
}
/*
Simulates resource requests by processes 
*/
void request_simulator(int pr_id, int* request_vector){

}
/*
Implementation of isSafe() as described in the slides
*/
int isSafe(int resourceType){
    int *work = malloc(numResource * sizeof(int));
    int *finish = malloc(numProcess * sizeof(int));
    //Step 1
    //initialize work array
    for(int j = 0 ; j < numResource ; j++){
        work[j] = avail[j];
    }
    //initialize finish array
    for(int k = 0 ; k < numProcess ; k++){
        finish[k] = 0;
    }
    //Step 2
    for(int k = 0 ; k < numProcess ; k++){
        if((finish[k]==0)&&((need[k*numResource+resourceType])<=work[resourceType])){
            //Step 3
            //modify the work and finish arrays correctly
            work[resourceType]+=hold[k*numResource+resourceType];
            finish[k] = 1;
            //restart step 2
            k = 0;
        }
    }
    //Step 4
    for(int i = 0 ; i < numProcess; i++){
        if(finish[i]==0){
            printf("Can't allocate resource %d \n", resourceType);
            return 0;
        }
    }
    return 1;
}
/*
Implementation of Bankers Algorithm as described in the slides
returns 1 if safe allocation 0 if not safe
*/
int bankers_algorithm(int pr_id, int* request_vector){
    int resourceType;
    for(resourceType = 0 ; resourceType < numResource ; resourceType++){
    
    //initialize the banker algorithm variables
    int reqj = request_vector[resourceType];
    int needij = need[pr_id*numResource+resourceType];
    int availj = avail[resourceType];
    int holdij = hold[pr_id*numResource+resourceType];
    if(reqj==0){
        //didn't need or request this resource, try the next one
        continue;
    }
    printf("Process %d is requesting %d instances of resource %d \nit needs a total of %d, there are %d available\n"
    , pr_id, reqj, resourceType, needij, availj);
    if(reqj > needij){
        printer();
        printf("Process %d does not match its needs, there is an error in this program\n", pr_id);
        exit(-1);
    }
    //if the request of a variable is less than what is available
    //provisionally allocate the resources
    if(reqj<=availj){
        avail[resourceType] -= reqj;
        hold[pr_id*numResource+resourceType] += reqj;
        need[pr_id*numResource+resourceType] -= reqj;
    }else{
        printf("Process %d is requesting %d instances of resource %d, there are only %d available \n", pr_id, reqj , resourceType,availj);
        return 0;
    }
    //if the system is not safe after the provisional allocation, 
    //cancel the allocations made above
    if(!isSafe(resourceType)){
        for(int i = 0 ; i <= resourceType ; i++){
            avail[i] += request_vector[i];
            hold[pr_id*numResource+i] -= request_vector[i];
            need[pr_id*numResource+i] += request_vector[i];
        }
        return 0;
    }

    }
    return 1;
    
}

/*
Simulates processes running on the system.
*/
void* process_simulator(void* pr_id){
    int *PID_ptr = (int *)pr_id;
    int PID = *PID_ptr;
    //check if a process is finished , if it's not
    while(1){
    //check if the process finished
    //even though no other process will write anything in another process need array
    //we still encapsulate the call with the mutex to protect the global variable
    sem_wait(&mutex);
    //create a request vector
    int *req = malloc(numResource * sizeof(int));
    //for each resource, generate a random number between 0 and what the process needs
    //since we're accessing the need matrix, we need to make sure that this is done exclusively of other process usage
    for(int j = 0 ; j < numResource ; j++){
        if(need[PID*numResource+j]==0){
            req[j] = 0;
        }else{
            //       rand() % (max + 1 - min) + min
            req[j] = rand() % (need[PID*numResource+j] +1);
        }
    }
    sem_post(&mutex);
    int unsuccessfulBanker = 1;
    while(unsuccessfulBanker){
        sem_wait(&mutex);
        int alloc = bankers_algorithm(PID, req);
        if(alloc){
            printf("Request vector [ ");
            for(int w = 0 ; w <numResource ; w++){
                printf("%d ", req[w]);
            }
            printf("] successfully allocated to process %d \n", PID);
            unsuccessfulBanker = 0;
        }else{
            printf("Failed to allocate request vector [ ");
            for(int w = 0 ; w <numResource ; w++){
                printf("%d ", req[w]);
            }
            printf("] to process %d, trying again \n", PID);
            printer();
            sem_post(&mutex);
            //wait a tiny bit before retrying to call banker to let another process free a resource
            usleep(1*1000);
        }
    }
    if(!checkProcessFinish(PID)){
        printf("Process %d has been completely allocated, and will release its resources \n", PID);
        break;
    }
    sem_post(&mutex);
    sleep(3);
    }
    for(int j = 0 ; j < numResource ; j++){
        avail[j] += hold[PID*numResource+j];
        hold[PID*numResource+j]= 0;
    }
    printer();
    sem_post(&mutex);
}

/*
Simulates a fault occuring on the system.

*/
void* fault_simulator(void* pr_id){
    printf("Starting the fault simulating thread\n");
    while(1){
        sleep(10);
        sem_wait(&mutex);
        printf("Running the fault simulating thread \n");
        int j = 0;
        //check if all processes are finished
        for(int i = 0 ; i < numProcess ; i++ ){
            if(!checkProcessFinish(i)){
                j++;
            }
        }
        //j will count how many processes have finished
        //if j == numProcess then all threads have finished
        if(j==numProcess){
            printf("Fault simulating thread exited, all processes finished\n");
            sem_post(&mutex);
            break;
        }
        //get a random index of a resource
        int randomResource = rand()% numResource;
        //generate a random number between 0 and 1
        int RNG = rand()%2;
        //if RNG = 1, remove the resource
        if(RNG){
            if(avail[randomResource]>0){
                avail[randomResource] -= 1;
                printf("Resource %d lost 1 instance \n", randomResource);
            }
        } 
        sem_post(&mutex);
    }
}

/*
Checks for deadlock
*/
void* deadlock_checker(){
     printf("Starting the deadlock checking thread\n");
    while(1){
        sleep(10);
        sem_wait(&mutex);
        printf("Running the deadlock checking thread \n");
        int j = 0;
        //check if all processes are finished
        for(int i = 0 ; i < numProcess ; i++ ){
            if(!checkProcessFinish(i)){
                j++;
            }
        }
        //j will count how many processes have finished
        //if j == numProcess then all threads have finished
        if(j==numProcess){
            printf("Deadlock checking thread exited, all processes finished\n");
            sem_post(&mutex);
            break;
        }

        int lockedProcesses = 0;
        int totalRunningProcess = numProcess;
        for(int i = 0 ; i < numProcess ; i++){
            //if a process has not finished
            if(checkProcessFinish(i)){
                //for all resources
                for(int j = 0 ; j < numResource ; j++){
                    //if a process needs more than what is available
                    //this processes is blocked, increment our lockedProcess counter
                    if(need[i*numResource+j] > avail[j]){
                        printf("Process %d is blocked \n", i);
                        lockedProcesses++;
                        break;
                    }
                }
            }else{
                //if process i is not runnign, the decrement our totalRunningProcess number
                totalRunningProcess--;
            }
        }
        //if the amount of processes that are locked is equal to
        //the total amount of processes we have currently running, then our system is in a deadlock
        if(lockedProcesses==totalRunningProcess){
            printf("Deadlock will occur as processes request more resources, exiting...\n");
            exit(-1);
        }
        sem_post(&mutex);
    }
}



int main()
{

    //Initialize all inputs to banker's algorithm
    //Initialize the number of proccesses and resources
    printf("Number of processes: ");
    scanf("%d" , &numProcess);

    printf("Number of resources: ");
    scanf("%d",&numResource);
    //initialize the avail, max, hold, and need arrays
    avail = malloc(numResource * sizeof(int));
    max = malloc(numProcess * numResource * sizeof(int));
    hold = malloc(numProcess * numResource * sizeof(int));
    need = malloc(numProcess * numResource * sizeof(int));
    //for each resource, initialize the number of instances
    for(int i = 0 ; i < numResource ; i++){
        printf("Number of instances for resource %d: ", i);
        scanf("%d", &avail[i]);
    }
    //for each process, declare how many instances of each resources will be required
    for(int i = 0 ; i < numProcess ; i++){
        for(int k = 0 ; k < numResource ; k++){
            hold[i*numResource+k] = 0;
            printf("Number of instances of resource %d proccess %d will use: ", k , i);
            int input;
            scanf("%d", &input);
            int successfulInput = 1;
            //if the user states that a process requires more resources that we have
            //ask again, until we have a successful input
            while(successfulInput){
                if(input>avail[k]){
                printf("Can't allocate %d instances of resource %d, only %d is available: ", input, k , avail[k]);
                scanf("%d", &input);
                }else{
                successfulInput = 0;
                }
            }
            //populate the max and need arrays
            max[i*numResource+k] = input;
            need[i*numResource+k] = input;
        }
    }
    printer();
    //initialie the mutex
    sem_init(&mutex, 0 , 1);
    
    //initialize fault simulating thread
    pthread_t fault_sim_thread;
    pthread_create(&fault_sim_thread, NULL , fault_simulator , NULL);
    //initialize deadlock checking thread
    pthread_t deadlock_checking_thread;
    pthread_create(&deadlock_checking_thread , NULL , deadlock_checker , NULL);

    //create threads simulating processes (process_simulator)
    int *PIDs[numProcess];
    pthread_t processes [numProcess];
    
    for(int i = 0; i < numProcess; i++){
        PIDs[i] = malloc(sizeof(int));
        *PIDs[i] = i;
        printf("Simulating proccess %d \n", *PIDs[i]);
        pthread_create(&processes[i], NULL , process_simulator , PIDs[i]);
        //need this tiny sleep otherwise i gets incremented before executring the thread
        usleep(15*1000); 
    }

    pthread_exit(NULL);
    return 0;
}