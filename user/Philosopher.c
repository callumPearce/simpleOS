#include "Philosopher.h"

extern int find_philo_pid();


//A Philosopher who simply obeys the waiters command and waits for 2 forks.
void main_Philosopher(){
  int status = 0;
  while(1){
    status = pipe_read(2);
    char* string = "  ";
    itoa(string,status);

    if(status == 2) write( STDOUT_FILENO, "| THINK ", 8 );
    if(status == 1) write( STDOUT_FILENO, "| EAT   ", 8 );
    yield();
  }

  exit( EXIT_SUCCESS );
}


//A philosopher which talks to the one next to him to find if they can use a fork.
void main_Talkative_Philosopher(){

  int pid = find_philo_pid();
  int pipe_id;
  if(pid == PHILOSOPHER+2){
    pipe_id = pipe(3);
  }
  else  pipe_id = pipe(pid+1);
  int status = 2;
  if (pid == 3 || pid == 9){
    status = 1;
  }

  while(1){
    if(status == 1) {
      write( STDOUT_FILENO, "| THINK ", 8 );

      pipe_write(pipe_id,1);
      status = 2;
      if(pipe_id == PHILOSOPHER-1) write( STDOUT_FILENO, "\n", 2 );
      yield();
    }
    else{
      if(pid == 3) status = pipe_read(PHILOSOPHER+2);
      else status = pipe_read(pid-1);

      if(status == 1){
        write( STDOUT_FILENO, "| EAT   ", 8 );
        pipe_write(pipe_id,2);
      }
      else {
        write( STDOUT_FILENO, "| THINK ", 8 );
        pipe_write(pipe_id,2);
      }
      if(pipe_id == PHILOSOPHER-1) write( STDOUT_FILENO, "\n", 2 );
      yield();
    }
  }
  exit( EXIT_SUCCESS );
}
