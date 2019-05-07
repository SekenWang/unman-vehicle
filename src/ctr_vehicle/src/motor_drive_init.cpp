#include "motor_drive_init.h"
#include <thread>
#include <wiringPi.h>
#include <softPwm.h>
#include <stdlib.h>
using namespace std;

extern int count[4];

void motor_drive_init()
{

    if(wiringPiSetup() == -1) //wiringPi initialize using the wPimode
    {
        cerr << "Setup wiringPi failed!" << endl;
    }

/*********MOTOR_CTR_IO_SET***********/
//left_front_wheel
    pinMode(PWM_LEFT_FRONT,OUTPUT);//pwm
    pinMode(A_LEFT_FRONT,OUTPUT);//a
    pinMode(B_LEFT_FRONT,OUTPUT);//b
//left_behind_wheel
    pinMode(PWM_LEFT_BEHIND,OUTPUT);//pwm
    pinMode(A_LEFT_BEHIND,OUTPUT);//a
    pinMode(B_LEFT_BEHIND,OUTPUT);//b
//right_front_wheel
    pinMode(PWM_RIGHT_FRONT,OUTPUT);//pwm
    pinMode(A_RIGHT_FRONT,OUTPUT);//a
    pinMode(B_RIGHT_FRONT,OUTPUT);//b
//right_behind_wheel
    pinMode(PWM_RIGHT_BEHIND,OUTPUT);//pwm
    pinMode(A_RIGHT_BEHIND,OUTPUT);//a
    pinMode(B_RIGHT_BEHIND,OUTPUT);//b
//softPwmCreate(int pin, int initialValue, int pwmRange))
    softPwmCreate(PWM_LEFT_FRONT  ,0,100);
    softPwmCreate(PWM_LEFT_BEHIND ,0,100);
    softPwmCreate(PWM_RIGHT_FRONT ,0,100);
    softPwmCreate(PWM_RIGHT_BEHIND,0,100);

/******HOWER_SENSOR_IO_SET*****/
    pinMode(HOWER_LEFT_FRONT_A,INPUT);
    pinMode(HOWER_LEFT_FRONT_B,INPUT);
    pinMode(HOWER_LEFT_BEHIND_A,INPUT);
    pinMode(HOWER_LEFT_BEHIND_B,INPUT);
    pinMode(HOWER_RIGHT_FRONT_A,INPUT);
    pinMode(HOWER_RIGHT_FRONT_B,INPUT);
    pinMode(HOWER_RIGHT_BEHIND_A,INPUT);
    pinMode(HOWER_RIGHT_BEHIND_B,INPUT);
    /**set the pull up resistor**/
    pullUpDnControl(HOWER_LEFT_FRONT_A,PUD_UP);
    pullUpDnControl(HOWER_LEFT_FRONT_B,PUD_UP);
    pullUpDnControl(HOWER_LEFT_BEHIND_A,PUD_UP);
    pullUpDnControl(HOWER_LEFT_BEHIND_B,PUD_UP);
    pullUpDnControl(HOWER_RIGHT_FRONT_A,PUD_UP);
    pullUpDnControl(HOWER_RIGHT_FRONT_B,PUD_UP);
    pullUpDnControl(HOWER_RIGHT_BEHIND_A,PUD_UP);
    pullUpDnControl(HOWER_RIGHT_BEHIND_B,PUD_UP);

    cout << "PI BOARD HAS BEEN INITILIZED!" << endl;
}


//depend on the cmd_velocity input to drive the motors
void start(float pwm[4])
{
	if (pwm[0] > 0 )
	{
	digitalWrite(A_LEFT_FRONT,HIGH);
	digitalWrite(B_LEFT_FRONT,LOW);
	
	}
	if (pwm[0] < 0 )
	{
	digitalWrite(A_LEFT_FRONT,LOW);
	digitalWrite(B_LEFT_FRONT,HIGH);
	}

	if (pwm[1] > 0 )
	{
	digitalWrite(A_RIGHT_FRONT,HIGH);
	digitalWrite(B_RIGHT_FRONT,LOW);
	
	}
	if (pwm[1] < 0 )
	{
	digitalWrite(A_RIGHT_FRONT,LOW);
	digitalWrite(B_RIGHT_FRONT,HIGH);
	}

	if (pwm[2] > 0 )
	{
	digitalWrite(A_LEFT_BEHIND,HIGH);
	digitalWrite(B_LEFT_BEHIND,LOW);
	
	}
	if (pwm[2] < 0 )
	{
	digitalWrite(A_LEFT_BEHIND,LOW);
	digitalWrite(B_LEFT_BEHIND,HIGH);
	}
	if (pwm[3] > 0 )
	{
	digitalWrite(A_RIGHT_BEHIND,HIGH);
	digitalWrite(B_RIGHT_BEHIND,LOW);
	
	}
	if (pwm[3] < 0 )
	{
	digitalWrite(A_RIGHT_BEHIND,LOW);
	digitalWrite(B_RIGHT_BEHIND,HIGH);
	}
//for stable we subtract  the max speed to half 
    softPwmWrite(PWM_LEFT_FRONT  ,50 * abs(pwm[0]) );
    softPwmWrite(PWM_RIGHT_FRONT ,50 * abs(pwm[1]) );
    softPwmWrite(PWM_LEFT_BEHIND ,50 * abs(pwm[2]) );
    softPwmWrite(PWM_RIGHT_BEHIND,50 * abs(pwm[3]) );
}

void hower_feedback()
{
    int flag_left_front = wiringPiISR (HOWER_LEFT_FRONT_A, INT_EDGE_RISING, interup_left_front() );
    int flag_right_front = wiringPiISR (HOWER_RIGHT_FRONT_A, INT_EDGE_RISING, interup_right_front() );
    int flag_left_behind = wiringPiISR (HOWER_LEFT_BEHIND_A, INT_EDGE_RISING, interup_left_behind() );
    int flag_right_behind = wiringPiISR (HOWER_RIGHT_BEHIND_B, INT_EDGE_RISING, interup_right_behind() );
    if (flag_left_front && flag_left_behind && flag_right_behind && flag_right_front < 0 )
      {
                cerr<<"cant open the hower interupt!"<<endl;
      }
}
//get the velocity of every wheel and the state of forward or backward
void* interup_left_front()
{

 if(digitalRead(HOWER_LEFT_FRONT_B) = LOW)
 {
      count[0]++; //the left_front_wheel is forword
 }
 else
 {
    count[0]--;//the left_front_wheel is backward
 }
}

void* interup_right_front()
{

 if(digitalRead(HOWER_LEFT_BEHIND_B) = LOW)
 {
      count[1]++; //the left_front_wheel is forword
 }
 else
 {
      count[1]--;//the left_front_wheel is backward
 }
}
void* interup_left_behind()
{

 if(digitalRead(HOWER_RIGHT_FRONT_B) = LOW)
 {
      count[2]++; //the left_front_wheel is forword
 }
 else
 {
      count[2]--;//the left_front_wheel is backward
 }
}
void* interup_right_behind()
{

 if(digitalRead(HOWER_RIGHT_BEHIND_B) = LOW)
 {
      count[3]++; //the left_front_wheel is forword
 }
 else
 {
      count[3]--;//the left_front_wheel is backward
 }
}

