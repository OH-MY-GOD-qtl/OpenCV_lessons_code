#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::VideoCapture cap(0);
    cv::Mat img, img_Gray, img_Blur, img_Canny, img_Dilate, img_Erode;
    while(true){
        cap.read(img);
        cv::resize(img, img, cv::Size(), 0.5, 0.5);
        
        cv::cvtColor(img, img_Gray, cv::COLOR_BGR2GRAY);                                //转灰度图
        cv::GaussianBlur(img, img_Blur, cv::Size(3, 3), 0, 0);                          //高斯模糊（降噪、平滑、去除细节）
        cv::Canny(img_Blur, img_Canny, 25, 75);                                         //边缘检测



        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));     //生成形态学结构元素
        cv::dilate(img_Canny, img_Dilate, kernel);                                      //膨胀（连接断裂边缘）
        cv::erode(img_Dilate, img_Erode, kernel);                                       //腐蚀（去除毛刺、分离粘连）

        cv::imshow("img", img);
        cv::imshow("img_Gray", img_Gray);
        cv::imshow("img_Blur", img_Blur);
        cv::imshow("img_Canny", img_Canny);
        cv::imshow("img_Dilate", img_Dilate);
        cv::imshow("img_Erode", img_Erode);

        cv::waitKey(1);
    }
    return 0;
}
