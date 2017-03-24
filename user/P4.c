#include "P4.h"

uint32_t gcd( uint32_t x, uint32_t y ) {
  if     ( x == y ) {
    return x;
  }
  else if( x >  y ) {
    return gcd( x - y, y );
  }
  else if( x <  y ) {
    return gcd( x, y - x );
  }
}

void main_P4() {
  int pipe_id = pipe(3);
  int pipe2_id = pipe(4);
  //if(pipe2_id == 1) write( STDOUT_FILENO, "YY", 2 );
  while( 1 ) {
    pipe_write(pipe_id , 50);
    pipe_write(pipe2_id , 30);

    write( STDOUT_FILENO, "P4", 2 );

    uint32_t lo = 1 <<  4;
    uint32_t hi = 1 <<  8;

    for( uint32_t x = lo; x < hi; x++ ) {
      for( uint32_t y = lo; y < hi; y++ ) {
        uint32_t r = gcd( x, y );
      }
    }
  }

  exit( EXIT_SUCCESS );
}
