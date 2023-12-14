#ifndef  _TERMINAL_H
#define _TERMINAL_H

#include "keyboard.h"



#define BUFFER_SIZE 128
#define MAX_TERMS   3
#define VIDEO_LOC       0xB8000
#define four_kB         4096
#define NUM_COLS  80

volatile int enter_flag;
volatile int start_term;
volatile int scheduling_flag;

//Reads FROM the keyboard buffer into buf, return number of bytes read
//char* file_name, int32_t fd, void* buf, int32_t nbytes, uint32_t offset
int terminal_read(int32_t fd, void* buf, int nbytes, char* file_name, uint32_t offset,uint32_t inode_index);
//Writes TO the screen from buf, return number of bytes written or -1
int terminal_write(int32_t fd, const void* buf, int32_t nbytes);
//initializes terminal stuff, return 0
int terminal_open(const uint8_t* filename);
//clears any terminal specific variables
int terminal_close(int32_t fd);
//initialize all the variables corresponding to the terminal
int terminal_init(void);

extern char screen_buf[BUFFER_SIZE]; //declaration

typedef struct terminal_t{
    //the overall process count for this terminal
    int proc_cnt;
    //the address of this terminal
    uint32_t addr;
    //the list of processes for this terminal
    uint8_t processes[6]; //maximum of 6 processes
    //cursor location
    uint16_t cursor;
    //corresponding input buffer
    char sbuf[BUFFER_SIZE];
}term_t;

//global variable to indicate which terminal you're on
volatile int curr_term_ID;
volatile int schedule_term_ID;
//global array of each buffer for terminal
term_t terminals[MAX_TERMS];

#endif


