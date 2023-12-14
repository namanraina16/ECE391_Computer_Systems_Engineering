#include "rtc.h"

unsigned freq_arr[10] = {1024,512,256,128,64,32,16,8,4,2};
volatile int flag = 0;
/* void init_rtc(void)
 * Inputs: void
 * Return Value: void
 * Function: initialize rtc inteerupt */
void init_rtc() {
    enable_irq(RTC_IV); //rtc interrupt
    outb(0x8B, RTC_PORT);		    // select register B, and disable NMI
    char prev=inb(RTC_PORT+1);	    // read the current value of register B
    outb(0x8B, RTC_PORT);		    // set the index again (a read will reset the index to register D)
    outb(prev | 0x40, RTC_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B
}

/* int rtc_handler(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: handle the rtc interrupt */
int rtc_handler(){
    // test_interrupts();
    //This is the code taken from osDev, but make it work with our system:
    cli();			                // disable interrupts
    outb(0x0C,RTC_PORT);	        // select register C
    inb(RTC_PORT+1);	
    flag = 0;
    sti();                          // enable interrupts

    send_eoi(RTC_IV);
    return 0;
}

/* int rtc_open(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: handle the rtc interrupt */
int rtc_open(const uint8_t* filename) {
    filename = (uint8_t*)"";
    int frequency = 2;
    rtc_write(0, &frequency, 0); 
    return 0;
}

/* int rtc_close(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: handle the rtc interrupt */
int rtc_close(int32_t fd) {
    fd = -1 ;
    return 0;
}

/* int rtc_read(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: handle the rtc interrupt */
int rtc_read(int32_t fd, void* buf, int32_t nbytes, char* file_name, uint32_t offset,uint32_t inode_index) {
    file_name="";
    fd = -1;
    inode_index = -1 ;
    buf = 0;
    nbytes = -1;
    flag = 1;
    while(flag);
    // putc('1');
    return 0;
}

/* int rtc_write(void)
 * Inputs: void
 * Return Value: int, 0 if successful
 * Function: handle the rtc interrupt */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes) {
    cli();	
    unsigned rate;
    uint32_t freq = *((uint32_t*)buf);
    for (rate = 0; rate <= 10; rate++) {
        if (rate == 10) {
            sti();
            return -1;          // check if freq is valid 
        }
        if (freq_arr[rate] == freq) break;  // set rate if it is valid
    }
    rate += 6;
    rate &= 0x0F;

    		                // disable interrupts
    outb(0x8A, RTC_PORT);		    // select register B, and disable NMI
    char prev=inb(RTC_PORT+1);	    // read the current value of register B
    outb(0x8A, RTC_PORT);		    // set the index again (a read will reset the index to register D)
    outb((prev & 0xF0) | rate, RTC_PORT+1);	// write the previous value ORed with 0x40. This turns on bit 6 of register B

    sti();                          // enable interrupts
    return 0;
}
