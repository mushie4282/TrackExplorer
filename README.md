
# Small Autonomous Track Explorer

This repository contains the code for a small autonomous car that can explore a track using IR sensors and a TM4C123 microcontroller. 
The car was made from a variety of materials, which included a small car chassis, two DC motors, a 12V battery pack, an L298N motor 
driver, and three Sharp IR sensors. The TM4C123 microcontroller used PWM to control the car's movement and ADC to process the data from the IR sensors.

The robotic car will start at an inactive state, then once activated, the IR sensors will do ten consecutive reads for any obstacles 
that might be around it. Then the car will move around the track avoiding any walls until it finds the middle and follows the path 
accordingly. After reaching the end, it will turn back until the beginning of the track.

## Demo Video

https://youtube.com/shorts/XE_INLto_ww?feature=share


## Features

- ADC
- Hardware Pulse Width Modulation

