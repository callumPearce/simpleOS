#ifndef __LIBC_H
#define __LIBC_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Define a type that that captures a Process IDentifier (PID).

typedef int pid_t;

/* The definitions below capture symbolic constants within these classes:
 *
 * 1. system call identifiers (i.e., the constant used by a system call
 *    to specify which action the kernel should take),
 * 2. signal identifiers (as used by the kill system call),
 * 3. status codes for exit,
 * 4. standard file descriptors (e.g., for read and write system calls),
 * 5. platform-specific constants, which may need calibration (wrt. the
 *    underlying hardware QEMU is executed on).
 *
 * They don't *precisely* match the standard C library, but are intended
 * to act as a limited model of similar concepts.
 */

#define SYS_YIELD     ( 0x00 )
#define SYS_WRITE     ( 0x01 )
#define SYS_READ      ( 0x02 )
#define SYS_FORK      ( 0x03 )
#define SYS_EXIT      ( 0x04 )
#define SYS_EXEC      ( 0x05 )
#define SYS_KILL      ( 0x06 )
#define SYS_PRIO_FORK ( 0x07 )
#define SYS_PIPE      ( 0x08 )
#define SYS_PIPE_WRITE ( 0x09 )
#define SYS_PIPE_READ ( 0x10 )
#define SYS_PIPE_CLOSE ( 0x11 )
#define SYS_CHECK_CLICK ( 0x12 )

#define SIG_TERM      ( 0x00 )
#define SIG_QUIT      ( 0x01 )

#define EXIT_SUCCESS  ( 0 )
#define EXIT_FAILURE  ( 1 )

#define  STDIN_FILENO ( 0 )
#define STDOUT_FILENO ( 1 )
#define STDERR_FILENO ( 2 )

#define PHILOSOPHER ( 16 )

// convert ASCII string x into integer r
extern int  atoi( char* x        );
// convert integer x into ASCII string r
extern void itoa( char* r, int x );

// cooperatively yield control of processor, i.e., invoke the scheduler
extern void yield();

// write n bytes from x to   the file descriptor fd; return bytes written
extern int write( int fd, const void* x, size_t n );
// read  n bytes into x from the file descriptor fd; return bytes read
extern int  read( int fd,       void* x, size_t n );

// perform fork, returning 0 iff. child or > 0 iff. parent process
extern int  fork();
// perform exit, i.e., terminate process with status x
extern void exit(       int   x );
// perform exec, i.e., start executing program at address x
extern void exec( const void* x );

// signal process identified by pid with signal x
extern int  kill( pid_t pid, int x );

// perform fork with priority set, returning 0 iff. child or > 0 iff. parent process
extern int prio_fork( int x );

// Create a new pipe between the program calling pipe and the inputted pid.
extern int pipe( int pid_end2 );

// Write data to the given pipe.
extern void pipe_write( int pipe_id , int content );

// Read data from the pipe connected to program calling read and the one to the correspondng pid
extern int pipe_read( int pid_end1 );

// Close the pipe corresponding to the given id.
extern void pipe_close( int pipe_id );

//Check if a click on a program has been made and return the program code.
extern int check_prog_click( );

#endif
