#include "hilevel.h"

/* Since we *know* there will be 2 processes, stemming from the 2 user
 * programs, we can
 *
 * - allocate a fixed-size process table (of PCBs), and use a pointer
 *   to keep track of which entry is currently executing, and
 * - employ a fixed-case of round-robin scheduling: no more processes
 *   can be created, and neither is able to complete.
 */
extern void     main_P3();
extern uint32_t tos_P3;
extern void     main_P4();
extern uint32_t tos_P4;
extern void     main_P5();
extern uint32_t tos_P5;
extern void     main_console();
extern uint32_t tos_console;

pcb_t pcb[ MAX_PROGS ], *current = NULL;

pipe_t pipes[ MAX_PIPES ];

pid_t max_pid = 1;

pipeId_t max_pipe_id = 0;

//Print function
void printIt(char* x, int n){
  for( int i = 0; i < n; i++ ) {
    PL011_putc( UART0, *x++, true );
  }
}

void convertInt( char* r, int x ) {
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

//Find the next pcb index to run based off of priority
int find_next_index_prio(int ind){
  int highest_prio = -1;
  int next_ind = ind;
  for(int i = 1; i <= MAX_PROGS; i++){
    if(pcb[(ind+i)%MAX_PROGS].status != TERMINATED && pcb[(ind+i)%MAX_PROGS].priority > highest_prio && (ind+i)%MAX_PROGS != ind){
        highest_prio = pcb[(ind+i)%MAX_PROGS].priority;
        next_ind = (ind+i)%MAX_PROGS;
   }
  }
  return next_ind;
}

//Finds the current pcb index
int find_pcb_index(int pid){
  for(int i = 0; i < MAX_PROGS; i++){
    if(pid == pcb[i].pid) return i;
  }
  return -1; //Return -1 if there is an error
}

//Find the next pcb index to run
int find_next_pcb_index(int ind){
  for(int i = 1; i <= MAX_PROGS; i++){
    if(pcb[(ind+i)%MAX_PROGS].status != TERMINATED) return (ind+i)%MAX_PROGS;
  }
  return -1;
}


//Schedular: Currently Round Robin implementation
void scheduler( ctx_t* ctx) {
  int current_pcb_index = find_pcb_index(current->pid);
  int next_pcb_index = find_next_index_prio(current_pcb_index);
  if(next_pcb_index != -1){
    memcpy( &pcb[current_pcb_index].ctx , ctx , sizeof(ctx_t));  //Save current program into it's location in the pcb table
    memcpy( ctx , &pcb[next_pcb_index].ctx , sizeof(ctx_t) );  //Copy the new program into in to the current context
    current = &pcb[next_pcb_index];                            //Update the current pcb pointer
  }
  return;
}

//Finds the next available pcb
int find_free_pcb(){
  for(int i = 0; i < MAX_PROGS; i++){
    if(pcb[i].status == TERMINATED) return i;
  }
  return -1; //Return -1 if there are no available pcb's
}

//Call fork but assign a specific priority to the pcb
void call_prio_fork( ctx_t* ctx , int i_prio ){
  //1)Get the new ID by incrementing the largest PCB ID by 1.
  int newID = find_free_pcb();

  //2)Initialise the new process
  memset( &pcb[ newID ], 0, sizeof( pcb_t ) );
  //memcpy( &pcb[ newID ], current , sizeof(pcb_t));
  memcpy( &pcb[ newID ].ctx , ctx , sizeof( ctx_t ));

  //3)Update the process ID
  pcb[ newID ].pid = max_pid;
  pcb[ newID ].status = READY;
  //pcb[ 1 ].status = READY;
  max_pid++;

  pcb[ newID ].priority = i_prio;

  int current_stack_entry = (void*) &tos_console + USERSS * find_pcb_index(current->pid);
  int new_stack_entry     = (void*) &tos_console + USERSS * newID;
  int stack_size = current_stack_entry - ctx->sp;

  //4)Update stack pointer value
  pcb[ newID ].ctx.sp = new_stack_entry - stack_size;

  //5)Copy the current programs stack to the new programs stack
  memcpy( (void *) new_stack_entry - USERSS , (void *) current_stack_entry - USERSS , USERSS);

  //6)Return zero for the new user process
  pcb[ newID ].ctx.gpr[0] = 0;
  //pcb[ newID ].ctx.gpr[1] = newID;
  ctx->gpr[0] = pcb[ newID ].pid;

  //7)Return the newID for the pcb created
  return;
}

//Fork the current user program
void call_fork( ctx_t* ctx ) {
  call_prio_fork( ctx , 0 );
  return;
}

//Exit the current user process accordingly
void call_exit( ctx_t* ctx ){
  int current_pcb_index = find_pcb_index(current->pid);

  //Reset Stack
  int current_stack_entry = (void*) &tos_console + USERSS * find_pcb_index(current->pid);
  memset((void *) current_stack_entry - USERSS, 0 , USERSS);

  //Reset relating gpr's
  for(int i = 0; i < 13; i++) ctx->gpr[i] = 0;

  //reset pid,lr,pc and cpsr
  ctx->sp = current_stack_entry;
  ctx->pc = ctx->gpr[ 0 ];
  ctx->cpsr = 0x50;
  ctx->lr = 0;

  //Set to status to TERMINATED
  pcb[current_pcb_index].status = TERMINATED;

  return;
}


//1)Reset the stack and stack pointer
//2)Set the sp to the top of the stack
//3)Set pc to the passed in gpr value
//4)Reset gpr's to a value of 0
void call_exec( ctx_t* ctx ){
  int current_stack_entry = (void*) &tos_console + USERSS * find_pcb_index(current->pid);
  memset((void *) current_stack_entry - USERSS, 0 , USERSS);
  ctx->sp = current_stack_entry;
  ctx->pc = ctx->gpr[ 0 ];
  ctx->cpsr = 0x50;
  ctx->lr   = 0;
  for(int i = 0; i < 13; i++) ctx->gpr[i] = 0;
  return;
}


int find_free_pipe(){
  for(int i = 0; i < MAX_PIPES; i++){
    if(pipes[i].status == TERMINATED) return i;
  }
  return -1;
}


int find_ind_pipe( pipeId_t pipe_id ){
  for(int i = 0; i < MAX_PIPES; i++){
    if(pipe_id == pipes[i].pipeId && pipes[i].status != TERMINATED) return i;
  }
  return -1; //Return -1 if there is an error
}

//1)Allocate pipes
//2)Return descriptors
void call_pipe( ctx_t* ctx ){
  //printIt("call_pipe",9);

  //1) Find a free pipe index
  int pipeInd = find_free_pipe();

  //2) Initialise the new pipe as 0
  memset( &pipes[ pipeInd ], 0, sizeof( pipe_t ) );

  //3) Fill in the new pipe values
  pipes[ pipeInd ].pipeId = max_pipe_id;
  max_pipe_id++;
  pipes[ pipeInd ].end1   = current->pid;
  pipes[ pipeInd ].end2   = ctx->gpr[0]; //Recieve (Assume user program knows other programs pid.)
  pipes[ pipeInd ].status = READY;

  //4) Return the pipe_id
  ctx->gpr[0] = pipes[ pipeInd ].pipeId;
  return;
}


//1) Write data to pipe
void call_pipe_write( ctx_t* ctx  ){
  //printIt("call_pipe_write",15);
  pipeId_t pipe_id = ctx->gpr[0];
  int pipe_ind = find_ind_pipe( pipe_id );
  pipes[ pipe_ind ].content = ctx->gpr[2]; //Read data written to the pipe through gpr[1]

  char* string = "   ";
  convertInt(string,pipes[ pipe_ind ].content);
  //printIt(string, 3);

  return;
}


//1) Read data from pipe
void call_pipe_read( ctx_t* ctx ){  //Assuming current program reading is the ctx, the pid is the on of the sender

  pid_t pid = ctx->gpr[0];
  for(int i = 0; i < MAX_PIPES; i++){

    if( ((current->pid == pipes[i].end1) && (pid == pipes[i].end2)) ) {
      if(pipes[i].status != TERMINATED){
        int j = pipes[i].content;
        ctx->gpr[0] = pipes[i].content;
        return;
      }
    }

    if( ((current->pid == pipes[i].end2) && (pid == pipes[i].end1)) ) {
      if(pipes[i].status != TERMINATED){
        int j = pipes[i].content;
        ctx->gpr[0] = pipes[i].content;
        return;
      }
    }

    else ctx->gpr[0] = -1;
  }
  return;
}


//1) Close pipe
void call_pipe_close( ctx_t* ctx ){
  //printIt("call_pipe_close",15);
  pipeId_t pipe_id = ctx->gpr[0];
  int pipeInd = find_ind_pipe(pipe_id);
  memset( &pipes[ pipeInd ], 0, sizeof( pipe_t ) ); //Reset data stored.
  pipes[ pipeInd ].status = TERMINATED; //Set pipe to terminated.
  return;
}


void hilevel_handler_rst( ctx_t* ctx              ) {
  //printIt("Called",6);
  /* Initialise PCBs representing processes stemming from execution of
   * the two user programs.  Note in each case that
   *
   * - the CPSR value of 0x50 means the processor is switched into USR
   *   mode, with IRQ interrupts enabled, and
   * - the PC and SP values matche the entry point and top of stack.
   */

   TIMER0->Timer1Load  = 0x00100000; // select period = 2^20 ticks ~= 1 sec
   TIMER0->Timer1Ctrl  = 0x00000002; // select 32-bit   timer
   TIMER0->Timer1Ctrl |= 0x00000040; // select periodic timer
   TIMER0->Timer1Ctrl |= 0x00000020; // enable          timer interrupt
   TIMER0->Timer1Ctrl |= 0x00000080; // enable          timer

   GICC0->PMR          = 0x000000F0; // unmask all            interrupts
   GICD0->ISENABLER1  |= 0x00000010; // enable timer          interrupt
   GICC0->CTLR         = 0x00000001; // enable GIC interface
   GICD0->CTLR         = 0x00000001; // enable GIC distributor


  for(int i = 0; i < MAX_PROGS; i++){
    //memset( &pcb[ i ], 0, sizeof( pcb_t ) );
    pcb[ i ].status = TERMINATED;
  }


  for(int i = 0; i < MAX_PIPES; i++){
    //memset( &pipes[ i ], 0, sizeof( pipe_t ) );
    pipes[ i ].status = TERMINATED;
  }

  memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
  pcb[ 0 ].pid      = 1;
  pcb[ 0 ].ctx.cpsr = 0x50;
  pcb[ 0 ].ctx.pc   = ( uint32_t )( &main_console );
  pcb[ 0 ].ctx.sp   = ( uint32_t )( &tos_console );
  pcb[ 0 ].status   =  READY;
  pcb[ 0 ].priority = 1;
  max_pid++;



  /* Once the PCBs are initialised, we (arbitrarily) select one to be
   * restored (i.e., executed) when the function then returns.
   */

  current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );

  int_enable_irq();

  return;
}

