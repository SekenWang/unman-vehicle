#ifndef MOTOR_DRIVE_INIT_H
#define MOTOR_DRIVE_INIT_H

#include <wiringPi.h>
#include <iostream>
#include <stdio.h>
#include <ros/ros.h>
#include "geometry_msgs/Twist.h"
#include <softPwm.h>

#define HOWER_LEFT_FRONT_A 8
#define HOWER_LEFT_FRONT_B 9
#define PWM_LEFT_FRONT 7
#define A_LEFT_FRONT 0
#define B_LEFT_FRONT 2

#define HOWER_LEFT_BEHIND_A 21
#define HOWER_LEFT_BEHIND_B 22
#define PWM_LEFT_BEHIND 23
#define A_LEFT_BEHIND 24
#define B_LEFT_BEHIND 25

#define HOWER_RIGHT_FRONT_A 15
#define HOWER_RIGHT_FRONT_B 16
#define PWM_RIGHT_FRONT 1
#define A_RIGHT_FRONT 4
#define B_RIGHT_FRONT 5

#define HOWER_RIGHT_BEHIND_A 31
#define HOWER_RIGHT_BEHIND_B 26
#define PWM_RIGHT_BEHIND 27
#define A_RIGHT_BEHIND 28
#define B_RIGHT_BEHIND 29



digitalRead

using namespace std;
//define pid struct variables


class run
{
public:
    float pwm[4]; //????pwm?????(-1,1)???1-???2-???3-???4-??
    void turn(float v, float p , float *pwm) //??????p?p>0???p<0??,v????????????pwm????-1?1???
{
	v=v*2;
	p=p*2;
        if (p >1||p<-1|| pwm == nullptr)
    {
            cerr << "illegal tele input error!" << endl;
            return;
    }

        if (p > 0)
    {
            if (v > 0)
            {
                 pwm[0] = v + (1 - v)*v*p; //??????????????????
			}            
			else
            {
                 pwm[0] = v + (1 + v)*v*p;
            }
                 pwm[2] = pwm[0];
                 pwm[1] = v*(1 - 2 * p);
                 pwm[3] = pwm[1];
        	
		
	}
        else
        {
            pwm[0] = v*(1 + 2 * p);
            pwm[2] = pwm[0];
            if (v > 0)
            {
               pwm[1] = v - (1 - v)*v*p;//??????????????????
            }
            else
            {
                pwm[1] = v - (1 + v)*v*p;
            }
            pwm[3] = pwm[1];
        }
     
}
private:

};

class pid
{
public:
    float SetSpeed[4];
    float ActualSpeed[4];
    float err[4];
    float err_last[4];
    float err_last_next[4];
    float Kp,Ki,Kd;


float PID_realize(float cmd_speed[4],float feedback_speed[4])
{

    float incrementSpeed[4] = {0};

    int i = 0;
    for (i = 0; i< 4; i++)
    {
        SetSpeed[i] = cmd_speed[i];
        ActualSpeed[i] = feedback_speed[i];

        err[i] = SetSpeed[i] - ActualSpeed[i];

        //increment PID
        incrementSpeed[i] = Kp * (err[i] -err_last[i] ) + Ki*err[i] + Kd*(err[i] -2* err_last[i] + err_last_next[i]);

        ActualSpeed[i] += incrementSpeed[i];
        err_last_next[i] = err_last[i];
        err_last[i] = err[i];

    }

    return ActualSpeed[4];
}

//(2) 初始化变量
void pid_value_init(void)
{
    cout << "pid_value_init begin \n" << endl;
    //system("pause");

    SetSpeed[4] = {0};
    ActualSpeed[4] = {0};
    err[4] = {0};
    err_last[4] = {0};
    err_last_next[4] = {0};

    Kp = 0.1;
    Ki = 0.15;
    Kd = 0.1;

    cout << "pid_value_init end \n" << endl;
}
};


void motor_drive_init();
void start(float pwm[4]);

void hower_feedback_on_left_front();
void hower_feedback_on_left_behind();
void hower_feedback_on_right_front();
void hower_feedback_on_right_behind();

void interup_left_front();
void interup_right_front();
void interup_left_behind();
void interup_right_behind();


#endif // MOTOR_DRIVE_INIT_H
