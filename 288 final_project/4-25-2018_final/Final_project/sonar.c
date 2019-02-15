/**
 * Sonar Class, contains methods used for the Sonar
 */
#include "sonar.h"
#include "lcd.h"
#include "Timer.h"
#include "driverlib/interrupt.h"
#include "stdbool.h"

//Variable used to keep track of rising edge
volatile unsigned rising_time = 0;

//Variable used to keep track of falling
volatile unsigned falling_time = 0;

volatile int count = 0; // used to keep track of the first interrupt and the second
volatile int width_cycles = 0; // the width of the cycles between rising and falling edge
volatile int SONARDISTANCE = 0; // the distance calculated from the sonar
volatile int overflows = 0; // number of overflows the results from the rising and falling edge

int final_distance = 0;

void sonar_init(void) // According to page 724 11.4.4
{
    //enable clock to GPIO, R1 = port B
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R1;

    //enable pin3 on port B
    GPIO_PORTB_DEN_R |= 0x08;

    //set the port control
    GPIO_PORTB_PCTL_R |= 0x7000;

    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R3; // Turn on clock to TIMER3

    //Disable Timer b
    TIMER3_CTL_R &= ~(TIMER_CTL_TBEN);

    //16/32 bit timers are split into two 16-bit timers, timer A
    //and timer B. 32/64 bit timers are split into two 32-bit timers.
    TIMER3_CFG_R |= 0x4;

    //Setting GPTIM Timer B mode (p.27 - slides)
    TIMER3_TBMR_R = 0x17;

    //Timer B Event mode to "both" (p.25 - slides)
    TIMER3_CTL_R |= 0xC00;

    // set timer prescaler (extended timer) and load init value to max
    TIMER3_TBPR_R = 0xFF; //might be 0

    // 24-bit timeout value
    TIMER3_TBILR_R = 0xFFFF;

    //Clearing the interrupt
    TIMER3_ICR_R |= 0x400; // CBECINT = bit 10, 0b100_0000_0000

    //Enabling interrupt
    TIMER3_IMR_R |= 0x400; // CBEIM = bit 10, 0b100_0000_0000

    // Set up NVIC for Timer3B IC interrupts (next slide)…
    NVIC_PRI9_R |= 0x20; // set bits 7:5 to 0b001 //Maybe 2000

    NVIC_EN1_R |= 0x10; // set bit 4

    //Bind Timer3B interrupt requests to your interrupt handler
    IntRegister(INT_TIMER3B, TIMER3B_Handler);
//
    //Enable interrupts
    IntMasterEnable();

    //Enable Timer b
    TIMER3_CTL_R |= TIMER_CTL_TBEN;

}

/* send out a pulse on PB3 */
void send_pulse()
{
    TIMER3_CTL_R &= ~(TIMER_CTL_TBEN);//turn off timer 3
    GPIO_PORTB_AFSEL_R &= ~0x08; // turn off alternate function
    GPIO_PORTB_PCTL_R &= 0xFFFF8FFF;//turn off port control

    GPIO_PORTB_DIR_R |= 0x08; // set PB3 as output
    GPIO_PORTB_DATA_R |= 0x08; // set PB3 to high
    // wait at least 5 microseconds based on data sheet
    timer_waitMicros(5);
    GPIO_PORTB_DATA_R &= 0xF7; // set PB3 to low
    GPIO_PORTB_DIR_R &= 0xF7; // set PB3 as input

    GPIO_PORTB_AFSEL_R |= 0x08;//set alternate function
    GPIO_PORTB_PCTL_R |= 0x7000;//set the pin control
    TIMER3_CTL_R |= TIMER_CTL_TBEN;//Turn on timer 3

}

/* start and read the ping sensor once, return distance in cm */
unsigned ping_read()
{
    int distance;

    //get time of the rising edge of the pulse
    if (count == 0)
    {
        rising_time = TIMER3_TBR_R;
        count++;
    }
    else
    {
        falling_time = TIMER3_TBR_R;
        width_cycles = abs(rising_time - falling_time);
        final_distance = time2dist(width_cycles);
        overflows += (falling_time > rising_time); // used to calculate overflow
        count = 0;
    }

    return SONARDISTANCE;
}

/* convert time in clock counts to single-trip distance in cm */
unsigned time2dist(unsigned time)
{
    double temp = time / 16000000.0;
    SONARDISTANCE = temp * (0.5) * 34000;
    return SONARDISTANCE;
}

void TIMER3B_Handler(void)
{
    TIMER3_ICR_R = TIMER_ICR_CBECINT; //clear the TIMER3B interrupt flag (page 754 of datasheet)
    SONARDISTANCE = ping_read();
}

int get_sonar_distance()
{
    return final_distance;
}
