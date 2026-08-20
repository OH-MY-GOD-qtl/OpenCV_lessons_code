#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat img;
std::vector<std::vector<int>> myColor {{0, 179, 110, 255, 220, 255}};
cv::VideoCapture cap(0);


cv::Mat findColor(cv::Mat img){
    cv::Mat img_HSV, mask;
    cv::cvtColor(img, img_HSV, cv::COLOR_BGR2HSV);
    for(int i = 0; i < myColor.size(); i++){
        cv::Scalar lower(myColor[i][0], myColor[i][2], myColor[i][4]);
        cv::Scalar upper(myColor[i][1], myColor[i][3], myColor[i][5]);
        cv::inRange(img_HSV, lower, upper, mask);
    }
    return mask;
}


void getContours(cv::Mat img_binary){
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_binary, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<cv::Rect> boundRects;

    for(int i = 0; i < contours.size(); i++){
        int area = cv::contourArea(contours[i]);
        std::cout << i << " Area:" << area << std::endl;
        if(area > 50){
            float peri = cv::arcLength(contours[i], true);
            std::vector<cv::Point> conPoly;
            cv::approxPolyDP(contours[i], conPoly, 0.2*peri, true);
            cv::Rect rect = cv::boundingRect(conPoly);
            boundRects.push_back(rect);
            cv::rectangle(img, rect.tl(), rect.br(), cv::Scalar(0, 0, 255), 2);
        }
    }

    if (boundRects.size() >= 2) {
        cv::Rect r1 = boundRects[0];
        cv::Rect r2 = boundRects[1];
        cv::line(img, r1.tl() + cv::Point(r1.width, 0), r2.tl() + cv::Point(0, r2.height), cv::Scalar(0, 255, 0), 2);
        cv::line(img, r2.tl(), r1.br(), cv::Scalar(0, 255, 0), 2);
    }
}


int main(){

    system("v4l2-ctl -d /dev/video0 -c auto_exposure=1");
    system("v4l2-ctl -d /dev/video0 -c exposure_time_absolute=40");

    while(true){
        cap.read(img);
    
        cv::resize(img, img, cv::Size(), 0.7, 0.7);

        getContours(findColor(img));
        cv::imshow("target", img);
        cv::waitKey(1);
    }
    return 0;
}