#include "mbed.h"
#include "PROJ100_Encoder.h"
#include "PROJ100_Encoder_Tests.h"
#include "motor.h"


//*************************************Simplest test***********************************************//
// Uses the encoder to work out when the wheel has completed a full rotation
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error

void simplest_test(){
    // Variables and objects
    DigitalOut led(LED1);       // We will toggle an LED every time we get a pulse               
    
    unsigned int pulse_counter = 0;        // Variable to hold a count of the number of pulses received
    unsigned int rotation_counter = 0;

    // Apply power to the motors
    Wheel.Speed(0.6f,0.6f);

    // This loops runs forever
    while(1){
        // Check to see if we have received a new pulse
        if(left_encoder.pulseReceived()>0){
            // If so, increment the pulse counter
            pulse_counter++;
            // Check to see if we have completed a full rotation
            if(pulse_counter>=left_encoder.getPulsesPerRotation()){
                // If so...
                rotation_counter++;     // Incremenmt rotation counter
                pulse_counter = 0;      // Reset pulse counter
                led=!led;               // Indicate a rotation by toggling LED

                printf("Rotation Number %d Complete\n",rotation_counter);  // Print a message
            }
        }
    }
}

//*************************************Simple test***********************************************//
// Uses the encoder to measure the time between pulses using a Timer object t.
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error

void simple_test(){
    // Variables and objects
    DigitalOut led(LED1);       // We will toggle an LED every time we get a pulse
    Timer t;                    // We will use a timer to work out how much time between pulses
    t.start();                  
    uint8_t counter = 0;        // Variable to hold a count of the number of pulses received

    // Apply power to the left motors
    Wheel.Speed(0.6f,0.6f);

    // This loops runs forever
    while(1){
        // Check to see if we have received a new pulse
        if(left_encoder.pulseReceived()>0){
            // If so, read the pulse time from the timer
            // This line is ugly but it just reads the timer and converts the time to milliseconds
            uint32_t ms = std::chrono::duration_cast<std::chrono::milliseconds>(t.elapsed_time()).count();
            
            // Reset the timer
            t.reset();

            // Toggle the LED
            led=!led;

            // Only print every 10 pulses
            counter++;
            if(counter % 10 == 0 ){
                printf("Pulse time %dms, Pulses received %d\r\n",ms,counter);   // Print the pulse time
            }
        }
    }
}


//*************************************Rotations test***********************************************//
// Uses the encoder to measure make the wheel complete 10 full rotations before stopping
// If you make a mark on the wheel, you should see it stop at the same angle it started at
// Uses non-blocking PROJ100_Encoder::pulseReceived() function which returns 1 if a pulse has been
// detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error


void rotations_test(){
    // Variables and objects
    
    unsigned int pulse_counter = 0;        // Variable to hold a count of the number of pulses received
    unsigned int rotation_counter = 0;

    // Apply power to the motors
    Wheel.Speed(0.6f,0.6f);

    // This loops runs forever
    while(1){
        // Check to see if we have received a new pulse
        if(left_encoder.pulseReceived()>0){
            // If so, increment the pulse counter
            pulse_counter++;
            // Check to see if we have completed a full rotation
            if(pulse_counter>=left_encoder.getPulsesPerRotation()){
                // If so...
                rotation_counter++;     // Incremenmt rotation counter
                pulse_counter = 0;      // Reset pulse counter

                if(rotation_counter==10){
                    Wheel.Stop();
                    while(1);
                }

                printf("Rotation Number %d Complete\n",rotation_counter);  // Print a message
            }
        }
    }
}


//*************************************Average Pulse Time Test***********************************************//
// Uses the encoder to calculate the average pulse time and rotation time, as well as counting the number of pulses recieved
// Uses non-blocking PROJ100_Encoder::getPulseTimeUs function which returns the number of microseconds since the last pulse
// if one has been detected since the last call, 0 if no pulse has been detected since the last call, -1 if the 
// wheel is stationary and -2 if there is an error