void hilevel_handler_svc( ctx_t* ctx, uint32_t id ) {
  /* Based on the identified encoded as an immediate operand in the
   * instruction,
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */

  switch( id ) {
    case 0x00 : { // 0x00 => yield()
      scheduler( ctx );
      break;
    }
    case 0x01 : { // 0x01 => write( fd, x, n )
      int   fd = ( int   )( ctx->gpr[ 0 ] );
      char*  x = ( char* )( ctx->gpr[ 1 ] );
      int    n = ( int   )( ctx->gpr[ 2 ] );

      for( int i = 0; i < n; i++ ) {
        PL011_putc( UART0, *x++, true );
      }
      ctx->gpr[ 0 ] = n;
      break;
    }
    case 0x03 : {
      call_fork( ctx );  // 0x03 => fork()
      break;
    }
    case 0x04 : {
      call_exit( ctx );
      scheduler(ctx);
      break;
    }
    case 0x05 : {
      call_exec( ctx );
      break;
    }
    case 0x07 : {
      //Some how pass the prio to the call_prio_fork function
      int prio = ( int ) (ctx->gpr[ 0 ]);
      call_prio_fork( ctx , prio );
      break;
    }
    case 0x08 : { //create pipe
      call_pipe( ctx );
      break;
    }
    case 0x09 : { //write
      call_pipe_write( ctx );
      break;
    }
    case 0x10 : { //read
      call_pipe_read( ctx );
      break;
    }
    case 0x11 : { //close
      call_pipe_close( ctx );
      break;
    }
    default   : { // 0x?? => unknown/unsupported
      break;
    }
  }

  return;
}

void hilevel_handler_irq(ctx_t* ctx) {
  // Step 2: read  the interrupt identifier so we know the source.

  uint32_t id = GICC0->IAR;

  // Step 4: handle the interrupt, then clear (or reset) the source.

  if( id == GIC_SOURCE_TIMER0 ) {
    //PL011_putc( UART0, 'c', true );
    printIt("c",1);
    scheduler(ctx); TIMER0->Timer1IntClr = 0x01;
  }

  // Step 5: write the interrupt identifier to signal we're done.

  GICC0->EOIR = id;

  return;
}
