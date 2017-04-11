#include "hilevel.h"

//External definitions for console functions.
extern void     main_console();
extern void     main_consoleGUI();

//External definition for the top of stack value for the console.
extern uint32_t tos_console;

//Frame buffer table which stores all pixels for the GUI.
uint16_t fb[ 600 ][ 800 ];

//Fixed size PCB table where all processes are stored.
pcb_t pcb[ MAX_PROGS ], *current = NULL;

//Fixed size PIPE table where all pipes are stored.
pipe_t pipes[ MAX_PIPES ];

//Global variable used to keep track of the largest process id initialised.
pid_t max_pid = 1;

//Global variable used to keep track of the largest pipe id initialised.
pipeId_t max_pipe_id = 0;


//Print function used for de-bugging.
void printIt(char* x, int n){
  for( int i = 0; i < n; i++ ) {
    PL011_putc( UART0, *x++, true );
  }
  return;
}


//Integer conversion function used for de-bugging.
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


//Finds the current pcb index
int find_pcb_index(int pid){
  for(int i = 0; i < MAX_PROGS; i++){
    if(pid == pcb[i].pid) return i;
  }
  return -1; //Return -1 if there is an error
}


//Finds the next pcb index to schedule in implements priority scheduling which is
//explained in the consoleGUI.c file.
int sched_count = 0;
int find_next_index_prio(int ind){
  for(int i = 0; i <= MAX_PROGS; i++){
    if(pcb[(ind+i)%MAX_PROGS].status != TERMINATED ){

      //Deals with high priority programs i.e. priority above 0.
      if ((pcb[(ind+i)%MAX_PROGS].priority  >= sched_count) && (pcb[(ind+i)%MAX_PROGS].priority != 0)){
        sched_count++;
        return (ind+i)%MAX_PROGS;
      }
      if ((pcb[(ind+i)%MAX_PROGS].priority + 1 == sched_count) && (pcb[(ind+i)%MAX_PROGS].priority != 0)) sched_count = 0;

      //Deals with low priority programs i.e. priority of 0.
      if (pcb[(ind+i)%MAX_PROGS].priority == 0 && (ind+i)%MAX_PROGS != ind) return (ind+i)%MAX_PROGS;

    }
  }
  return -1;
}


//Schedular: Currently using Priority scheduling scheme.
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


//Finds the next available pcb i.e. if a pcb is TERMINATED it should be used.
int find_free_pcb(){
  for(int i = 0; i < MAX_PROGS; i++){
    if(pcb[i].status == TERMINATED) return i;
  }
  return -1;
}


//Forks a new identical child process but assigns a specific priority to the pcb it has been forked into.
void call_prio_fork( ctx_t* ctx , int i_prio ){
  //1)Get the new ID by incrementing the largest PCB ID by 1.
  int newID = find_free_pcb();

  //2)Initialise the new process.
  memset( &pcb[ newID ], 0, sizeof( pcb_t ) );
  memcpy( &pcb[ newID ].ctx , ctx , sizeof( ctx_t ));

  //3)Update the process ID.
  pcb[ newID ].pid = max_pid;
  pcb[ newID ].status = READY;
  max_pid++;

  pcb[ newID ].priority = i_prio;

  int current_stack_entry = (void*) &tos_console + USERSS * find_pcb_index(current->pid);
  int new_stack_entry     = (void*) &tos_console + USERSS * newID;
  int stack_size = current_stack_entry - ctx->sp;

  //4)Update stack pointer value.
  pcb[ newID ].ctx.sp = new_stack_entry - stack_size;

  //5)Copy the current programs stack to the new programs stack.
  memcpy( (void *) new_stack_entry - USERSS , (void *) current_stack_entry - USERSS , USERSS);

  //6)Return zero for the new user process and the pid of the child to the parent process.
  pcb[ newID ].ctx.gpr[0] = 0;
  ctx->gpr[0] = pcb[ newID ].pid;

  //7)Return the newID for the pcb created.
  return;
}


//Fork the current user program with a fixed low priority of 0.
void call_fork( ctx_t* ctx ) {
  call_prio_fork( ctx , 0 );
  return;
}


