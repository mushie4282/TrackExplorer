/**
 * @file LEDSW.h
 * @author Michelle Tran, Grecia Francisco
 * @brief The onboard LED is used as an indicator
 *  1. blue - track explorer finishes maze
 *  2. red - track explorer is about to crash
 *  3. green - track explorer is safe
 */

#ifndef LED_SW_H
#define LED_SW_H

#include "tm4c123gh6pm.h"

#define All_Port_F  0x1F
#define Dark    	0x00
#define Red     	0x02
#define Blue    	0x04
#define Green   	0x08
#define Yellow  	0x0A
#define Cran        0x0C
#define White   	0x0E
#define Purple  	0x06

void LEDSW_Init(void);
#endif