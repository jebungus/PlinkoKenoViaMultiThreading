#include <pthread.h>

#include <stdio.h>

#include <stdlib.h>

#include <stdint.h>

#include <math.h>

#include <time.h>

#include <unistd.h>

#include <threads.h>





// global for total threads
int upper = 1;
int lower = 0;
int counter = 0;

int totalBreaks=0;
double avgTotals=0;
double betAmount;
double trialTimeTotal=0;
int kenoSquares;
int difficulty;
double initialBank;
double avgBank=0;
// using lcg64_temper to get random seed for each thread
//  credit: http://stackoverflow.com/a/19083740/2635342

static int32_t temper(int32_t x) {
  x ^= x >> 11;
  x ^= x << 7 & 0x9D2C5680;
  x ^= x << 15 & 0xEFC60000;
  x ^= x >> 18;
  return x;
}

int32_t lcg64_temper(uint64_t *seed) {
  *seed = 6364136223846793005ULL * *seed + 1;
  return temper(*seed >> 32);
}

int threadFunction(void *data) {
  int32_t id = *(int32_t *)data;
 // printf("thread %d up\n", id);

  uint64_t threadSeed = time(NULL) ^ id;

  int32_t n = lcg64_temper(&threadSeed) % 20 + 1;

  
  thrd_sleep(&(struct timespec){.tv_sec = n/5},
             NULL); // Sleep for a random number of seconds.
  // random number between 0 and 1
  double x;
  //win multiplier
  double winMtp;
  //bet Amount is global
  //total win = bet * win multiplier
  double totalWin = 0;
  //each i represents 1 ms of time 
  // for loop represents a 5 minute
  //bank starts at 1000 dollars
  double bank=initialBank;
  int loops=0;
  for (int i = 0; i<1000000; i=i+70){
  loops++;
// can only play if bet is less than bank
    if(bank>=betAmount){
    // random number between 0 and 1
    x = (double)rand() / (double)RAND_MAX;
    bank=bank-betAmount;
    }
    else { 
      totalBreaks++;
    break;
    }
    // anaylse result as a percent and assign value from 0.2x to 5.6?x

    //WIN CHANCES
    
    //0.2x win - 27.3438% chance
    if(x<=0.273438) {
       winMtp = 0.5;
      
    }
    //1x win - 43.75%
    else if (x<=0.710938) {
       winMtp = 1.0;
      
    }
    //1.1x - 21.75%
    else if (x<=0.929688) {
      winMtp = 1.1;
      
    }
    //2.1x - 6.250%
    else if (x<=0.992188) {
      winMtp = 2.1;
      
      
    }
    //5.6x - 0.7812%
    else {
      winMtp = 5.6;
      
    }

  
  totalWin = totalWin + betAmount;
  
    bank = bank + (betAmount * winMtp);
   // printf("$%.4lf : $%.4lf win \n",bank, (betAmount * winMtp));
   // printf("%lf - %.1lf\n",x,winMtp);
//end of for loop
}
//printf("thread %d total = %lf\n", id, totalWin);
  double avgWin = totalWin / loops;
  avgBank+=bank;
  avgTotals+=totalWin;
  trialTimeTotal+=(loops * 70);
 // printf("thread %d done\n",id);
 // printf("average win= %lf GC\n", avgWin);
  return 0;
}








//=========
//Keno
int threadFunctionK(void *data) {
  int32_t id = *(int32_t *)data;


  uint64_t threadSeed = time(NULL) ^ id;

  int32_t n = lcg64_temper(&threadSeed) % 20 + 1;


  thrd_sleep(&(struct timespec){.tv_sec = n/5},
             NULL); // Sleep for a random number of seconds.
  // x assigned to random number between 0 and 1
  double x;
  double bank = initialBank;
  int loops =0;
  double totalWin = 0;
  double winMtp;
  for (int i = 0; i < 300; i = i + 1.5) {
  loops++;
    
    if (bank >= betAmount) {
      x = (double)rand() / (double)RAND_MAX;
       bank = bank - betAmount;
    }
    else {
      totalBreaks++;
      break;
    }
    
   
    
    
  //odds change based on number of keno Squares played
 switch(kenoSquares){
   case 1:

     switch(difficulty){
       
       case 1:
         if(x>0.25){
           winMtp = 0.7;
         }
         else{
           winMtp = 1.85;
         }
       break;

       case 2:
         if(x<=0.25){
           winMtp = 0.4;
         }
         else{
           winMtp = 2.75;
         }
       break;
       
       case 3:
         if(x<=0.25){
           winMtp = 0.0;
         }
         else{
           winMtp = 3.96;
         }
       break;
     }
     
  break;

   
   
  }
 //printf("%.2lf\n",winMtp);
 bank = bank + (betAmount * winMtp);
    totalWin = totalWin + betAmount;

 }
  avgTotals+=totalWin;
  trialTimeTotal+=(loops * 70);
  return 0;
}







int main(void) {

  // clock start time
  clock_t start, end;
  double cpu_time_used;

  start = clock();

  //local definitions
  
  int threadCount;
  char betType[20];
  
  printf("Plinko or Keno: ");
  scanf("%s", betType);

  if(betType[0] == 'K' || betType[0] == 'k'){

    printf("How many numbers to play : ");
    scanf( "%d", &kenoSquares);

    printf("\nDifficulty (1-3) : ");
    scanf( "%d", &difficulty);
    
  }
  printf("\nBank Amount: ");
  scanf("%lf", &initialBank);
  
  printf("\nBet Amount: ");
  scanf("%lf", &betAmount);
  printf("\nEnter number of threads: ");
 scanf("%d", &threadCount);
  printf("\n");




  
  

  // number of threads
  const size_t NUM_THREADS = threadCount;

  int32_t threadData[NUM_THREADS];
  thrd_t threads[NUM_THREADS];

  // Initialize data for each thread.
  for (size_t i = 0; i < NUM_THREADS; ++i) {
    threadData[i] = i;
  }

  // Create the threads.
  for (size_t i = 0; i < NUM_THREADS; ++i) {
    if (betType[0] == 'P' || betType[0] == 'p') {

    if (thrd_create(threads + i, threadFunction, threadData + i) !=
        thrd_success) {
      printf("thread: %zu - thread create error\n", i);
      return 1;
    }
    }
  else {
    if (thrd_create(threads + i, threadFunctionK, threadData + i) != thrd_success) {
      printf("thread: %zu - thread create error\n", i);
      return 1;
    }
  }


    
  }

  // Wait for all threads to complete.
  for (size_t i = 0; i < NUM_THREADS; ++i)
    thrd_join(threads[i], NULL);

// average all threads and give results

  avgBank/=threadCount;
  printf("\n\nAverage spent in %d trials: %.2lf SC\n", threadCount, avgTotals/threadCount);

  printf( "Average bank after trial: %.2lf SC\n\n",avgBank);

  printf("Trials needed to achieve 10000 spent: %.2lf\n", 10000.00/(avgTotals/threadCount));

  printf("SC needed at this bet Amount: %.2lf\n\n", (initialBank-avgBank) * (10000.00/(avgTotals/threadCount)));
  
 printf( "Amount of trials that did not exhaust 5 min timer: %d\n", totalBreaks);
if(totalBreaks>0){

  printf("Average time per trial: %.2lf s\n\n", trialTimeTotal/threadCount/1000);

    }
else{
  
  printf("Average time per trial: 5 mins\n");
}


  
  // clock end time
  end = clock();
  cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
  printf("Time taken to execute in seconds : %f seconds\n", cpu_time_used);
  return 0;
}