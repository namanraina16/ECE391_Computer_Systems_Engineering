/* i8259.c - Functions to interact with the 8259 interrupt controller
 * vim:ts=4 noexpandtab
 */

#include "i8259.h"
#include "lib.h"

/* Interrupt masks to determine which interrupts are enabled and disabled */
uint8_t master_mask; /* IRQs 0-7  */
uint8_t slave_mask;  /* IRQs 8-15 */

/*
For masking -- 1 = disabled/masked, 0 = enabled/unmasked
*/

/* void i8259_init(void)
 * Inputs: void
 * Return Value: void
 * Function: initializethe PIC, with master and slave PICs */
void i8259_init(void) {
    /*
     * Start PIC in cascade mode
     * Master offset
     * Slave offset
     * Tell master about the slave
     * Tell slave its identity
     * Set mode for pics
     * enable_irq(#)
     */

    outb(ICW1, MASTER_COMMAND); //initialization sequence
    outb(ICW1, SLAVE_COMMAND);
    outb(ICW2_MASTER, MASTER_DATA); //Master PIC vector offset
    outb(ICW2_SLAVE, SLAVE_DATA); //Slave PIC vector offset
    outb(ICW3_MASTER, MASTER_DATA); //telling master that slave exists -- at IRQ2
    outb(ICW3_SLAVE, SLAVE_DATA); //telling slave that its cascade identity (2)
    outb(ICW4, MASTER_DATA);
    outb(ICW4, SLAVE_DATA);

    master_mask = 0xFB; //1111 1011 --irq2 is disabled since that is where the PIC is cascaded
    slave_mask = 0xFF;

    outb(master_mask, MASTER_DATA);
    outb(slave_mask, SLAVE_DATA); //push the masks to the respective ports
}

/* void disable_irq(uint32_t irq_num)
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num) {
    if (irq_num >= SEC_PIC_OFFSET){
        slave_mask |= (1 << (irq_num - SEC_PIC_OFFSET));
        outb( slave_mask, SLAVE_DATA); //send the new mask to slave PIC
    }
    else{
        master_mask |= (1 << irq_num);
        outb( master_mask, MASTER_DATA); //send the new mask to master PIC
    }
    
}

/* void enable_irq(uint32_t irq_num)
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num) {
    if (irq_num >= SEC_PIC_OFFSET){
        slave_mask &= ~(1 << (irq_num-SEC_PIC_OFFSET)); //clears that bit on the mask
        outb( slave_mask, SLAVE_DATA);
    }
    else{
        master_mask &= ~(1 << irq_num);
        outb( master_mask, MASTER_DATA);
    }
    printf("IRQ Num: %d", irq_num);  //idt -> i8259 -> rtc
}

/* void send_eoi(uint32_t irq_num)
 * Inputs: uint32_t irq_num
 * Return Value: void
 * Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num) {
    if (irq_num >= SEC_PIC_OFFSET){
        outb( (EOI | (irq_num - SEC_PIC_OFFSET)), SLAVE_COMMAND);
        //MIGHT NOT NEED THIS LINE
        outb((EOI | 0x2), MASTER_COMMAND);
    } else 
        outb( (EOI | irq_num), MASTER_COMMAND);
}
