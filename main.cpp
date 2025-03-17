#define USER_BUTTON PC_13

#include "mbed.h"
#include "motor.h"
#include "tunes.h"
#include "pwm_tone.h"
#include "PROJ100_Encoder.h"
#include "PROJ100_Encoder_Tests.h"

#define TIME_PERIOD                 10      //Constant compiler Values here 10 equates to 10ms or 100Hz base Frequency
#define ENCODER_PIN_LEFT            D8
#define ENCODER_PIN_RIGHT           D6 
#define PULSES_PER_ROTATION         20
#define DEBOUNCE_US                 26000

DigitalIn microswitch1(D4);         //Instance of the DigitalIn class called 'microswitch1'
DigitalIn microswitch2(D3);         //Instance of the DigitalIn class called 'microswitch2'
DigitalIn myButton(USER_BUTTON);    //Instance of the DigitalIn class called 'myButton'   
DigitalOut greenLED(LED1);          //Instance of the DigitalOut class called 'greenLED'

Motor Wheel(D13,D11,D9,D10);      //Instance of the Motor Class called 'Wheel' see motor.h and motor.cpp

PROJ100_Encoder right_encoder (ENCODER_PIN_RIGHT,PULSES_PER_ROTATION);  //Instance of the PROJ100Encoder class called 'right_encoder'
PROJ100_Encoder left_encoder(ENCODER_PIN_LEFT,PULSES_PER_ROTATION);     //Instance of the PROJ100Encoder class called 'left_encoder'

UnbufferedSerial ser(USBTX,USBRX,115200);   // Serial object for printing info

bool switch1 = false;
bool switch2 = false;


void leftspeedcontroller(){

    float left_target_rpm = 40.0f;
    float right_target_rpm = 40.0f;       // What rpm are we going to try to achieve?
    float pwm_increment = 0.003f;   // This value affects how quickly we can reach the target but also the stability of our controller
    int loop_delay_ms = 1;          // This sets how often the loop runs
    int ppr = 20;//left_encoder.getPulsesPerRotation();      // To work out how fast we are going we need to know how many pulses are in a complete rotation
    
    Timer left_print_timer;
    Timer right_print_timer;
    left_print_timer.start();

    float left_last_rpm = 0.0f;
    while(1){
        int32_t left_time = left_encoder.getLastPulseTimeUs(); //check pulse
        if(left_time>0){ //if pulse received
            float left_rpm = (60000000.0f/(ppr*left_time));               //calculate rpm
            float err = left_target_rpm-left_rpm;                         //calculate error rate
            if(left_target_rpm==0.0f){           //check if target rpm is 0
                Wheel.Speed(0.0f ,0.0f);     // If so stop the motors 
            }
            else{                                        //alter speed
                if(left_rpm < (left_target_rpm)){
                    Wheel.Speed(0.0f,Wheel.getSpeedLeft()+pwm_increment);
                }
                else if(left_rpm > left_target_rpm){
                    Wheel.Speed(0.0f,Wheel.getSpeedLeft()-pwm_increment);
                }
            }  

            left_last_rpm = left_rpm;
        }
 
        // Check to see if the wheel IS stationary
        else if(left_time==-2){                      //check if wheel is stationary
            left_last_rpm = 0.0f;        
            if(left_target_rpm !=0.0f){
                Wheel.Speed(0.0f,Wheel.getSpeedLeft()+pwm_increment);   //if stationary but needs to move increase speed
            }
            else{
                Wheel.Speed(0.0f,0.0f);                                 //if needs to be stationary, stop the motor
            }
        }
        
        // Time will only = -1 if there is an error
        else if(left_time==-1){
            left_last_rpm = -999.0f;
        } 
        
        // Print periodically
        if(std::chrono::duration_cast<std::chrono::milliseconds>(left_print_timer.elapsed_time()).count()>=500){ //Was print_timer.read_ms()
            printf("\033[2J\033[1;1H\r\n");
            printf("****LEFT WHEEL SPEED****\r\n");
            printf("Target: \t%4.3frpm\r\n",left_target_rpm);
            printf("Current:\t%4.3frpm\r\n",left_last_rpm);
            left_print_timer.reset();
        } 

        // A delay can be used to set how often the loop runs
        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));
    }
}

