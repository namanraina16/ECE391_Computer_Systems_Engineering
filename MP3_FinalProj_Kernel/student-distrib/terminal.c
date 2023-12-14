#include "terminal.h"
#define NULL 0
#define BUFFER_SIZE 128

char screen_buf[BUFFER_SIZE]; //re-declaration from header_file


/*  terminal_read
 *  inputs: fd --   file descriptor val
 *          buf --  the values to print to memory
 *          nbytes --   number of characters to print
 *  outputs:    
 *  Return Value: # of bytes to read
 */
int terminal_read(int32_t fd, void* buf, int nbytes, char* file_name, uint32_t offset,uint32_t inode_index){
// fail if buffer is null or nbytes is less than 0
    if(buf == NULL || nbytes < 0){
        return -1;                   // fail
    }
    inode_index = - 1;
    start_term = curr_term_ID;
    while(enter_flag == 0){}
    
    int i = 0;
    for (i = 0; i < nbytes; i++){
        if (keyb_buff[i] == '\0'){
            break; //if we hit a null character
        }
        *((char *)buf+i) = keyb_buff[i]; //copies from the passed in keyboard buffer to screen_buf 

    }
    int numBytesRead = i;

    //this updates the term buffer array with the buffer corresponding to this terminal
    //terminals[curr_term_ID]->sbuf = (char *) buf;
    //memcpy(terminals[start_term]->sbuf, (char *) buf, numBytesRead);

    //reset the keyboard buffer & enter flag
    memset(keyb_buff, 0, KEYBOARD_BUFFER_SIZE);
    enter_flag = 0;

    return numBytesRead;

}
/*  terminal_write
 *  inputs: fd --   file descriptor val
 *          buf --  the values to print to memory
 *          nbytes --   number of characters to print
 *  outputs:    write on screen
 *  Return Value: # of bytes written for success; -1 for fail
 */
int terminal_write(int fd, const void* buf, int nbytes){
    // fail if buffer is null or nbytes is less than 0
    //update_cursor(terminals[curr_term_ID].cursor % NUM_COLS, terminals[curr_term_ID].cursor / NUM_COLS);
    
    if(buf == NULL || nbytes < 0){
        return -1;                   // fail
    }
    start_term = curr_term_ID;
    int s = curr_term_ID;
    int i;
    for(i = 0; i < nbytes; i++){
        //until we hit the null character in our buffer - indicating the end of our string
        if (curr_term_ID == s){ //if the terminal we are in is the one the terminal_write was executed in -- put it to video memory
            if (*((char *)buf+i) != '\0'){
                putc(*((char *)buf+i));
            }
        }
        else{ //if not put it to the non video mem in background
            background_putc(*((char *)buf+i), s);
        }
       
    }
    return nbytes;


}
/*  terminal_open
 *  inputs: filename -- name of file
 *  outputs:    
 *  Return Value: 0 for success; -1 for fail
 */
int terminal_open(const uint8_t* filename){
    return 0;

}
/*  terminal_close
 *  inputs: fd --   file descriptor val
 *  outputs:    
 *  Return Value: 0 for success; -1 for fail
 */
int terminal_close(int32_t fd){

    return 0;

}
/*  terminal_init
 *  inputs:
 *  outputs:    
 *  Return Value: 0 for success; -1 for fail
 */
int terminal_init(void){
    curr_term_ID = 0;
    schedule_term_ID = 0;

    term_t t1;
    term_t t2;
    term_t t3;
    t1.addr = VIDEO_LOC + 1 * four_kB;
    t2.addr = VIDEO_LOC + 2 * four_kB;
    t3.addr = VIDEO_LOC + 3 * four_kB;

    memset(t1.sbuf, 0, BUFFER_SIZE);
    memset(t2.sbuf, 0, BUFFER_SIZE);
    memset(t3.sbuf, 0, BUFFER_SIZE);

    t1.cursor = 0;
    t2.cursor = 0;
    t3.cursor = 0;

    memset(t1.processes, 0, 6); t1.processes[0] = -1;
    memset(t2.processes, 0, 6); t2.processes[0] = -1;
    memset(t3.processes, 0, 6); t3.processes[0] = -1;

    t1.proc_cnt = 0;
    t2.proc_cnt = 0;
    t3.proc_cnt = 0;

    terminals[0] = t1;
    terminals[1] = t2;
    terminals[2] = t3;

    return 0;
}




