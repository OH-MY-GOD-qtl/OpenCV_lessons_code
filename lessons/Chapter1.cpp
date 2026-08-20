#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::Mat img;
    img = cv::imread("/home/qtl/桌面/Visual Studio Code/imgs/RGB.png");

    cv::imshow("Img", img);

    cv::waitKey(0);
    return 0;
}