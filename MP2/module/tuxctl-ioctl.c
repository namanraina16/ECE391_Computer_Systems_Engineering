/* tuxctl-ioctl.c
 *
 * Driver (skeleton) for the mp2 tuxcontrollers for ECE391 at UIUC.
 *
 * Mark Murphy 2006
 * Andrew Ofisher 2007
 * Steve Lumetta 12-13 Sep 2009
 * Puskar Naha 2013
 */

#include <asm/current.h>
#include <asm/uaccess.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/file.h>
#include <linux/miscdevice.h>
#include <linux/kdev_t.h>
#include <linux/tty.h>
#include <linux/spinlock.h>

#include "tuxctl-ld.h"
#include "tuxctl-ioctl.h"
#include "mtcp.h"

#define debug(str, ...) \
	//printk(KERN_DEBUG "%s: " str, __FUNCTION__, ## __VA_ARGS__)


/*Previous LED State*/
volatile int previous_LED ;

/*This is a global struct that holds info for the current button state*/
struct BTN 
{
	int up,down,left,right ; //direction buttons
	int start,a,b,c ; // other buttons
};
typedef struct BTN BTN ;

/*Init Global Dtypes*/
static BTN TUX_BUTTON ; 
int temp_kernel,result;
int ct = 0;
int ack_flag ; 


/************************ Protocol Implementation *************************/

/*An array of bitmasks for each digit on display_LED*/
static unsigned char hex_formats[16] = {0xF7,0x16,0xDB,0x9F,0x3E,0xBD,0xFD,0x96,0xFF,0xBE,0xFE,0x7d,0xF1,0x5F,0xF9,0xF8} ; 
/*[0,1,2,3,4,5,6,7,8,9,A,B,C,D,E,F]*/



/*
 * set_led_tux
 *   DESCRIPTION: Does formatting and bit masking of 32 bit integer input to set the
 * 				  LEDs to print the hexadecimal present in the lower 16-bits.
 *   INPUTS: LED -- The 32 bit integer, I made it of type int although it is supposed 
 * 					to be a long. This holds info for the hex value, LEDs that are to 
 * 					be on and what decimal points are to be on.
 *           tty -- Tux object that is used to ensure we make the correct call using 
 * 					ldisc_put. 
 *   OUTPUTS: Prints nothing since this is in kernel space.
 *   RETURN VALUE: Returns 0 on success otherwise -EINVAL. 
 *   SIDE EFFECTS: Sends byte data to tux controller.
 */
int set_led_tux(int LED, struct tty_struct* tty)
{	
	int i,index,on,hex,dp ; //init vars
	char buffer[6];
	unsigned char arg ;
	previous_LED = LED ; //store previous state in global
	hex = LED & 0xFFFF ; //bit masking to access the hexadecimal, LED 4 bits and D.P bits
	on = (LED>>16) & 0xFF ; 
	dp = (LED>>24)  & 0xF ; 
	buffer[0] = MTCP_LED_SET ; //set opcode call to tux
	buffer[1] = 0xF ; //set all LEDs to on - send 0x00 for LED OFF
	for (i = 2 ; i < 6 ; i++) //loop 4 times
	{
		if ((on & 0x1) == 1) //if LED ON
		{
			index = hex & 0xF ; 
			arg = hex_formats[index] ; 
			if ((dp & 0x1) == 0) //if D.P off for specific LED ON
			{
				arg = arg ^ 0x10 ; //Set D.P to 0
			}
				buffer[i] = arg ; //Put to buffer
		}
		else //if LED OFF
		{	
			if ((dp & 0x1) == 1) //if D.P ON regardless of LED State
			{
				buffer[i] = 0x10 ; 
			}
			else //if D.P OFF make sure all is 0
			{
				buffer[i] = 0x00 ;
			}
		}
		on = on>>1 ; //shift LED bits once
		dp = dp>>1; //shift D.P once
		hex = hex>>4 ; //to access next hex you need to skip 4 bits
	}
	if (tuxctl_ldisc_put(tty,buffer,6) != 0) // check for invalid ldisc put
	{
		return -EINVAL ;
	}
	tuxctl_ldisc_put(tty,buffer,6) ;
	return 0 ; // success
}

/*
 * set_init_tux
 *   DESCRIPTION: Initializes the tux to the starting state by sending button interrupt 
 * 				  enable and set LED user mode to the tux controller. Then set all LEDs to
 * 				  the off state. Initialize start state to 0.
 *   INPUTS: 	  tty -- Tux object that is used to ensure we make the correct call using 
 * 						 ldisc_put. 
 *   OUTPUTS: Prints nothing since this is in kernel space.
 *   RETURN VALUE: Returns 0 on success otherwise -EINVAL. 
 *   SIDE EFFECTS: Sends initialization byte data to tux controller.
 */
int init_tux(struct tty_struct* tty) 
{
	char input_buffer[8] ;
	ack_flag = 0 ; //set ack flag to enable
	input_buffer[0] = MTCP_BIOC_ON; 
	input_buffer[1] = MTCP_LED_USR; //send opcodes
	input_buffer[2] = MTCP_LED_SET ;
	input_buffer[3] = 0xF ; //set LEDs to OFF
	input_buffer[4] = 0x00 ;
	input_buffer[5] = 0x00 ;
	input_buffer[6] = 0x00 ;
	input_buffer[7] = 0x00 ;
	TUX_BUTTON.start = 0; //start button to off
	if (tuxctl_ldisc_put(tty,input_buffer,8) != 0) // check for invalid ldisc put
	{
		return -EINVAL ;
	}
	tuxctl_ldisc_put(tty, input_buffer, 8) ;
	return 0 ;
}



