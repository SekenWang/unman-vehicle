#include "ros/ros.h"
#include "sensor_msgs/Joy.h"
#include "geometry_msgs/Twist.h"
#include <sstream>


class tele_op_vehicle
{
public:
    tele_op_vehicle();

private:

    void JoyCallback(const sensor_msgs::Joy::ConstPtr& joy);

    ros::NodeHandle nh;

    int linear_,angular_;
    double l_scale_,a_scale_;
    ros::Publisher vel_pub_;
    ros::Subscriber joy_sub_;


};

tele_op_vehicle::tele_op_vehicle():
    linear_(1),
    angular_(3),
    l_scale_(0.5),
    a_scale_(0.5)
{

    nh.param("axis_linear",linear_,linear_);
    nh.param("axis_angular",angular_,angular_);
    nh.param("scale_angular",l_scale_,l_scale_);
    nh.param("scale_linear",a_scale_,a_scale_);


    //Here we create a publisher that will advertise on the command_velocity topic of the turtle.
    vel_pub_ = nh.advertise<geometry_msgs::Twist>("vehicle/cmd_vel",1);
    std::cout<<"test pub"<<std::endl;
    //Here we subscribe to the joystick topic for the input to drive the turtle.
    //If our node is slow in processing incoming messages on the joystick topic,
    //up to 10 messages will be buffered before any are lost.

    joy_sub_ = nh.subscribe<sensor_msgs::Joy>("joy",10, &tele_op_vehicle::JoyCallback, this);
    std::cout<<"test sub "<<std::endl;
}

void tele_op_vehicle::JoyCallback(const sensor_msgs::Joy::ConstPtr& joy)
{
    geometry_msgs::Twist twist;
    twist.angular.z = a_scale_ * joy->axes[angular_];
    twist.linear.x = l_scale_ * joy->axes[linear_];

    ROS_INFO("MOTION   VELOCITY = [%f]",twist.linear.x);

    ROS_INFO("ROTATION VELOCITY = [%f]",twist.angular.z);
    vel_pub_.publish(twist);
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "teleop_vehicle");
    std::cout << "the insturctor node  is initializing! " << std::endl;
    tele_op_vehicle teleop_vehicle;
    std::cout << "the insturctor node has been initialized! " << std::endl;

    ros::spin(); //hold on this process
}
