/*
 ----------------- COMP 310/ECSE 427 Winter 2018 -----------------
 Dimitri Gallos
 Assignment 2 skeleton
 
 -----------------------------------------------------------------
 I declare that the awesomeness below is a genuine piece of work
 and falls under the McGill code of conduct, to the best of my knowledge.
 -----------------------------------------------------------------
 */

//Please enter your name and McGill ID below
//Name: Daniel Bairamian
//McGill ID: 260669560

 

#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <semaphore.h>
#include <string.h>

int BUFFER_SIZE = 100; //size of queue

sem_t sem; //semaphore that locks the queue
sem_t semQueueEmpty; // semaphore that makes the taxis wait if the queue is empty
int ret1; //return value of the initialization of sem (not used really)
int ret2; //return value of the initialization of semQueueEmpty (not used either)

int pshared; //variable that determines if sem is shared within process (irrelevant here since we have 1 process)
int pshared2; //variable that determines if semQueueEmpty is shared within process (irrelevant here since we have 1 process)

int value;  //initial value of sem
int value2; //initial value of semQueueEmpty


// A structure to represent a queue
struct Queue
{
    int front, rear, size;
    unsigned capacity;
    int* array;
};
 
// function to create a queue of given capacity. 
// It initializes size of queue as 0
struct Queue* createQueue(unsigned capacity)
{
    struct Queue* queue = (struct Queue*) malloc(sizeof(struct Queue));
    queue->capacity = capacity;
    queue->front = queue->size = 0;
    queue->rear = capacity - 1;  // This is important, see the enqueue
    queue->array = (int*) malloc(queue->capacity * sizeof(int));
    return queue;
}
 
// Queue is full when size becomes equal to the capacity 
int isFull(struct Queue* queue)
{
    return ((queue->size ) >= queue->capacity);
}
 
// Queue is empty when size is 0
int isEmpty(struct Queue* queue)
{
    return (queue->size == 0);
}
 
// Function to add an item to the queue.  
// It changes rear and size
int enqueue(struct Queue* queue, int item , int planeId)
{
    if (isFull(queue))
        return 0;
    queue->rear = (queue->rear + 1)%queue->capacity;
    queue->array[queue->rear] = item;
    queue->size = queue->size + 1;
    printf("Passenger %d of airplane %d arrives to platform \n", item, planeId);
    return 1;
}

// Function to remove an item from queue. 
// It changes front and size
int dequeue(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    int item = queue->array[queue->front];
    queue->front = (queue->front + 1)%queue->capacity;
    queue->size = queue->size - 1;
    return item;
}

// Function to get front of queue
int front(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->front];
}
 
// Function to get rear of queue
int rear(struct Queue* queue)
{
    if (isEmpty(queue))
        return 0;
    return queue->array[queue->rear];
}

void print(struct Queue* queue){
    if (queue->size == 0){
        return;
    }
    
    for (int i = queue->front; i < queue->front +queue->size; i++){
        
        printf(" Element at position %d is %d \n ", i % (queue->capacity ), queue->array[i % (queue->capacity)]);
    }
    
}

struct Queue* queue;

/*Producer Function: Simulates an Airplane arriving and dumping 5-10 passengers to the taxi platform */
void *FnAirplane(void* cl_id)
{
    //get the id of the plane
    int *planeID = (int *)cl_id;
    int planeId = *planeID;
    while(1){
    //generate a random number between 5 and 10 representing the number of passengers
    int dumpPass = rand()%(10+1-5) + 5;
    printf("Airplane %d arrives with %d passengers \n" , planeId ,dumpPass);
    //for each passenger
    for(int i = 0 ; i < dumpPass ; i++){
        //construct the ID of the passengers
        //passIDBuff represents the part of the ID of the passenger that relates to the passenger number (YYY)
        char passIDBuff [3];
        sprintf(passIDBuff , "%d" ,i);
        //planeIDBuff represents the part of the ID of the passenger that relates to the plane number (ZZZ)
        char planeIDBuff [3];
        sprintf(planeIDBuff, "%d", planeId);
        //passId is the final ID
        //it always starts with 1 and is always of length 7
        char passId  [7];
        strcpy(passId, "1");
        //depending on the size of the planeID
        //we need to concatenante 0's
        //if planeId is bigger than 99, then all 3 digits of the planeIDBuff would be full, we don't need to cat any zeros
        if(planeId > 99){
            strcat(passId, planeIDBuff);
        //if planeID is bigger than 9 but less than 99, then we need to cat one 0
        }else if(planeId > 9){
            strcat(passId, "0");
            strcat(passId, planeIDBuff);
        //if the planeID is smaller than 10, then we need to cat two zeors
        }else{
            strcat(passId, "0");
            strcat(passId, "0");
            strcat(passId, planeIDBuff);
        }
        //the same logic of the planeId is applied for the passIdBuff here
        if(i > 99){
            strcat(passId, passIDBuff);
        }else if(i > 9){
            strcat(passId, "0");
            strcat(passId, passIDBuff);
        }else{
            strcat(passId, "0");
            strcat(passId, "0");
            strcat(passId, passIDBuff);
        }

        int passengerID = atoi(passId);
        //before putting the passenger in the queue, make sure that the queue is not being used by another thread
        sem_wait(&sem);
        int enqueueResult = enqueue(queue , passengerID , planeId);
        //if enqueue returns 0, this means that the queue was full and could not take any more passengers
        if(enqueueResult == 0){
            printf("Platform is full: Rest of passengers of plane %d take the bus \n" ,planeId);
            //release the queue
            sem_post(&sem);
            //the queue was full at a given point, stop trying to put passengers, break from the loop and let the plane leave
            break;
        }else{
            //if enqueue was successful for a passenger, then increment the semQueueEmpty
            sem_post(&semQueueEmpty);
        }
    //release the queue
    sem_post(&sem);
    } 
    //the plane now sleeps for 1 seconds (it will return after 1 "hour")
    sleep(1);
    }
    
}