/*
 * tuxctl_handle_packet
 *   DESCRIPTION: Driver handles packets (3 8-byte packets) coming from TUX controller.
 * 				  1st packet is the opcode which we interpret and 2nd/3rd packet may or 
 * 				  may not hold data.
 *   INPUTS: 	  tty -- Tux object that is used to ensure we make the correct call using 
 * 						 set_led_tux. 
 * 				  packet -- Pointer to array of 3 char packets sent from controller to driver.
 *   OUTPUTS: Prints nothing since this is in kernel space.
 *   RETURN VALUE: Void function returns NONE. 
 *   SIDE EFFECTS: Calls other functions based on opcode packet. Assume packets are correct.
 */
void tuxctl_handle_packet (struct tty_struct* tty, unsigned char* packet)
{	
		
		unsigned a, b, c ;
		a = packet[0]; /* Avoid printk() sign extending the 8-bit */
		b = packet[1]; /* values when printing them. */
		c = packet[2];
		if (a == MTCP_BIOC_EVENT) //if button event
		{
			TUX_BUTTON.start = (b & 0x1) ; //save all buttons to global
			TUX_BUTTON.a = ((b>>1) & 0x1) ;
			TUX_BUTTON.b = ((b>>2) & 0x1);
			TUX_BUTTON.c = ((b>>3) & 0x1) ;
			TUX_BUTTON.up = (c & 0x1) ;
			TUX_BUTTON.left= ((c>>1) & 0x1) ;
			TUX_BUTTON.down = ((c>>2) & 0x1) ;
			TUX_BUTTON.right = ((c>>3) & 0x1) ;
		}
		else if (a == MTCP_RESET) //if reset called
		{	
			/*Initialize tux and set LEDs to previous state*/
			(void) init_tux(tty) ;
			(void) set_led_tux(previous_LED,tty); 
		}
		else if (a == MTCP_ACK) //if ack happens
		{
			ack_flag = 0 ; //enable flag
		}
		
}


/******** IMPORTANT NOTE: READ THIS BEFORE IMPLEMENTING THE IOCTLS ************
 *                                                                            *
 * The ioctls should not spend any time waiting for responses to the commands *
 * they send to the controller. The data is sent over the serial line at      *
 * 9600 BAUD. At this rate, a byte takes approximately 1 millisecond to       *
 * transmit; this means that there will be about 9 milliseconds between       *
 * the time you request that the low-level serial driver send the             *
 * 6-byte SET_LEDS packet and the time the 3-byte ACK packet finishes         *
 * arriving. This is far too long a time for a system call to take. The       *
 * ioctls should return immediately with success if their parameters are      *
 * valid.                                                                     *
 *                                                                            *
 ******************************************************************************/


/*
 * buttons_tux
 *   DESCRIPTION: Initializes a pointer in the kernel space and formats button data from 
 * 				  global struct BTN into this int. Performs a copy to user call to copy 	
 * 				  the data at pointer in kernel space to the inputted userspace pointer. 
 *   INPUTS: 	  button -- Pointer to int in userspace that will hold the current states
 * 							of the buttons.
 *   OUTPUTS: Prints nothing since this is in kernel space.
 *   RETURN VALUE: Returns 0 on success otherwise -EINVAL. 
 *   SIDE EFFECTS: Calls copy_to_user so make sure to check for edge cases. 
 */
int buttons_tux (int* button) 
{
	if (button == NULL) //invalid button pointer then return -EINVAL
	{
		return -EINVAL ;
	}
	else 
	{	
		*button = 0 ; //initialize
		result = 0 ;
		temp_kernel = 0 ;
		temp_kernel += TUX_BUTTON.start ; 
		temp_kernel += (TUX_BUTTON.a)<<1 ;
		temp_kernel += (TUX_BUTTON.b)<<2 ;
		temp_kernel += (TUX_BUTTON.c)<<3  ; 
		temp_kernel += (TUX_BUTTON.up)<<4 ;
		temp_kernel += (TUX_BUTTON.down)<<5  ;
		temp_kernel += (TUX_BUTTON.left)<<6 ;
		temp_kernel += (TUX_BUTTON.right)<<7 ;
		result = copy_to_user(button,&temp_kernel,1) ; //copy from kernel to user
		
		if (result != 0) //invalid copy
		{
			return -EINVAL ; 
		}
	}
	return 0 ; //success
}

/*
 * tuxctl_ioctl
 *   DESCRIPTION: Dispatcher function that handles calls using the cmd input. Depending on cmd it jumps
 * 				  to appropriate function. 
 *   INPUTS: 	  tty -- Object struct for TUX.
 * 				  file -- This is "fd" which we open for "reading" (port opened for reading)
 * 			      cmd -- Allows us to jump to the appropriate function.
 *   OUTPUTS: Prints nothing since this is in kernel space.
 *   RETURN VALUE: Returns 0 on success otherwise -EINVAL for unused functions. 
 *   SIDE EFFECTS: May change the acknowledge flag.
 */
int tuxctl_ioctl (struct tty_struct* tty, struct file* file, 
	      unsigned cmd, unsigned long arg)
{
    switch (cmd) {
	case TUX_INIT:
		init_tux(tty); //break after each successful function call to return 0
		break; 
	case TUX_BUTTONS:
		buttons_tux ((int*) arg); 
		break ;
	case TUX_SET_LED:
		 if (ack_flag == 0) // only jump if ack_flag is enabled
		 {
			ack_flag = 1 ; //immediately set to 1
			set_led_tux((int) arg, tty) ;
		 }
		break ;
	case TUX_LED_ACK: 
		return -EINVAL ;
	case TUX_LED_REQUEST: 
		return -EINVAL ;
	case TUX_READ_LED: 
		return -EINVAL ;
	default:
	    return -EINVAL;
    }
	return 0 ;
}




	


