#include <ros/ros.h>
#include "motor_drive_init.h"
#include "geometry_msgs/Twist.h"
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>


pid param_pid;
run vehicle;
int count[4];
float vel_feedback[4];


PI_THREAD (feedback_thread)
{
    auto time_last = ros::Time::now();
    fprintf("Time\t speed[left_front]\t speed[right_front]\t speed[left_behind]\t speed[right_behind]");
    while(ros::ok())
    {
        hower_feedback();
        auto time_scale = ros::Time::now() - time_last;
        if(time_scale.toSec() >= 0.2)
        {

            for(int i = 0 , i != 4 ,i++)
            {
                vel_feedback[i] = count[i] / time_scale;
                count[i] = 0;
                vel_feedback[i] = vel_feedback[i] * 0.135 / (18.8 * 2 * 3.1415926);
            }

        std::cout << "vel_feedback = " << vel_feedback << std::endl;
        time_last = ros::Time::now();
        fprintf("%lf\t %f\t %f\t %f\t",time_last,vel_feedback[0],vel_feedback[1],vel_feedback[2],vel_feedback[3]);

        }
    }
    fclose(hower_speed_log);

}
*/
void cmdCallback(const geometry_msgs::Twist & cmd_vel)//subscirbe cmd_vel messages from ROS
{
    ROS_INFO("Received a /cmd_vel message!");
    ROS_INFO("the vel_move = %f\nthe vel_rotate = %f\n", cmd_vel.linear.x , cmd_vel.angular.z);
    vehicle.turn(cmd_vel.linear.x,cmd_vel.angular.z,vehicle.pwm);
	
    start(vehicle.pwm);



}
int main(int argc, char **argv)
{
    FILE *hower_speed_log;
    hower_speed_log = fopen("./logout.txt","w");
    if(hower_speed_log = NULL)
    {
        std::cerr << "cant open save howerspeed file!" << std::endl;
    }

    ros::init(argc, argv, "ctr_vehicle_node");
    ros::NodeHandle nh;
    //ros::Publisher feedback_vel=nh.advertise<geometry_msgs::Twist>("vehicle/feedback_vel",1);
    param_pid.pid_value_init();
    int flag_thread = piThreadCreate(feedback_thread);
    if(flag_thread != 0 )
    {
        cerr << "cant creat a new process! " << endl;
    }

	motor_drive_init();
	cout << "start" << endl;
    ros::Subscriber sub = nh.subscribe("vehicle/cmd_vel" , 1 , &cmdCallback);

    ros::spin();

    return 0;

}
