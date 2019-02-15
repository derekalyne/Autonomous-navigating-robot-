#include "lcd.h"
#include "Timer.h"
#include "servo.h"
#include "IR.h"
#include "sonar.h"
#include "button.h"
#include "WiFi.h"
#include "uart.h"
#include <math.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "sensor.h"
#include "open_interface.h"
#include "music.h" //192.168.1.1
void move_forward(oi_t *sensor_data, int centimeters) // working
{
    int bumpRight=0;
     int bumpLeft=0;
     int cliffleft=0;
     int cliffright=0;
     int cliffFrontLeft=0;
     int cliffFrontRight=0;
     int whiteleftSig=0;
     int whiterightSig=0;
     int whitefrontLeftSig=0;
     int whitefrontRightSig=0;
     char line[100];

     int sum = 0;
     oi_setWheels(150, 150);
     while (sum < ((centimeters-1)*10)-7)
     {
        oi_update(sensor_data);
        sum += sensor_data->distance;
        if(sensor_data->bumpRight || sensor_data->bumpLeft){
            bumpRight=sensor_data->bumpRight;
            bumpLeft=sensor_data->bumpLeft;
            backward(sensor_data, 50);
            sprintf(line, "Bumped State: BumpRight:%d BumpLeft%d\r\n", bumpRight, bumpLeft);
            uart_sendStr(line);
            break;
        }
        if(sensor_data->cliffLeft || sensor_data->cliffRight || sensor_data->cliffFrontLeft || sensor_data->cliffFrontRight){
            cliffleft=sensor_data->cliffLeft;
            cliffright=sensor_data->cliffRight;
            cliffFrontLeft=sensor_data->cliffFrontLeft;
            cliffFrontRight=sensor_data->cliffFrontRight;
            backward(sensor_data, 100);
            sprintf(line, " Crater State: cliffleft:%d Cliffright:%d cliffFrontLeft:%d cliffFrontRight:%d\r\n", cliffleft, cliffright, cliffFrontLeft, cliffFrontRight);
            uart_sendStr(line);
            break;
        }

        if(sensor_data->cliffLeftSignal > 2800||sensor_data->cliffFrontLeftSignal > 2800||sensor_data->cliffFrontRightSignal > 2800||sensor_data->cliffRightSignal > 2800)
        {
            if(sensor_data->cliffLeftSignal > 2800)
            {
                backward(sensor_data, 100);
                whiteleftSig=1;

            }

            else if(sensor_data->cliffFrontLeftSignal > 2800)
            {
                backward(sensor_data, 100);
                whitefrontLeftSig=1;

            }
            else if(sensor_data->cliffFrontRightSignal > 2800)
            {
                backward(sensor_data, 100);
                whitefrontRightSig=1;
            }
            else if(sensor_data->cliffRightSignal > 2800)
            {
                backward(sensor_data, 100);
                whiterightSig=1;
            }
            sprintf(line, " Boundary State: WhiteLeft:%d WhiteRight:%d WhiteFrontLeft:%d WhiteFrontRight:%d\r\n", whiteleftSig, whiterightSig, whitefrontLeftSig, whitefrontRightSig);
            uart_sendStr(line);
            break;
        }

     }
    oi_setWheels(0, 0);

}
void turn_clockwise(oi_t *sensor_data, int degrees)  // working
{
    double sum=degrees+0.1;
    oi_setWheels(-250, 250);
    while (sum >= 0)
    {
        oi_update(sensor_data);
        sum--;
    }
    oi_setWheels(0, 0);

}
void backward(oi_t *sensor_data, int dist)  // working
{
    double sum=dist;
    oi_setWheels(-250, -250);
    while (sum >= 0)
    {
        oi_update(sensor_data);
        sum += sensor_data->distance;
    }
    oi_setWheels(0, 0);

}
void turn_counterclockwise(oi_t *sensor_data, int degrees)  // working
{
    double sum=0;
    oi_setWheels(250, -250);
    while (sum <= degrees)
    {
        oi_update(sensor_data);
        sum ++;
    }
    oi_setWheels(0, 0);
}
volatile char newData='\0';
void main()
{
    lcd_init();
    uart_init();
    button_init();
    servo_init(); // initialize servo
    IR_init(); // initialize IR
    sonar_init(); // initialize sonar

    oi_t *sensor_data = oi_alloc();
    oi_init(sensor_data);
    load_songs();
    char str[30];
    char splitStrings[2][30];
    int i=0; int dec=0;
    int j=0; int cnt=0;
    for(i=0; i<30; i++){
        str[i]='\0';
    }
    i=0;
    while(1)
    {
            newData=uart_receive();

        if(i<30 && newData!='\r' && newData!='\n'){
            str[i]=newData;
            uart_sendChar(str[i]);
            i++;
        }
        else if(newData=='\r'){
            uart_sendChar('\n');
            uart_sendChar('\r');
            for(i=0; i<=strlen(str); i++)
            {
                if(str[i]==' ' || str[i]=='\0'){
                    splitStrings[cnt][j]='\0';
                    cnt++;
                    j=0;
                }

                else {
                    splitStrings[cnt][j]=str[i];
                    j++;
                }
            }
            for(i=0; i<strlen(splitStrings[1]); i++){
                dec=dec*10+(splitStrings[1][i]-'0');
            }
            i=0;
            j=0;
            cnt=0;
            for(i=0; i<30; i++)
                str[i]='\0';
            lcd_printf("command: %s\n number: %d", splitStrings[0],dec);
            i=0;
        }
        if(strcmp(splitStrings[0], "mf")==0){
            move_forward(sensor_data, dec);
            for(i=0; i<2; i++)
                for(j=0; j<30; j++)
                {
                    splitStrings[i][j]='\0';
                }
            dec=0;
            for(i=0; i<30; i++)
                str[i]='\0';
            i=0;
            j=0;
        }
        else if(strcmp(splitStrings[0], "tc")==0){

            turn_clockwise(sensor_data, dec);

          for(i=0; i<2; i++)
            for(j=0; j<30; j++)
            {
                splitStrings[i][j]='\0';
            }
            dec=0;
            for(i=0; i<30; i++)
                str[i]='\0';
            i=0;
            j=0;
        }
        else if(strcmp(splitStrings[0], "tcc")==0){

            turn_counterclockwise(sensor_data, dec);

           for(i=0; i<2; i++)
            for(j=0; j<30; j++)
            {
               splitStrings[i][j]='\0';
            }
            dec=0;
            for(i=0; i<30; i++)
               str[i]='\0';
            i=0;
            j=0;
        }
        else if(strcmp(splitStrings[0], "sweep")==0){
//            timer_waitMillis(500);
            sweep(); //originally used moveServo, changed to move_to_servo on 4/17, but "pointing at smallest object" does not work because of it
            for(i=0; i<2; i++)
                        for(j=0; j<30; j++)
                        {
                           splitStrings[i][j]='\0';
                        }
                        dec=0;
                        for(i=0; i<30; i++)
                           str[i]='\0';
                        i=0;
                        j=0;
        }
        else if(strcmp(splitStrings[0], "play")==0)
        {
            oi_play_song(0);
            for(i=0; i<2; i++)
            {
                for(j=0; j<30; j++)
                {
                    splitStrings[i][j]='\0';
                }
            }
            dec=0;
            for(i=0; i<30; i++)
            {
                str[i]='\0';
            }
            i=0;
            j=0;
        }
        else if(strcmp(splitStrings[0], "led")==0){
            int i=0;
            while(i<10){
                oi_setLeds(1,1,100,250);
                timer_waitMillis(100);
                oi_setLeds(1,1,0,250);
                i++;
            }
            for(i=0; i<2; i++)
                        for(j=0; j<30; j++)
                        {
                           splitStrings[i][j]='\0';
                        }
                        dec=0;
                        for(i=0; i<30; i++)
                           str[i]='\0';
                        i=0;
                        j=0;
        }
        dec=0; // don't ask why and don't delete it
    }
}
