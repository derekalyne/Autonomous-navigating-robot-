#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "lcd.h"
#include "Timer.h"
#include "driverlib/interrupt.h"
/**
 * Initializes the sonar in our program
 */
void sonar_init(void);

void send_pulse();

unsigned ping_read();

unsigned time2dist(unsigned time);

void TIMER3B_Handler(void);

int get_sonar_distance();

