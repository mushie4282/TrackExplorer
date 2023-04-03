// Motors.h
// Runs on TM4C123 for CECS347 Project 2

#define PB6 0x40	// LEFT motor 0100_0000
#define PERIOD 16000 //  16MHz/1000 = 16000
#define WHEEL_DIR (*((volatile unsigned long *)0x400050F0)) // PB5432 are the four direction pins for L298
#define FORWARD 0x28
#define BACKWARD 0x14

// configure the system to get its clock from the PLL
void Motors_Init(void);
// control L298N motor drivers
void Car_Dir_Init(void);
// adjust the duty cycle of the motors
void PWM_PB6PD1_Duty(unsigned long duty_L, unsigned long duty_R);

void start(void); 
void turnRight(void); 
void stop(void); 
void turnLeft(void); 
void moveStraight(void);
