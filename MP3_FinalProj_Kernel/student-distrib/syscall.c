#include "syscall.h"
#include "rtc.h"
#include "paging.h"
#include "terminal.h"

#define LOW_FD  2
#define HIGH_FD     7



int32_t pidVal = 0;

struct file_ops stdin_fops ;
struct file_ops stdout_fops ;

uint8_t current_command[500] = {};
extern void haltVoid(uint32_t esp_, uint32_t ebp_, uint32_t status);




/* -------------------------------- sys_halt -------------------------------- */
/*  inputs: status
 *  Return Value: 0
 */
int32_t sys_halt (uint32_t status)
{   
    
    cli();
    pcb_t* current_pcb = get_cur_pcb();
 
    if (current_pcb->Parent_id == -1) {
        printf("exited out of first shell, restarting new one...\n");
        terminals[current_pcb->term_id].proc_cnt -= 1;
        sti();
        sys_execute((const uint8_t*)"shell");
        return 1;
    }
    // Restore parent data
    pidVal = current_pcb->Parent_id;
    pcb_t* parent_pcb = get_cur_pcb();

    term_t* current_term = &terminals[current_pcb->term_id];
    current_term->proc_cnt -= 1;

    // Restore parent paging
    //Similar to execute, but this time, parent paging info
    uint32_t dummy = (TWO + parent_pcb->Pid);
    pages_directory[first_10bitsof_128Mb].p4m.base_address = dummy;
    loadPageDirectory((unsigned int*)pages_directory); //flushes TLB

    // Close any relevant FDs
    int i;
    for(i = 2; i < MAX_NUM_FILE; i++){
        current_pcb->file_descriptor[i].file_position = 0 ; //set zero offset
        current_pcb->file_descriptor[i].file_table_pointer = 0 ; //set zero offset
        current_pcb->file_descriptor[i].internal_ftype = 0 ; //set zero offset
        current_pcb->file_descriptor[i].flags = 0 ; //set to not busy
        current_pcb->file_descriptor[i].name_of_file = "" ; //set zero offset
    }
    tss.ss0 = KERNEL_DS; 
    tss.esp0 = EIGHT_MB - (EIGHT_KB* parent_pcb->Pid) -4;

    // Jump to execute return
    haltVoid(current_pcb->saved_esp, current_pcb->saved_ebp, status);
    return FAILURE;

}
/* -------------------------------------------------------------------------- */

/* ------------------------------- sys_execute ------------------------------ */
/*  inputs: command
 *  outputs:    none
 *  Return Value:   -1:     if the command cannot be executed
 *                  256:    if the program dies by an exception
 *                  0-255:  if the program executes a halt system call
 */
