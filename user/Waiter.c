#include "Waiter.h"

extern void main_Philosopher();

/* Implementing the Arbitrator solution for the Dining Philosopher's problem

   -Waiter distributes forks according to some system

      -System chosen TIME, every time peroid waiter takes both forks
       from each Philosopher who was previously eating and distrutrs
       to those who were not eating.
*/

void main_Waiter(){

  int pipe[ PHILOSOPHER*2 ];
  int j = 0;

  //Forks PHILOSOPHER Philosophers programs and Creates 2 pipes per Philosopher
  for(int i = 0; i < PHILOSOPHER; i++){
    pid_t pid = fork();
    if( 0 == pid ) {
      exec( &main_Philosopher );
      break;
    }
    pipe[j] = pipe(i);
    j++;
    pipe[j] = pipe(i);
    j++;
  }

  //Waiter manges the forks
  bool group1 = TRUE;
  while(1){
    if(group1){
      
    }


  }

  exit( EXIT_SUCCESS );
}
