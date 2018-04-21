#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function 
// an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//function to swap 2 integers
void swap(int *a, int *b)
{
    if (*a != *b)
    {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
        return;
    }
}

void sort(int* array , int arraySize){
    for(int i = 0 ; i < arraySize-1 ; i++){
        int min = i;
        for(int j = i+1 ; j < arraySize ; j++){
            if(array[j]<array[min]){
                min = j;
            }
        }
        if(min != i){
            swap(&array[i],&array[min]);
        }
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest)
{
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest)
{
    //simplest part of assignment
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
void accessSSTF(int *request, int numRequest)
{
    //initiate a nextTest variable which holds the next variable to test against
    int nextTest = START;
    int toSwap = -1;
    //for all requests in our array, do a nested forloop
    for(int i = 0 ; i < numRequest ; i ++){
        //the maximum is our current maximum, for each i
        int currentMax = abs(cmpfunc(&nextTest,&request[i]));
        //for each element, test with all following elements
        for(int j = i ; j < numRequest ; j++){
            //if we find an element closer to our current test, then it is our new maximum, and we save the index
            int testMax = abs(cmpfunc(&nextTest,&request[j]));
            if( testMax < currentMax  ){
                toSwap = j;
                currentMax = testMax;
            }

        }
        //if the index is not negative 1 , then this means that we found something to swap
        if(toSwap != -1){
        swap(&request[i] , &request[toSwap]);
        }
        //the next variable to test is the next in the array
        //reset the variable that stores the index to -1
        nextTest = request[i];
        toSwap = -1;
    }
    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest)
{
    int leftStart ;
    int rightStart;
    int goingLeft;
    int newCnt = numRequest+1;
    //sort the request array
    sort(request , numRequest);

    if(START-LOW > HIGH-START){
        goingLeft = 0;
    }else {
        goingLeft = 1;
    }
    
    //find the index before and after the start value
    for(int i = 0 ; i < numRequest ; i++){
        if(request[i]>START){
            rightStart =  i;
            if(rightStart > 0){
                leftStart = i-1;
            }else{
                //if the first value we find is the first value of the array, then we are only going to go right
                leftStart = i;
                goingLeft = 0;
                newCnt = numRequest;
            }
            break;
        }
        //if we get to this if statement, this means we're at the last element of the array and none of the values are bigger than the start
        //then we are only going to go left
        if(i == numRequest-1){
            leftStart=i;
            rightStart=i;
            goingLeft = 1;
            newCnt = numRequest;
        }
    }
    //initialize the new request array, which will have either the same size or +1
    int newReqCounter = 0;
    int *newRequest = malloc(newCnt * sizeof(int));

    //going right
    if(!goingLeft){
        //start at the rightStart value, keep going until the rest of the array
        for(int i = rightStart ; i < numRequest ; i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going right and hit the end of the array, if newReqCounter != numRequest this means we have
        //values before the start that we still need to address, so we then go to the left start value and go left
        if(newReqCounter!=numRequest){
            newRequest[newReqCounter] = HIGH;
            newReqCounter++;

            for(int i = leftStart ; i >= 0 ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    //going left
    }else{
        //start at the leftStart value, keep going until the beginning of the array
        for(int i = leftStart ; i >= 0 ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going left and hit the beginning of the array, if newReqCounter != numRequest this means we have
        //values after the start that we still need to address, so we then go to the right start value and go right
        if(newReqCounter!=numRequest){
            newRequest[newReqCounter] = LOW;
            newReqCounter++;

        for(int i = rightStart ; i < numRequest ; i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    }
    

    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int *request, int numRequest)
{

    int leftStart ;
    int rightStart;
    int goingLeft = 0;
    int newCnt = numRequest+2;
    //sort the request array
    sort(request , numRequest);

    if(START-LOW > HIGH-START){
        goingLeft = 0;
    }else {
        goingLeft = 1;
    }

    //find the index before and after the start value
    for(int i = 0 ; i < numRequest ; i++){
        if(request[i]>START){
            rightStart =  i;
            if(rightStart > 0){
                leftStart = i-1;
            }else{
                //if the first value we find is the first value of the array, then we are only going to go right
                leftStart = i;
                goingLeft = 0;
                newCnt = numRequest;
            }
            break;
        }
        //if we get to this if statement, this means we're at the last element of the array and none of the values are bigger than the start
        //then we are only going to go left
        if(i == numRequest-1){
            leftStart=i;
            rightStart=i;
            goingLeft = 1;
            newCnt = numRequest;
        }
    }
    //initialize the new request array, which will have either the same size or +1
    int newReqCounter = 0;
    int *newRequest = malloc(newCnt * sizeof(int));

    //going right
    if(!goingLeft){
        //start at the rightStart value, keep going until the rest of the array
        for(int i = rightStart ; i < numRequest ; i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going right and hit the end of the array, if newReqCounter != numRequest this means we have
        //values before the start that we still need to address, so we then go to the left start value and go left
        if(newReqCounter!=numRequest){
            newRequest[newReqCounter] = HIGH;
            newReqCounter++;
            newRequest[newReqCounter] = LOW;
            newReqCounter++;

            for(int i = 0 ; i < rightStart ;i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    //going left
    }else{
        //start at the leftStart value, keep going until the beginning of the array
        for(int i = leftStart ; i >= 0 ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going left and hit the beginning of the array, if newReqCounter != numRequest this means we have
        //values after the start that we still need to address, so we then go to the right start value and go right
        if(newReqCounter!=numRequest){
            newRequest[newReqCounter] = LOW;
            newReqCounter++;
            newRequest[newReqCounter] = HIGH;
            newReqCounter++;

        for(int i = numRequest-1 ; i >= rightStart ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    }
    

    printf("\n----------------\n");
    printf("CSCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in LOOK
void accessLOOK(int *request, int numRequest)
{

	int leftStart ;
    int rightStart;
    int goingLeft;

    //sort the request array
    sort(request , numRequest);

    if(START-LOW > HIGH-START){
        goingLeft = 0;
    }else {
        goingLeft = 1;
    }

    //find the index before and after the start value
    for(int i = 0 ; i < numRequest ; i++){
        if(request[i]>START){
            rightStart =  i;
            if(rightStart > 0){
                leftStart = i-1;
            }else{
                //if the first value we find is the first value of the array, then we are only going to go right
                leftStart = i;
                goingLeft = 0;
            }
            break;
        }
        //if we get to this if statement, this means we're at the last element of the array and none of the values are bigger than the start
        //then we are only going to go left
        if(i == numRequest-1){
            leftStart=i;
            rightStart=i;
            goingLeft = 1;
        }
    }
    //initialize the new request array, which will have either the same size or +1
    int newCnt = numRequest;
    int newReqCounter = 0;
    int *newRequest = malloc(newCnt * sizeof(int));

    //going right
    if(!goingLeft){
        //start at the rightStart value, keep going until the rest of the array
        for(int i = rightStart ; i < numRequest ; i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going right and hit the end of the array, if newReqCounter != numRequest this means we have
        //values before the start that we still need to address, so we then go to the left start value and go left
        if(newReqCounter!=numRequest){
            for(int i = leftStart ; i >= 0 ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    //going left
    }else{
        //start at the leftStart value, keep going until the beginning of the array
        for(int i = leftStart ; i >= 0 ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going left and hit the beginning of the array, if newReqCounter != numRequest this means we have
        //values after the start that we still need to address, so we then go to the right start value and go right
        if(newReqCounter!=numRequest){
        for(int i = rightStart ; i < numRequest ; i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    }
    
    printf("\n----------------\n");
    printf("LOOK :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int *request, int numRequest)
{

    int leftStart ;
    int rightStart;
    int goingLeft = 1;
    int newCnt = numRequest+1;
    //sort the request array
    sort(request , numRequest);

    if(START-LOW > HIGH-START){
        goingLeft = 0;
    }else {
        goingLeft = 1;
    }
    
    //find the index before and after the start value
    for(int i = 0 ; i < numRequest ; i++){
        if(request[i]>START){
            rightStart =  i;
            if(rightStart > 0){
                leftStart = i-1;
            }else{
                //if the first value we find is the first value of the array, then we are only going to go right
                leftStart = i;
                goingLeft = 0;
                newCnt = numRequest;
            }
            break;
        }
        //if we get to this if statement, this means we're at the last element of the array and none of the values are bigger than the start
        //then we are only going to go left
        if(i == numRequest-1){
            leftStart=i;
            rightStart=i;
            goingLeft = 1;
            newCnt = numRequest;
        }
    }
    //initialize the new request array, which will have either the same size or +1
    int newReqCounter = 0;
    int *newRequest = malloc(newCnt * sizeof(int));

    //going right
    if(!goingLeft){
        //start at the rightStart value, keep going until the rest of the array
        for(int i = rightStart ; i < numRequest ; i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going right and hit the end of the array, if newReqCounter != numRequest this means we have
        //values before the start that we still need to address, so we then go to the left start value and go left
        if(newReqCounter!=numRequest){
            newRequest[newReqCounter] = LOW;
            newReqCounter++;

            for(int i = 0 ; i < rightStart ;i++){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    //going left
    }else{
        //start at the leftStart value, keep going until the beginning of the array
        for(int i = leftStart ; i >= 0 ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
        }
        //we were going left and hit the beginning of the array, if newReqCounter != numRequest this means we have
        //values after the start that we still need to address, so we then go to the right start value and go right
        if(newReqCounter!=numRequest){
            newRequest[newReqCounter] = HIGH;
            newReqCounter++;

        for(int i = numRequest-1 ; i >= rightStart ; i--){
            newRequest[newReqCounter] = request[i];
            newReqCounter++;
            }
        }
    }
    

    printf("\n----------------\n");
    printf("CLOOK :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

int main()
{
    int *request, numRequest, i,ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d",&ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1: accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2: accessSSTF(request, numRequest);
        break;

        //access the disk location in SCAN
     case 3: accessSCAN(request, numRequest);
        break;

        //access the disk location in CSCAN
    case 4: accessCSCAN(request,numRequest);
        break;

    //access the disk location in LOOK
    case 5: accessLOOK(request,numRequest);
        break;

    //access the disk location in CLOOK
    case 6: accessCLOOK(request,numRequest);
        break;

    default:
        break;
    }
    return 0;
}