#include <iostream>
#include <opencv2/opencv.hpp>

int main(){
    cv::VideoCapture cap(0);
    cv::Mat video;

    cv::CascadeClassifier faceCascade;
    faceCascade.load("/usr/share/opencv4/haarcascades/haarcascade_frontalface_default.xml");

    if(faceCascade.empty()) std::cout << "123" << std::endl;

    
    while(true){
        cap.read(video);
        
        cv::Mat video_val;
        cv::resize(video, video_val, cv::Size(), 0.6, 0.6);
        std::vector<cv::Rect> faces;
        faceCascade.detectMultiScale(video_val, faces, 1.1, 10);
    
        for(int i = 0; i < faces.size(); i++){
            cv::rectangle(video_val, faces[i].tl(), faces[i].br(), cv::Scalar(255, 0, 255), 3);
    
        }
        cv::imshow("Video",video_val);
        cv::waitKey(1);
    }
    return 0;
}