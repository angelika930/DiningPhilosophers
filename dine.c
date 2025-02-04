#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#ifndef NUM_PHILOSOPHERS
#define NUM_PHILOSOPHERS 5
#endif
#define STRINGSIZE 10

void dawdle(void); //function prototype for dawdle

//Create global semaphore for printing
sem_t semPrint;
//Initiate list of semaphores
sem_t semaphores[NUM_PHILOSOPHERS];
//Make a global variable to hold number of times
//philosophers should eat and think
int runs;
//Initiate struct philosopher data
typedef struct philosopher_info {
   sem_t leftFork;
   sem_t rightFork;
   char status[NUM_PHILOSOPHERS + STRINGSIZE];
   pthread_t threadPtr;
   int id; //used to specify if phil is even or odd numbered
   int numRuns;
} Philosopher;

//Initiate list of philosopher structs
Philosopher *listPhil[NUM_PHILOSOPHERS];

//Purpose: print out status of philosophers
//whatState: 0->eating, 1->thinking
void printStatus(Philosopher *phil, int whatState, int left, int right) {
   //First find what number semaphore the left and right fork is
   int lFork = 0;
   int rFork = 0;
   if (phil->id == 0) {
      lFork = phil->id;
      rFork = NUM_PHILOSOPHERS - 1; //get last indexed semaphore due to circular nature
   }
   else {
      lFork = phil->id;
      rFork = phil->id - 1;
   }
   //Initialize status to all dashses
   memset(phil->status, '-', NUM_PHILOSOPHERS+STRINGSIZE);

   //set left and right forks in printing message 
   if (left == 1) { //philosopher is eating  
      phil->status[lFork] = '0' + lFork;
   }
   if (right == 1) {
      phil->status[rFork] = '0' + rFork;
   }
   //concatenate status with eating or thinking
   if (whatState == 0) {
      strcat(phil->status, "  Eat");
   }
   else if (whatState == 1) {
      strcat(phil->status, "  Think");  
   }

   //print out status of all philosophers
   for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
      printf("%s     ", listPhil[i]->status);
   }
   printf("\n");
}

void* philAction( void *arg) {

   Philosopher *child = (Philosopher *) arg;
   while (child->numRuns < runs) {
      if (child->id % 2 == 0) { //Case 1: Even Philosophers
         sem_wait(&child->rightFork);

         sem_wait(&semPrint);
         //printing for right fork
         printStatus(child, -1, 0, 1);
         sem_post(&semPrint);

         sem_wait(&child->leftFork);
      
         sem_wait(&semPrint);
         //printf for picking up left fork
         printStatus(child, -1, 1, 0);
         printStatus(child, 0, 1, 1); 
         sem_post(&semPrint);

         dawdle(); //eat for random amount
      
         sem_post(&child->rightFork); //set down right fork
   
         sem_wait(&semPrint);
         //putting down right fork
         printStatus(child, -1, 1, 1); 
         printStatus(child, -1, 1, 0);
         sem_post(&semPrint);

         sem_post(&child->leftFork); //set down left fork
      
         sem_wait(&semPrint);
         //print changing line aka "-----"
         printStatus(child, -1, -1, -1);
         sem_post(&semPrint);

         sem_wait(&semPrint);
         //print philosopher thinking
         printStatus(child, 1, -1, -1);
         sem_post(&semPrint);
         
         dawdle(); //dawdle for think

         sem_wait(&semPrint);
         //print changing line aka "-----"
         printStatus(child, -1, -1, -1);
         sem_post(&semPrint);
      }
      //Case 2: odd philosophers (but yes they're all odd.)
      else{          
         sem_wait(&child->leftFork);//pick up left fork first

         sem_wait(&semPrint);
         //printing for left fork
         printStatus(child, -1, 1, 0);
         sem_post(&semPrint);

         sem_wait(&child->rightFork);//then pick up right fork
      
         sem_wait(&semPrint);
         //printf for picking up right fork
         printStatus(child, -1, 1, 1);
         printStatus(child, 0, 1, 1); //display phil is eating
         sem_post(&semPrint);

         dawdle(); //eat for random amount
      
         sem_post(&child->leftFork); //set down left fork first
   
         sem_wait(&semPrint);
         //display stopped eating
         printStatus(child, -1, 1, 1); 
         //display putting down left fork
         printStatus(child, -1, 0, 1);
         sem_post(&semPrint);

         sem_post(&child->rightFork); //set down left fork
      
         sem_wait(&semPrint);
         //print changing line aka "-----"
         printStatus(child, -1, -1, -1);
         sem_post(&semPrint);

         sem_wait(&semPrint);
         //print philosopher thinking
         printStatus(child, 1, -1, -1);
         sem_post(&semPrint);
         
         dawdle(); //dawdle for think

         sem_wait(&semPrint);
         //print changing line aka "-----"
         printStatus(child, -1, -1, -1);
         sem_post(&semPrint);
      }       
      child->numRuns++;
   
   }
   pthread_exit(NULL);
   return (void *)child;

}

int main(int argc, char *argv[]) {

   if (argv[1] == NULL) {
      runs = 1;
   }
   else {
      runs = atoi(argv[1]); //set global variable
   }
   //initialize sempahore for printing status
   sem_init(&semPrint, 0, 1); 
   //Initiate semaphore list, each fork is rep by semaphore
   for (int j = 0; j < NUM_PHILOSOPHERS; j++) {
      sem_init(&semaphores[j], 0, 1); 
   }

   for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
      //malloc for new philosopher struct 
      struct philosopher_info *newPhilosopher = malloc(sizeof(Philosopher));
      if (newPhilosopher == NULL) {
         perror("Malloc failed to create philosopher struct");
         exit(1);
      }   
      listPhil[i] = newPhilosopher;
     
      printStatus(newPhilosopher, -1, -1, -1); 
      newPhilosopher->id = i; //number the philosophers
      newPhilosopher->numRuns = 0;
       
      if (newPhilosopher->id == 0) {
         newPhilosopher->leftFork = semaphores[newPhilosopher->id];
         newPhilosopher->rightFork = semaphores[NUM_PHILOSOPHERS - 1]; //get last indexed semaphore due to circular nature
      }
      else {
         newPhilosopher->leftFork = semaphores[newPhilosopher->id];
         newPhilosopher->rightFork = semaphores[newPhilosopher->id - 1];
      }
      int newThread = pthread_create(&listPhil[i]->threadPtr,NULL,philAction,(void *)listPhil[i]);   
      if (newThread != 0) {
         perror("Failed to create a new thread");
         exit(1);
      }
   
   }

   for (int a = 0; a < NUM_PHILOSOPHERS; a++) {
      void* retVal;
      pthread_join(listPhil[a]->threadPtr, &retVal);
      if (retVal == PTHREAD_CANCELED) {
         perror("Thread canceled unexpectedly");
      }
   }



}