void ave_pulse_time_test(){
    // We will print the current and target speed periodically using a timer
    int pulse_counter = 0;
    uint32_t pulse_time_sum = 0;
    DigitalOut indicator(LED2);

    // Apply power to the wheels
    Wheel.Fwd(0.6);

    while(1){

        // Check to see if a pulse has been received and it's time in microseconds
        int32_t pulse_time = left_encoder.getLastPulseTimeUs();

        // If a pulse has been received..
        if(pulse_time>0){
            // Add the pulse time to the sum
            pulse_time_sum = pulse_time_sum + pulse_time;
            // Increment the pulse counter
            pulse_counter++;
            // Check if we have gone around a full rotation
            if((pulse_counter%left_encoder.getPulsesPerRotation())==0){
                // If so calculate the average pulse time
                float ave = (float)pulse_time_sum/pulse_counter;
                // Display on terminal
                printf("Ave pulse time:%dms\tAverage rotation_time:%dms\tTotal pulses:%d\n",(int)(ave/1000),(int)(ave/1000)*left_encoder.getPulsesPerRotation(),pulse_counter);
            }
            // Switch off the blue LED if the wheel is moving
            indicator=1;
        }
        else if(pulse_time==-2){
            // Switch off the blue LED if the wheel is staionary
            indicator = 0;
        }

    }

}


//*************************************Speed test***********************************************//
// Uses the encoder information to control the speed of the wheels and display it on the terminal
void speed_test(){

    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Experiment with these values
    float target_rpm = 40.0f;       // What rpm are we going to try to achieve?
    float pwm_increment = 0.003f;   // This value affects how quickly we can reach the target but also the stability of our controller
    int loop_delay_ms = 1;          // This sets how often the loop runs
    int ppr = 20;//left_encoder.getPulsesPerRotation();      // To work out how fast we are going we need to know how many pulses are in a complete rotation
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


    // We will print the current and target speed periodically using a timer
    Timer print_timer;
    print_timer.start();

    // We will save the previous speed so that it can be printed
    float last_rpm = 0.0f;

    while(1){

        // Check to see if a pulse has been received and it's time in us
        int32_t time = left_encoder.getLastPulseTimeUs();

        // If a pulse has been received..
        if(time>0){

            // Calculate RPM
            float rpm = (60000000.0f/(ppr*time));               // 60000000us = 60secs
            
            // Calculate error term
            float err = target_rpm-rpm;

            if(target_rpm==0.0f){           // Is the target 0 rpm?...
                Wheel.Speed(0.0f,0.0f);     // If so stop the motors 
            }
            // Otherwise increase or decrease PWM if we are going too fast or slow
            else{
                if(rpm < (target_rpm)){
                    Wheel.Speed(0.0f,Wheel.getSpeedLeft()+pwm_increment);
                }
                else if(rpm > target_rpm){
                    Wheel.Speed(0.0f,Wheel.getSpeedLeft()-pwm_increment);
                }
            }  

            //Update value for printing
            last_rpm = rpm;
        }
 
        // Check to see if the wheel IS stationary
        else if(time==-2){
            //Update value for printing
            last_rpm = 0.0f;        

            // SHOULD the wheel be stationary? 
            if(target_rpm !=0.0f){
                Wheel.Speed(0.0f,Wheel.getSpeedLeft()+pwm_increment);   // If not increase the power to turn the wheel
                                                                        // (This serves to start the motor if it is not yet turning)
            }
            else{
                Wheel.Speed(0.0f,0.0f);                                 //If so, ensure the motor is stopped
            }
        }
        
        // Time will only = -1 if there is an error
        else if(time==-1){
            last_rpm = -999.0f;
        } 
        
        // Print periodically
        if(std::chrono::duration_cast<std::chrono::milliseconds>(print_timer.elapsed_time()).count()>=500){ //Was print_timer.read_ms()
            printf("\033[2J\033[1;1H\r\n");
            printf("****PROJ100 Speed Test****\r\n");
            printf("Target: \t%4.3frpm\r\n",target_rpm);
            printf("Current:\t%4.3frpm\r\n",last_rpm);
            print_timer.reset();
        } 

        // A delay can be used to set how often the loop runs
        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));
    }
}