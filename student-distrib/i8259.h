/* i8259.h - Defines used in interactions with the 8259 interrupt
 * controller
 * vim:ts=4 noexpandtab
 */

#ifndef _I8259_H
#define _I8259_H

#include "types.h"

//char *pic_ints[] = {"timer chip", "keyboard", "(cascade)", "IRQ3", "serial port", "IRQ5", "IRQ6", "IRQ7", "IRQ8: RTC", "IRQ9", "IRQ10", "IRQ11: etho0", "IRQ12: PS/2 mouse", "IRQ13", "IRQ14 - ide0(hard drive)", "IRQ15"};


/* Ports that each PIC sits on */
#define MASTER_8259_PORT    0x20
#define SLAVE_8259_PORT     0xA0
#define MASTER_COMMAND      MASTER_8259_PORT
#define SLAVE_COMMAND       SLAVE_8259_PORT
#define MASTER_DATA         (MASTER_8259_PORT + 1)
#define SLAVE_DATA          (SLAVE_8259_PORT + 1)

/* Initialization control words to init each PIC.
 * See the Intel manuals for details on the meaning
 * of each word */
/*
ICW1 -- initialization
ICW2 -- its vector offset
ICW3 -- tell how it is wired to master/slaves
ICW4 -- gives additional info about the environment
*/


#define ICW1                0x11 //initialize command
#define ICW2_MASTER         0x20
#define ICW2_SLAVE          0x28
#define ICW3_MASTER         0x04
#define ICW3_SLAVE          0x02
#define ICW4                0x01
#define ICW1_ICW4           0x01
#define SEC_PIC_OFFSET      0x8
/* End-of-interrupt byte.  This gets OR'd with
 * the interrupt number and sent out to the PIC
 * to declare the interrupt finished */
#define EOI                 0x60

/* Externally-visible functions */

/* Initialize both PICs */
void i8259_init(void);
/* Enable (unmask) the specified IRQ */
void enable_irq(uint32_t irq_num);
/* Disable (mask) the specified IRQ */
void disable_irq(uint32_t irq_num);
/* Send end-of-interrupt signal for the specified IRQ */
void send_eoi(uint32_t irq_num);

#endif /* _I8259_H */
