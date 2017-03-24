#include "console.h"

/* The following functions are special-case versions of a) writing,
 * and b) reading a string from the UART (the latter case returning
 * once a carriage return character has been read, or an overall
 * limit reached).
 */

void puts( char* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    PL011_putc( UART1, x[ i ], true );
  }
}

void gets( char* x, int n ) {
  for( int i = 0; i < n; i++ ) {
    x[ i ] = PL011_getc( UART1, true );

    if( x[ i ] == '\x0A' ) {
      x[ i ] = '\x00'; break;
    }
  }
}

int find_prio( char* prio ){
  if ( 0 == strcmp( prio , "1" )) return 1;
  if ( 0 == strcmp( prio , "0")) return 0;
  else return -1;
}

/* Since we lack a *real* loader (as a result of lacking a storage
 * medium to store program images), the following approximates one:
 * given a program name, from the set of programs statically linked
 * into the kernel image, it returns a pointer to the entry point.
 */

extern void main_P3();
extern void main_P4();
extern void main_P5();

void* load( char* x ) {
  //puts(x , 2);
  if     ( 0 == strcmp( x, "P3" ) ) {
    return &main_P3;
  }
  else if( 0 == strcmp( x, "P4" ) ) {
    return &main_P4;
  }
  else if( 0 == strcmp( x, "P5" ) ) {
    return &main_P5;
  }

  return NULL;
}

/* The behaviour of the console process can be summarised as an
 * (infinite) loop over three main steps, namely
 *
 * 1. write a command prompt then read a command,
 * 2. split the command into space-separated tokens using strtok,
 * 3. execute whatever steps the command dictates.
 */

 /* Current Implementing Notes: Priority Based scheduling
 * 1. Current call_fork should be left in, add a another attribute to pcb for priority status. *DONE
   2. Write function call_prio_fork( ctx_t ctx , Priority prio) (in hilevel.c) which *DONE
      sets the priority of the program to what you input
   3. Write 2 functions fork_prio in libc.c and console.c which reads in correct values and *DONE
      sends correct values to hilevel.c to deal with in call_prio_fork
   4. Edit schedular so that it executes higher priority pcb's first*/

void main_console() {
  char* p, x[ 1024 ];

  while( 1 ) {
    puts( "shell$ ", 7 ); gets( x, 1024 ); p = strtok( x, " " );

    if     ( 0 == strcmp( p, "fork" ) ) {
      pid_t pid = fork();
      if( 0 == pid ) {
        void* addr = load( strtok( NULL, " " ) );
        exec( addr );
      }
    }

    else if( 0 == strcmp( p, "kill" ) ) {
      pid_t pid = atoi( strtok( NULL, " " ) );
      int   s   = atoi( strtok( NULL, " " ) );

      kill( pid, s );
    }

    else if( 0 == strcmp( p, "prio_fork") ){
      char* prio = strtok( NULL, " " );
      int i_prio = find_prio(prio);

      if (i_prio == -1) puts( "unknown priority\n", 16 );
      else{
        pid_t pid = prio_fork(i_prio);
        if( 0 == pid ) {
          void* addr = load( strtok( NULL, " " ) );
          exec( addr );
        }
      }
    }

    else {
      puts( "unknown command\n", 16 );
    }
  }

  exit( EXIT_SUCCESS );
}