int32_t sys_execute(const uint8_t* command)
{   
    cli();
    // get the next pidVal to execute
    pidVal = terminals[0].proc_cnt + terminals[1].proc_cnt + terminals[2].proc_cnt;

    // cap the max Pidval
    if (pidVal >= 6) { //max pid val is 6
        sti();
        return FAILURE;
    } 

    int idx;

    // Parse arguments (cat frame1.txt)
    memset(current_command, 0, 500);
    for (idx = 0; idx < strlen((const char*)command); idx++) {
        current_command[idx] = command[idx];
    }

    // get the exe file
    struct file_dentry_t dummy;
	struct file_inode_t inode_dummy;
    char cmd[32] = {};
    for (idx = 0; idx < strlen((const char*)command); idx++) {
        if (command[idx] == 0 ||  command[idx] == ' ') //CONSTANT TIME
            break;
        cmd[idx] = command[idx];
    }
	if (read_dentry_by_name((const uint8_t*)cmd,&dummy) == FAILURE) {sti(); return FAILURE;}
    inode_dummy = start_inode[dummy.inode_num] ;

    // read the first 4 bytes
    static uint8_t exeBuf[4];
    int size = start_inode[dummy.inode_num].length;
	file_read(0, (uint8_t*)(exeBuf), 4, (char*)cmd, 0,dummy.inode_num);
    int isExe = 1;

    // check if file is an exe file
    isExe = (exeBuf[0] == EXE_B0) && (exeBuf[1] == EXE_B1) && (exeBuf[2] == EXE_B2) && (exeBuf[3] == EXE_B3);
    if (!isExe) {sti();return FAILURE;}
    
    // Create Process Control Block (PCB)
    if(pidVal <= 2) pidVal = curr_term_ID;
    pcb_t* current_pcb = get_cur_pcb();
    term_t* current_term = &terminals[curr_term_ID];
    current_pcb->Pid = pidVal;

    int par_pid;
    if (current_term->proc_cnt == 0) par_pid = -1;
    else par_pid = current_term->processes[current_term->proc_cnt-1];
    
    current_pcb->Parent_id = par_pid; 
    current_term->proc_cnt += 1;
    current_term->processes[current_term->proc_cnt-1] = pidVal;
    current_pcb->term_id = curr_term_ID;

    printf("we are in terminal %d\n", curr_term_ID);

    /*Initialize the fd array*/
    /*STDIN*/
    stdin_fops.write = 0 ;
    stdin_fops.read = &terminal_read ; 
    stdin_fops.close = &terminal_close ; 
    stdin_fops.open = &terminal_open ; 
    current_pcb->file_descriptor[0].file_table_pointer = &stdin_fops ;
    /*STDIN*/

    /*STDOUT*/
    stdout_fops.write = &terminal_write ;
    stdout_fops.read = 0 ; 
    stdout_fops.close = &terminal_close ; 
    stdout_fops.open = &terminal_open ; 
    current_pcb->file_descriptor[1].file_table_pointer = &stdout_fops ;
    /*STDOUT*/

    for (idx = LOW_FD ; idx < HIGH_FD+1 ; idx++)
    {
        current_pcb->file_descriptor[idx].name_of_file = "" ;
        current_pcb->file_descriptor[idx].file_position = 0;
        current_pcb->file_descriptor[idx].file_table_pointer = 0;
        current_pcb->file_descriptor[idx].flags = 0; //free = 0 and busy = 1
        current_pcb->file_descriptor[idx].inode_index = -1;
    }
    /*Initialize the fd array - END*/     

    /*
    to my knowledge, every spawned process is given a virtual address space by the kernel which 
    is done by modifying the one page directory, the same one from cp1. you do change a particular 
    virtual address mapping to point at a different phys address for each execute call
    */

    // TODO: map that startingAddr to virtual mem VM_START_ADDR (not sure if still needed, need Naman)
    setup_PID(current_pcb);


    // User-level Program Loader (load from FS to program page)
	file_read( 0, (uint8_t*)VM_START_ADDR, size, (char*)command, 0,dummy.inode_num);
    uint32_t prog_eip; // this is where EIP should be
    uint8_t prog_buffer[4] ; 
    read_data(dummy.inode_num, 24, (uint8_t *)(&prog_buffer), 4); // byte 24-27  = eip
    uint8_t* progcastptr = (uint8_t*)&prog_eip ;
    *(progcastptr+3) = prog_buffer[3];
    *(progcastptr+2) = prog_buffer[2];
    *(progcastptr+1) = prog_buffer[1];
    *(progcastptr+0) = prog_buffer[0];

    // Setup old stack & eip, and setup TSS
    current_pcb->active = 1; 
    tss.ss0 = KERNEL_DS; 
    tss.esp0 = EIGHT_MB - (EIGHT_KB * current_pcb->Pid) -4;

    // Goto usermode and IRET
    // switch_to_user_mode(prog_eip);

    uint32_t user_esp = ONE28_MB + FOUR_MB - sizeof(uint32_t);
    register uint32_t saved_ebp asm("ebp");
    register uint32_t saved_esp asm("esp");

    current_pcb->saved_esp = saved_esp;
    current_pcb->saved_ebp = saved_ebp;
         
    sti();
    asm volatile("              ;\
        cli                     ;\
        movw  %%dx, %%ds       ;\
        pushl %%edx             ;\
        pushl %%eax             ;\
        pushf                   ;\
        pop %%esi               ;\
        or $0x200, %%esi        ;\
        push %%esi              ;\
        pushl %%ecx             ;\
        push %%ebx              ;\
        iret                    ;\
        "
        :
        : "a" (user_esp), "b"(prog_eip), "c"(USER_CS), "d" (USER_DS)
        : "memory"
        );
    

    return 1 ; //NEED TO RETURN STH ????
}





