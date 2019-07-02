# uav-cam-trigger
Hardware:
- Arduino Uno
- LCD display shield with 6 buttons - Up, Down, Left, Right, Select, Reset

This program uses an arduino to collect flight mission parameters
from a user. It then calculates and performs the correct timing of
shutter events given the input.

This is built around a PWM triggering camera system. You may need
to adjust the output from pin 3 in loop().
