/*
 * ee1910music.h
 *
 *  Created on: Dec 5, 2019
 *      Author: ross
 */

#ifndef EE1910MUSIC_H_
#define EE1910MUSIC_H_

#include "msp.h"

void make_music(uint32_t freq) {
    /*
     * timerA_pwm_example.c
     *
     *  Created on: 10/9/18
     *      Author: johnsontimoj
     *  Modified on: 12/6/19 for variable frequency
     *      Author: ross
     */
    //////////////////////////////////////
    //
    //  Timer A setup to generate a 50% duty cycle
    //  Frequency freq square wave on Pin 36 via PWM
    //
    ////////////////////////////////////
    //
    //  Pin 36 is P6.6
    //  P6.6 is TimerA_2, Output 3
    //
    //  Use SMCLK for timer clock
    //      SMCLK defaults to HSMCLK which defaults to HFXTCLK
    //      all with a default of divide by 1
    //          --> SMCLK = 3MHz
    //      with no divide on the Timer
    //          counts --> 3M * period = 3000000/freq
    //          50% duty cycle --> half counts high, half counts low
    //
    //  Note - not concerned about the first cycle so not resetting the counter
    //
    ///////////////////////////////////
           // setup pins
        //
        // P6.6 in PSEL=01 mode and set as output
        P6->SEL0 |= 0x40;
        P6->SEL1 &= ~0x40;
        P6->DIR |= 0x40;

        //
        // TIMER_A2
        // CTL
        //        SMCLK   /1   up    noclr  no int
        // xxxx xx 10     00   01  x   0      0     x
        TIMER_A2->CTL = 0x0210;
        //
        // CCR[0] set period
        TIMER_A2->CCR[0] = 3000000/freq;
        //
        // CCR[3] set 50% duty
        TIMER_A2->CCR[3] = 1500000/freq;    // integer divide!!!
        //
        // CCTL[3]
        // no cap  xx  x  x  x  comp  r-s  no int  x  x  x  x
        //   00                  0    111    0
        TIMER_A2->CCTL[3] = 0x00E0;

        return;
     } // end setup_timer

void stop_music(void){

       // Turn off timer
    TIMER_A2->CTL = 0;
}

#endif /* EE1910MUSIC_H_ */
