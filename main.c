/**
 * Author: Adam IW
 **/

#include <stdlib.h> 
#include  <stdio.h>
#include  <sys/types.h>	
#include <unistd.h>	
#include <stdlib.h>     
#include <sys/wait.h> 	
#include <pthread.h>

#define PRODUCER_NO 5	//number of producer threads
#define NUM_PRODUCED 20 //number of iterations for each producer thread 

void *generator_function(void*);
void *print_function(void*);
long sum; //sum of generated values
long finished_producers; // number of the producer threads that have finished producing 
static pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;//mutex declaration and initialization
pthread_cond_t condVar = PTHREAD_COND_INITIALIZER;//condition variable declaration and initialization


int main(void) {
    int i; //used in subsequent loops
    sum = 0; //total sum initialized to 0
    finished_producers=0; //producers finished initialized to 0
    pthread_t counterThread[5]; //the five producer threads
    pthread_t printThread; //the print thread
    
    pthread_create(&printThread, NULL, print_function, NULL); //creates print thread

    //creates five producer threads
    for(i=0;i<5;i++){
       pthread_create(&counterThread[i], NULL, generator_function, NULL); 
    }

    //makes sure that all producer threads have finished before proceeding
    for(i=0;i<5;i++){
       pthread_join(counterThread[i], NULL); 
    }
    
    pthread_join(printThread, NULL);//makes sure the print thread has finished before proceeding
    
    return (0);
}

void * generator_function(void* junk) {
    pthread_mutex_lock(&mutex1); //lock mutex
    
    long counter = 0; //initialize counter to 0
    long sum_this_generator = 0; //initialize sum of current thread/generator to 0
   
    while (counter < NUM_PRODUCED) {
        printf("Total sum: %ld | Sum of current generator: %ld, adding 1...\n", sum, sum_this_generator);
        sum++;
        counter++;
        sum_this_generator++;
        usleep(1000);
    }
    printf("--+---+----+----------+---------+---+--+---+------+----\n");
    printf("The sum of produced items for this number generator at the end is: %ld \n", sum_this_generator);
    printf("--+---+----+----------+---------+---+--+---+------+----\n");
    finished_producers++; //increase finished producers by 1
    pthread_mutex_unlock(&mutex1); // unlock mutex

    //if all five threads are done producing, a signal is fired on the condition variable 
    if(finished_producers == PRODUCER_NO){
      pthread_cond_signal(&condVar);    
    }
   
    return 0;
}
    
void *print_function(void* junk) {
    printf("Print thread started...\n"); //this will show where in the program output the print thread starts
    pthread_mutex_lock(&mutex1); //locks mutex1 since the wait call will unlock it
    while(finished_producers<PRODUCER_NO){ 
        pthread_cond_wait(&condVar, &mutex1); //print thread is blocked/put to sleep until signaled  
    }  
    pthread_mutex_unlock(&mutex1); //once signal is fired, print thread wakes up and mutex1 gets locked, so this line unlocks it
    printf("Print thread resumed...\n"); //this will show where in the program output the print thread resumes
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    printf("The value of the counter at the end is: %ld \n", sum); //prints final sum
    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
}
