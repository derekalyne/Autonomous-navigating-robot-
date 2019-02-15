#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "lcd.h"
#include "Timer.h"
#include "driverlib/interrupt.h"
/**
 * Initializes the sonar in our program
 */

void send_pulse();

unsigned time2dist(unsigned time);

unsigned ping_read();

void TIMER3B_Handler(void);

void ping_init(void);