/* Consumer Function: simulates a taxi that takes n time to take a passenger home and come back to the airport */
void *FnTaxi(void* pr_id)
{
    //get the id of the taxi
    int *taxiID = (int *)pr_id;
    int taxiId = *taxiID;
    while(1){
    //before doing anything, make sure that we have access to the locked variable queue
    printf("Taxi driver %d arrives \n", taxiId);
    int emptyPointer;
    //get the value of the semaphore that checks if the queue is empty
    //if it is 0, then the queue is empty, print that the taxi is waiting
    sem_getvalue(&semQueueEmpty , &emptyPointer);
    if(emptyPointer == 0){
        printf("Taxi driver %d waits for passengers to enter the platform \n" , taxiId);
    }
    sem_wait(&semQueueEmpty);
    sem_wait(&sem);
    //once the queue is non empty and we get access to it
    //take a passenger from the queue
    int passenger = dequeue(queue);
    printf("Taxi driver %d picked up client %d from the platform \n", taxiId , passenger);
    //the taxi took a passenger from the queue, it can release it
    sem_post(&sem);
    //generate a random number between 30 and 10
    //divide by 60 and convert to microseconds for usleep()
    int taxiSleep = rand()%(30+1-10) + 10;
    float taxiSleepTime = taxiSleep/60.0;
    taxiSleepTime*=1000;
    usleep(taxiSleepTime*1000);
    }
}
    


int main(int argc, char *argv[])
{

  int num_airplanes;
  int num_taxis;

  num_airplanes=atoi(argv[1]);
  num_taxis=atoi(argv[2]);
  
  printf("You entered: %d airplanes per hour\n",num_airplanes);
  printf("You entered: %d taxis\n", num_taxis);
  
  
  //initialize queue
  queue = createQueue(BUFFER_SIZE);
  
  //declare arrays of threads and initialize semaphore(s)
  pthread_t airplaneThreads [num_airplanes];
  pthread_t taxiThreads [num_taxis];
  pshared = 0;
  pshared2 = 0;
  value = 1;
  value2 = 0;
  ret1 = sem_init(&sem, pshared, value);
  ret2 = sem_init(&semQueueEmpty, pshared2 , value2);
  

  //create arrays of integer pointers to ids for taxi / airplane threads
  int *taxi_ids[num_taxis];
  int *airplane_ids[num_airplanes];
    
  //create threads for airplanes
  for(int i=0;i<num_airplanes;i++)
  {
    printf("Creating airplane thread %d \n" , i);
    airplane_ids[i] = &i;
    pthread_create(&airplaneThreads[i],NULL,FnAirplane,airplane_ids[i]);
    pthread_detach(airplaneThreads[i]);
    //need this tiny sleep otherwise i gets incremented before executring the thread
    usleep(15*1000); 
  }
  //create threads for taxis
  for(int i=0;i<num_taxis;i++)
  {
    taxi_ids[i] = &i;
    pthread_create(&taxiThreads[i],NULL,FnTaxi,taxi_ids[i]);
    pthread_detach(taxiThreads[i]);
    //same sleep as above
    usleep(15*1000);
  }

  pthread_exit(NULL);
}
