#include "video_processor.hpp"
#include "armor_detector.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>

cv::Point2f fitCircle(const std::vector<cv::Point2f>& points, float& radius_out) {
    int N = points.size();
    if (N < 3) {
        std::cerr << "需要至少3个点来拟合圆" << std::endl;
        return cv::Point2f(-1, -1);
    }

    cv::Mat A(N, 3, CV_32F);
    cv::Mat B(N, 1, CV_32F);

    for (int i = 0; i < N; ++i) {
        float x = points[i].x;
        float y = points[i].y;
        A.at<float>(i, 0) = x;
        A.at<float>(i, 1) = y;
        A.at<float>(i, 2) = 1.0;
        B.at<float>(i, 0) = -(x * x + y * y);
    }

    cv::Mat X;
    cv::solve(A, B, X, cv::DECOMP_SVD);

    float a = -0.5f * X.at<float>(0, 0);
    float b = -0.5f * X.at<float>(1, 0);
    float c = X.at<float>(2, 0);
    float radius = std::sqrt(a * a + b * b - c);

    radius_out = radius;
    return cv::Point2f(a, b);
}


std::vector<cv::Point2f> processVideo(const std::string& filename) {
    cv::VideoCapture cap(filename);
    std::vector<cv::Point2f> rotation_centers; // 只存储旋转中心坐标的容器
    
    if (!cap.isOpened()) {
        std::cerr << "无法打开视频文件: " << filename << std::endl;
        return rotation_centers;
    }

    cv::Mat frame;
    int frame_id = 0;
    std::vector<cv::Point2f> trajectory_buffer;

    while (cap.read(frame)) {
        auto centers = detectArmorCenters(frame);
        std::cout << "帧 #" << frame_id << " 识别到 " << centers.size() << " 个装甲板:" << std::endl;

        // 可视化识别的装甲板中心
        for (const auto& pt : centers) {
            cv::circle(frame, pt, 4, cv::Scalar(0, 0, 255), -1);
            trajectory_buffer.push_back(pt);  // 添加到历史轨迹
        }

        // 控制轨迹长度
        const size_t MAX_POINTS = 20;
        if (trajectory_buffer.size() > MAX_POINTS) {
            trajectory_buffer.erase(
                trajectory_buffer.begin(),
                trajectory_buffer.begin() + (trajectory_buffer.size() - MAX_POINTS)
            );
        }

        // 拟合旋转中心
        if (trajectory_buffer.size() >= 5) {
            float radius = 0.0f;
            cv::Point2f center = fitCircle(trajectory_buffer, radius);
            
            // 只保存旋转中心坐标
            rotation_centers.push_back(center);
            
            // 可视化
            cv::circle(frame, center, 5, cv::Scalar(255, 0, 0), -1);
            std::cout << "拟合旋转中心: " << center << std::endl;
        }

        cv::imshow("Armor + Rotation Center", frame);
        frame_id++;

        if (cv::waitKey(1) == 27) break; // ESC键退出
    }
    
    return rotation_centers;
}