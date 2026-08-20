#include <algorithm>
#include <homework2.hpp>


ArmorDetector::ArmorDetector(EnemyColor color) {
    setEnemyColor(color);
    // 初始化默认参数
    binary_threshold_ = 100;
    color_threshold_ = 40;
    light_min_area_ = 10.0f;
    light_max_ratio_ = 1.0f;            // 灯条宽/高必须小于1
    light_max_angle_diff_ = 10.0f;      // 角度差不超过10度
    light_max_height_diff_ = 0.5f;      // 高度相对差不超过50%
    armor_min_ratio_ = 1.6f;            // 装甲板宽/高最小值
    armor_max_ratio_ = 3.0f;            // 装甲板宽/高最大值
}


void ArmorDetector::setEnemyColor(EnemyColor color) {                               //设置敌方颜色
    enemy_color_ = color;
}


cv::Mat ArmorDetector::preprocessImage(const cv::Mat& frame) {                      // 核心步骤1：图像预处理，利用通道相减提取特定颜色发光体
    cv::Mat result;
    std::vector<cv::Mat> channels;

    cv::split(frame, channels);                                                     // 分离BGR三通道
    if (enemy_color_ == EnemyColor::RED) {
        result = channels[2] - channels[0];                                         // 红色：R通道 - B通道，去掉白色和蓝色干扰
    } else {
        result = channels[0] - channels[2];                                         // 蓝色：B通道 - R通道，去掉白色和红色干扰
    }
        
    cv::Mat color_mask;
    cv::threshold(result, color_mask, color_threshold_, 255, cv::THRESH_BINARY);    // 颜色阈值过滤，去除非目标颜色的微弱噪点
    cv::imshow("123", color_mask);

    cv::Mat gray;
    cv::cvtColor(frame, gray, cv::COLOR_BGR2GRAY);                                  // 灰度图二值化，提取高亮区域（灯条通常很亮）
    
    cv::Mat bright_mask;
    cv::threshold(gray, bright_mask, binary_threshold_, 255, cv::THRESH_BINARY);    // 将“颜色符合”且“亮度符合”的区域取交集
    // cv::imshow("123", bright_mask);
        
    cv::Mat binary_img;
    cv::bitwise_and(color_mask, bright_mask, binary_img);                           // 形态学操作，填补灯条内部空洞，连接断裂部分
        
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::dilate(binary_img, binary_img, kernel);                                     // 膨胀
    cv::dilate(binary_img, binary_img, kernel);
    return binary_img;
}
    
    
std::vector<LightBar> ArmorDetector::findLightBars(const cv::Mat& binary_img) {     // 核心步骤2：寻找灯条轮廓
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(binary_img, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
    std::vector<LightBar> lights;
    for (const auto& contour : contours) {
            
        if (contour.size() < 5) continue;                                           // 去除过小的噪点
        
        float area = cv::contourArea(contour);                                      // 计算轮廓面积
        if (area < light_min_area_) continue;
        // 拟合旋转矩形
        cv::RotatedRect r_rect = cv::fitEllipse(contour);                           // 使用fitEllipse对长条状更稳定

        float ratio = r_rect.size.width / r_rect.size.height;                       // 筛选宽高比（灯条应该是细长的，宽必须小于高）
        if (ratio > light_max_ratio_) continue;
        // 修正角度
        // float angle = r_rect.angle;
        // if (r_rect.size.width > r_rect.size.height) {
        //     angle = angle + 90.0f;
        // }
            
        // if (std::abs(angle - 90.0f) > 40.0f && std::abs(angle + 90.0f) > 40.0f) continue;   // 灯条倾斜角不应过大（排除地面反光等）
        lights.emplace_back(LightBar(r_rect));
    }
    return lights;
}


std::vector<Armor> ArmorDetector::matchLightBars(const std::vector<LightBar>& lights) {         // 核心步骤3：灯条两两匹配，组成装甲板
    std::vector<Armor> armors;
 
    std::vector<LightBar> sorted_lights = lights;                                               // 按照X坐标从左到右排序，减少匹配复杂度
    std::sort(sorted_lights.begin(), sorted_lights.end(), [](const LightBar& l1, const LightBar& l2) { return l1.rect.center.x < l2.rect.center.x; });
 
    
    for (size_t i = 0; i < sorted_lights.size(); ++i) {                                         // 两两遍历匹配
        for (size_t j = i + 1; j < sorted_lights.size(); ++j) {
            const LightBar& left_light = sorted_lights[i];
            const LightBar& right_light = sorted_lights[j];
 
            
            float angle_diff = std::abs(left_light.rect.angle - right_light.rect.angle);        // 1. 角度差判断（两个灯条应该近似平行）
            if (angle_diff > light_max_angle_diff_) continue;
 
            
            float y_diff = std::abs(left_light.rect.center.y - right_light.rect.center.y);      // 2. 高度差判断（两个灯条中心Y坐标差异不应过大）
            float avg_height = (left_light.length + right_light.length) / 2.0f;
            if (y_diff / avg_height > light_max_height_diff_) continue;
 
            
            float length_diff = std::abs(left_light.length - right_light.length) / avg_height;  // 3. 长度差判断（两个灯条长度不应相差悬殊）
            if (length_diff > 0.5f) continue;
 
            
            float width = std::abs(right_light.rect.center.x - left_light.rect.center.x);       // 4. 装甲板宽高比判断
            float ratio = width / avg_height;
            if (ratio < armor_min_ratio_ || ratio > armor_max_ratio_) continue;
 
            
            armors.emplace_back(Armor(left_light, right_light));                                // 匹配成功，加入装甲板列表
            
            
            break;                                                                              // 假设一个灯条只属于一个装甲板，找到后跳出内层循环（视情况可优化为全匹配）
        }
    }
    return armors;
}
 
// 主函数
std::vector<Armor> ArmorDetector::detect(const cv::Mat& frame) {
    // 1. 预处理
    cv::Mat binary_img = preprocessImage(frame);
    
    // 2. 找灯条
    std::vector<LightBar> lights = findLightBars(binary_img);
    
    // 3. 匹配装甲板
    std::vector<Armor> armors = matchLightBars(lights);
    
    return armors;
}


int main() {
    cv::VideoCapture cap(0);
    system("v4l2-ctl -d /dev/video0 -c auto_exposure=1");
    system("v4l2-ctl -d /dev/video0 -c exposure_time_absolute=45");

    // 创建检测器，默认识别蓝色装甲板 (如果是红色则传入 EnemyColor::RED)
    ArmorDetector detector(EnemyColor::BLUE);
    cv::Mat frame;
    while (true) {
        cap >> frame;
        if (frame.empty()) break;
        // 缩小图像以提升处理速度（视分辨率而定）
        cv::resize(frame, frame, cv::Size(640, 480));
        // 运行检测
        std::vector<Armor> armors = detector.detect(frame);
        // 绘制结果
        for (const auto& armor : armors) {
            // 绘制装甲板外接矩形
            cv::Point2f vertices[4];
            armor.rect.points(vertices);
            for (int i = 0; i < 4; i++) {
                cv::line(frame, vertices[i], vertices[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
            }
            // 绘制中心点
            cv::circle(frame, armor.rect.center, 5, cv::Scalar(0, 0, 255), -1);
            // 标注大小装甲板
            // std::string label = (armor.armor_type == 1) ? "Large" : "Small";
            // cv::putText(frame, label, armor.rect.center - cv::Point2f(10, 10), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 0), 1);
        }
        cv::imshow("Armor Detector", frame);

        if (cv::waitKey(1) == 27) break;
    }
    cap.release();
    cv::destroyAllWindows();
    return 0;
}
