#include "msp.h"
#include "ee1910delay.h" // It uses ee1910delay.h to delay execution of further code
#include "ee1910analog.h" // The program uses ee1910analog.h for analog to digital conversions - input and output
#include <stdio.h> // Necessary for non-janky printf

#define LB 0x02
#define RB 0x10

/*
This program will read the output of a LM34 temperature sensor and accordingly light up an LED or print the temperature to console.
The LED color will be blue for a temperature below normal body temp, green for normal body temp, and red for a fever.
By John Bilkey, modified from Lab 4 from Dr. Ross's EE 1910 Class.

  Required Hardware:
  * MSP 432 Board
  * Breadboard
  * LM 34 Sensor connected to pin 5.5
 */

void main(void)
{
    P1->DIR &= ~(LB | RB);  // Buttons are use Port 1 Pin 1 (Left) and Port 1 Pin 4 (Right).  The direction of a button pin is input (0).
    P1->REN |= (LB | RB);  // Set pull-up resistor for button
    P1->OUT |= (LB | RB);   //Set out register for 1 (input) 
    P2->DIR |= 0b11111111; // Set up LEDs
    P2->OUT &= 0b00000000; // Set up LEDs
    analogSetup();
    int PM = 0;   // Selects between printing to console (1) or LED output (0)
    float temp = 0;
    while(1){
     // PRINT TO CONSOLE *OR* OUTPUT TO LED BASED ON IF STATEMENTS

    temp = analogRead();  //  float temp is set to analogRead - making sure decimal calculations don't have issues
    temp = ((temp*3.3)/(40.95));  // Convert analogRead output to degrees Fahrenheit

        // CONTINUE IF PRINT TRUE
    if (PM == 1){
        analogWrite(0,50); // R
        analogWrite(1,50); // G
        analogWrite(2,0); // B
            printf("Degrees F: %f\n",temp); // Print conversion output to console
            delay(500); // Wait 500ms
    }

   // CONTINUE IF PRINT FALSE
   if (PM == 0){

        //BEGIN LED OUTPUT
        if (temp<97){
             analogWrite(0,0); // R
             analogWrite(1,0); // G
             analogWrite(2,100); // B  Colder than Body
        }

        if ((temp>=97) & (temp<99)){
             analogWrite(0,0);
             analogWrite(1,100);
             analogWrite(2,0); // Normal Body Temp
            }

        if (temp>=99){
             analogWrite(0,100);
             analogWrite(1,0);
             analogWrite(2,0); // Fever
        }
    }

        // EXECUTE REGARDLESS OF PM VALUE
        delay(500); // 500 ms delay

        // RIGHT BUTTON - Sets PM to 1 or 0.
        // 1 = Print Temp to Console 0 = LED Color Output (Default)
        if ( (P1->IN & RB) == 0) {
            if (PM == 1) PM=0;
            else {
                analogWrite(0,0);
                analogWrite(1,0);
                analogWrite(2,0);
                PM=1;
            }
            while((P1->IN & RB) == 0){}
        }
   }
  }
