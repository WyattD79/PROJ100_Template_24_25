#ifndef PROJ100_ENCODER_TESTS_H
#define PROJ100_ENCODER_TESTS_H
#include "mbed.h"
#include "PROJ100_Encoder.h"
#include "motor.h"
#include <chrono>
#include <cstdio>
#include <cwchar>

// Objects from main
extern PROJ100_Encoder left_encoder;
extern PROJ100_Encoder right_encoder;
extern Motor Wheel;
extern UnbufferedSerial ser;


//*************************************Simplest test***********************************************//
// Uses the encoder to measure the time between pulses using a Timer object t.
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error
void simple_test();

//*************************************Rotations test***********************************************//
// Uses the encoder to measure make the wheel complete 10 full rotations before stopping
// If you make a mark on the wheel, you should see it stop at the same angle it started at
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error
void rotations_test();

//*************************************Simplest test***********************************************//
// Uses the encoder to work out when the wheel has completed a full rotation
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error
void simplest_test();

//*************************************Average Pulse Time Test***********************************************//
// Uses the encoder to calculate the average pulse time and rotation time, as well as counting the number of pulses recieved
// Uses non-blocking PROJ100_Encoder::getPulseTimeUs function which returns the number of microseconds since the last pulse
// if one has been detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error
void ave_pulse_time_test();


//*************************************Speed test***********************************************//
// Uses the encoder information to control the speed of the wheels and display it on the terminal
void speed_test();



#endif