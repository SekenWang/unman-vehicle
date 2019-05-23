#ifndef CAMERA_H
#define CAMERA_H
#include "ros/ros.h"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include "fstream"
#include "string"
#include "vector"

class camera
{
public:
    camera(const int& camera_id,const int& camera_width,const int& camera_height,const int& camera_fps);
    ~camera();
    bool save_iamge();
    const cv::Mat show_image();
    const bool getstate();
    cv::Mat image;
private:
    std::stringstream tempname;
    std::string filename;
    ros::Time time;
    cv::VideoCapture cap;


};

#endif // CAMERA_H
