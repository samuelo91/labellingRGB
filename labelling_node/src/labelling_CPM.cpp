#include "ros/ros.h"
#include "std_msgs/String.h"

#include "PoseEstimatorCPM.hpp"
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <image_transport/image_transport.h>
#include <cv_bridge/cv_bridge.h>
#include <geometry_msgs/QuaternionStamped.h>

cv_bridge::CvImagePtr cv_ptr;
geometry_msgs::QuaternionStamped mQuaternion;



void imageCallback(const sensor_msgs::ImageConstPtr& msg) {

    try {
        cv_ptr = cv_bridge::toCvCopy(msg, sensor_msgs::image_encodings::BGR8);
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
}

void quaternionCallback(const geometry_msgs::QuaternionStamped& msg) {

    try {
        mQuaternion = msg;
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("cv_bridge exception: %s", e.what());
        return;
    }
}

int main(int argc, char **argv)
{
    sleep(1);
    ros::init(argc, argv, "labelling_CPM");
    ros::NodeHandle nh_;
    image_transport::ImageTransport it_(nh_);

    std::string ns = nh_.getNamespace();
    image_transport::Subscriber mImageSub = it_.subscribe(ns + "/real/image_raw", 1, imageCallback);
    ros::Subscriber mCameraQuaternionSub = nh_.subscribe(ns + "/real/metadata/camera_quat", 1, quaternionCallback);

    PoseEstimatorCPM pose_estimator;

    while (true) {

        ros::spinOnce();
        cv::Mat img = cv_ptr->image;
        cv::Mat croppedImg = img(cv::Rect(363, 25, 90, 225));

        std::vector<cv::Point> limbs = pose_estimator.detectLimbs(croppedImg);
//        for (cv::Point limb: limbs) {
//          cv::circle(croppedImg, limb, 2, cv::Scalar(0, 255, 0), 3);
//        }

//        cv::namedWindow("Display View", CV_WINDOW_AUTOSIZE);
//        cv::imshow("Display View", croppedImg);
//        cv::waitKey(0);
    }


    return 0;
}
