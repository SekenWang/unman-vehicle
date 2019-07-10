#ifndef _PID_H_
#define _PID_H_
#include "math.h"
typedef struct _pid{
float SetPixel;
float ActualPixel;
float err;
float err_last;
float Kp, Ki, Kd;
float rotatevel;
float integral;
float umax;
float umin;
}Pid;


class Pid_control
{
public:

void PID_init();
float PID_realize(float speed);

private:
int index;
Pid pid;
};
#endif
