#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "lcd.h"
#include "Timer.h"
/**
 * Initializes the IR in our program
 */
void IR_init(void);

//read the IR data
int IR_read(void);

//Returns the IR distance
double get_IR_distance();

void IR_calculate();

