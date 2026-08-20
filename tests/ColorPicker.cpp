#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat img, imgHSV, mask;
int hmin = 0, smin = 0, vmin = 0;
int hmax = 179, smax = 255, vmax = 255;

cv::VideoCapture cap(0);

int main(){
    
    system("v4l2-ctl -d /dev/video0 -c auto_exposure=1");
    system("v4l2-ctl -d /dev/video0 -c exposure_time_absolute=40");


    cv::namedWindow("Trackbars", cv::WINDOW_NORMAL);
    // cv::resizeWindow("Trackbars", 640, 600);
    cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);     
    cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
    cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
    cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
    cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);
    
    while(true){
        
        cap.read(img);
        // cv::resize(img, img, cv::Size(), 0.6, 0.6); 

        cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
        
        cv::Scalar lower(hmin, smin, vmin);
        cv::Scalar upper(hmax, smax, vmax);
        cv::inRange(imgHSV, lower, upper, mask);
        

        cv::imshow("Video", img);
        // cv::imshow("VideoHSV", imgHSV);
        cv::imshow("Videomask", mask);

        cv::waitKey(1);
    }
    return 0;
}



// cv::Mat img, imgHSV, mask;
// int hmin = 0, smin = 0, vmin = 0;
// int hmax = 179, smax = 255, vmax = 255;


// int main(){
    
//     cv::namedWindow("Trackbars", cv::WINDOW_NORMAL);
//     cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);     
//     cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
//     cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
//     cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
//     cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
//     cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);
    
//     while(true){
//         img = cv::imread("/home/qtl/桌面/Visual Studio Code/imgs/111.jpg");
//         cv::resize(img, img, cv::Size(), 0.2, 0.2);
//         cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);
        
//         cv::Scalar lower(hmin, smin, vmin);
//         cv::Scalar upper(hmax, smax, vmax);
//         cv::inRange(imgHSV, lower, upper, mask);
        
        
//         cv::imshow("Video", img);
//         cv::imshow("Videomask", mask);

//         cv::waitKey(1);
//     }
//     return 0;
// }