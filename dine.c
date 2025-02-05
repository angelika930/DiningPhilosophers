#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <string.h>
#ifndef NUM_PHILOSOPHERS
#define NUM_PHILOSOPHERS 5
#endif
#define STRINGSIZE 8

void dawdle(void); //function prototype for dawdle

//Create global semaphore for printing
sem_t *semPrint;
//Initiate list of semaphores
sem_t *semaphores[NUM_PHILOSOPHERS];
//Make a global variable to hold number of times
//philosophers should eat and think
int runs;
//Initiate struct philosopher data
typedef struct philosopher_info {
   sem_t *leftFork;
   sem_t *rightFork;
   char status[NUM_PHILOSOPHERS + STRINGSIZE];
   pthread_t threadPtr;
   int id; //used to specify if phil is even or odd numbered
   int numRuns;
} Philosopher;

//Initiate list of philosopher structs
struct philosopher_info *listPhil[NUM_PHILOSOPHERS];

//Purpose: print out status of philosophers
//whatState: 0->eating, 1->thinking
void printStatus(Philosopher *phil, int whatState, int left, int right) {
   sem_wait(semPrint);

   //First find what number semaphore the left and right fork is
   int lFork = 0;
   int rFork = 0;
   if (phil->id == 0) {
      lFork = phil->id;
      //get last indexed semaphore due to circular nature
      rFork = NUM_PHILOSOPHERS - 1;    
   }
   else {
      lFork = phil->id;
      rFork = phil->id - 1;
   }
   //Initialize status to all dashses
   memset(phil->status, '-', NUM_PHILOSOPHERS);
   snprintf(phil->status+NUM_PHILOSOPHERS, STRINGSIZE, "%s", "       ");

   //set left and right forks in printing message 
   if (left == 1) { //philosopher is eating  
      phil->status[lFork] = '0' + lFork;
   }
   if (right == 1) {
      phil->status[rFork] = '0' + rFork;
   }
   //concatenate status with eating or thinking
   if (whatState == 0) {
      snprintf(phil->status+NUM_PHILOSOPHERS, STRINGSIZE, "%s", "    Eat");
   }
   else if (whatState == 1) {
      snprintf(phil->status+NUM_PHILOSOPHERS, STRINGSIZE, "%s", "  Think");
   }

   //print out status of all philosophers
   int i;
   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      printf("|%s     |", listPhil[i]->status);
   }
   printf("\n");
   sem_post(semPrint);

}

void* philAction( void *arg) {

   Philosopher *child = (Philosopher *) arg;
   while (child->numRuns < runs) {
      if (child->id % 2 == 0) { //Case 1: Even Philosophers
         sem_wait(child->rightFork);

         //printing for right fork
         printStatus(child, -1, 0, 1);

         sem_wait(child->leftFork);
 
         //print for picking up left fork
         printStatus(child, -1, 1, 1);
         //print both forks picked up and "Eat"
         printStatus(child, 0, 1, 1); 

         dawdle(); //eat for random amount
      
         sem_post(child->rightFork); //set down right fork
   
         //putting down right fork
         printStatus(child, -1, 1, 1); 
         printStatus(child, -1, 1, 0);


         printStatus(child, -1, -1, -1);



         sem_post(child->leftFork); //set down left fork
      
         //print changing line aka "-----"
        // printStatus(child, -1, -1, -1);

         //print philosopher thinking
         printStatus(child, 1, -1, -1);
         
         dawdle(); //dawdle for think

         //print changing line aka "-----"
         printStatus(child, -1, -1, -1);
      }
      //Case 2: odd philosophers (but yes they're all odd.)
      else{          
         sem_wait(child->leftFork);//pick up left fork first

         //printing for left fork
         printStatus(child, -1, 1, 0);
         
         sem_wait(child->rightFork);//then pick up right fork
         //printf for picking up right fork
         printStatus(child, -1, 1, 1);
         printStatus(child, 0, 1, 1); //display phil is eating

         dawdle(); //eat for random amount
      
         sem_post(child->leftFork); //set down left fork first
   
         //display stopped eating
         printStatus(child, -1, 1, 1); 
         //display putting down left fork
         printStatus(child, -1, 0, 1);
  
       
         printStatus(child, -1, -1, -1);



         sem_post(child->rightFork); //set down left fork
      
         //print changing line aka "-----"
         //printStatus(child, -1, -1, -1);

         //print philosopher thinking
         printStatus(child, 1, -1, -1);
         
         dawdle(); //dawdle for think

         //print changing line aka "-----"
         printStatus(child, -1, -1, -1);
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

   //case where there's no philosophers
   if (NUM_PHILOSOPHERS == 0) {
      return 0;
   }
   
   //initialize sempahore for printing status
   semPrint = malloc(sizeof(sem_t));
   if (semPrint == NULL) {
      perror("malloc failed for semPrint");
      exit(1);
   }
   if (sem_init(semPrint, 0, 1) == -1) {
      perror("Failed to create print semaphore");
      free(semPrint);
      exit(1);
   } 
   //Initiate semaphore list, each fork is rep by semaphore
   int j;
   for (j = 0; j < NUM_PHILOSOPHERS; j++) {
            
      semaphores[j] = malloc(sizeof(sem_t));
      if (semaphores[j] == NULL) {
         perror("malloc failed for semaphore");
         exit(1);
      }
      if (sem_init(semaphores[j], 0, 1) == -1) {
         perror("Failed to create semaphore for fork.");
         exit(1);
      } 
   }
   int i;
   for (i = 0; i < NUM_PHILOSOPHERS; i++) {
      //malloc for new philosopher struct 
struct philosopher_info *newPhil = malloc(sizeof(struct philosopher_info));
      if (newPhil == NULL) {
         perror("Malloc failed to create philosopher struct");
         exit(1);
      }   
      listPhil[i] = newPhil;
     
      //Initialize status to all dashses
      memset(newPhil->status, '-', NUM_PHILOSOPHERS);
      strcat(newPhil->status, "       ");
      
      newPhil->id = i; //number the philosophers
      newPhil->numRuns = 0;

       
      if (newPhil->id == 0) {
         newPhil->leftFork = semaphores[newPhil->id];
         //get last indexed semaphore due to circular nature
         newPhil->rightFork = semaphores[NUM_PHILOSOPHERS - 1];       }
      else {
         newPhil->leftFork = semaphores[newPhil->id];
         newPhil->rightFork = semaphores[newPhil->id - 1];
      }
   
   }
   //Launch pthreads
   int b, newThread;
   for (b = 0; b < NUM_PHILOSOPHERS; b++) {
pthread_create(&listPhil[b]->threadPtr,NULL,philAction,(void *)listPhil[b]);   
/*      if (newThread != 0) {
         perror("Failed to create a new thread");
         exit(1);
      }*/
   }
   int a;
   for (a = 0; a < NUM_PHILOSOPHERS; a++) {
      void* retVal;
      pthread_join(listPhil[a]->threadPtr, &retVal);
      if (retVal == PTHREAD_CANCELED) {
         perror("Thread canceled unexpectedly");
      }
      free(listPhil[a]);
   }
   free(semPrint);
   return 0;


}
