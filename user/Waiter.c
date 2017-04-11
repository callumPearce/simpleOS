#include "Waiter.h"

extern void main_Philosopher();
extern void main_Talkative_Philosopher();



//Implementing the Arbitrator solution where we oscillate between two groups of
//8 philosophers eating at time. This produces an optimal concurrent solution to
//the problem using IPC for six philosphers.
void main_Waiter(){

  int chan[ PHILOSOPHER ]; //1 pipe per Philosopher to tell if eating or thinking
  int philIds[ PHILOSOPHER ]; //pid's for the Philosophers

  //1)Forks PHILOSOPHER Philosophers programs and Creates 1 pipe per Philosopher
  for(int i = 0; i < PHILOSOPHER; i++){
    philIds[i] = prio_fork(0);
    if( 0 == philIds[i] ) {
      exec( &main_Philosopher );
      break;
    }

    chan[i] = pipe(philIds[i]);
  }


  //2)Initialise which Philosophers can eat
  for(int j = 0; j < PHILOSOPHER; j++){ //loop through the pipes
    if(j%2==0) pipe_write(j,1);
    else pipe_write(j,2);
  }
  yield();


  //3)Waiter manges the forks
  //After every round a Philosopher puts their forks down if they have eaten
  //Waiter reads pipes one by one and if a Philosophers right and left fork are available waiter tells them to eat
  while(1){
    for(int k = 0; k < PHILOSOPHER; k++){
      int x = pipe_read(k+3);
      if (x == 1) pipe_write(k,2);
      if (x == 2) pipe_write(k,1);

    }
    write( STDOUT_FILENO, "\n", 2 );
    yield();

  }

  exit( EXIT_SUCCESS );
}


//Finds the pid of the current philosopher.
int x_pid = 3;
int find_philosopher_pid(){
  int pid = x_pid;
  x_pid++;
  return pid;
}


//Impletementing single philosopher eating solution which demonstrates IPC's
//power. This is less efficient however demonstrates greater inter process communication.
void main_Dining(){
  int chan[ PHILOSOPHER*2 ]; //1 pipe per Philosopher to tell if eating or thinking
  int philIds[ PHILOSOPHER ]; //pid's for the Philosophers

  //1)Forks PHILOSOPHER Philosophers programs
  for(int i = 0; i < PHILOSOPHER; i++){
    philIds[i] = prio_fork(0);
    if( 0 == philIds[i] ) {
      exec( &main_Talkative_Philosopher );
      break;
    }
  }

  exit( EXIT_SUCCESS );
}
