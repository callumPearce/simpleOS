#include "mouse.h"

//Frame buffer table which stores all pixels for the GUI.
uint16_t fb[ 600 ][ 800 ];
//Defines a new mouse struct.
mouse_t mouse;
//Defines the program code which is to be loaded.
int loading_prog = 0;


//Checks if a program button has been clicked
void check_prog(ctx_t* ctx){
  if( loading_prog != 0 ){
    ctx->gpr[ 3 ] = loading_prog;
    loading_prog = 0;
  }
  else ctx->gpr[ 3 ] = 0;
  return;
}


//Refreshes all pixels on UI
void ui_refresh(){

  //White background
  for( int i = 0; i < 600; i++ ) {
    for( int j = 0; j < 800; j++ ) {
      fb[ i ][ j ] = white;
    }
  }

  //P3
  for( int k = 50; k < 200; k++) {
    for( int l = 50; l < 250; l++) {
      fb[ k ][ l ] = blue;
    }
  }
  for( int k = 150; k < 200; k++){
    for( int l = 50; l < 150; l++){
      fb[ k ][ l ] = black;
    }
  }
  for( int k = 150; k < 200; k++){
    for( int l = 150; l < 250; l++){
      fb[ k ][ l ] = orange;
    }
  }
  //P3


  //P4
  for( int k = 50; k < 200; k++) {
    for( int l = 300; l < 500; l++) {
      fb[ k ][ l ] = red;
    }
  }
  for( int k = 150; k < 200; k++){
    for( int l = 300; l < 400; l++){
      fb[ k ][ l ] = black;
    }
  }
  for( int k = 150; k < 200; k++){
    for( int l = 400; l < 500; l++){
      fb[ k ][ l ] = orange;
    }
  }
  //P4


  //P5
  for( int k = 50; k < 200; k++) {
    for( int l = 550; l < 750; l++) {
      fb[ k ][ l ] = green;
    }
  }
  for( int k = 150; k < 200; k++){
    for( int l = 550; l < 650; l++){
      fb[ k ][ l ] = black;
    }
  }
  for( int k = 150; k < 200; k++){
    for( int l = 650; l < 750; l++){
      fb[ k ][ l ] = orange;
    }
  }
  //P5

  //Waiter
  for( int k = 250; k < 550; k++) {
    for( int l = 50; l < 375; l++) {
      fb[ k ][ l ] = yellow;
    }
  }

  //Dining
  for( int k = 250; k < 550; k++) {
    for( int l = 425; l < 750; l++) {
      fb[ k ][ l ] = purple;
    }
  }

}


//Updates mouses position in the UI based on the current state of the mouse struct
void update_mouse_position(){
  uint16_t x = mouse.location_x;
  uint16_t y = mouse.location_y;

  fb[y+7][x+6] = 000000;
  fb[y+6][x+6] = 000000;
  fb[y+6][x+5] = 000000;
  fb[y+5][x+6] = 000000;
  fb[y+5][x+5] = 000000;
  fb[y+5][x+4] = 000000;
  fb[y+4][x+4] = 000000;
  fb[y+4][x+5] = 000000;
  fb[y+4][x+3] = 000000;
  fb[y+3][x+4] = 000000;
  fb[y+3][x+3] = 000000;
  fb[y+2][x+3] = 000000;
  fb[y+3][x+2] = 000000;
  fb[y+2][x+2] = 000000;
  fb[y+4][x]   = 000000;
  fb[y+4][x+1] = 000000;
  fb[y+3][x+1] = 000000;
  fb[y+3][x]   = 000000;
  fb[y+2][x+1] = 000000;
  fb[y+2][x]   = 000000;
  fb[y+1][x+1] = 000000;
  fb[y+1][x]   = 000000;
  fb[y][x+1]   = 000000;
  fb[y][x]     = 000000;
  fb[y][x+2]   = 000000;
  fb[y+1][x+2] = 000000;
  fb[y][x+3]   = 000000;
  fb[y+1][x+3] = 000000;
  fb[y][x+4]   = 000000;
  fb[y+1][x+4] = 000000;
}


//Checks if the proposed x cords of the mouse goes out of bounds, return true if so.
bool out_of_bounds(uint16_t relx, uint16_t rely){
  uint16_t proposedx = relx + mouse.location_x;
  uint16_t proposedy = rely + mouse.location_y;

  if(proposedx >= 793 || proposedy >= 592 || proposedx <= 1 || proposedy <= 1) return true;

  else return false;
}


//Checks what function so call when the mouse has been clicked based
//off of current location.
void find_button_click(){
  uint16_t x = mouse.location_x;
  uint16_t y = mouse.location_y;

  //Check for P3 prio = 0
  if( (y >= 50 && y <= 150) && (x >= 50 && x <= 250) ){
    loading_prog = 1;
  }
  //check for P3 prio = 1
  else if( (y >= 150 && y <= 200) && (x >= 50 && x <= 150 ) ){
    loading_prog = 2;
  }
  //check for P3 prio = 2
  else if( (y >= 150 && y <= 200) && (x >= 150 && x <= 250 )){
    loading_prog = 3;
  }


  //Check for P4 prio = 0
  else if( ( y >= 50 && y <= 150 ) && ( x >= 300 && x <= 500 ) ){
    loading_prog = 4;
  }
  //check for P4 prio = 1
  else if( (y >= 150 && y <= 200) && (x >= 300 && x <= 400) ){
    loading_prog = 5;
  }
  //check for P4 prio = 2
  else if( (y >= 150 && y <= 200) && (x >= 400 && x <= 500) ){
    loading_prog = 6;
  }


  //Check for P5
  else if( ( y >= 50 && y <= 150 ) && ( x >= 550 && x <= 750 ) ){
    loading_prog = 7;
  }
  //check for P5 prio = 1
  else if( (y >= 150 && y <= 200) && (x >= 550 && x <= 650) ){
    loading_prog = 8;
  }
  //check for P5 prio = 2
  else if( (y >= 150 && y <= 200) && (x >= 650 && x <= 750) ){
    loading_prog = 9;
  }


  //Check for Waiter
  else if( (y >= 250 && y <= 550) && (x >= 50 && x <= 375) ){
    loading_prog = 10;
  }

  //Check for Dining
  else if( (y >= 250 && y <= 550) && (x >= 425 && x <= 750) ){
    loading_prog = 11;
  }

}


int parsed = 1;
uint16_t state;
uint16_t relx;
uint16_t rely;
//Updates the buffer value of a mouse and updates it's UI positioning accordingly
void update_mouse_buffer(uint8_t x){

  //state
  if(parsed == 1){
    state = ( uint16_t ) x;
    //if clicked then check where it is clicked call function accordingly
    if(state == 9) find_button_click();
    parsed = 2;
  }

  //x
  else if(parsed == 2){
    uint16_t bit2 = ( uint16_t ) x;
    relx = bit2 - (( state << 4) & 0x100);
    parsed = 3;
  }

  //y
  else{
    uint16_t bit3 = ( uint16_t ) x;
    rely = - (bit3 - (( state << 3) & 0x100));
    if(!out_of_bounds(relx,rely)){
      ui_refresh();
      mouse.location_x += relx;
      mouse.location_y += rely;
      update_mouse_position();
    }
    parsed = 1;
  }
}
