#include "lcd.h"
#include "Timer.h"
#include "servo.h"
#include "IR.h"
#include "sonar.h"
#include "button.h"
#include "WiFi.h"
#include "uart.h"
#include <math.h>
#include<stdio.h>
#include<ctype.h>
#include <string.h>
#include "sensor.h"
#include "open_interface.h"
void move_forward(oi_t *sensor_data, int centimeters)
{

    int sum = 0;
    oi_setWheels(500, 500);
    while (sum < (centimeters*10))
    {
        oi_update(sensor_data);
        sum += sensor_data->distance;
    }
    oi_setWheels(0, 0);

}
void turn_clockwise(oi_t *sensor_data, int degrees)
{
    int sum=0;
    oi_setWheels(250, -250);
    while (sum < degrees)
    {
        oi_update(sensor_data);
        sum += sensor_data->angle;
    }
    oi_setWheels(0, 0);

}
volatile char newData='\0';
void main()
{
    lcd_init(); // initialize lcd
    servo_init(); // initialize servo
    IR_init(); // initialize IR
    sonar_init(); // initialize sonar
    uart_init(); // initialize uart
    button_init(); // initialize button
    char str[30]; //takes it data from putty one character a time ex: move_forward 50
    char splitStrings[3][14]; // Separates move_forward and 50 into different strings
    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    int i=0; int j=0; int cnt=0;
    int dec=0; int dec2=0;
        while(1){
            newData=uart_receive();
            if(newData=='\r'){
               str[i]=newData;
               i++;
            }
            for(i=0; i<=strlen(str); i++){
                if(str[i]==' ' || str[i]=='\0'){
                    splitStrings[cnt][j]='\0';
                    cnt++
                    j=0;
                }
                else {
                    splitStrings[cnt][j]=str[i];
                    j++;
                }
            }
            for(i=0; i<strlen(splitStrings[1]); i++){

            }
            if(strcmp(splitStrings[0], "move_forward"))
        }
        oi_free(sensor_data);
}
