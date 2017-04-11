#include "libc.h"

extern int find_philosopher_pid();

int  atoi( char* x        ) {
  char* p = x; bool s = false; int r = 0;

  if     ( *p == '-' ) {
    s =  true; p++;
  }
  else if( *p == '+' ) {
    s = false; p++;
  }

  for( int i = 0; *p != '\x00'; i++, p++ ) {
    r = s ? ( r * 10 ) - ( *p - '0' ) :
            ( r * 10 ) + ( *p - '0' ) ;
  }

  return r;
}

void itoa( char* r, int x ) {
  char* p = r; int t, n;

  if( x < 0 ) {
    p++; t = -x; n = 1;
  }
  else {
         t = +x; n = 1;
  }

  while( t >= n ) {
    p++; n *= 10;
  }

  *p-- = '\x00';

  do {
    *p-- = '0' + ( t % 10 ); t /= 10;
  } while( t );

  if( x < 0 ) {
    *p-- = '-';
  }

  return;
}

void yield() {
  asm volatile( "svc %0     \n" // make system call SYS_YIELD
              :
              : "I" (SYS_YIELD)
              : );

  return;
}

int write( int fd, const void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_WRITE
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_WRITE), "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int prio_fork( int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r1 = x
                "svc %1     \n" // make system call SYS_PRIO_FORK
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_PRIO_FORK), "r" (x)
              : "r0", "r1" );

  return r;
}

int  read( int fd,       void* x, size_t n ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 = fd
                "mov r1, %3 \n" // assign r1 =  x
                "mov r2, %4 \n" // assign r2 =  n
                "svc %1     \n" // make system call SYS_READ
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_READ),  "r" (fd), "r" (x), "r" (n)
              : "r0", "r1", "r2" );

  return r;
}

int fork() {
  int r;

  asm volatile( "svc %1     \n" // make system call SYS_FORK
                "mov %0, r0 \n" // assign r  = r0
              : "=r" (r)
              : "I" (SYS_FORK)
              : "r0" );

  return r;
}


void exit( int x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 =  x
                "svc %0     \n" // make system call SYS_EXIT
              :
              : "I" (SYS_EXIT), "r" (x)
              : "r0" );

  return;
}

void exec( const void* x ) {
  asm volatile( "mov r0, %1 \n" // assign r0 = x
                "svc %0     \n" // make system call SYS_EXEC
              :
              : "I" (SYS_EXEC), "r" (x)
              : "r0" );

  return;
}

int kill( int pid, int x ) {
  int r;

  asm volatile( "mov r0, %2 \n" // assign r0 =  pid
                "mov r1, %3 \n" // assign r1 =    x
                "svc %1     \n" // make system call SYS_KILL
                "mov %0, r0 \n" // assign r0 =    r
              : "=r" (r)
              : "I" (SYS_KILL), "r" (pid), "r" (x)
              : "r0", "r1" );

  return r;
}

 int pipe( int pid_end2 ){
   int r;
   //write( STDOUT_FILENO, "__pipe__", 8 );
   asm volatile( "mov r0, %2 \n" // assign r0 =  pid_end2
                 "svc %1     \n" // make system call SYS_PIPE
                 "mov %0, r0 \n" // assign r0 =    r
               : "=r" (r)
               : "I" (SYS_PIPE), "r" (pid_end2)
               : "r0" );

   return r;
 }


 void pipe_write( int pipe_id , int content ){

   asm volatile( "mov r0, %1 \n" // assign r0 =  pipe_id
                 "mov r1, %2 \n" // assign r1 = content
                 "svc %0     \n" // make system call SYS_PIPE_WRITE
               :
               : "I" (SYS_PIPE_WRITE), "r" (pipe_id), "r" (content)
               : "r0", "r1" );

  return;
 }


int pipe_read( int pid_end1 ){
   int r;

   asm volatile( "mov r0, %2 \n" // assign r0 =  pid_end1
                 "svc %1     \n" // make system call SYS_PIPE_READ
                 "mov %0, r0 \n" // assign r = r0
               : "=r" (r)
               : "I" (SYS_PIPE_READ), "r" (pid_end1)
               : "r0" );


    return r;
 }


 void pipe_close( int pipe_id ){

   asm volatile( "mov r0, %1 \n" // assign r0 =  pid_id
                 "svc %0     \n" // make system call SYS_PIPE_CLOSE
               :
               : "I" (SYS_PIPE_CLOSE), "r" (pipe_id)
               : "r0" );

   return;
 }


 int find_philo_pid(){
   int x = find_philosopher_pid();
   return x;
 }


 int check_prog_click(){
   int r;

   asm volatile( "svc %1     \n" // make system call SYS_READ
                 "mov %0, r3 \n" // assign r  = r3
               : "=r" (r)
               : "I" (SYS_CHECK_CLICK)
               : "r3");

   return r;
 }
