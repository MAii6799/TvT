#include <iostream>
#include "armor_detector.hpp"
#include "video_processor.hpp" 
#include "tracker.hpp"

int main() {
    std::string video_path = "../../videos/9.mp4";
    
    // 处理视频并进行装甲板识别然后获取一系列旋转中心坐标
    std::vector<cv::Point2f> observed_centers = processVideo(video_path);
    
    // 输出结果
    std::cout << "\n检测到的旋转中心坐标:" << std::endl;
    for (const auto& center : observed_centers) {
        std::cout << "(" << center.x << ", " << center.y << ")" << std::endl;
    }
    
    // 使用卡尔曼滤波进行追踪
    std::cout << "\n追踪预测坐标:" << std::endl;
    std::vector<cv::Point2f> kalman_predictions = kalmanTrackRotationCenters(observed_centers);
    
    for (const auto& predict : kalman_predictions) {
        std::cout << "(" << predict.x << ", " << predict.y << ")" << std::endl;
    }
    //tf

   
    return 0;
}