#include <iostream>
#include <opencv2/opencv.hpp>

float w = 250, h = 350;
cv::Mat matrix, imgWarp;

int main(){
    cv::Mat img = cv::imread("/home/qtl/桌面/Visual Studio Code/imgs/RGB.png");

    cv::imshow("Img", img);
    std::cout<<img.size()<<std::endl;   //显示图像大小

    cv::Point2f src[4] = {{529, 146}, {671, 190}, {405, 395}, {574, 457}};      //本地图像取点
    cv::Point2f dst[4] = {{0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, h}};          //目标图像点

    for(int i = 0; i < 4; i++){
        cv::circle(img, src[i], 10, cv::Scalar(0, 0, 255), cv::FILLED);         //本地图像取点画圆
    }
    
    matrix = cv::getPerspectiveTransform(src, dst);                             //计算透视变换
    cv::warpPerspective(img, imgWarp, matrix, cv::Point(w, h));                 //执行透视变换

    cv::imshow("Img", img);
    cv::imshow("Img_Warp", imgWarp);
    cv::waitKey(0);
    return 0;
}
