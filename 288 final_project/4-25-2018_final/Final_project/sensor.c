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
struct data
{
   int start;
   int end;
   int distance;
   double width;
   int angular;
}object[10];
//function to scan the area
void sweep(void);
//function to scan the area
void sweep(void)
{

    int visible = 0; // True if an object in in view, otherwise false
    int index = 0; // holds the count of objects

    double angularWidth = 0; // holds how many degrees an object was in view for

    int startAngle = 0; // holds the current start angle of object in view
    int endAngle = 0; // holds the current end angle of object in view

    int distanceOfObject = 0; // Holds the current distance of object in view

    angle = moveServo(90, 1); // move the servo to 180 degrees for the initial start position
    angle=180;
    timer_waitMillis(500);
    char line[100];

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
        if (visible == 0 && angularWidth >2)
        {
            //angularWidth = 2; // Sets it back to 4 to avoid any miscalculations of angular width of an object
            startAngle = angle + 2; // fixes the offset of the startAngle
            distanceOfObject = sonar_distance; // gets the distance of object
            index++; // counter for the objects
            visible++; //sets visible to true
            object[index-1].start=startAngle;
        }

        //Gets the size of the object by using a counter for each angle that the object is in view
        if (IR_distance < 100)
        {
            angularWidth++; //counter
            int tempSonarDistance = sonar_distance;
            distanceOfObject = tempSonarDistance;
            object[index-1].angular=angularWidth;
        }
        else //Object is no longer in view
        {
            object[index-1].end=object[index-1].start-object[index-1].angular+1;
            double tempWidth = sin((object[index-1].angular*M_PI/180)/2)* object[index-1].distance* 2;
            object[index-1].width=tempWidth;
            object[index-1].distance=distanceOfObject;

            angularWidth = 0; //resets width to get ready for next object in view
            visible = 0; //resets to identify that object is no longer in view
            startAngle = 0; //resets startAngle to get ready for next object in view
            endAngle = 0; //resets endAngle to get ready for next object in view
        }

        //Formats the text for Putty
        sprintf(line, "%-14d %-20.0f %-20d\r\n", angle, IR_distance, sonar_distance);

        //Sends the text to Putty
        uart_sendStr(line);
    }

    //Moves servo to the center of smallest object
    moveServo(90, 1);

    char all_objectData[500];
    int i=0;
    for(i=0;i<index;i++)
    {
        sprintf(all_objectData, "Object:%d distance:%d start angle:%d end angle:%d width:%.2lf\r\n",i+1, object[i].distance,object[i].start, object[i].end, object[i].width);
        uart_sendStr(all_objectData);
    }

    timer_waitMillis(500);
}
