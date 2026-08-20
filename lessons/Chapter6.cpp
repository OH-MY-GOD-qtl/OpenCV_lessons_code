#include <iostream>
#include <opencv2/opencv.hpp>

int hmin = 0, smin = 110, vmin = 153;
int hmax = 19, smax = 240, vmax = 255;

int main(){
    cv::Mat img, imgHSV, mask;
    img = cv::imread("/home/qtl/桌面/Visual Studio Code/imgs/RGB.png");
    cv::resize(img, img, cv::Size(), 0.6, 0.6); 
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);               //转为HSV图

    cv::namedWindow("Trackbars", cv::WINDOW_NORMAL);            //创建滑动窗口
    // cv::resizeWindow("Trackbars", 640, 600);
    cv::createTrackbar("Hue Min", "Trackbars", &hmin, 179);     
    cv::createTrackbar("Hue Max", "Trackbars", &hmax, 179);
    cv::createTrackbar("Sat Min", "Trackbars", &smin, 255);
    cv::createTrackbar("Sat Max", "Trackbars", &smax, 255);
    cv::createTrackbar("Val Min", "Trackbars", &vmin, 255);
    cv::createTrackbar("Val Max", "Trackbars", &vmax, 255);

    while(true){
        cv::Scalar lower(hmin, smin, vmin);                     //构造HSV下界
        cv::Scalar upper(hmax, smax, vmax);                     //构造HSV上界
        cv::inRange(imgHSV, lower, upper, mask);                //满足条件像素变白存到mask里
        

        cv::imshow("Video", img);
        cv::imshow("VideoHSV", imgHSV);
        cv::imshow("Videomask", mask);
        cv::waitKey(1);
    }
    return 0;
}