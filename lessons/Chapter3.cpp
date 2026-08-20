#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::VideoCapture cap(0);                            //读摄像头
    cv::Mat img, img_Crop;
    while(true){
        cap.read(img);
        cv::resize(img ,img, cv::Size(), 0.5, 0.5);

        cv::Rect roi(100, 100, 100, 100);               //提取局部图像（左上，长宽）
        img_Crop = img(roi);
        
        cv::imshow("Img", img);
        cv::imshow("Img_Crop", img_Crop);
        cv::waitKey(1);
    }
    return 0;
}
