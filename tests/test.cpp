#include <iostream>
#include <opencv2/opencv.hpp>

int main() {
    cv::VideoCapture cap(0);
    while(true){
        cv::Mat bgr_img;
        cap.read(bgr_img);

        cv::GaussianBlur(bgr_img, bgr_img, cv::Size(9, 9), 0);

        cv::Mat gray_img;
        cv::cvtColor(bgr_img, gray_img, cv::COLOR_BGR2GRAY);

        cv::Mat gray_small;
        cv::resize(gray_img, gray_small, cv::Size(), 0.3, 0.3);
        cv::imshow("gray", gray_small);

        cv::Mat binary_img;
        cv::threshold(gray_img, binary_img, 50, 255, cv::THRESH_BINARY);

        cv::Mat binary_small;
        cv::resize(binary_img, binary_small, cv::Size(), 0.3, 0.3);
        cv::imshow("binary", binary_small);

        std::vector<std::vector<cv::Point>> contours;
        cv::findContours(binary_img, contours, cv::RETR_EXTERNAL, cv:: CHAIN_APPROX_NONE);

        cv::Mat draw_contours = bgr_img.clone();
        cv::drawContours(draw_contours, contours, -1, cv::Scalar(0, 255, 0), 2);
        cv::Mat contour_small;
        cv::resize(draw_contours, contour_small, cv::Size(), 0.3, 0.3);
        cv::imshow("contours", contour_small);

        cv::Mat draw_rect = bgr_img.clone();
        for(size_t i=0; i < contours.size(); i++){
            cv::RotatedRect rect = cv::minAreaRect(contours[i]);
            cv::Point2f vertices[4];
            rect.points(vertices);
            for(int j = 0; j < 4; j++) cv::line(draw_rect, vertices[j], vertices[(j+1)%4], cv::Scalar(0, 0, 255), 2);
        }

        cv::Mat rect_small;
        cv::resize(draw_rect, rect_small, cv::Size(), 0.6, 0.6);
        cv::imshow("rotated rects", rect_small);

        cv::waitKey(1);
    }
    return 0;
}