//Exit the current user process and reset the pcb it's stored in.
void call_exit( ctx_t* ctx ){
  int current_pcb_index = find_pcb_index(current->pid);

  //1)Reset Stack
  int current_stack_entry = (void*) &tos_console + USERSS * find_pcb_index(current->pid);
  memset((void *) current_stack_entry - USERSS, 0 , USERSS);

  //2)Reset relating gpr's
  for(int i = 0; i < 13; i++) ctx->gpr[i] = 0;

  //3)Reset pid,lr,pc and cpsr
  ctx->sp = current_stack_entry;
  ctx->pc = ctx->gpr[ 0 ];
  ctx->cpsr = 0x50;
  ctx->lr = 0;

  //4)Set to status to TERMINATED
  pcb[current_pcb_index].status = TERMINATED;

  return;
}


//Execute the user program at the pc value specified.
void call_exec( ctx_t* ctx ){
  //1)Reset the stack and stack pointer.
  int current_stack_entry = (void*) &tos_console + USERSS * find_pcb_index(current->pid);
  memset((void *) current_stack_entry - USERSS, 0 , USERSS);
  ctx->sp = current_stack_entry;

  //2)Set pc to the passed in gpr value.
  ctx->pc = ctx->gpr[ 0 ];

  //3)Reset gpr's, lr and cpsr to default values.
  ctx->cpsr = 0x50;
  ctx->lr   = 0;
  for(int i = 0; i < 13; i++) ctx->gpr[i] = 0;

  return;
}


//Find a free pipe in the pipe table i.e one which is terminated.
int find_free_pipe(){
  for(int i = 0; i < MAX_PIPES; i++){
    if(pipes[i].status == TERMINATED) return i;
  }
  return -1;
}


//Find the index of a pipe in the pipe table by it's id.
int find_ind_pipe( pipeId_t pipe_id ){
  for(int i = 0; i < MAX_PIPES; i++){
    if(pipe_id == pipes[i].pipeId && pipes[i].status != TERMINATED) return i;
  }
  return -1; //Return -1 if there is an error
}


//Allocates a pipe in the pipe table using the context passed in and returns the pipes id.
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


//Write data to the pipe_id recieved from the context and update the pipes content
//value using the context passed in.
void call_pipe_write( ctx_t* ctx  ){
  pipeId_t pipe_id = ctx->gpr[0];
  int pipe_ind = find_ind_pipe( pipe_id );
  pipes[ pipe_ind ].content = ctx->gpr[2];
  return;
}



