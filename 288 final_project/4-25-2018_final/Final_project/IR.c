/**
 * IR Class, contains methods used for the IR
 */
#include "IR.h"

#include <math.h>

int SAMPLESIZE = 16; //The SAMPLESIZE of the recordings

double IRDISTANCE = 0.0; // Holds the distance of the IR

//initialize
void IR_init(void)
{
    //enable ADC 0 module on port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    //enable clock for ADC0
    SYSCTL_RCGCADC_R |= 0x1;

    //enable port B pin 4 to work as alternate functions
    GPIO_PORTB_AFSEL_R |= 0x10;

    //enable pin 4 // THIS IS DISABLING IT!!!!!!!
    GPIO_PORTB_DEN_R &= 0b11110111;

    //disable analog isolation for the pin 4
    GPIO_PORTB_AMSEL_R |= 0x10;

    //initialize the port trigger source as processor (default)
    GPIO_PORTB_ADCCTL_R = 0x00;

    //disable SS1 sample sequencer to configure it
    ADC0_ACTSS_R &= ~ADC_ACTSS_ASEN1;

    //initialize the ADC trigger source as processor (default)
    ADC0_EMUX_R = ADC_EMUX_EM1_PROCESSOR;

    //set 1st sample to use the AIN10 ADC pin
    ADC0_SSMUX1_R |= 0x000A;

    //enable raw interrupt status
    ADC0_SSCTL1_R |= (ADC_SSCTL1_IE0 | ADC_SSCTL1_END0);

    //enable oversampling to average
    ADC0_SAC_R |= ADC_SAC_AVG_64X;

    //re-enable ADC0 SS1
    ADC0_ACTSS_R |= ADC_ACTSS_ASEN1;

}

//read the data from the IR Sensor
int IR_read(void)
{
    ADC0_PSSI_R = ADC_PSSI_SS1;

    //wait for ADC conversion to be complete
    while ((ADC0_RIS_R & ADC_RIS_INR1) == 0)
    {
        //wait
    }

    int temp = ADC0_SSFIFO1_R;
    return temp;

}
void IR_calculate()
{
    int volt; //Used to store the ADC data
    double distance;

    int i;
    for (i = 0; i < SAMPLESIZE; ++i) //Recording to the desired SAMPLESIZE
    {
        volt = IR_read(); // reading the ADC
        double temp = 140580 * pow(volt, -1.215); //Special equation to find the distance (cm)
        distance += temp;
    }

    distance = distance / SAMPLESIZE; //computing average distance
    distance = distance - 0.4; //fixing the offset

    IRDISTANCE = distance;
//    lcd_printf("%d, %.2f cm", volt, distance); // printing ADC and distance on the lcd
//    timer_waitMillis(500);//wait, better clarity on the lcd
}

double get_IR_distance()
{
    return IRDISTANCE;
}
