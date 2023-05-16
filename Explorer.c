// TrackExplorer.c
// Runs on TM4C123
// This is the starter file for CECS 347 Project 2 - A Track Explorer
// This project uses hardware PWM to control two DC Motors, 
// ADC to collect analog inputs from three Sharp IR sensors.
// The three Sharp analog IR distance sensors (GP2Y0A21YK0F) are used
// to allowthe robot to navigate through a track with two walls: 
// one mounted looking directly forward to avoid a head-on collision, 
// the other two looking forward to the left and to the right to detect  
// the distances between the car and the two walls. The goal is to 
// control power to each wheel so the left and right distances to the 
// walls are equal.
// If an object is detected too close to the robot, 
// the robot should be able to avoid it.
/*
    ------------------------------------------wall---------
                      /
                     /
                    / 
                   /
         -----------
         |         |
         | Robot   | ---> direction of motion and third sensor
         |         |
         -----------
                   \
                    \
                     \
                      \
    ------------------------------------------wall---------
*/
// The original project is designed by Dr. Daniel Valvano, Jonathan Valvano
// September 12, 2013
// Modifications are made by Dr. Min He.

// PE1 connected to forward facing IR distance sensor
// PE4 connected to right IR distance sensor
// PE5 connected to left IR distance sensor

#include "tm4c123gh6pm.h"
#include "Sensors.h"
#include "Motors.h"
#include "LEDSW.h"
#include "PLL.h"
#include "stdint.h"
#include "LEDSW.h"

// basic functions defined at end of startup.s
extern void DisableInterrupts(void); // Disable interrupts
extern void EnableInterrupts(void);  // Enable interrupts
extern void WaitForInterrupt(void);  // low power mode

void Delay(void){
//	unsigned long volatile  time = 727240*500/91;  // 0.25sec
	unsigned long volatile  time = 727240*200/91;
	while(time){time--;}
	}
// You use datasheet to calculate the following ADC values
// then test your sensors to adjust the values 
#define CRASH            IR15CM// if there is less than this distance ahead of the robot, it will immediately stop
#define IR15CM           2233  // ADC output for 15cm:1.8v -> (1.8/3.3)*4095=2233 
#define IR20CM           1724  // ADC output for 20cm:1.39v -> (1.39/3.3)*4095=1724
#define IR30CM           1116  // ADC output for 30cm:0.9v -> (0.9/3.3)*4095=1116
#define IR40CM           918   // ADC output for 40cm:0.74v -> (0.74/3.3)*4095=918
#define IR80CM           447   // ADC output for 80cm:0.4v -> (0.4/3.3)*4095=496
                                // with equal power to both motors (LeftH == RightH), the robot still may not drive straight
                                // due to mechanical differences in the motors, so bias the left wheel faster or slower than
                                // the constant right wheel					
#define LEFTPOWER        0.3*PERIOD   // duty cycle of left wheel 
#define RIGHTPOWER       0.3*PERIOD   // duty cycle of left wheel
	
void System_Init(void);
void steering(uint16_t ahead_dist,uint16_t right_dist, uint16_t left_dist);
uint8_t motorsFlag = 0; // motors' state flag
	
int main(void){
	uint16_t left, right, ahead; // each sensor values

	DisableInterrupts();  // disable interrupts while initializing
	System_Init(); 		  // initialize system
	EnableInterrupts();   // enable after all initialization are done

	// TODO: Calibrate the sensors: read at least 10 times from the sensor 
	// before the car starts to move: this will allow software to filter the sensor outputs.
	for(int x = 0; x < 10; x++)
	{
		ReadSensorsFIRFilter(&left, &ahead, &right);
	}

	while(1){
		// check if the motors are on or if the flag has been set before sampling
		if(motorsFlag || ((PWM0_ENABLE_R & PWM_ENABLE_PWM0EN) && (PWM1_ENABLE_R & PWM_ENABLE_PWM1EN)))
		{
			ReadSensorsFIRFilter(&left, &ahead, &right); //read IR sensors
			steering(ahead,right,left); // steer the car based on the sensor values
		}
	}
}

/**
 * @brief call functions to initialize MCU system
 * 
 */
void System_Init(void) {
	PLL_Init();            // bus clock at 16 MHz
	Sensors_Init();        // initialize ADC to sample AIN2 (PE1), AIN9 (PE4), AIN8 (PE5)
	LEDSW_Init();          // configure onboard LEDs and push buttons
	Motors_Init();         // Initialize signals for the two DC Motors
	Car_Dir_Init(); 	   // direction pin (PB2-PB5)
	PWM_PB6PD1_Duty(LEFTPOWER, RIGHTPOWER); // set starting duty cycles
}

/**
 * @brief Handles stopping and starting the track explorer using onboard swithces
 * 	SW2 - Stop track explorer
 *  SW1 - Start track explorer
 */
void GPIOPortF_Handler(void){
	// PF0 (SW2) -> stop car
	if(!(GPIO_PORTF_DATA_R & 0x01))
	{
		GPIO_PORTF_ICR_R = 0x01; // acknowledge interrupt Flag 0 
		stop(); // clear PWM enable register to stop the motors
		GPIO_PORTF_DATA_R = White;
		motorsFlag = 0; // clear flag so sensors will stop reading
	}
	// TODO: start with moving forward, LED green
	// PF4 (SW1) -> start car
	if(!(GPIO_PORTF_DATA_R & 0x10))
	{
		GPIO_PORTF_ICR_R = 0x10; // acknowledge interrupt Flag 4
		start(); // set PWM enable register to start the motors
		GPIO_PORTF_DATA_R = Green;
		motorsFlag = 1; // set flag to start reading the sensors
		Delay(); // allow a little head start within the track
	}
}

// Suggest the following simple control as starting point:
// 1. Take care of off center positions:
//    If left off center track, turn right; 
//    If right off center track, turn left
// 2. Take care of turning track: 
//    make a right turn if left opening detected;
//    make a left turn if right opening detected;
// 3. Optional: Take care of crach ahead: stop if crash ahead6
// 4.	go straight if moving in the center area
// Feel free to add more controls to fine tune your robot car.
// Make sure to take care of both wheel movements and LED display.
void steering(uint16_t ahead_dist,uint16_t right_dist, uint16_t left_dist){	
	// backup before crashing (attempt to U-turn)
	if(right_dist >= CRASH && left_dist >= CRASH && ahead_dist >= CRASH)
	{
		GPIO_PORTF_DATA_R = Red;
		WHEEL_DIR = BACKWARD;
		moveStraight();
	}
	// turn right when left sensor is close to a wall
	else if((left_dist >= IR20CM && ahead_dist >= IR20CM) || (left_dist >= IR20CM && right_dist <= IR40CM))
	{ // left & center sensors are within 20cm OR 
		// left sensor is within 20cm, but the right sensor detects opening
		turnRight(); 
	}
	// turn left when right sensor is close to a wall
	else if((right_dist >= IR20CM && ahead_dist >= IR20CM) || (right_dist >= IR20CM && left_dist <= IR40CM))
	{ // right & center sensors are within 20cm OR 
		// right sensor is within 20cm, but the left sensor detects opening
		turnLeft();
	}
	// all sensors read out of range -> track is assumed finished
	else if(right_dist <= IR80CM && left_dist <= IR80CM && ahead_dist <= IR80CM)
	{
		GPIO_PORTF_DATA_R = Blue;
		stop();
		motorsFlag = 0;
		return;
	}
	else
	{
		// car is within range
		GPIO_PORTF_DATA_R = Green;
		WHEEL_DIR = FORWARD;
		moveStraight();
	}
}
