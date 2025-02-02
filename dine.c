#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#ifndef NUM_PHILOSOPHERS
#define NUM_PHILOSOPHERS 5
#endif
#define STRINGSIZE 15


//Create global semaphore for printing
sem_t semPrint;
//Initiate struct philosopher data
typedef struct philosopher_info {
   sem_t leftFork;
   sem_t rightFork;
   char status[STRINGSIZE];
   pthread_t *threadPtr;
   bool isEven = 0; //used to specify if phil is even or odd numbered
} Philosopher;

//print out status of philosophers
void printStatus(Philosopher *listPhil[]) {
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
   if (child->isEven == 0) { //Pick up right fork first
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


}

int main() {

//initialize sempahore for printing status
sem_init(&semPrint, 0, 1); 
//Initiate semaphore list, each fork is rep by semaphore
for (int j = 0; j < NUM_PHILOSOPHERS; j++) {
   sem_init(&semaphores[j], 0, 1); 
}

//Allocate memory for philosopher structs
for (int i = 0; i < NUM_PHILOSOPHERS; i++) {
   //malloc for new philosopher struct 
   Philosopher newPhilosopher = malloc(sizeof(Philosopher));
   if (newPhilosopher == NULL) {
      perror("Malloc failed to create philosopher struct");
      exit(1);
   }   
   listPhil[i] = newPhilosopher;
   newPhilosopher->status = "-----"; //initialize to changing state
   int newThread = pthread_create(&listPhil[i]->threadPtr,NULL,philAction,(void *)listPhil[i]);   
   if (newThread != 0) {
      perror("Failed to create a new thread");
      exit(1);
   }
   //check if phil is even or odd to determine which fork to pick up first
   //avoids deadlocking
   if (i%2 == 0) {
      isEven = 1;
   }
   else {
      isEven = 0;
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
