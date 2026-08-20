#include <iostream>
#include <opencv2/opencv.hpp>

cv::Mat img, img_Gray, img_Blur, img_Canny, img_Dilate, img_Erode, img_Ther, img_Warp;
std::vector<cv::Point> initialPoints, docPoints;
float w = 420, h = 596;
cv::VideoCapture cap(0);


cv::Mat preProcessing(cv::Mat img){                                                 //预处理图像

    cv::cvtColor(img, img_Gray, cv::COLOR_BGR2GRAY);                                //转灰度图
    cv::GaussianBlur(img, img_Blur, cv::Size(3, 3), 0, 0);                          //高斯模糊
    cv::Canny(img_Blur, img_Canny, 25, 75);                                         //边缘检测

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));     //结构元素
    cv::dilate(img_Canny, img_Dilate, kernel);                                      //膨胀
    cv::erode(img_Dilate, img_Erode, kernel);                                       //腐蚀

    return img_Dilate;                                                              //返回膨胀图
}


std::vector<cv::Point> getContours(cv::Mat img_binary){                             //获取边界轮廓

    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierachy;
    cv::findContours(img_binary, contours, hierachy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    std::vector<std::vector<cv::Point>> conPoly(contours.size());                   //轮廓多边形逼近后点集
    std::vector<cv::Rect> boundRect(contours.size());                               //外接正矩形
    std::vector<cv::Point> biggest;                                                 //最大矩形角点集
    int maxArea = 0;

    for(int i = 0; i < contours.size(); i++){
        int area = cv::contourArea(contours[i]);                                    //计算轮廓面积
        std::cout << "Area:" << area << std::endl;

        if(area > 100 && area < 200000){                                            //面积阈值
            float peri = cv::arcLength(contours[i], true);                          //计算轮廓周长
            cv::approxPolyDP(contours[i], conPoly[i], 0.05*peri, true);             //多边形逼近

            if(area > maxArea || conPoly[i].size() == 4){
                cv::drawContours(img, conPoly, i, cv::Scalar(255, 0, 255), 4);      //绘制当前轮廓
                biggest = {conPoly[i][0], conPoly[i][1], conPoly[i][2], conPoly[i][3]};
                maxArea = area;
            }
        }
    }
    return biggest;                                                                 //返回最大矩形角点集
}


void drawPoints(std::vector<cv::Point> points, cv::Scalar color){                   //画点
    for(int i = 0;i < points.size(); i++){
        cv::circle(img, points[i], 10, color, cv::FILLED);
        cv::putText(img, std::to_string(i), points[i], cv::FONT_HERSHEY_PLAIN, 4, color, 4);
    }
    
}


std::vector<cv::Point> reorder(std::vector<cv::Point> points){
    std::vector<cv::Point> newPoints;
    std::vector<int> sumPoints, subPoints;

    for(int i = 0; i < 4; i++){
        sumPoints.push_back(points[i].x + points[i].y);
        subPoints.push_back(points[i].x - points[i].y);
    }
    newPoints.push_back(points[min_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);
    newPoints.push_back(points[max_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
    newPoints.push_back(points[min_element(subPoints.begin(), subPoints.end()) - subPoints.begin()]);
    newPoints.push_back(points[max_element(sumPoints.begin(), sumPoints.end()) - sumPoints.begin()]);

    return newPoints;
}


cv::Mat getWarp(cv::Mat img, std::vector<cv::Point> points, float w, float h){
    cv::Point2f src[4] = {points[0], points[1], points[2], points[3]};
    cv::Point2f dst[4] = {{0.0f, 0.0f}, {w, 0.0f}, {0.0f, h}, {w, h}};

    cv::Mat matrix = cv::getPerspectiveTransform(src, dst);
    cv::warpPerspective(img, img_Warp, matrix, cv::Point(w, h));

    return img_Warp;
}


// int main(){
//     img = cv::imread("/home/qtl/桌面/Visual Studio Code/imgs/333.png");
//     cv::resize(img, img, cv::Size(), 0.4, 0.4);
//     img_Ther = preProcessing(img);
//     initialPoints = getContours(img_Ther);
//     drawPoints(initialPoints, cv::Scalar(0, 0, 255));
//     docPoints = reorder(initialPoints);
//     drawPoints(docPoints, cv::Scalar(0, 255, 0));
//     cv::imshow("img0", img);
//     cv::imshow("img_Dilate", img_Ther);
//     cv::waitKey(0);
//     return 0;
// }


int main(){
    while(true){
        cap.read(img);
        cv::resize(img, img, cv::Size(), 0.4, 0.4);

        img_Ther = preProcessing(img);

        initialPoints = getContours(img_Ther);
        docPoints = reorder(initialPoints);
        drawPoints(docPoints, cv::Scalar(0, 0, 255));

        img_Warp = getWarp(img, docPoints, w, h);

        cv::imshow("img0", img);
        cv::imshow("img_Dilate", img_Ther);
        cv::imshow("img_Warp", img_Warp);
        cv::waitKey(1);
    }
    return 0;
}