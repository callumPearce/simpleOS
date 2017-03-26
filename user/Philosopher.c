#include "Philosopher.h"

void main_Philosopher(){
  int status = 0;
  while(1){
    status = pipe_read(2);
    char* string = "  ";
    itoa(string,status);
    //write( STDOUT_FILENO, string , 2 );

    if(status == 2) write( STDOUT_FILENO, "THINK", 5 );
    if(status == 1)  write( STDOUT_FILENO, "EAT", 3 );
    //else            write( STDOUT_FILENO, "THINK", 4 );
    yield();
  }

  exit( EXIT_SUCCESS );
}
