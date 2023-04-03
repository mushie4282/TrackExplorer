// Motors.c
// Runs on TM4C123 for CECS347 Project 2
#include "tm4c123gh6pm.h"
#include "Motors.h"

// Initialize motors to use Hardware PWM
// Right Wheel - PB6 - M0PWM0
// Left Wheel - PD1 - M1PWM1
// The signal goes Low on the LOAD value & High on the COMPx value
// Use PLL clock (16 MHz)
void Motors_Init(void){
	if((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOB) == 0)
	{ // initialize Port B clk
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;
		while((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOB) == 0){};
	}
	
	if((SYSCTL_RCGC2_R & SYSCTL_RCGC2_GPIOD) == 0)
	{// activate port D clk
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOD; 							
		while((SYSCTL_PRGPIO_R & SYSCTL_RCGC2_GPIOD) == 0){};
	}
	
	// PB6 M0PWM0
	GPIO_PORTB_AFSEL_R |= PB6; 						// enable PB6 alt. func.
	GPIO_PORTB_PCTL_R &= ~GPIO_PCTL_PB6_M; // clear M0PWM0 slot
	GPIO_PORTB_PCTL_R |= GPIO_PCTL_PB6_M0PWM0; // enable M0PWM0
	GPIO_PORTB_DR8R_R |= PB6; 						// enable 8mA drive
	GPIO_PORTB_DEN_R |= PB6;
	// Initialize PWM settings
	SYSCTL_RCGCPWM_R |= 0x03; 						// PWM Module 0 & Module 1
	SYSCTL_RCC_R = (SYSCTL_RCC_R & (~0x001E0000))|SYSCTL_RCC_USEPWMDIV;
	PWM0_0_CTL_R = 0;											// re-loading down-counting mode
	PWM0_0_GENA_R |= 0xC8; 								// low on LOAD, high on CMPA down => DUTY = COMPA
	PWM0_0_LOAD_R = PERIOD - 1;
	PWM0_0_CMPA_R = 0; 										// count value when output rises
	PWM0_0_CTL_R |= PWM_0_CTL_ENABLE;			// start the timers in PWM generator
	PWM0_ENABLE_R &= ~PWM_ENABLE_PWM0EN;	// disable to NOT produce PWM signals on initialization
    
	// Using PD1 M1PWM1
	GPIO_PORTD_AFSEL_R |= 0x02;           // enable alt funct on PD1
	GPIO_PORTD_PCTL_R &= ~0x000000F0;    	// configure PD0 as PWM0
	GPIO_PORTD_PCTL_R |= 0x00000050;
	GPIO_PORTD_DEN_R |= 0x02;             // enable digital I/O on PD1
	GPIO_PORTD_DR8R_R |= 0x02;            // 8mA
 
	PWM1_0_CTL_R = 0;                     // re-loading down-counting mode
	PWM1_0_GENB_R |= 0xC08;                 // low on LOAD, high on CMPA down
	PWM1_0_LOAD_R = PERIOD - 1;     // cycles needed to count down to 0
	PWM1_0_CMPB_R = 0;    								// count value when output rises
	PWM1_0_CTL_R |= PWM_0_CTL_ENABLE;     // set PWM0
	PWM1_ENABLE_R &= ~PWM_ENABLE_PWM1EN;  // disable PD1/M1PWM1
}

// Description: Initializes PB5432 for use with L298N motor driver direction
void Car_Dir_Init(void){
	if ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)==0) {
		SYSCTL_RCGC2_R |= SYSCTL_RCGC2_GPIOB;	// Activate B clocks
		while ((SYSCTL_RCGC2_R&SYSCTL_RCGC2_GPIOB)==0){};
	}
  GPIO_PORTB_AMSEL_R &= ~0x3C;	// disable analog function
	GPIO_PORTB_AFSEL_R &= ~0x3C;	// no alternate function
  GPIO_PORTB_PCTL_R &= ~0x00FFFF00;	// GPIO clear bit PCTL 
	GPIO_PORTB_DIR_R |= 0x3C; // output on pin(s)
  GPIO_PORTB_DEN_R |= 0x3C;	// enable digital I/O on pin(s)
}

// Dependency: PWM_Init()
// Inputs: 
//	duty_L is the value corresponding to the duty cycle of the left wheel
//	duty_R is the value corresponding to the duty cycle of the right wheel
// Outputs: None 
// Description: Changes the duty cycles of by changing the CMP registers
void PWM_PB6PD1_Duty(unsigned long duty_L, unsigned long duty_R){
	PWM0_0_CMPA_R = duty_L - 1; // PB6 count value when output rises
  PWM1_0_CMPB_R = duty_R - 1;	// PD1 count value when output rises
}

// turn right by slowing down the right motor
void turnRight(void)
{
	WHEEL_DIR = FORWARD;
	PWM_PB6PD1_Duty(PERIOD * 0.3, 0);
//	PWM0_ENABLE_R |= PWM_ENABLE_PWM0EN;
//	PWM1_ENABLE_R &= ~PWM_ENABLE_PWM1EN;
}

// turn left by slowing down the left motor
void turnLeft(void)
{
	WHEEL_DIR = FORWARD;
	PWM_PB6PD1_Duty(0, PERIOD * 0.3); 
//	PWM0_ENABLE_R &= ~PWM_ENABLE_PWM0EN;
//	PWM1_ENABLE_R |= PWM_ENABLE_PWM1EN;
}

// drive the motors at a certain duty cycle
void moveStraight(void)
{
	PWM_PB6PD1_Duty(PERIOD * 0.3, PERIOD * 0.3);
}

// enable both motors
void start(void)
{
	WHEEL_DIR = FORWARD;
	PWM0_ENABLE_R |= PWM_ENABLE_PWM0EN;
	PWM1_ENABLE_R |= PWM_ENABLE_PWM1EN;
}

// disable both motors
void stop(void)
{
	PWM0_ENABLE_R &= ~PWM_ENABLE_PWM0EN; 
	PWM1_ENABLE_R &= ~PWM_ENABLE_PWM1EN; 
}

