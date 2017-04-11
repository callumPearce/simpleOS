#ifndef __HILEVEL_H
#define __HILEVEL_H

// Include functionality relating to newlib (the standard C library).

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include <string.h>

// Include functionality relating to the platform.

#include   "GIC.h"
#include "PL011.h"
#include "SP804.h"

// Include functionality relating to the   kernel.

#include "lolevel.h"
#include     "int.h"
#include   "mouse.h"

/* The kernel source code is made simpler via three type definitions:
 *
 * - a type that captures a Process IDentifier (PID), which is really
 *   just an integer,
 * - a type that captures each component of an execution context (i.e.,
 *   processor state) in a compatible order wrt. the low-level handler
 *   preservation and restoration prologue and epilogue, and
 * - a type that captures a process PCB.
 */

//Defines the max number of programs available in the kernel.
#define MAX_PROGS 18
//Defines the max number of pipes available in the kernel.
#define MAX_PIPES 16
//Defines the stack size for each user program.
#define USERSS 0x00005000

//Defines the type for representing process id.
typedef int pid_t;
//Defines the type for representing pipe id.
typedef int pipeId_t;

//Defines the type for representing pipes and programs statuses.
//NOTE: Some are not used however are included to follow conventions.
typedef enum {
  CREATED,
  READY,
  EXECUTING,
  WAITING,
  TERMINATED
} status_t;

//Defines the struct for a pcb.
typedef struct {
  pid_t pid;
  ctx_t ctx;
  status_t status;
  int priority;
} pcb_t;

//Defines the struct for a pipe.
typedef struct {
  pipeId_t pipeId;
  pid_t end1;
  pid_t end2;
  int content;
  status_t status;
} pipe_t;

#endif