//Read data from the pipe and update the context with the value read.
//Bi-directional communication enabled by this pipe read function.
void call_pipe_read( ctx_t* ctx ){

  pid_t pid = ctx->gpr[0];
  for(int i = 0; i < MAX_PIPES; i++){

    //For the reciever of the pipe attempting to read.
    if( ((current->pid == pipes[i].end1) && (pid == pipes[i].end2)) ) {
      if(pipes[i].status != TERMINATED){
        int j = pipes[i].content;
        ctx->gpr[0] = pipes[i].content;
        return;
      }
    }
    //For the initial creator of the pipe attempting to read.
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


//Closes the current pipe by resetting all of its contained values and setting
//it's status to TERMINATED.
void call_pipe_close( ctx_t* ctx ){
  pipeId_t pipe_id = ctx->gpr[0];
  int pipeInd = find_ind_pipe(pipe_id);
  memset( &pipes[ pipeInd ], 0, sizeof( pipe_t ) );
  pipes[ pipeInd ].status = TERMINATED;
  return;
}


//Handles reset interrupts.
void hilevel_handler_rst( ctx_t* ctx              ) {

  // Configure the LCD display into 800x600 SVGA @ 36MHz resolution.

  SYSCONF->CLCD      = 0x2CAC;     // per per Table 4.3 of datasheet
  LCD->LCDTiming0    = 0x1313A4C4; // per per Table 4.3 of datasheet
  LCD->LCDTiming1    = 0x0505F657; // per per Table 4.3 of datasheet
  LCD->LCDTiming2    = 0x071F1800; // per per Table 4.3 of datasheet

  LCD->LCDUPBASE     = ( uint32_t )( &fb );

  LCD->LCDControl    = 0x00000020; // select TFT   display type
  LCD->LCDControl   |= 0x00000008; // select 16BPP display mode
  LCD->LCDControl   |= 0x00000800; // power-on LCD controller
  LCD->LCDControl   |= 0x00000001; // enable   LCD controller

  /* Configure the mechanism for interrupt handling by
   *
   * - configuring then enabling PS/2 controllers st. an interrupt is
   *   raised every time a byte is subsequently received,
   * - configuring GIC st. the selected interrupts are forwarded to the
   *   processor via the IRQ interrupt signal, then
   * - enabling IRQ interrupts.
   */

  PS20->CR           = 0x00000010; // enable PS/2    (Rx) interrupt
  PS20->CR          |= 0x00000004; // enable PS/2 (Tx+Rx)
  PS21->CR           = 0x00000010; // enable PS/2    (Rx) interrupt
  PS21->CR          |= 0x00000004; // enable PS/2 (Tx+Rx)

  uint8_t ack;

        PL050_putc( PS20, 0xF4 );  // transmit PS/2 enable command
  ack = PL050_getc( PS20       );  // receive  PS/2 acknowledgement
        PL050_putc( PS21, 0xF4 );  // transmit PS/2 enable command
  ack = PL050_getc( PS21       );  // receive  PS/2 acknowledgement

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
   GICD0->ISENABLER1 |= 0x00300000; // enable PS2          interrupts
   GICC0->CTLR         = 0x00000001; // enable GIC interface
   GICD0->CTLR         = 0x00000001; // enable GIC distributor


  for(int i = 0; i < MAX_PROGS; i++){
    pcb[ i ].status = TERMINATED;
  }


  for(int i = 0; i < MAX_PIPES; i++){
    pipes[ i ].status = TERMINATED;
  }

  /*
  memset( &pcb[ 0 ], 0, sizeof( pcb_t ) ); //TODO swap to use without GUI
  pcb[ 0 ].pid      = 1;
  pcb[ 0 ].ctx.cpsr = 0x50;
  pcb[ 0 ].ctx.pc   = ( uint32_t )( &main_console );
  pcb[ 0 ].ctx.sp   = ( uint32_t )( &tos_console );
  pcb[ 0 ].status   =  READY;
  pcb[ 0 ].priority = 1;
  max_pid++;*/


  memset( &pcb[ 0 ], 0, sizeof( pcb_t ) );
  pcb[ 0 ].pid      = 1;
  pcb[ 0 ].ctx.cpsr = 0x50;
  pcb[ 0 ].ctx.pc   = ( uint32_t )( &main_consoleGUI );
  pcb[ 0 ].ctx.sp   = ( uint32_t )( &tos_console );
  pcb[ 0 ].status   =  READY;
  pcb[ 0 ].priority = 1;
  max_pid++;

  /* Once the PCBs are initialised, we (arbitrarily) select one to be
   * restored (i.e., executed) when the function then returns.
   */

  current = &pcb[ 0 ]; memcpy( ctx, &current->ctx, sizeof( ctx_t ) );

  int_enable_irq();

  ui_refresh();

  return;
}


//Handles supervisor interrupt calls.
void hilevel_handler_svc( ctx_t* ctx, uint32_t id ) {
  /* Based on the identified encoded as an immediate operand in the
   * instruction,
   *
   * - read  the arguments from preserved usr mode registers,
   * - perform whatever is appropriate for this system call,
   * - write any return value back to preserved usr mode registers.
   */

  switch( id ) {
    case 0x00 : {
      scheduler( ctx );
      break;
    }
    case 0x01 : {
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
      call_fork( ctx );
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
      int prio = ( int ) (ctx->gpr[ 0 ]);
      call_prio_fork( ctx , prio );
      break;
    }
    case 0x08 : {
      call_pipe( ctx );
      break;
    }
    case 0x09 : {
      call_pipe_write( ctx );
      break;
    }
    case 0x10 : {
      call_pipe_read( ctx );
      break;
    }
    case 0x11 : {
      call_pipe_close( ctx );
      break;
    }
    case 0x12 : {
      check_prog( ctx );
      break;
    }
    default   : { // 0x?? => unknown/unsupported
      break;
    }
  }

  return;
}


//Handles irq interrupt calls.
void hilevel_handler_irq(ctx_t* ctx) {

  uint32_t id = GICC0->IAR;


  //TIMER INTERRUPT
  if( id == GIC_SOURCE_TIMER0 ) {
    //PL011_putc( UART0, 'c', true );
    printIt("\n",2);
    scheduler(ctx); TIMER0->Timer1IntClr = 0x01;
  }

  //UI INTERRUPT
  if     ( id == GIC_SOURCE_PS20 ) {
    uint8_t x = PL050_getc( PS20 );
  }
  else if( id == GIC_SOURCE_PS21 ) {
    uint8_t x = PL050_getc( PS21 );
    update_mouse_buffer(x);
  }

  GICC0->EOIR = id;

  return;
}
