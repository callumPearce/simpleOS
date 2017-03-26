#include "Waiter.h"

extern void main_Philosopher();

/* Implementing the Arbitrator solution for the Dining Philosopher's problem

   -Waiter distributes forks according to some system

      -System chosen TIME, every time peroid waiter takes both forks
       from each Philosopher who was previously eating and distrutrs
       to those who were not eating.
*/

void main_Waiter(){

  int chan[ PHILOSOPHER ]; //1 pipe per Philosopher to tell if eating or thinking
  int philIds[ PHILOSOPHER ]; //pid's for the Philosophers
  //int forks[ PHILOSOPHER ];

  //1)Forks PHILOSOPHER Philosophers programs and Creates 2 pipes per Philosopher
  for(int i = 0; i < PHILOSOPHER; i++){
    philIds[i] = prio_fork(1);
    if( 0 == philIds[i] ) {
      exec( &main_Philosopher );
      break;
    }

    chan[i] = pipe(philIds[i]);
  }


  //2)Initialise what Philosophers can eat
  for(int j = 0; j < PHILOSOPHER; j++){ //loop through the pipes
    if(j%2==0) pipe_write(j,1);
    else pipe_write(j,2);
  }
  yield();


  //3)Waiter manges the forks
  //After every round a Philosopher puts their forks down if they have eaten
  //Waiter reads pipes one by one and if a Philosophers right and left fork are available waiter tells them to eat
  char* string = "  ";
  while(1){
    for(int k = 0; k < PHILOSOPHER; k++){


      int x = pipe_read(k+3);
      char* string;
      itoa(string,x);
      //write( STDOUT_FILENO, string, 2 );

      if (x == 1) pipe_write(k,2);
      if (x == 2) pipe_write(k,1);

    }
    yield();

  }

  exit( EXIT_SUCCESS );
}
