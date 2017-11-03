#include <openpose/headers.hpp>
#include <twaipose/pose/poseWrapper.hpp>

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <std_msgs/String.h>
#include "sensor_msgs/image_encodings.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <sstream>

#include "sensor_msgs/Image.h"


namespace enc = sensor_msgs::image_encodings;
//using namespace sensor_msgs;
//image_transport::Publisher publish_result;
ros::Publisher publish_pose;
image_transport::Publisher publish_poseimage;
std_msgs::String pose;

static tp::PoseWrapper* poseWrapper = nullptr;

void imageCallback(const sensor_msgs::ImageConstPtr& msg) {
    cv_bridge::CvImagePtr cv_ptr;
    cv::Mat outimg;
    try {
        cv_ptr = cv_bridge::toCvCopy(msg, enc::BGR8);
    }

    catch (cv_bridge::Exception &e) {
        ROS_ERROR("cv_bridge exception is %s", e.what());
        return;
    }

    std::stringstream ss;
    std::string result = poseWrapper->getPoseKeypoint( cv_ptr->image, cv_ptr->image);
    ss<<result;
    pose.data= ss.str();
   // cv::imshow("IN", outimg);
    cv::waitKey(1);
    if (cv_ptr->image.empty())
      return;

    publish_pose.publish(pose);


    sensor_msgs::ImagePtr image = cv_ptr->toImageMsg();
    publish_poseimage.publish(image);
    /*
    // publish result image with annotation.
    if (!FLAGS_result_image_topic.empty()) {
        poseRenderer->renderPose(outputArray, poseKeypoints);
        faceRenderer->renderFace(outputArray, faceKeypoints);

        auto outputImage = opOutputToCvMat->formatToCvMat(outputArray);

        sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", outputImage).toImageMsg();
        publish_result.publish(msg);
    }*/
}

int main(int argc, char *argv[])
{
    ros::init(argc, argv, "openpose_ros_node");

    std::string resolution = "1280x720";
    std::string model_folder = "/home/liuzhenkun/work/openpose/models/";

    if (poseWrapper)
    {
        delete poseWrapper;
    }

    poseWrapper = new tp::PoseWrapper(model_folder, resolution);

    // subscribe image
    ros::NodeHandle nh;
    image_transport::ImageTransport img_t(nh);
    image_transport::Subscriber sub = img_t.subscribe("/image_raw", 1, imageCallback);

    publish_pose = nh.advertise<std_msgs::String>("/openpose/pose", 1);

    image_transport::ImageTransport nh_openpose(nh);
    publish_poseimage = nh_openpose.advertise("openpose_image",1);

    ros::spin();

    delete poseWrapper;
    poseWrapper=nullptr;
    return 0; 
}
