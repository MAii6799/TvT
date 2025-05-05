#pragma once
#include <string>
#include "armor_detector.hpp"

cv::Point2f fitCircle(const std::vector<cv::Point2f>& points, float& radius_out);//寻找旋转中心
std::vector<cv::Point2f> processVideo(const std::string& filename);
