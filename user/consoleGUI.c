#include "consoleGUI.h"

//The current priority the program should be forked with.
int prio_stored = 0;

//Constantly checks if the kernel has sent a program to load, if so get find the correct
//priority of the program to load and update the prio_stored value based on this and return
//the program value to load.
int gets2(){
  bool load_program = false;
  int program = 0;
  while( !load_program ){
    program = check_prog_click();
    if( 0 != program ) {
      load_program = true;
      if(program == 1 || program == 4 || program == 7 || program == 10 || program == 11) prio_stored = 0;
      if(program == 2 || program == 5 || program == 8 ) prio_stored = 1;
      if(program == 3 || program == 6 || program == 9) prio_stored = 2;
    }
  }
  return program; //then load from this
}


//The main functions for the user programs.
extern void main_P3();
extern void main_P4();
extern void main_P5();
extern void main_Waiter();
extern void main_Dining();

/* Since we lack a *real* loader (as a result of lacking a storage
 * medium to store program images), the following approximates one:
 * given a program name, from the set of programs statically linked
 * into the kernel image, it returns a pointer to the entry point.
 */


//Translates the input program code to correct program to load.
void* load_in( int x ) {
  if     ( x == 1 ) {
    return &main_P3;
  }
  else if( x == 2 ) {
    return &main_P3;
  }
  else if( x == 3 ) {
    return &main_P3;
  }
  else if( x == 4 ){
    return &main_P4;
  }
  else if( x == 5 ){
    return &main_P4;
  }
  else if( x == 6 ) {
    return &main_P4;
  }
  else if( x == 7 ) {
    return &main_P5;
  }
  else if( x == 8 ){
    return &main_P5;
  }
  else if( x == 9 ){
    return &main_P5;
  }
  else if( x == 10 ){
    return &main_Waiter;
  }
  else if( x == 11 ){
    return &main_Dining;
  }

  return NULL;
}


//Permanently running consoleGUI process which handles input from GUI and provides correct functionality.
void main_consoleGUI() {
  char* p, x[ 1024 ];
  int prog_to_load;

  while( 1 ) {
    prog_to_load = gets2();

    pid_t pid = prio_fork(prio_stored);
    //puts2( "called progdam\n", 16 );
    if( 0 == pid ) {
      void* addr = load_in( prog_to_load );
      prog_to_load = 0;
      exec( addr );
    }
  }

  exit( EXIT_SUCCESS );
}
