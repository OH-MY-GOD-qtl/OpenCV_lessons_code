#include <iostream>
#include <opencv2/opencv.hpp>


cv::Mat img;
std::vector<std::vector<int>> newPoints;

std::vector<std::vector<int>> myColors {{100, 150, 170, 255, 200, 255}, {30, 50, 50, 255, 100, 255}};   //我的颜色
std::vector<cv::Scalar> myColorValues {{255, 0, 255}, {0, 255, 0}};                                     //画线颜色

cv::Point getContours(cv::Mat img_Dilate){
    std::vector<std::vector<cv::Point>> contours;                                                       //向量容器
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_Dilate, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);      //二值图中检索轮廓
    
    std::vector<std::vector<cv::Point>> conPoly(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());

    cv::Point myPoint(0, 0);

    for(int i = 0; i < contours.size(); i++){
        
        int area = cv::contourArea(contours[i]);                                                        //计算包围面积
        std::cout << area << std::endl;                                                                 //显示包围面积大小
        if(area > 0 && area < 100000){                                                                  //筛选目标面积范围
            float peri = cv::arcLength(contours[i], true);                                              //计算轮廓周长
            cv::approxPolyDP(contours[i], conPoly[i], 0.02*peri, true);
            std::cout << conPoly.size() << std::endl;
            boundRect[i] = cv::boundingRect(conPoly[i]);
            
            myPoint.x = boundRect[i].x + boundRect[i].width / 2;
            myPoint.y = boundRect[i].y;
            
            cv::drawContours(img, contours, -1, cv::Scalar(255, 0, 255), 2);                            //绘制符合条件轮廓
            cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 255, 0), 5);         //绘制边界框
        }
    }
    return myPoint;
}
std::vector<std::vector<int>> findColor(cv::Mat img){                                                   //寻找目标颜色
    cv::Mat imgHSV;
    cv::cvtColor(img, imgHSV, cv::COLOR_BGR2HSV);                                                       //转灰度图

    for(int i = 0; i < myColors.size(); i++){                                                           //检测颜色
        cv::Scalar lower(myColors[i][0], myColors[i][2], myColors[i][4]);                               //检测颜色
        cv::Scalar upper(myColors[i][1], myColors[i][3], myColors[i][5]);                               //检测颜色

        cv::Mat mask;
        cv::inRange(imgHSV, lower, upper, mask);
        // cv::resize(mask, mask, cv::Size(), 0.6, 0.6);
        // cv::imshow(std::to_string(i), mask);
        cv::Point myPoint = getContours(mask);

        if(myPoint.x != 0 && myPoint.y != 0){

            newPoints.push_back({myPoint.x, myPoint.y, i});
        }
    }
    return newPoints;
}
void drawOnCanvas(std::vector<std::vector<int>> newPoints, std::vector<cv::Scalar> myColorValues){
    for(int i = 0; i < newPoints.size(); i++){

        cv::circle(img, cv::Point(newPoints[i][0], newPoints[i][1]), 10, myColorValues[newPoints[i][2]], cv::FILLED);
    }

}

int main(){
    cv::VideoCapture cap(0);
    
    while(true){
        cap.read(img);

        newPoints = findColor(img);
        drawOnCanvas(newPoints, myColorValues);


        cv::resize(img, img, cv::Size(), 0.6, 0.6);
        cv::imshow("Img", img);
        cv::waitKey(1);
    }
    return 0;
}