// The read system call reads data from the keyboard, a file, device (RTC), or directory. This call returns the number
// of bytes read. If the initial file position is at or beyond the end of file, 0 shall be returned (for normal files and the
// directory). In the case of the keyboard, read should return data from one line that has been terminated by pressing
// Enter, or as much as fits in the buffer from one such line. The line returned should include the line feed character.
// In the case of a file, data should be read to the end of the file or the end of the buffer provided, whichever occurs
// sooner. In the case of reads to the directory, only the filename should be provided (as much as fits, or all 32 bytes), and
// subsequent reads should read from successive directory entries until the last is reached, at which point read should
// repeatedly return 0. For the real-time clock (RTC), this call should always return 0, but only after an interrupt has
// occurred (set a flag and wait until the interrupt handler clears it, then return 0). You should use a jump table referenced
// by the task’s file array to call from a generic handler for this call into a file-type-specific function. This jump table
// should be inserted into the file array on the open system call (see below).


/* -------------------------------------------------------------------------- */

/* -------------------------------- sys_open -------------------------------- */
/*  inputs: filename -- name of file to open
 *  Return Value:   -1: named file DNE || no descriptors are free
 *                  file descriptor
 */
int32_t sys_open(const uint8_t* filename)
{
    struct file_dentry_t current_dentry ;
    static struct file_ops rtc_fops ;
    static struct file_ops dir_fops ;
    static struct file_ops file_fops ;
    // pidVal -= 1;
    pcb_t* current_pcb = get_cur_pcb();
    // pidVal += 1;
    // if (current_pcb->term_id != curr_term_ID) return FAILURE;
    if (read_dentry_by_name(filename,&current_dentry) == -1) //search dentry by name
    {
        return -1 ;
    }
    int free_id ;
    free_id = free_idx_fd(current_pcb->file_descriptor) ; //find first free idx in fd array
    if (free_id == 8) //8 is invalid fd (fd>7)
    {
        return -1 ;
    }
    switch(current_dentry.filetype)
    {
        case 0: //RTC files
            current_pcb->file_descriptor[free_id].name_of_file = (char*) filename ;
            current_pcb->file_descriptor[free_id].file_position = 0;
            current_pcb->file_descriptor[free_id].inode_index = 0 ;
            current_pcb->file_descriptor[free_id].internal_ftype = 0; //set attribs

            rtc_fops.open = &rtc_open ;
            rtc_fops.close = &rtc_close ;
            rtc_fops.read = &rtc_read ;  
            rtc_fops.write = &rtc_write ;

            current_pcb->file_descriptor[free_id].file_table_pointer = &rtc_fops ; //NEED TO GDB
            break ;
           
        case 1: //Directory
            current_pcb->file_descriptor[free_id].name_of_file = (char*) filename ;
            current_pcb->file_descriptor[free_id].file_position = 0;
            current_pcb->file_descriptor[free_id].inode_index = 0 ;
            current_pcb->file_descriptor[free_id].internal_ftype = 1;

            dir_fops.open = &dir_open ;
            dir_fops.close = &dir_close ;
            dir_fops.read = &dir_read ;  
            dir_fops.write = &dir_write ;

            current_pcb->file_descriptor[free_id].file_table_pointer =&dir_fops ; //NEED TO GDB
            break ;

        case 2: //Regular File
            current_pcb->file_descriptor[free_id].name_of_file = (char*) filename ;
            current_pcb->file_descriptor[free_id].file_position = 0;
            current_pcb->file_descriptor[free_id].inode_index = current_dentry.inode_num ;
            current_pcb->file_descriptor[free_id].internal_ftype = 2;

            file_fops.open = &file_open ;
            file_fops.close = &file_close ;
            file_fops.read = &file_read ;
            file_fops.write = &file_write ;

            current_pcb->file_descriptor[free_id].file_table_pointer =  &file_fops ; //NEED TO GDB
            break ;
    }
    return free_id;
}