void rightspeedcontroller(){

    
    float right_target_rpm = 40.0f;       // What rpm are we going to try to achieve?
    float pwm_increment = 0.003f;   // This value affects how quickly we can reach the target but also the stability of our controller
    int loop_delay_ms = 1;          // This sets how often the loop runs
    int ppr = 20;//left_encoder.getPulsesPerRotation();      // To work out how fast we are going we need to know how many pulses are in a complete rotation
    
    
    Timer right_print_timer;
    right_print_timer.start();

    float right_last_rpm = 0.0f;
    while(1){
        int32_t right_time = right_encoder.getLastPulseTimeUs(); //check pulse
        if(right_time>0){ //if pulse received
            float right_rpm = (60000000.0f/(ppr*right_time));               //calculate rpm
            float err = right_target_rpm-right_rpm;                         //calculate error rate
            if(right_target_rpm==0.0f){           //check if target rpm is 0
                Wheel.Speed(0.0f ,0.0f);     // If so stop the motors 
            }
            else{                                        //alter speed
                if(right_rpm < (right_target_rpm)){
                    Wheel.Speed(Wheel.getSpeedRight()+pwm_increment,0.0f);
                }
                else if(right_rpm > right_target_rpm){
                    Wheel.Speed(Wheel.getSpeedRight()-pwm_increment,0.0f);
                }
            }  

            right_last_rpm = right_rpm;
        }
 
        // Check to see if the wheel IS stationary
        else if(right_time==-2){                      //check if wheel is stationary
            right_last_rpm = 0.0f;        
            if(right_target_rpm !=0.0f){
                Wheel.Speed(Wheel.getSpeedRight()+pwm_increment,0.0f);   //if stationary but needs to move increase speed
            }
            else{
                Wheel.Speed(0.0f,0.0f);                                 //if needs to be stationary, stop the motor
            }
        }
        
        // Time will only = -1 if there is an error
        else if(right_time==-1){
            right_last_rpm = -999.0f;
        } 
        
        // Print periodically
        if(std::chrono::duration_cast<std::chrono::milliseconds>(right_print_timer.elapsed_time()).count()>=500){ //Was print_timer.read_ms()
            printf("\033[2J\033[1;1H\r\n");
            printf("****RIGHT WHEEL SPEED****\r\n");
            printf("Target: \t%4.3frpm\r\n",right_target_rpm);
            printf("Current:\t%4.3frpm\r\n",right_last_rpm);
            right_print_timer.reset();
        } 

        // A delay can be used to set how often the loop runs
        ThisThread::sleep_for(std::chrono::milliseconds(loop_delay_ms));
    }
}







int main ()
{

    // Clear the terminal and print an intro
    printf("\033[2J\033[1;1H\r\n");
    printf("Plymouth University PROJ100 2024/25 Optical Encoder Demonstration\n\r");

    // Set the PWM frequency
    Wheel.Period_in_ms(TIME_PERIOD);                

    // Ensure our motors and encoders are stopped at the beginning
    right_encoder.stop();
    left_encoder.stop();
    Wheel.Stop();

    // The encoder pulses can be noisy and so are debounced within the class
    // You can experiment with changing the debouce time if you wish
    left_encoder.setDebounceTimeUs(DEBOUNCE_US);
    right_encoder.setDebounceTimeUs(DEBOUNCE_US);

    // Wait for the blue button to be pressed
    printf("Press Blue Button To Begin\n\r");
    while (myButton == 0){greenLED = !greenLED; ThisThread::sleep_for(100ms);}

    // Start the encoders
    left_encoder.start();
    right_encoder.start();


    //*******Demostration of the pwm_tune library***********//
    //jingle_bells(2);
   // close_encounter(2);



    while(true){

        Wheel.Speed(-0.6f,-0.6f);
        
        if(microswitch1.read()==1){ //LEFT
            
            switch1 = true;
        }
        else{
            switch1 = false;
        }
        if(microswitch2.read()==1){ //RIGHT
            
            switch2 = true;
        }
        else{
            switch2 = false;
        }
        if((switch1 == true)and(switch2 == true)){ //no angle, full stop
            printf("Stop\n");
            Wheel.Speed(0.0f,0.0f);
        }
        if((switch1 == true)and(switch2 == false)){ //angled to the left
            printf("Pointing left\n");
            Wheel.Speed(-0.6f,0.0f);
        }
        if((switch1 == false)and(switch2 == true)){ //angled to the right
            printf("Pointing right\n");
            Wheel.Speed(0.0f,-0.6f);
        }
        wait_us(500000);

        

        // Write the parts of your code which should run in a loop between here..
        
        

           


        
    }
}