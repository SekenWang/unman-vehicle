#include "camera.h"

camera::camera(const int& camera_id,const int& camera_width,const int& camera_height,const int& camera_fps)
{
    cap.open(camera_id);
    cap.set(CV_CAP_PROP_FRAME_HEIGHT,camera_height);
    cap.set(CV_CAP_PROP_FRAME_WIDTH,camera_width);
    cap.set(CV_CAP_PROP_FPS,camera_fps);

    if(!cap.isOpened())
    {
        std::cerr << "Open camera error!" <<std::endl;

    }
    image = cv::Mat(camera_height,camera_width,CV_8UC3,cv::Scalar(0,0,255));
}


camera::~camera()
{
    cap.release();
    filename.clear();
    tempname.clear();
}

bool camera::save_iamge()
{
    show_image();

    if(!image.empty())
    {
        time = ros::Time::now();
        tempname << time;
        tempname >> filename;
        filename = filename + ".jpg";
        cv::imwrite(filename,image);
    }
    else
    {
        std::cerr << "image is empty!"<< std::endl;
        return false;
    }
    tempname.clear();
    filename.clear();
    return true;
}

const cv::Mat camera::show_image()
{

    cap >> image;


    if(!image.empty())
    {
        cv::imshow("image",image);

    }
    else
    {
        std::cerr << "image is emty at show image!" << std::endl;

    }
    std::cout << "the type of gray image = " << image.size <<image.type() << std::endl;
    return this->image;
}

const bool camera::getstate()
{
    return cap.isOpened();
}
