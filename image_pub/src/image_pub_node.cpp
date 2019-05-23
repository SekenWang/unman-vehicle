#include "camera.h"
#include "sensor_msgs/Image.h"
#include <cv_bridge/cv_bridge.h>
#include "image_transport/image_transport.h"

int main(int argc, char **argv)
{
    /*if(argc < 2)
    {
        std::cerr << "please input the node name and topic name" << std::endl;
    }
    */
    ros::init( argc , argv , "image_pub_node");

    ros::NodeHandle n;

    image_transport::ImageTransport it(n);
    image_transport::Publisher img_pub = it.advertise("/cam0/image_raw",1);
    sensor_msgs::ImagePtr img_msg;
    cv_bridge::CvImageConstPtr ptr;


    ros::Rate loop_rate(30);

    std::cout << "2222"<<std::endl;
    camera cam(1,640,480,30);
    while (ros::ok() && cam.getstate())
    {
        std_msgs::Header header;
        header.stamp = ros::Time::now();

        img_msg = cv_bridge::CvImage(header,"bgr8", cam.image).toImageMsg();
        cam.show_image();
        if( cv::waitKey(10) == 's')
        {
           cam.save_iamge();
        }
        if( cv::waitKey(10) == 'q')
        {
            break;
        }
        img_pub.publish(img_msg);
        std::cout << "header = " << img_msg->header.stamp << std::endl;

        //ros::spinOnce();

        loop_rate.sleep();
    }

    return 0;
}

