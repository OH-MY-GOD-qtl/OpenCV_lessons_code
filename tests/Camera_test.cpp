#include <iostream>
#include <opencv2/opencv.hpp>

int main(){

    // system("v4l2-ctl -d /dev/video0 -c auto_exposure=1");
    // system("v4l2-ctl -d /dev/video0 -c exposure_time_absolute=40");
    cv::VideoCapture cap(0);
    cv::Mat video;
    while(true){
        cap.read(video);

        cv::resize(video, video, cv::Size(), 0.6, 0.6);
        cv::imshow("Video",video);
        cv::waitKey(1);
    }
    return 0;
}