#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Image.h"
#include "cv_bridge/cv_bridge.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui.hpp"
#include "geometry_msgs/Twist.h"
#include "pid.h"
#include "camera.h"
#include "sensor_msgs/Joy.h"

ros::Publisher pub_twist;
Pid_control pid;
cv::FileStorage filewriter("logdata.yml",cv::FileStorage::WRITE);
bool model_switch = false;

//this funciton: color is to do the color filter to find where are the black zone
void imageProcess_color( cv::Mat src , cv::Mat& color_bi_image ,uint threshold_min = 50 , uint threshold_max = 110)
{
    cv::cvtColor(src,src,CV_RGB2GRAY);
    //this loop is to select the pixel which in [threshold_min,threshold_max];
    for( int i = 0 ; i  <  src.rows ; ++i)
    {
        uchar* data = src.ptr<uchar>(i); // get the adress of the rows i
        for (int j = 0 ; j < src.cols ; ++j)
        {
            if ( data[j] < threshold_min || data[j] > threshold_max)
            {
                data[j] = 0;
            }
            else
            {
                data[j] = 255;
            }
        }
    }
    color_bi_image = src;
    //cv::threshold(color_bi_image,color_bi_image,threshold,255,CV_THRESH_BINARY_INV);
    //cv::imshow("the color filter",color_bi_image);
}


//this function only for the orient x to do the sobel compute
void imageProcess_sobel(cv::Mat src , cv::Mat& dst ,uint threshold_min = 200 ,uint threshold_max = 255)
{

    //cv::cvtColor(src_copy,src,CV_RGB2GRAY);
    cv::Sobel(src,src,-1,1,0);
    cv::convertScaleAbs(src,src);
    for( int i = 0 ; i  <  src.rows ; ++i)
    {
        uchar* data = src.ptr<uchar>(i); // get the adress of the rows i
        for (int j = 0 ; j < src.cols * src.channels(); ++j)
        {

            if ( data[j] < threshold_min || data[j] > threshold_max)
            {
                data[j] = 0;
            }
            else
            {
                data[j] = 255;
            }
        }
    }
    cv::cvtColor(src,dst,CV_RGB2GRAY);

    //cv::cvtColor(src,src,CV_RGB2GRAY);
    cv::threshold(dst,dst,100,255,CV_THRESH_BINARY);
    std::cout << "dst channels = " << dst.channels() << std::endl;
    //cv::imshow("the soble for x",dst);
}


void bird_eye_transform(cv::Mat src , cv::Mat& dst )
{
    cv::Point2f src_points[4] = { cv::Point2f(170,100),
                           cv::Point2f(150,240),
                           cv::Point2f(270,100),
                           cv::Point2f(290,240)};
    cv::Point2f dst_points[4] = { cv::Point2f(150,100),
                           cv::Point2f(150,240),
                           cv::Point2f(280,100),
                           cv::Point2f(290,240)};

    cv::Mat transform,transform_inv;
    transform = cv::getPerspectiveTransform(src_points,dst_points);
    //transform_inv = cv::getPerspectiveTransform(dst_points,src_points);
    ;
    cv::warpPerspective(src , dst , transform , cv::Size(src.cols,src.rows) );
    //cv::warpPerspective(src , dst , transform , cv::Size(720,640) );
}

void imageProcess(cv::Mat src , cv::Mat& dst)
{
    uint threshold_min_color = 50 ,
         threshold_max_color = 110,
         threshold_min_sobel = 200,
         threshold_max_soble = 255;
    float weight_color = 0.5,
          weight_sobel = 0.5;
    cv::Mat dstImage_color,dstImage_sobel;

    imageProcess_color(src,dstImage_color,threshold_min_color,threshold_max_color);
    imageProcess_sobel(src,dstImage_sobel,threshold_min_color,threshold_max_soble);

    cv::addWeighted(dstImage_color,weight_color,dstImage_sobel,weight_sobel,0,dst);
    cv::threshold(dst,dst,150,255,CV_THRESH_BINARY); //here we get the pulic feature of the image
    bird_eye_transform(dst,dst);

}

cv::Point find_lane_point(const cv::Mat& src)
{
    cv::Mat src_copy = src.clone();
    //std::vector<cv::Point> vec_pos ;

    cv::Point positon(0,0);
//TODO :THIS LOOP ONLY GET THE ONE OF THE LANE,NEED TO DIVIDE THE IMAGE INTO 2 PARTS AND GET THE OTHER LINE!
    for (int i = 0 ; i < src_copy.cols ; ++i)
    {
        int max_count = 0;
        for (int j = 0 ; j < src_copy.rows ; ++j)
        {
            if (src_copy.at<uchar>(j,i) >= 120)
                max_count += 1;

        }
        if (max_count > positon.y)
        {
            positon.y = max_count;
            positon.x = i;
        }

    }
    return positon; //(x,y): x is the cols of the max , y is the max value
}


