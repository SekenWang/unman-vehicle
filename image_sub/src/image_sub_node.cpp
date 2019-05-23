#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"

void img_callback(const sensor_msgs::ImageConstPtr &img_msg)
{
  try
  {
    cv::imshow("received", cv_bridge::toCvShare(img_msg, "bgr8")->image);
    cv::waitKey(30);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", img_msg->encoding.c_str());
  }
}


int main(int argc, char **argv)
{
    ros::init(argc, argv, "image_sub_node");
    ros::NodeHandle nh;

    ros::Subscriber sub = nh.subscribe("/cam0/image_raw", 1, img_callback);

    ros::spin();
    cv::destroyWindow("received");
    return 0;
}
