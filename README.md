# Small Autonomous Track Explorer
Our team created a small autonomous car that can explorer a closeed track. The car was built using a two DC motor chassis with one castor wheel from Amazon. The MCU used hardware PWM to control the direction and speed of the car based on the sensors' ADC reading of how close an incoming wall is.   

The car will start at an inactive state, then once activated, the IR sensors will do ten consecutive reads for any obstacles that might be around it. Then the car will move around the track avoiding any walls until it finds the middle and follows the path accordingly. After reaching the end, it will turn back until the beginning of the track.

## Demo Video
https://youtube.com/shorts/XE_INLto_ww?feature=share

## Components
    1. Texas Intrument TM4C123 Tiva Launch Pad
    2. 2 DC motors
    3. 3 Sharp IR sensors
    4. L298N (Motor Driver)
    5. 12V battery pack