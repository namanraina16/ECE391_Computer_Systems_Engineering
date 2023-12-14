#include "keyboard.h"

/* void init_keyboard(void)
 * Inputs: void
 * Return Value: void
 * Function: initialize keyboard interrupt and populate scan_codes table */
void init_keyboard() {
    enable_irq(KEYBOARD_IV); //keyboard interrupt

    kbb_ind = 0;
    ctrl_flag = 0;
    caps_lock_flag = 0;
    shift_flag = 0;
    alt_flag = 0;
    enter_flag = 0;
    // temp arrays that hold characters 
    char num[12] = {'1','2','3','4','5','6','7','8','9','0', '-', '='};
    char shiftnum[12]= {'!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+'};

    char firstr[12] = {'q','w','e','r','t','y','u','i','o','p', '[', ']'};
    char capfirstr[12] = {'Q','W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}'};

    char secondr[12] = {'a','s','d','f','g','h','j','k','l', ';', '\'','`'};
    char capsecondr[12] = {'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '\"', '~'};

    char thirdr[10] = {'z','x','c','v','b','n','m', ',', '.', '/'};
    char capthirdr[10] = {'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?'};


    // populate the scan_code table
    int i;
    for (i = 0; i < TOTAL_CODE; i++) { 
        if (i >= SCANCODE_ONE && i <= SCANCODE_EQUALS) scan_codes[i] = num[i - SCANCODE_ONE];
        else if (i >= SCANCODE_Q && i <= SCANCODE_ENDBKT) scan_codes[i] = firstr[i - SCANCODE_Q];
        else if (i >= SCANCODE_A && i <= SCANCODE_BTICK) scan_codes[i] = secondr[i - SCANCODE_A];
        else if (i >= SCANCODE_Z && i <= SCANCODE_SLASH) scan_codes[i] = thirdr[i - SCANCODE_Z];

        else if (i >= SCANCODE_Q + CAPS_OFFSET && i <= SCANCODE_ENDBKT + CAPS_OFFSET) scan_codes[i] = capfirstr[i-SCANCODE_Q-CAPS_OFFSET];
        else if (i >= SCANCODE_A + CAPS_OFFSET && i <= SCANCODE_BTICK + CAPS_OFFSET) scan_codes[i] = capsecondr[i-SCANCODE_A-CAPS_OFFSET];
        else if (i >= SCANCODE_Z + CAPS_OFFSET && i <= SCANCODE_SLASH + CAPS_OFFSET) scan_codes[i] = capthirdr[i-SCANCODE_Z-CAPS_OFFSET];
        
        else if (i >= SCANCODE_ONE + SHIFT_OFFSET && i <= SCANCODE_EQUALS + SHIFT_OFFSET) scan_codes[i] = shiftnum[i - SCANCODE_ONE - SHIFT_OFFSET];

        else if (i == SCANCODE_BACKSLASH) scan_codes[i] = '\\';
        else if (i == SCANCODE_BACKSLASH + CAPS_OFFSET) scan_codes[i] = '|';

        else scan_codes[i] = '\0';   
    }

}

// uint8_t t_init[3] = {0,0,0};

/* int keyboard_handler(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: handle the keyboard interrupt */
int keyboard_handler(){
    cli();
 
    uint8_t scancode = inb(KEYBOARD_PORT) & 0xFF;
    // 0xFF gives the least significant 8 bits 
    
    // check if the scancode value is legit
    if (scancode > 1) {
        if (alt_flag == 1){
            if (scancode == SCANCODE_ALT_R){
                alt_flag = 0;
            }
            else if (scancode == SCANCODE_F1){
                //implement extra terminal logic here
                send_eoi(KEYBOARD_IV);
                switch_terminal(0);
                // if (t_init[0] == 0) {
                //     t_init[0] = 1;
                //     sys_execute((uint8_t*)"shell");
                // }
            }
            else if (scancode == SCANCODE_F2){
                //implement extra terminal logic here
                send_eoi(KEYBOARD_IV);
                switch_terminal(1);
                // if (t_init[1] == 0) {
                //     t_init[1] = 1;
                //     clear();
                //     sys_execute((uint8_t*)"shell");
                // }
            }
            else if (scancode == SCANCODE_F3){
                //implement thrid terminal logic here
                send_eoi(KEYBOARD_IV);
                switch_terminal(2);
                // if (t_init[2] == 0) {
                //     t_init[2] = 1;
                //     clear();
                //     sys_execute((uint8_t*)"shell");
                // }
            }
        }
        // if (schedule_term_ID != curr_term_ID) update_mapping(0); 
        if(ctrl_flag == 1){
            if (scancode == SCANCODE_CTRL_R){
                ctrl_flag = 0;
            }
            if (scancode == SCANCODE_L){
                clear();
                memset(keyb_buff, 0, KEYBOARD_BUFFER_SIZE);
                enter_flag = 1;
            }
            else if (scancode == SCANCODE_S){
                if (scheduling_flag == 0){
                    scheduling_flag = 1;}
                else{
                    scheduling_flag = 0;
                }
            }
            else if (scancode == SCANCODE_C){
                send_eoi(KEYBOARD_IV);
                sys_halt(0);
            }
        }
        else if (((scancode >= SCANCODE_ONE) && (scancode <= SCANCODE_EQUALS))){ //if it is in top row
            putc(scan_codes[scancode + (shift_flag * SHIFT_OFFSET)]);
            keyb_buff[kbb_ind] = scan_codes[scancode + (shift_flag * SHIFT_OFFSET)];
            kbb_ind += 1;
        }
        else if ((((scancode >= SCANCODE_Z) && (scancode <= SCANCODE_SLASH)) ||  //if it is a char
            ((scancode >= SCANCODE_A) && (scancode <= SCANCODE_BTICK)) ||
            ((scancode >= SCANCODE_Q) && (scancode <= SCANCODE_ENDBKT))) && kbb_ind < 127) 
            { 
            putc(scan_codes[scancode + ((caps_lock_flag ^ shift_flag) * CAPS_OFFSET)]);
            keyb_buff[kbb_ind] = scan_codes[scancode + ((caps_lock_flag ^ shift_flag) * CAPS_OFFSET)];
            kbb_ind += 1;
        }
        else if (scancode == SCANCODE_BACKSLASH){ //if backslash
            putc(scan_codes[scancode + ((caps_lock_flag ^ shift_flag) * CAPS_OFFSET)]);
            keyb_buff[kbb_ind] = scan_codes[scancode + ((caps_lock_flag ^ shift_flag) * CAPS_OFFSET)];
            kbb_ind += 1;
        }
        else if (scancode == SCANCODE_ENTER_PRESS){ //upon enter, clear the key_buffer and tell terminal_read enter was pressed
            putc('\n');
            keyb_buff[kbb_ind] = '\n';
            keyb_buff[kbb_ind + 1] = '\0';
            enter_flag = 1;
            //terminal_read(0, screen_buf, kbb_ind, 0, 0);
            //terminal_write(1, keyb_buff, kbb_ind);
            kbb_ind = 0;
        }
        else if (scancode == SCANCODE_BACKSPACE){ //backspace
            //print '\0' in the spot right before
            if (kbb_ind >= 1) {
                if (keyb_buff[kbb_ind - 1] == '\t'){
                    putc('\0'); //delete the four spaces if tab
                    putc('\0');
                    putc('\0');
                    putc('\0');
                }
                else{
                    putc('\0'); //delete just one space if tab
                }
                kbb_ind -= 1;
            }
        }
        else if (scancode == SCANCODE_SPACEBAR && kbb_ind < 127){ //spacebar
            putc(' ');
            keyb_buff[kbb_ind] = ' ';
            kbb_ind += 1;
        }
        else if (scancode == SCANCODE_TAB && kbb_ind < 127){ //tab
            putc(' ');
            putc(' ');
            putc(' ');
            putc(' ');
            keyb_buff[kbb_ind] = '\t';
            kbb_ind += 1;
        }
        else if (scancode == SCANCODE_CTRL){ //SHIFT and CTRL
            ctrl_flag = 1;
        }
        else if (scancode == SCANCODE_LSHIFT || scancode == SCANCODE_RSHIFT){
            shift_flag = 1;
        }
        else if (scancode == SCANCODE_LSHIFT_R || scancode == SCANCODE_RSHIFT_R){
            shift_flag = 0;
        }
        else if (scancode == SCANCODE_ALT){
            alt_flag = 1;
        }
        
        else if (scancode == SCANCODE_CAPSLOCK){
            if (caps_lock_flag == 1){
                caps_lock_flag = 0;
            }
            else{
                caps_lock_flag = 1;
            }
        }
        // if (schedule_term_ID != curr_term_ID) update_mapping(schedule_term_ID+1);
    }


    // send end of interrupt
    send_eoi(KEYBOARD_IV);
    sti();
    return 0;
}

/* void keyboard_handler(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: Print the beginning thing at each line */
void begString(void){
    const char * welcome = "leapOS> ";
    terminal_write(1, welcome, 8);
}
