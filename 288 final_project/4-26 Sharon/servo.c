/**
 * c file to move servo maybe
 *
 *
 */
#include "servo.h"

void servo_init(void)
{
    //Turn on GPIO Port B
    SYSCTL_RCGCGPIO_R |= 0x2;

    //Set direction of GPIO Port pin 5 to output
    GPIO_PORTB_DIR_R |= 0x20;

    //enable port B pin 5 to work as alternate functions
    GPIO_PORTB_AFSEL_R |= 0x20;

    //Configuring port control for pin 5 and setting the alternate function to use T1CCP1
    GPIO_PORTB_PCTL_R |= 0x700000;

    //enable pin 5
    GPIO_PORTB_DEN_R |= 0x20;

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1; // Turn on clock to TIMER1

    //Disable Timer b
    TIMER1_CTL_R &= ~(TIMER_CTL_TBEN);

    // Configuring the timer setting to be 16/32 or 32/64 bit (in this case we are doing 16/32)
    TIMER1_CFG_R |= 0x4;

    //Set TBAMS to 0x1, TBCMR to 0x0, and TBMR to 0x2
    TIMER1_TBMR_R |= 0xA;

    //set the prescaler, upper 8 bits
    TIMER1_TBPR_R = 320000 >> 16;

    //timer's start value, lower 16 bits
    TIMER1_TBILR_R = (320000) & 0x0000FFFF;

    //Load the timer's match value, lower 16 bits
    TIMER1_TBMATCHR_R = (296725) & 0x0000FFFF;

    //Load the timer's match value, upper 8 bits
    TIMER1_TBPMR_R |= 296725 >> 16;

    //Enable Timer b
    TIMER1_CTL_R |= TIMER_CTL_TBEN;

}

double moveServo(double angle, int direction)
{
    //matchValue is used to hold the match value
    static int matchValue = 296725;

    //variable to hold the current value of angle
    static double final_angle = 90.0;

   /*
    * Steps to calibration:
    * Step 1: Find lowest and highest match values (0 degrees, and 180 degrees)
    * Step 2: Calculate 90 degrees from your match values in step 1 (in the middle of 0 and 180)
    * Step 3: Highest match value - minimum match value = rateOfChange (of match value)
    */

    int rateOfChange = 26000;

    if ((final_angle <= 0 && direction == 0))
    {
        return 0.0;
    }
    else if ((final_angle >= 180 && direction == 1))
    {
        return 180.0;
    }

    //turning left
    if(direction == 0)
    {
        matchValue = matchValue + ((angle/180)*rateOfChange);
        final_angle -= angle;
    }
    else
    {
        matchValue = matchValue - ((angle/180)*rateOfChange);

        final_angle += angle;
    }

    //Load the timer's match value, lower 16 bits
    TIMER1_TBMATCHR_R = (matchValue) & 0x0000FFFF;

    //Load the timer's match value, upper 8 bits
    TIMER1_TBPMR_R |= matchValue >> 16;

    return final_angle;
}



/*
 *
 * 0 = 304000
 *
 * 90 = 296000
 *
 * 180 = 288000
 *
 * Cybot 7
 * rateOfChange = 27500
 *
 *
 * Cybot 9
 * rateOfChange = 26000
 *
 *
 */

