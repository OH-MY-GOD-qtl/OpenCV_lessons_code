#include <opencv2/opencv.hpp>
#include <opencv2/calib3d.hpp>
#include <iostream>
#include <vector>

int main() {
    // ============ 1. 准备相机内参 ============
    // 假设使用普通USB摄像头，640x480分辨率
    double fx = 1.3598e+03;  // 焦距x
    double fy = 1.3598e+03;  // 焦距y
    double cx = 674.1557;  // 主点x
    double cy = 540.5553;  // 主点y
    
    cv::Mat cameraMatrix = (cv::Mat_<double>(3,3) << 
        fx, 0, cx,
        0, fy, cy,
        0, 0, 1);
    
    cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);  // 假设无畸变
    
    // ============ 2. 准备3D世界坐标点 ============
    std::vector<cv::Point3f> objectPoints;
    // 定义一个10cm x 10cm的正方形，4个角点
    objectPoints.push_back(cv::Point3f(0, 0, 0));
    objectPoints.push_back(cv::Point3f(10, 0, 0));
    objectPoints.push_back(cv::Point3f(10, 10, 0));
    objectPoints.push_back(cv::Point3f(0, 10, 0));
    
    // 添加更多点以提高精度
    objectPoints.push_back(cv::Point3f(5, 5, 0));  // 中心点
    
    // ============ 3. 准备对应的2D图像坐标点 ============
    // 这些是模拟点，实际使用中需要从图像中检测
    std::vector<cv::Point2f> imagePoints;
    imagePoints.push_back(cv::Point2f(250, 200));
    imagePoints.push_back(cv::Point2f(350, 200));
    imagePoints.push_back(cv::Point2f(350, 300));
    imagePoints.push_back(cv::Point2f(250, 300));
    imagePoints.push_back(cv::Point2f(300, 250));  // 中心点
    
    // ============ 4. PnP解算 ============
    // 旋转向量和平移向量
    cv::Mat rvec, tvec;
    
    // 方法1: SOLVEPNP_ITERATIVE (迭代法，最常用)
    bool success = cv::solvePnP(
        objectPoints,      // 3D世界坐标点
        imagePoints,       // 2D图像坐标点
        cameraMatrix,      // 相机内参矩阵
        distCoeffs,        // 畸变系数
        rvec,              // 输出：旋转向量
        tvec,              // 输出：平移向量
        false,             // 是否使用初始值
        cv::SOLVEPNP_ITERATIVE  // 求解方法
    );
    
    if (!success) {
        std::cerr << "PnP求解失败！" << std::endl;
        return -1;
    }
    
    // ============ 5. 结果输出 ============
    std::cout << "======= PnP解算结果 =======" << std::endl;
    
    // 旋转向量
    std::cout << "旋转向量(rvec): " << rvec.t() << std::endl;
    
    // 平移向量
    std::cout << "平移向量(tvec): " << tvec.t() << " cm" << std::endl;
    
    // ============ 6. 转换为旋转矩阵 ============
    cv::Mat rotationMatrix;
    cv::Rodrigues(rvec, rotationMatrix);
    
    std::cout << "\n旋转矩阵:" << std::endl;
    std::cout << rotationMatrix << std::endl;
    
    // ============ 7. 计算欧拉角（更直观） ============
    // 从旋转矩阵提取欧拉角 (ZYX顺序)
    double sy = sqrt(rotationMatrix.at<double>(0,0) * rotationMatrix.at<double>(0,0) + 
                     rotationMatrix.at<double>(1,0) * rotationMatrix.at<double>(1,0));
    
    bool singular = sy < 1e-6;
    double roll, pitch, yaw;
    
    if (!singular) {
        roll = atan2(rotationMatrix.at<double>(2,1), rotationMatrix.at<double>(2,2));
        pitch = atan2(-rotationMatrix.at<double>(2,0), sy);
        yaw = atan2(rotationMatrix.at<double>(1,0), rotationMatrix.at<double>(0,0));
    } else {
        roll = atan2(-rotationMatrix.at<double>(1,2), rotationMatrix.at<double>(1,1));
        pitch = atan2(-rotationMatrix.at<double>(2,0), sy);
        yaw = 0;
    }
    
    // 转换为角度
    roll = roll * 180.0 / CV_PI;
    pitch = pitch * 180.0 / CV_PI;
    yaw = yaw * 180.0 / CV_PI;
    
    std::cout << "\n欧拉角(度):" << std::endl;
    std::cout << "Roll (绕X轴):  " << roll << "°" << std::endl;
    std::cout << "Pitch (绕Y轴): " << pitch << "°" << std::endl;
    std::cout << "Yaw (绕Z轴):   " << yaw << "°" << std::endl;
    
    // ============ 8. 重投影验证 ============
    std::vector<cv::Point2f> projectedPoints;
    cv::projectPoints(objectPoints, rvec, tvec, cameraMatrix, distCoeffs, projectedPoints);
    
    std::cout << "\n======= 重投影误差 =======" << std::endl;
    double totalError = 0;
    for (size_t i = 0; i < imagePoints.size(); i++) {
        double error = cv::norm(imagePoints[i] - projectedPoints[i]);
        totalError += error;
        std::cout << "点" << i << ": 误差 = " << error << " 像素" << std::endl;
    }
    std::cout << "平均重投影误差: " << totalError / imagePoints.size() << " 像素" << std::endl;
    
    // ============ 9. 可视化 ============
    cv::Mat img = cv::Mat::zeros(480, 640, CV_8UC3);
    img.setTo(cv::Scalar(255, 255, 255));
    
    // 绘制原始点（红色）
    for (const auto& pt : imagePoints) {
        cv::circle(img, pt, 3, cv::Scalar(0, 0, 255), -1);
        cv::circle(img, pt, 5, cv::Scalar(0, 0, 255), 1);
    }
    
    // 绘制重投影点（蓝色）
    for (const auto& pt : projectedPoints) {
        cv::circle(img, pt, 3, cv::Scalar(255, 0, 0), -1);
        cv::circle(img, pt, 5, cv::Scalar(255, 0, 0), 1);
    }
    
    // 绘制坐标轴
    std::vector<cv::Point3f> axis;
    axis.push_back(cv::Point3f(0, 0, 0));
    axis.push_back(cv::Point3f(5, 0, 0));  // X轴 红色
    axis.push_back(cv::Point3f(0, 5, 0));  // Y轴 绿色
    axis.push_back(cv::Point3f(0, 0, 5));  // Z轴 蓝色
    
    std::vector<cv::Point2f> imageAxis;
    cv::projectPoints(axis, rvec, tvec, cameraMatrix, distCoeffs, imageAxis);
    
    cv::arrowedLine(img, imageAxis[0], imageAxis[1], cv::Scalar(0, 0, 255), 2);
    cv::arrowedLine(img, imageAxis[0], imageAxis[2], cv::Scalar(0, 255, 0), 2);
    cv::arrowedLine(img, imageAxis[0], imageAxis[3], cv::Scalar(255, 0, 0), 2);
    
    cv::putText(img, "X (Red)", imageAxis[0] + cv::Point2f(10, -10), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 0, 255), 1);
    cv::putText(img, "Y (Green)", imageAxis[1] + cv::Point2f(10, -10), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(0, 255, 0), 1);
    cv::putText(img, "Z (Blue)", imageAxis[2] + cv::Point2f(10, -10), 
                cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 0, 0), 1);
    
    cv::putText(img, "Red: Original points", cv::Point(10, 30),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(0, 0, 255), 1);
    cv::putText(img, "Blue: Reprojected points", cv::Point(10, 60),
                cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 0, 0), 1);
    
    cv::imshow("PnP Demo", img);
    cv::waitKey(0);
    
    return 0;
}