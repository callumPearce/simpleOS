
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// Include functionality relating to the platform.

#include   "GIC.h"
#include "PL011.h"
#include "PL050.h"
#include "PL111.h"
#include   "SYS.h"

//Struct for a mouse.
typedef struct {
  uint16_t location_x;
  uint16_t location_y;
  uint16_t status;
} mouse_t;


void update_mouse_buffer( uint8_t x );
void ui_refresh();
void check_prog();

//Struct for a context (ctx).
typedef struct {
  uint32_t cpsr, pc, gpr[ 13 ], sp, lr;
} ctx_t;

#define blue ( 0x001F )

//0000 0000 0001 1111 0x001F

#define red ( 0x7C00 )

//0111 1100 0000 0000

#define green (0x03E0)

//0000 0011 1110 0000

#define yellow ( 0x03FF )

//0000 0011 1111 1111

#define purple ( 0x7C1F )

//0111 1100 0001 1111

#define white ( 0xFFFF )

#define black ( 0x0000 )

#define orange ( 0x1A7F )

//01111 1100 1100110
