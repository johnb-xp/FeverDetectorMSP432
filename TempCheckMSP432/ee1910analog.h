/*
 * ee1910analog.h
 *
 *  Created on: Dec 5, 2018
 *      Author: ross
 */

#ifndef EE1910ANALOG_H_
#define EE1910ANALOG_H_
/* analogSetup() will set P2.0. P2.1 P2.2 P2.3 to PWM output,
 * and Analog In 0 (P5.5) to read analog input voltages.
 */
#include "msp.h"
void analogSetup(void) {
    // Set P2 pins to output and turn them off initially
    P2->DIR |= 0xF;
    P2->OUT &= ~0xF;
    // Set TimerA CCR0 to 100x64, to define 100% of the period
    TIMER_A0->CCR[0] = (100 << 6);
    // Set CCRN to zero--these will define the turn-off time of P2.0 through P2.3
    TIMER_A0->CCR[1] = 0;
    TIMER_A0->CCR[2] = 0;
    TIMER_A0->CCR[3] = 0;
    TIMER_A0->CCR[4] = 0;
    // Enable interrupts that will turn off the pins
    TIMER_A0->CCTL[1] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_CCIE;
    TIMER_A0->CCTL[4] = TIMER_A_CCTLN_CCIE;
    // Start timer in up mode
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1 | TIMER_A_CTL_IE;
    __enable_irq();
    NVIC->ISER[0] |= 1 << ((TA0_N_IRQn) & 31);

    // Function to setup Analog input A0
    // for use in A/D conversion
    // Setup ADC Input 0
    // Pin 30 --> P5.5
    P5->SEL0 |= 0x20; // Select alternate mode 11
    P5->SEL1 |= 0x20;
    P5->DIR &= ~0x20; // input
    P5->REN &= ~0x20; // No pull u/d
    // ADC Setup
    // You must enable the Analog 0 pin...
    ADC14->CTL0 = 0x04000210; // S/H timer, 16clk S/H, ADC ON
    ADC14->CTL1 = 0x00000020; // 12-bit conversion
    ADC14->MCTL[0] = 0x00000000;

}

/* analogWrite(pin,bright) will set the duty cycle percentage on P2.pin to bright.
 * Attempt to set duty cycle greater than 100% results in 100% duty cycle.
 */

void analogWrite(uint8_t pin, double bright_fp) {
    uint16_t bright = 0;
    if(bright_fp>100)
            bright=(100<<6);
    if(bright_fp> 97.5)
        bright=bright_fp*64;
    else if (bright_fp>1)
        bright = (bright_fp+2.45)*64;
    TIMER_A0->CCR[pin+1] = bright;  // Set turn off time to bright*64
}

// ISR to turn P2 pins on/off for software PWM
void TA0_N_IRQHandler(void)
{
    if(TIMER_A0->CCTL[1]&TIMER_A_CCTLN_CCIFG)  // If CCTL1 interrupt
    {
        TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG;  // clear flag
        if(TIMER_A0->CCR[1] < (100 << 6))           // if DC is not 100%
            P2->OUT &= ~0x1;                        // turn off pin
    }
    if(TIMER_A0->CCTL[2]&TIMER_A_CCTLN_CCIFG)  // same for other CCTL
    {
        TIMER_A0->CCTL[2] &= ~TIMER_A_CCTLN_CCIFG;
        if(TIMER_A0->CCR[2] < (100 << 6))
            P2->OUT &= ~0x2;
    }
    if(TIMER_A0->CCTL[3]&TIMER_A_CCTLN_CCIFG)
    {
        TIMER_A0->CCTL[3] &= ~TIMER_A_CCTLN_CCIFG;
        if(TIMER_A0->CCR[3] < (100 << 6))
            P2->OUT &= ~0x4;
    }
    if(TIMER_A0->CCTL[4]&TIMER_A_CCTLN_CCIFG)
    {
        TIMER_A0->CCTL[4] &= ~TIMER_A_CCTLN_CCIFG;
        if(TIMER_A0->CCR[4] < (100 << 6))
            P2->OUT &= ~0x8;
    }
    if(TIMER_A0->CTL&TIMER_A_CTL_IFG)   // If timer overflow
    {
        TIMER_A0->CTL &= ~TIMER_A_CTL_IFG;  // clear flag
        if(TIMER_A0->CCR[1]>0)              // If DC greater than 0%
            P2->OUT |= 0x01;                // turn on pin
        if(TIMER_A0->CCR[2]>0)
            P2->OUT |= 0x02;
        if(TIMER_A0->CCR[3]>0)
            P2->OUT |= 0x04;
        if(TIMER_A0->CCR[4]>0)
            P2->OUT |= 0x08;
    }
}
int analogRead(void){
    // Function to perform a single
    // A/D conversion on Analog input 0
    // Start sampling/conversion
    ADC14->CTL0 |= 0x00000003; // enable ADC, start conversion
    // Wait for conversion to complete
    // Conversion is complete when ADC0 flag is set
    while (!ADC14->IFGR0){
        ;
    }
    // returning a full int instead of a uint16_t for simplicity
    return ADC14->MEM[0];
} // end adc0_conv
#endif
