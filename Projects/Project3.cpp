#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::VideoCapture cap(0);
    cv::Mat img;

    cv::CascadeClassifier plateCascade;
    plateCascade.load("/usr/share/opencv4/haarcascades/haarcascade_russian_plate_number.xml");

    if(plateCascade.empty()) std::cout << "123" << std::endl;
    
    std::vector<cv::Rect> plate;
    
    while(true){
        cap.read(img);
        
        cv::resize(img, img, cv::Size(), 0.6, 0.6);
        plateCascade.detectMultiScale(img, plate, 1.1, 10);
    
        for(int i = 0; i < plate.size(); i++){
            cv::Mat img_Crop = img(plate[i]);
            cv::imshow(std::to_string(i), img_Crop);
            cv::rectangle(img, plate[i].tl(), plate[i].br(), cv::Scalar(255, 0, 255), 3);
    
        }
        cv::imshow("Video",img);
        cv::waitKey(1);
    }
    return 0;
}