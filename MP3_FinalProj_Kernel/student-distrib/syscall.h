#ifndef _SYSCALL_H
#define _SYSCALL_H
#define ACTUAL_MAX_DIRS 17

#include "x86_desc.h"
#include "lib.h"
#include "i8259.h"
#include "file_system_driver.h"
#include "terminal.h"

#define SUCCESS         0
#define FAILURE         -1
#define MAX_NUM_FILE    8
#define OUTPUTVAL       1
#define INPUTVAL        0
#define THIRTYTWO       32
#define TWO             2
#define THIRTYFOUR      34

#define IDT_IV     0x80

#define EXE_B0  0x7f
#define EXE_B1  0x45
#define EXE_B2  0x4c
#define EXE_B3  0x46

#define VM_START_ADDR   0x08048000
#define EIGHT_MB        0x800000
#define FOUR_MB         0x00400000
#define EIGHT_KB        0x002000
#define FOUR_KB         0x1000
#define ONE32_MB        0x08400000
#define ONE28_MB        0x08000000
#define VM_ADDR         0x8800000

extern int32_t pidVal;

int32_t sys_halt (uint32_t status);

int32_t sys_execute(const uint8_t* command);

int32_t sys_read(int32_t fd, void* buf, int32_t nbytes);

int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes);

int32_t sys_open(const uint8_t* filename);

int32_t sys_close (int32_t fd);

int32_t sys_getargs (uint8_t* buf, int32_t nbytes);

int32_t sys_vidmap (uint8_t** screen_start);

int32_t sys_set_handler (int32_t signum, void* handler_address);

int32_t sys_sigreturn (void);

int32_t free_idx_fd(file_descriptor_t file_descriptor_array[8]);

extern int32_t sys_call_handler (void);

void switch_to_user_mode(uint32_t saved_eip);


typedef struct PCB 
{
    uint32_t Pid ;                  //pid of the process we're executing
    int32_t Parent_id ;             //pid of the shell that it was called in
    file_descriptor_t file_descriptor[8] ; //describes the files
    
    uint32_t saved_esp  ;           //save the esp, ebp of what called the process so it can return to it in halt
    uint32_t saved_ebp ;    
    
    uint32_t sched_esp  ;           //ebp of the executing program -- used for scheduling
    uint32_t sched_ebp ; 

    uint32_t active ;               //indicates whether it is active or not
    uint32_t term_id;               //terminal id of which terminal the process was called in
}pcb_t;

//Functions to get certain PCBs:
pcb_t* get_cur_pcb();

pcb_t* get_pcb(int process_ID);




#endif