/* -------------------------------------------------------------------------- */

/* -------------------------------- sys_read -------------------------------- */
/*  inputs: fd --   file descriptor val
 *          buf --  the values to print to memory
 *          nbytes --   number of characters to print
 *  outputs:    none
 *  Return Value:   nbytes : bytes success read
 *                  -1: FAILURE
 */
int32_t sys_read(int32_t fd, void* buf, int32_t nbytes)
{
    if(buf == NULL || nbytes <= 0 || fd > 7 || fd < 0) //parameter checking
    {
        return FAILURE;
    }
    // pidVal -= 1;
    // struct file_dentry_t tempdentry ;
    pcb_t* cur_pcb_ptr = get_cur_pcb();
    // if (fd != 0 && fd != 1)
    // {
    //     read_dentry_by_name((uint8_t*)cur_pcb_ptr->file_descriptor[fd].name_of_file,&tempdentry);
    //     cur_pcb_ptr->file_descriptor[fd].internal_ftype = tempdentry.filetype ;
    // }
    //memset(buf,0,(start_inode+cur_pcb_ptr->file_descriptor[fd].inode_index)->length);
    // pidVal += 1;
    //call generic read func

    // if (cur_pcb_ptr->term_id != curr_term_ID) return FAILURE;
    if (cur_pcb_ptr->file_descriptor[fd].internal_ftype == 1)//IF DIR
    {
        if (cur_pcb_ptr->file_descriptor[fd].file_position == ACTUAL_MAX_DIRS)
        {
            return 0 ;
        }
    }
    if (fd >= LOW_FD && cur_pcb_ptr->file_descriptor[fd].flags == 0) 
        return FAILURE;
    struct file_ops temp_ops = *cur_pcb_ptr->file_descriptor[fd].file_table_pointer;
    if (fd < LOW_FD && temp_ops.read == 0)
        return FAILURE;
    //memset((char*)buf, 0, cur_pcb_ptr->file_descriptor[fd].inode_index);
    //for (i = 0; i < ; i++) *((char *)buf+i) = '\0';
    int bytes_read;
    bytes_read = cur_pcb_ptr->file_descriptor[fd].file_table_pointer[0].read(fd,buf,nbytes, cur_pcb_ptr->file_descriptor[fd].name_of_file, cur_pcb_ptr->file_descriptor[fd].file_position,cur_pcb_ptr->file_descriptor[fd].inode_index);
    
    if (bytes_read == -1) //invalid read
    {
        return FAILURE ;
    }

    if (cur_pcb_ptr->file_descriptor[fd].internal_ftype == 2)//reg file
    {
        cur_pcb_ptr->file_descriptor[fd].file_position += bytes_read ;
    }
    else if (cur_pcb_ptr->file_descriptor[fd].internal_ftype == 1)//dir
    {
        cur_pcb_ptr->file_descriptor[fd].file_position++ ; 
    }
    else //rtc or terminal read
    {
        cur_pcb_ptr->file_descriptor[fd].file_position = 0 ; 
    }
    
    return bytes_read ;
    // if(cur_pcb_ptr -> file_descriptor[fd].flags == OUTPUTVAL) {
    //     return FAILURE;
    // }
    // cur_pcb_ptr->file_descriptor[fd].file_position ;
    // cur_pcb_ptr->file_descriptor[fd].file_position += cur_pcb_ptr->file_descriptor[fd].file_table_pointer->read(fd,buf,nbytes);
    // return cur_pcb_ptr->file_descriptor[fd].file_table_pointer->read(fd,buf,nbytes);

}