void img_callback(const sensor_msgs::ImageConstPtr &img_msg)
{
  try
  {
    cv::Mat srcImage = cv_bridge::toCvShare(img_msg,"bgr8")->image;
    /***************************TODO*************************/
    srcImage = srcImage(cv::Rect(100,240,440,240));
    cv::Mat dstImage;
    imageProcess(srcImage,dstImage);
    cv::Point Id_of_lane = find_lane_point(dstImage);

    /********************************************************/

    cv::Point2f cmd_vw;
    cmd_vw.x = 1; //set the speed and then caculate the rotate speed w


    cmd_vw.y = pid.PID_realize(Id_of_lane.x);

    /****************pub the twist command*******************/
    geometry_msgs::Twist twist_msg;

    twist_msg.linear.x  = cmd_vw.x;
    twist_msg.angular.z = cmd_vw.y;
    ROS_INFO("publish the cmd of w = %f",twist_msg.angular.z);
    pub_twist.publish(twist_msg);
    filewriter<< img_msg->header.frame_id <<  " \t " << cmd_vw.y << " \n";
    cv::waitKey(10);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("Could not convert from '%s' to 'bgr8'.", img_msg->encoding.c_str());
  }

}

void JoyCallback(const sensor_msgs::JoyConstPtr& joy_msg)
{
    if ( joy_msg->buttons[1] == 1)
    {
        model_switch = !model_switch;
    }

}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "autodrive_node");
    ros::NodeHandle nh;


    pid.PID_init();
    //ros::Subscriber sub = nh.subscribe("/cam0/image_raw", 10, img_callback);
    pub_twist = nh.advertise<geometry_msgs::Twist>("/vehicle/cmd_vel",10);
    joy_sub_ = nh.subscribe<sensor_msgs::Joy>("joy",1, JoyCallback);

    cv::VideoWriter video_writer;
    cv::Size videoSize(640,480);
    const std::string strFileName = "log.mp4";
    video_writer.open(strFileName,CV_FOURCC('D','I','V','X'),30,videoSize);


    filewriter << "NUM_ID \t rotate_vel \n";

    camera cam(0,640,480,30);
    geometry_msgs::Twist twist_msg;

    int frame_id = 0;
    ros::Rate loop_rate(30);

    ROS_INFO("autodrive_node has been initialized! \n");

    while(nh.ok() && cam.getstate())
    {

    if(model_switch != false) //if model switch is true,the twist_msg is send by this node;
                              //if model switch is false,the wist_msg is send by  the instructor_node;
                              //and we set the button[1] to switch the model_switch;
    {
    /*****************Here we do the initial process of picture************************************/
    cv::Mat srcImage,dstImage; //dst1,dst2 are the tempory varible
    //cv::MatND dstHist;
    //srcImage=cv::imread("0.jpg");
    srcImage = cam.image;
    srcImage = srcImage(cv::Rect(100,240,440,240));
    cv::imshow("src",srcImage);
    imageProcess(srcImage,dstImage);
    cv::imshow("dstImage",dstImage);


    cv::Point Id_of_lane = find_lane_point(dstImage);
    std::cout << "Id of lane = " << Id_of_lane << std::endl;
    cv::Point2f cmd_vw;
    cmd_vw.x = 1; //set the speed and then caculate the rotate speed w


    cmd_vw.y = pid.PID_realize(Id_of_lane.x);

    /****************pub the twist command*******************/


    twist_msg.linear.x  = cmd_vw.x;
    twist_msg.angular.z = cmd_vw.y;
    ROS_INFO("publish the cmd of w = %f",twist_msg.angular.z);
    pub_twist.publish(twist_msg);
    filewriter<< frame_id <<  " \t " << cmd_vw.y << " \n";

    std::string image_text;
    image_text = std::to_string(frame_id);
    cv::putText(srcImage,image_text,cv::Point(10,10),cv::FONT_HERSHEY_SIMPLEX,1,(0,0,255));
    video_writer.write(srcImage);

    frame_id += 1;
    cv::waitKey(10);
    loop_rate.sleep();
    /*****************************************************************************/
    }
    }
    cv::destroyAllWindows();
    //ros::spin();
    video_writer.release();
    filewriter.release();

    return 0;
}
