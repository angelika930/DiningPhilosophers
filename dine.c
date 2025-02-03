#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#ifndef NUM_PHILOSOPHERS
#define NUM_PHILOSOPHERS 5
#endif
#define STRINGSIZE 10


//Create global semaphore for printing
sem_t semPrint;
//Initiate struct philosopher data
typedef struct philosopher_info {
   sem_t leftFork;
   sem_t rightFork;
   char status[NUM_PHILOSOPHERS + STRINGSIZE];
   pthread_t *threadPtr;
   int id; //used to specify if phil is even or odd numbered
} Philosopher;

//Purpose: print out status of philosophers
//whatState: 0->eating, 1->thinking, 2->changing
void printStatus(Philosopher *phil, int whatState, int left, int right) {
   //First find what number semaphore the left and right fork is
   int lFork = rFork = 0;
   if (phil->id == 0) {
      lFork = phil->id;
      rFork = NUM_PHILOSOPHERS - 1; //get last indexed semaphore due to circular nature
   }
   else {
      lFork = phil->id;
      rFork = phil->id - 1;
   }
   //Initialize status to all dashses
   memset(phil->status, "-", NUM_PHILOSOPHERS);

   //set left and right forks in printing message 
   if (left == 1) { //philosopher is eating  
      phil->status[phil->id] = '0' + phil->id;
   }
   if (right == 1) {
      //handle right fork due to circular structure
      if (phil->id == 0) {
         phil->status[NUM_PHILOSOPHERS-1] = '0' + (NUM_PHILOSOPHERS - 1);
      }
      else {
         phil->status[phil->id-1] = '0' + (phil->id - 1);
      }
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
//Initiate list of semaphores
sem_t semaphores[NUM_PHILOSOPHERS];
//Initiate list of philosopher structs
Philosopher *listPhil[NUM_PHILOSOPHERS];

void philAction(Philosopher *child) {
   if (child->id % 2 == 0) { //Pick up right fork first
      sem_wait(&child->rightFork);

      sem_wait(&semPrint);
      //printf FIGURE OUT HOW TO CHANGE NUMBERS IN PRINT STATUS
      sem_post(&semPrint);

      sem_wait(&child->leftFork);
      
      sem_wait(&semPrint);
      //printf for picking up left fork
      sem_post(&semPrint);

      dawdle(); //eat for random amount
      sem_post(&child->rightFork); //set down right fork
   
      sem_wait(&semPrint);
      //PRINT AGAIN STATUS HERE of putting down right fork
      sem_post(&semPrint);

      sem_post(&child->leftFork); //set down left fork
      
      sem_wait(&semPrint);
      //PRINT AGAIN STATUS HERE for putting down left fork
      //print new line
      //print -------
      //print think line
      sem_post(&semPrint);

      dawdle(); //dawdle for think

      sem_wait(&semPrint);
      //PRINT AGAIN STATUS HEre for -----
      sem_post(&semPrint);
   }
   else{} //don't forget odd

}

int main(int argc, char *argv[]) {

//initialize sempahore for printing status
sem_init(&semPrint, 0, 1); 
//Initiate semaphore list, each fork is rep by semaphore
for (int j = 0; j < NUM_PHILOSOPHERS; j++) {
   sem_init(&semaphores[j], 0, 1); 
}
//SUS!!! This means each thread would need to finish at the same time
for (int k = 0; k < argv[1]; k++) {
   //Allocate memory for philosopher structs
   for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
      //malloc for new philosopher struct 
      Philosopher newPhilosopher = malloc(sizeof(Philosopher));
      if (newPhilosopher == NULL) {
         perror("Malloc failed to create philosopher struct");
         exit(1);
      }   
      listPhil[i] = newPhilosopher;
      newPhilosopher->id = i; //number the philosophers
      newPhilosopher->status = "-----"; //initialize to changing state
      int newThread = pthread_create(&listPhil[i]->threadPtr,NULL,philAction,(void *)listPhil[i]);   
      if (newThread != 0) {
         perror("Failed to create a new thread");
         exit(1);
      }
      //specifies which semaphores are the left and right forks
      if (i == 0) {//special right fork for first philosophers
         leftFork = semaphores[0];
         rightFork = semaphores[NUM_PHILOSOPHERS-1];          
      }
      else {
         leftFork = semaphores[i];
         rightFork = semaphores[i-1];
      }
   }
}

}
