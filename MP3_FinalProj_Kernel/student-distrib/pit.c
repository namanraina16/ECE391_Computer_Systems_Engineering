#include "pit.h"
#include "paging.h"

extern void linkage_sched(uint32_t ebp_);
uint8_t t_init[3] = {0,0,0};

/* void init_pit
 * Inputs: None
 * Return Value: None
 * Function: Does some enabling and creates the divisor for the PIT
 * */
void init_pit() {
    enable_irq(PIT_IV);
    int divisor = 1193180 / DEFAULT_HZ;       
    outb(0x36, PIT_CMD);             
    outb(divisor & 0xFF, PIT_PORT);   
    outb(divisor >> 8, PIT_PORT);  
}
/* void pit_handler()
 * Inputs: None
 * Return Value: None
 * Function: Does some enabling and creates the divisor for the PIT
 * */
int pit_handler() {
     if (scheduling_flag == 0)
    {
        if (t_init[schedule_term_ID] == 0) {
            t_init[schedule_term_ID]++;
            switch_terminal(schedule_term_ID);
            schedule_term_ID = (schedule_term_ID+1)%3;
            clear();
            send_eoi(PIT_IV);
            sys_execute((uint8_t*)"shell");
            return 0;
        } else {
            cli();
            send_eoi(PIT_IV);
            scheduler();
            sti();
        }
    } else send_eoi(PIT_IV);
    return 0;

}

void scheduler() 
{
    if (t_init[0] == 1) {
        t_init[0]++;
        /* for non scheduling*/
        switch_terminal(0);
        scheduling_flag = 1;
        return;
    }
}