// The write system call writes data to the terminal or to a device (RTC). In the case of the terminal, all data should
// be displayed to the screen immediately. In the case of the RTC, the system call should always accept only a 4-byte
// integer specifying the interrupt rate in Hz, and should set the rate of periodic interrupts accordingly. Writes to regular
// files should always return -1 to indicate FAILUREure since the file system is read-only. The call returns the number of bytes
// written, or -1 on FAILUREure.



/* -------------------------------------------------------------------------- */


/* -------------------------------- sys_write ------------------------------- */
/*  inputs: fd --   file descriptor val
 *          buf --  the values to print to memory
 *          nbytes --   number of characters to print
 *  Return Value:   nbytes
 *                  -1: FAILURE
 */
int32_t sys_write(int32_t fd, const void* buf, int32_t nbytes)
{
// FAILURE if buffer is null or nbytes is less than 0 or fd is not with in 0-8
    if(buf == NULL || nbytes < 0 || fd < 0 || fd > HIGH_FD){
        return FAILURE;
    }
    // pidVal -= 1;
    pcb_t* cur_pcb_ptr = get_cur_pcb();
    // pidVal += 1;
    // if (cur_pcb_ptr->term_id != curr_term_ID) return FAILURE;
    if (fd >= LOW_FD && cur_pcb_ptr->file_descriptor[fd].flags == 0)  //if fd is valid and not busy
        return FAILURE;
    struct file_ops temp_ops = *cur_pcb_ptr->file_descriptor[fd].file_table_pointer;
    if (fd < LOW_FD && temp_ops.write == 0)
        return FAILURE;

    int bytes_written;
    bytes_written = cur_pcb_ptr->file_descriptor[fd].file_table_pointer->write(fd,buf,nbytes);
    
    if (bytes_written == FAILURE)
    {
        return FAILURE ;
    }
    // char* text = "391OS> ";
    // if ((char*)buf != text)
    //     {memset((void*)buf,0,nbytes);}
    return bytes_written ;
    // return cur_pcb_ptr->file_descriptor[fd].file_table_pointer->write(fd,buf,nbytes,file_descriptor[fd]);

}




// The open system call provides access to the file system. The call should find the directory entry corresponding to the
// named file, allocate an unused file descriptor, and set up any data necessary to handle the given type of file (directory,
// RTC device, or regular file). If the named file does not exist or no descriptors are free, the call returns -1.

// The close system call closes the specified file descriptor and makes it available for return from later calls to open.
// You should not allow the user to close the default descriptors (0 for input and 1 for output). Trying to close an invalid
// descriptor should result in a return value of -1; successful closes should return 0.


// Each directory entry gives a name (up to 32 characters, zero-padded, but not necessarily including a terminal EOS
// or 0-byte), a file type, and an index node number for the file. File types are 0 for a file giving user-level access to
// the real-time clock (RTC), 1 for the directory, and 2 for a regular file. The index node number is only meaningful for
// regular files and should be ignored for the RTC and directory types.




/* -------------------------------------------------------------------------- */

/* -------------------------------- sys_close ------------------------------- */
/*  inputs: fd --   file descriptor val
 *  Return Value:   0 : success
                    -1 : FAILURE --> invalid fd or fd entry is alr 0
 */
