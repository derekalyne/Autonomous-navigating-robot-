/**
 * main.c
 */

#include "lcd.h"
#include "Timer.h"
#include "servo.h"
#include "IR.h"
#include "sonar.h"
#include "button.h"
#include "WiFi.h"
#include "uart.h"
#include <math.h>

#define M_PI 3.1415926535

int angle = 0; //keep track of angle
int sonar_distance = 0; // keep track of sonar distance
double IR_distance = 0.0; //keep track of the IR distance

//function to scan the area
void sweep(void);

/* int main(void)
{
    lcd_init(); // initialize lcd
    servo_init(); // initialize servo
    IR_init(); // initialize IR
    sonar_init(); // initialize sonar
    uart_init(); // initialize uart
    button_init(); // initialize button

    WiFi_start("cpre288_2018"); //Starts the wifi connection with the password

    //print the format of the header for the data that is to be recorded (Degrees, IR Distance, Sonar Distance)
    //then calls sweep
    while (1)
    {
        while (button_getButton() != 2)
        {
        }

        char header[100];

        char one[] = "Degrees";
        char two[] = "IR Distance (cm)";
        char three[] = "Sonar Distance (cm)";

        sprintf(header, "%-14s %-20s %-20s\n", one, two, three);
        uart_sendChar(13);
        uart_sendStr(header);
        timer_waitMillis(250);

        sweep();

    }
}
*/

//function to scan the area
void sweep(void)
{
    int visible = 0; // True if an object in in view, otherwise false
    int index = 0; // holds the count of objects

    int angularWidth = 0; // holds how many degrees an object was in view for

    int SO_angularWidth = 200; // holds the size of the smallest object, 200 because no object will ever be greater than 180

    int SO_index = 0; //holds which object is the smallest

    int startAngle = 0; // holds the current start angle of object in view
    int endAngle = 0; // holds the current end angle of object in view

    int SO_startAngle = 0; // holds the start angle of the smallest object
    int SO_endAngle = 0; // holds the end angle of the smallest object
    double SO_centerAngle = 0; // holds the center angle of the smallest object
    int distanceOfObject = 0; // Holds the current distance of object in view
    int SO_distance = 0; //Distance of smallest object
    double SO_width = 200; //The width of smallest object in cm, 200 because no object will ever be greater than 180
    angle = moveServo(90, 1); // move the servo to 180 degrees for the initial start position

    timer_waitMillis(500);

    while (angle > 0) // move the servo to 0 degrees and read data as this is being done
    {
        angle = moveServo(1, 0); //increment by 2 degrees, may need to be 1 degree

        timer_waitMillis(10); //gives the servo time to move

        //Calculates and gets the distance of the sonar
        send_pulse();
        timer_waitMillis(10); // gives time for the pulse to be received
        sonar_distance = get_sonar_distance(); //gets the distance from sonar

        //Calculates the distance of the IR
        IR_calculate();
        IR_distance = get_IR_distance(); //gets the distance from IR

        //Detects if there is an actual object in view
        if (visible == 0 && angularWidth >= 2)
        {
            angularWidth = 2; // Sets it back to 4 to avoid any miscalculations of angular width of an object
            startAngle = angle + 2; // fixes the offset of the startAngle
            distanceOfObject = sonar_distance; // gets the distance of object
            index++; // counter for the objects
            visible++; //sets visible to true
        }

        //Gets the size of the object by using a counter for each angle that the object is in view
        if (IR_distance < 100)
        {
            angularWidth++; //counter
            int tempSonarDistance = sonar_distance;
            if(tempSonarDistance < distanceOfObject)
            {
                distanceOfObject = tempSonarDistance;
            }
        }

        else //Object is no longer in view
        {

            double tempWidth = (tan((angularWidth *M_PI) /180) * distanceOfObject)/2.0;
            endAngle = angle + 1; //fixes the offset of endAngle

            //Checks if the previous object(recently in view) is smaller than the smallest object
            if ((tempWidth < SO_width) && visible == 1
                    && abs(startAngle - endAngle) >= 1)
            {
                SO_startAngle = startAngle; //the new start angle of smallest object
                SO_endAngle = endAngle; //the new end angle of smallest object
                SO_distance = distanceOfObject; //the new distance of smallest object
//                SO_angularWidth = angularWidth; //the new angle width of smallest object
                SO_index = index; //the new index of smallest object
                SO_width = tempWidth;//Updates the value of SO_width with the new width
            }

            angularWidth = 0; //resets width to get ready for next object in view

            visible = 0; //resets to identify that object is no longer in view

            startAngle = 0; //resets startAngle to get ready for next object in view
            endAngle = 0; //resets endAngle to get ready for next object in view
        }

        //Formats the text for Putty
        char line[100];
        sprintf(line, "%-14d %-20.2f %-20d\n", angle, IR_distance,
                sonar_distance);

        //Sends the text to Putty
        uart_sendChar(13);
        uart_sendStr(line);
    }

    //Calculates center angle of smallest object
    SO_centerAngle = SO_endAngle + ((SO_startAngle - SO_endAngle) / 2.0);

    //Calibration of the center angle
    SO_centerAngle -= 6.5;

    //Moves servo to the center of smallest object
    angle = moveServo(SO_centerAngle, 1);

//    //Calculates the width in cm of smallest object, converts from radian to degrees
    //SO_width= tan((SO_angularWidth *M_PI) /180) * SO_distance;
//
//    //Calibration of the width
    //SO_width = SO_width / 2.0;

    //Prints the required info on lcd
    lcd_printf("Num Of Objects: %d\nSmallest: %d\nDistance: %d\nWidth: %.2f",
               index, SO_index, SO_distance, SO_width);

    timer_waitMillis(500);

}
