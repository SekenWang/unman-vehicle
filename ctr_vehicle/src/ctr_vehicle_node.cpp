#include <ros/ros.h>
#include "motor_drive_init.h"
#include "geometry_msgs/Twist.h"
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <stdio.h>
#include "pthread.h"


int speed_count_left_front,
    speed_count_right_front,
    speed_count_left_behind,
    speed_count_right_behind;

ros::NodeHandle nh;

PI_THREAD (left_front_count)
{
    while( ros::ok() )
    {

        if( digitalRead(HOWER_LEFT_FRONT_A) == HIGH )
        {
          if(digitalRead(HOWER_LEFT_FRONT_B) == LOW )
          {
            piLock(0);
            speed_count_left_front += 1;
            piUnlock(0);
          }
          else(digitalRead(HOWER_LEFT_FRONT_B) == HIGH)
          {
            piLock(0);
            speed_count_left_front -= 1;
            piUnlock(0);
          }
        }
    }
    return NULL;
}

PI_THREAD (left_behind_count)
{
    while( ros::ok() )left_front_count
    {

        if( digitalRead(HOWER_LEFT_BEHIND_A) == HIGH )
        {
          if(digitalRead(HOWER_LEFT_BEHIND_B) == LOW )
          {
            piLock(1);
            speed_count_left_behind += 1;
            piUnlock(1);
          }
          else(digitalRead(HOWER_LEFT_BEHIND_B) == HIGH)
          {
            piLock(1);
            speed_count_left_behind -= 1;
            piUnlock(1);
          }
        }
    }
    return NULL;
}

PI_THREAD (right_front_count)
{
    while( ros::ok() )
    {

        if( digitalRead(HOWER_RIGHT_FRONT_A) == HIGH )
        {
          if(digitalRead(HOWER_RIGHT_FRONT_B) == LOW )
          {
            piLock(2);
            speed_count_right_front += 1;
            piUnlock(2);
          }
          else(digitalRead(HOWER_RIGHT_FRONT_B) == HIGH)
          {
            piLock(2);
            speed_count_right_front -= 1;
            piUnlock(2);
          }
        }
    }
    return NULL;
}

PI_THREAD (left_right_count)
{
    while( ros::ok() )
    {

        if( digitalRead(HOWER_RIGHT_BEHIND_A) == HIGH )
        {
          if(digitalRead(HOWER_RIGHT_BEHIND_B) == LOW )
          {
            piLock(3);
            speed_count_right_behind += 1;
            piUnlock(3);
          }
          else(digitalRead(HOWER_RIGHT_BEHIND_B) == HIGH)
          {
            piLock(3);
            speed_count_right_behind -= 1;
            piUnlock(3);
          }
        }
    }
    return NULL;
}

void cmdCallback(const geometry_msgs::Twist & cmd_vel)//subscirbe cmd_vel messages from ROS
{
    //ROS_INFO("Received a /cmd_vel message!");
    //ROS_INFO("the vel_move = %f\nthe vel_rotate = %f\n", cmd_vel.linear.x , cmd_vel.angular.z);
    vehicle.turn(cmd_vel.linear.x,cmd_vel.angular.z,vehicle.pwm);
	
    start(vehicle.pwm);
}
int main(int argc, char **argv)
{

    FILE *hower_speed_log;
    ros::init(argc, argv, "ctr_vehicle_node");

    pid param_pid;
    run vehicle;

    hower_speed_log = fopen("~/log/logout.txt","w");
    if(hower_speed_log = NULL)
    {
        std::cerr << "cant open save howerspeed file!" << std::endl;
    }

    fprintf(hower_speed_log,"time \t speed[0] \t speed[1] \t speed[2] \t speed[3] \n");


    //ros::Publisher feedback_vel=nh.advertise<geometry_msgs::Twist>("vehicle/feedback_vel",1);
    param_pid.pid_value_init();

    if(wiringPiSetup() == -1) //wiringPi initialize using the wiringPi mode
    {
        cerr << "Setup wiringPi failed!" << endl;
    }

    motor_drive_init();
    cout << "start" << endl;

    double rotaion_speed_feed[4];

    //creat four theads to get the rotaion speed of every motor;
    if(piThreadCreate(left_front_count) || piThreadCreate(left_behind_count) ||  piThreadCreate(right_front_count) || piThreadCreate(left_right_count) != 0 )
    {
        cerr << "cant creat a new process! " << endl;
    }

    piHiPri(5);

    auto time_last = ros::Time::now();
    while(ros::ok())
    {
      ros::Subscriber sub = nh.subscribe("vehicle/cmd_vel" , 1 , &cmdCallback);
      auto time_now = ros::Time::now();
      if( (time_now-time_last).toSec() >= 0.1 ) //during the set 0.1 second , get the rotation vel;
      {

          double time_duration = (time_now-time_last).toSec();


          rotaion_speed_feed[0] = speed_count_left_front / time_duration;
          rotaion_speed_feed[1] = speed_count_right_front / time_duration;
          rotaion_speed_feed[2] = speed_count_left_behind / time_duration;
          rotaion_speed_feed[3] = speed_count_right_behind / time_duration;

          speed_count_left_behind = 0;
          speed_count_left_front = 0;
          speed_count_right_behind = 0;
          speed_count_right_front = 0;
          time_last = ros::Time::now();


          fprintf(hower_speed_log,"%lf \t %lf \t %lf \t %lf \t %lf \n",time_last.toSec(),
                                                                        rotaion_speed_feed[0],
                                                                        rotaion_speed_feed[1],
                                                                        rotaion_speed_feed[2],
                                                                        rotaion_speed_feed[3]);

      }

      ros::spin();
    }



    fclose(hower_speed_log);

    return 0;

}