int32_t sys_close (int32_t fd)
{
    // TODO: reset flag
    // pidVal -= 1;
    pcb_t* cur_pcb_ptr = get_cur_pcb();
    if(fd < LOW_FD || fd > HIGH_FD || cur_pcb_ptr->file_descriptor[fd].flags == 0){ //if invalid fd or fd alr not busy (=0)
        return -1;
    }
    cur_pcb_ptr->file_descriptor[fd].file_position = 0 ; //set zero offset
    cur_pcb_ptr->file_descriptor[fd].file_table_pointer = 0 ; //set zero offset
    cur_pcb_ptr->file_descriptor[fd].internal_ftype = 0 ; //set zero offset
    cur_pcb_ptr->file_descriptor[fd].flags = 0 ; //set to not busy
    cur_pcb_ptr->file_descriptor[fd].name_of_file = "" ; //set zero offset

    return 0;
}


/* -------------------------------------------------------------------------- */

/* ------------------------------- sys_getargs ------------------------------ */
/*  inputs: 
 *          buf --  the values to print to memory
 *          nbytes --   number of characters to print
 *  Return Value:   0: SUCCESS
 *                  -1: FAILURE
 */
int32_t sys_getargs (uint8_t* buf, int32_t nbytes){
    int idx;

    // search for beginning of args
    for (idx = 0; idx < strlen((const char*)current_command); idx++) 
        if (current_command[idx] == ' ') 
            break;
    
    if (idx == strlen((const char*)current_command) || strlen((const char*)current_command) - idx > nbytes)
        return FAILURE;

    memcpy(buf, current_command + idx + 1, strlen((const char*)current_command) - idx);
    return 0;
}
/* -------------------------------------------------------------------------- */

/* ------------------------------- sys_vidmap ------------------------------- */
/*  inputs: screen_start -- double pointer to user
 *  Return Value: -1: for fail;  0 for success
 */
int32_t sys_vidmap (uint8_t** screen_start){
    // check if NULL and within range
    if(screen_start == NULL || (uint32_t) screen_start < ONE28_MB || (uint32_t) screen_start > ONE32_MB)
        return FAILURE;
    flush_tlb();
    *screen_start = (uint8_t*)ONE32_MB;
    // loadPageDirectory((unsigned int*)vid_map_directory); //flushes TLB
    return SUCCESS;
}
/* -------------------------------------------------------------------------- */

/* ----------------------------- sys_set_handler ---------------------------- */
/*  inputs: signum
 *          handler_address
 *  Return Value:   -1: FAILURE
 */
int32_t sys_set_handler (int32_t signum, void* handler_address){
    return FAILURE;
    
}
/* -------------------------------------------------------------------------- */

/* ------------------------------ sys_sigreturn ----------------------------- */
/*  inputs: void
 *  Return Value:   -1: FAILURE
 */
int32_t sys_sigreturn (void){
    return FAILURE;
    
}
/* -------------------------------------------------------------------------- */



/*  free_idx_fd
 *  inputs: fd -- file_descriptor_array
 *  outputs:    none
 *  Return Value:   nbytes : bytes success read
 *                  -1: FAILURE
 */
int32_t free_idx_fd(file_descriptor_t file_descriptor_array[8])
{
    int i = 2 ; 
    while (i < MAX_NUM_FILE)
    {
        if(file_descriptor_array[i].flags != 1)//if not busy
        {
            file_descriptor_array[i].flags = 1 ;//set to busy 
            break ;
        }
        i++ ;
    }
    return i ;
}

/*  get_cur_pcb
 *  inputs: void
 *  outputs:    none
 *  Return Value: the top pcb
 */
pcb_t* get_cur_pcb()
{
    return (pcb_t*)(EIGHT_MB - EIGHT_KB*(pidVal + 1));
}
/*  get_cur_pcb
 *  inputs: process_ID
 *  outputs:    none
 *  Return Value: the PCB corresponding to the passed in PID
 */
pcb_t* get_pcb(int process_ID){
    return (pcb_t*)(EIGHT_MB - EIGHT_KB*(process_ID + 1));
}

//UNUSED
void switch_to_user_mode(uint32_t saved_eip)
{
    //0x2B -- USER_DS, 0x23 -- USER_CS
    //uint32_t user_esp = ONE32_MB - sizeof(uint32_t);

}



