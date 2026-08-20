#ifndef ARMOR_DETECTOR_H
#define ARMOR_DETECTOR_H

#include <opencv2/opencv.hpp>


// 灯条结构体
struct LightBar {
    cv::RotatedRect rect;   // 旋转矩形（包含中心、宽高、角度）
    cv::Point2f top;        // 灯条顶点
    cv::Point2f bottom;     // 灯条底点
    float length;           // 灯条长度

    LightBar() = default;
    LightBar(const cv::RotatedRect& r) : rect(r) {
        // 确保长度是较长的边
        length = std::max(r.size.width, r.size.height);
        // 提取旋转矩形的四个顶点
        cv::Point2f vertices[4];
        r.points(vertices);
        // 找出最高的两个点作为top，最低的两个点作为bottom
        // 因为RM灯条基本是垂直的，所以通过Y坐标排序
        std::sort(vertices, vertices + 4, [](const cv::Point2f& p1, const cv::Point2f& p2) {
            return p1.y < p2.y;
        });
        top = (vertices[0] + vertices[1]) / 2.0f;
        bottom = (vertices[2] + vertices[3]) / 2.0f;
    }
};


// 装甲板结构体
struct Armor {
    LightBar left_light;    // 左灯条
    LightBar right_light;   // 右灯条
    cv::RotatedRect rect;   // 装甲板的旋转矩形
    int armor_type;         // 装甲板类型：0-小装甲板，1-大装甲板
    Armor(const LightBar& l, const LightBar& r) : left_light(l), right_light(r) {
        // 计算装甲板中心
        cv::Point2f center = (l.rect.center + r.rect.center) / 2.0f;
        // 计算装甲板宽度和高度
        float width = std::sqrt(std::pow(l.top.x - r.top.x, 2) + std::pow(l.top.y - r.top.y, 2));
        float height = (l.length + r.length) / 2.0f;
        // 判断大小装甲板（宽高比阈值）
        armor_type = (width / height > 2.2) ? 1 : 0;
        // 构造装甲板的旋转矩形
        rect = cv::RotatedRect(center, cv::Size2f(width, height), 0);
    }
};
// 敌方颜色枚举
enum class EnemyColor { RED, BLUE };
class ArmorDetector {
public:
    ArmorDetector(EnemyColor color = EnemyColor::BLUE);
    // 主识别接口
    std::vector<Armor> detect(const cv::Mat& frame);
    // 设置敌方颜色
    void setEnemyColor(EnemyColor color);
private:
    EnemyColor enemy_color_;
    // 预处理：提取颜色并二值化
    cv::Mat preprocessImage(const cv::Mat& frame);
    // 查找并筛选灯条
    std::vector<LightBar> findLightBars(const cv::Mat& binary_img);
    // 灯条两两匹配，寻找装甲板
    std::vector<Armor> matchLightBars(const std::vector<LightBar>& lights);
    // ---------- 可调参数 ----------
    int binary_threshold_;       // 二值化阈值
    int color_threshold_;        // 颜色通道差值阈值
    float light_min_area_;       // 灯条最小面积
    float light_max_ratio_;      // 灯条最大宽高比
    float light_max_angle_diff_; // 两灯条最大角度差
    float light_max_height_diff_;// 两灯条最大高度差(归一化)
    float armor_min_ratio_;      // 装甲板最小宽高比
    float armor_max_ratio_;      // 装甲板最大宽高比
};
#endif