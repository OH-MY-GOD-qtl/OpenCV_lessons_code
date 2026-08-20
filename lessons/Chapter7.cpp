#include <iostream>
#include <opencv2/opencv.hpp>

void getContours(cv::Mat img_Dilate, cv::Mat img){
    std::vector<std::vector<cv::Point>> contours;                                                       //向量容器
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(img_Dilate, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);      //二值图中检索轮廓
    
    std::vector<std::vector<cv::Point>> conPoly(contours.size());
    std::vector<cv::Rect> boundRect(contours.size());
    
    for(int i = 0; i < contours.size(); i++){
        
        std::string objectType;

        int area = cv::contourArea(contours[i]);                                                        //计算包围面积
        std::cout << area << std::endl;                                                                 //显示包围面积大小
        if(area > 4000 && area < 8000){
            float peri = cv::arcLength(contours[i], true);                                              //计算轮廓周长
            cv::approxPolyDP(contours[i], conPoly[i], 0.02*peri, true);
            std::cout << conPoly.size() << std::endl;
            boundRect[i] = cv::boundingRect(conPoly[i]);
            
            int objCor = (int)conPoly[i].size();
            
            if(objCor == 3) objectType = "Tri";
            if(objCor == 4) {
                float aspRatio = (float)boundRect[i].width / boundRect[i].height;                       //长宽比判定正方形
                std::cout << aspRatio << std::endl;                                                     //打印长宽比
                if(aspRatio > 0.85 && aspRatio < 1.15) objectType = "Square";                           //识别正方形
                else objectType = "Rect";                                                               //识别矩形
            }
            if(objCor > 4) objectType = "Circle";

            cv::drawContours(img, contours, -1, cv::Scalar(0, 255, 0), 2);                              //绘制符合条件轮廓
            cv::rectangle(img, boundRect[i].tl(), boundRect[i].br(), cv::Scalar(0, 0, 255), 5);
            cv::putText(img, objectType, {boundRect[i].x + 5, boundRect[i].y + 15},cv::FONT_HERSHEY_PLAIN, 0.75, cv::Scalar(0, 69, 255), 1);

        }
    }
}
int main(){
    cv::Mat img, img_Gray, img_Blur, img_Canny, img_Dilate;
    img = cv::imread("/home/qtl/桌面/Visual Studio Code/imgs/321.png");
    cv::resize(img, img, cv::Size(), 0.2, 0.2);

    cv::cvtColor(img, img_Gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(img, img_Blur, cv::Size(1, 1), 0, 0);
    cv::Canny(img_Blur, img_Canny, 25, 75);
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(img_Canny, img_Dilate, kernel);

    getContours(img_Dilate, img);

    cv::imshow("img", img);
    cv::imshow("img_Gray", img_Gray);
    cv::imshow("img_Blur", img_Blur);
    cv::imshow("img_Canny", img_Canny);
    cv::imshow("img_Dilate", img_Dilate);

    cv::waitKey(0);
    return 0;
}