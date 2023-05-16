/**
 * @file LEDSW.c
 * @author Michelle Tran, Grecia Francisco
 * @brief Initialized onboard RGB LED (Port F)
 */
#include "LEDSW.h"

// use the on-board LEDs
void LEDSW_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGC2_GPIOF;    // 2) activate port F
	SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOF; 
	delay = SYSCTL_RCGCGPIO_R;            			// allow time to finish activating
	GPIO_PORTF_LOCK_R = GPIO_LOCK_KEY;
	GPIO_PORTF_CR_R = All_Port_F; 							// unlock PF0
	GPIO_PORTF_PCTL_R &= ~All_Port_F; 					// clear PCTL to select GPIO
	GPIO_PORTF_AFSEL_R &= ~All_Port_F;          // no alternate functions on PF0,1,2,3,4
	GPIO_PORTF_AMSEL_R &= ~All_Port_F;         	// disable analog functionality on PF0,1,2,3,4
	GPIO_PORTF_DIR_R &= ~0x11; 				          // set PF0 & PF4 as input
	GPIO_PORTF_DIR_R |= 0x0E; 									// set PF1, PF2, PF3 as output
	GPIO_PORTF_PUR_R |= 0x11; 									// enable pull-up on PF0 & PF4
	GPIO_PORTF_DEN_R |= All_Port_F;           	// enable digital I/O on PF1, PF2

	// Edge Interrupt to start/stop motors (fall edge trigger, interrupt)
	GPIO_PORTF_IS_R &= ~0x11; // PF4 & PF0 is edge-sensitive
	GPIO_PORTF_IBE_R &= ~0x11; // PF4 & PF0 is not both edges
	GPIO_PORTF_IEV_R &= ~0x11; // PF4 & PF0 falling edge event
	GPIO_PORTF_ICR_R = 0x11; // clear flag 0 & flag 4
	GPIO_PORTF_IM_R |= 0x11; // arm interrupt on PF4
	NVIC_PRI7_R &= ~0x00F00000; // clear bits 23-21
	NVIC_PRI7_R |= (0x00 << 20); // set priority 0 
	NVIC_EN0_R |= (0x01 << 30);
}
