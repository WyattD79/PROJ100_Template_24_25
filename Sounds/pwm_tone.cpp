/* Includes ------------------------------------------------------------------*/
#include "pwm_tone.h"
#include <chrono>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
   * @brief     Tune Function
   * @param  name : Choose the PwmOut
                    period : this param is tune value. (C_3...B_5)
                    beat : this param is beat value. (1..16) 1 means 1/16 beat
   * @retval    None
   */
void Tune(PwmOut& name, int period, int beat)
{  
    int delay,count;
    delay = beat*63;
    name.period_us(period);
    name.write(0.50f); // 50% duty cycle
    ThisThread::sleep_for(std::chrono::milliseconds(delay));
    name=0;
}

/**
   * @brief     Auto tunes Function
   * @param  name : Choose the PwmOut
                    period : this param is tune value. (C_3...B_5)
                    beat : this param is beat value. (1..16) 1 means 1/16 beat
   * @retval    None
*/

void Auto_tunes(PwmOut& name, int period, int beat)
{    
    int delay;
    delay = beat*63;
    name.period_us(period);
    name.write(0.50f); // 50% duty cycle
    thread_sleep_for(delay);
}

/**
   * @brief     Stop tunes Function
   * @param  name : Choose the PwmOut
   * @retval    None
   */
void Stop_tunes(PwmOut& name)
{
    name.period_us(0);
}
