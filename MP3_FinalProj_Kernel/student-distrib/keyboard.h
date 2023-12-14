#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "terminal.h"
#include "syscall.h"



/* Interrupt vector for keyboard and rtc */
#define KEYBOARD_IV     0x01
#define KEYBOARD_PORT   0x60
#define KEYBOARD_BUFFER_SIZE 127
#define TOTAL_CODE      0xFF
/* macros for first and last char of each row */
#define SCANCODE_Q      0x10
#define SCANCODE_P      0x19
#define SCANCODE_A      0x1E
#define SCANCODE_L      0x26
#define SCANCODE_Z      0x2C
#define SCANCODE_M      0x32
#define SCANCODE_C      0x2E
#define SCANCODE_S      0x1F

#define SCANCODE_BACKSPACE     0x0E 
#define SCANCODE_ENTER_PRESS   0x1C
#define SCANCODE_ENTER_RELEASE 0x9C
#define SCANCODE_CTRL          0x1D
#define SCANCODE_CTRL_R        0x9D
#define SCANCODE_CAPSLOCK      0x3A 
#define SCANCODE_LSHIFT        0x2A
#define SCANCODE_RSHIFT        0x36
#define SCANCODE_LSHIFT_R      0xAA
#define SCANCODE_RSHIFT_R      0xB6
#define SCANCODE_SPACEBAR      0x39
#define SCANCODE_TAB           0x0F
#define SCANCODE_ALT           0x38   
#define SCANCODE_ALT_R         0xB8
#define SCANCODE_F1            0xBB
#define SCANCODE_F2            0xBC
#define SCANCODE_F3            0xBD

#define SCANCODE_BACKSLASH     0x2B


#define CAPS_OFFSET            0x40
#define SHIFT_OFFSET           0x80 //completely arbitrary offsets

      

/* macros for first and last char of num */
#define SCANCODE_ONE    0x02
#define SCANCODE_ZERO   0x0B
#define SCANCODE_EQUALS 0x0D
#define SCANCODE_ENDBKT 0x1B
#define SCANCODE_BTICK  0x29
#define SCANCODE_SLASH  0x35

/* table that map scancode to character */
char scan_codes[TOTAL_CODE];
char keyb_buff[KEYBOARD_BUFFER_SIZE];
void switch_terminal(int new_term_ID);

int kbb_ind;
int ctrl_flag;
int caps_lock_flag;
int shift_flag;
int alt_flag;
volatile int enter_flag;

/* init functions */ 
void init_keyboard();
/* keyboard handler and its linkage function */
int keyboard_handler();
extern int linkage_keyboard_handler();


void begString(void);


#endif /* _KEYBOARD_H */
