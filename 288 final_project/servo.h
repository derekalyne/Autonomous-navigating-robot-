#include <inc/tm4c123gh6pm.h>
#include <stdint.h>
#include "lcd.h"
#include "Timer.h"
#include "driverlib/interrupt.h"
/**
 * Header file for the servo class
 */

//Initializes the servo
void servo_init(void);

//moveServo method changes the MATCH value so Servo can move in the given direction for the given angle
double moveServo(double angle, int direction);
