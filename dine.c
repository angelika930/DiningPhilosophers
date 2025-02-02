#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#ifndef NUM_PHILOSOPHERS
#define NUM_PHILOSOPHERS 5
#endif
#define STRINGSIZE 15

//Initiate struct philosopher data
typedef struct philosopher_info {
   sem_t leftFork;
   sem_t rightFork;
   char status[STRINGSIZE];
   pthread_t *threadPtr;
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
void philAction(Philosopher *child) {}










int main() {



//Initiate semaphore list, each fork is rep by semaphore
for (int j = 0; j < NUM_PHILOSOPHERS; j++) {
   sem_init(&semaphores[j], 0, 1); 
}

//Allocate memory for philosopher structs
for (int i = 0; i < NUM_PHILOSOPHERS; i++) {

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
