#include <iostream>
#include "pid.h"



void  Pid_control::PID_init()
{
    pid.SetPixel = 220.0;
    pid.ActualPixel = 0.0;
    pid.err = 0.0;
    pid.err_last = 0.0;
    pid.rotatevel = 0.0;
    pid.integral = 0.0;
    pid.Kp = 1.2;
    pid.Ki = 0.1;
    pid.Kd = 0.0;
    pid.umax = 1.5;
    pid.umin = -1.5;
}

float Pid_control::PID_realize(float feedbackpixel)
{
    int index;

    pid.ActualPixel = feedbackpixel;
    pid.err = pid.SetPixel - pid.ActualPixel;

    if (pid.rotatevel>pid.umax)
        {

            if (abs(pid.err) > 150.0)
                {
                    index = 0;
                }
            else
                {
                    index = 1;
                    pid.integral += pid.err;
                }

        }
    else if (pid.rotatevel<pid.umin)
        {
            if (abs(pid.err)>150)
                {
                    index = 0;
                }
            else{
                index = 1;
                if (pid.err>0)
                    {
                        pid.integral += pid.err;
                    }
                }
        }
    else
        {
            if (abs(pid.err)>150)
                {
                    index = 0;
                }
            else{
                    index = 1;
                    pid.integral += pid.err;
                }
        }

    pid.rotatevel = ( pid.Kp*pid.err + index*pid.Ki*pid.integral + pid.Kd*(pid.err - pid.err_last) )/220;
    pid.err_last = pid.err;


    return pid.rotatevel;
}

/*
main.cpp
#include "pid.h"
#include <iostream>

using namespace std;

int main()
{
Pid_control Pid;

Pid.PID_init();

int count = 0;

while (count<1000)
{
float speed = Pid.PID_realize(200.0);

cout << speed << ";" << " ";

count++;
}
cout << endl;

system("pause");

return 0;
}
*/
