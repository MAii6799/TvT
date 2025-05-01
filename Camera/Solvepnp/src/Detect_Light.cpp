#include "Detect_Light.hpp"

cv::Mat ProcessImage(cv::Mat& src){

    std::vector<cv::Mat> channels;
    cv::split(src,channels);//分离颜色通道

    cv::Mat blue_mask;
    cv::subtract(channels[0],channels[2],blue_mask);//突出蓝色区域

    cv::Mat binary_image;
    cv::threshold(blue_mask,binary_image,50,255,cv::THRESH_BINARY);//二值化操作

    cv::Mat morph_image;
    cv::Mat kernel=cv::Mat::ones(5,5,CV_8U);
    cv::morphologyEx(binary_image,morph_image,cv::MORPH_CLOSE,kernel);
    cv::morphologyEx(morph_image,morph_image,cv::MORPH_OPEN,kernel);//开闭运算去噪

    return morph_image;
} 

bool IsLight(const cv::RotatedRect& minRect,double min_aspect_ratio,double max_aspect_ratio){
    // 获取矩形的长和宽
    float width = minRect.size.width;
    float height = minRect.size.height;
    // 计算长宽比
    float aspect_ratio = std::max(width, height) / std::min(width, height);
    // 判断长宽比是否在合理范围内
    return (aspect_ratio >= min_aspect_ratio && aspect_ratio <= max_aspect_ratio);
}

Light::Light(cv::Point2f top, cv::Point2f bottom) 
: top_center(top), bottom_center(bottom) {}

double Light::getHeight() const {
    return cv::norm(bottom_center - top_center);
}

void Light::draw(cv::Mat& image, cv::Scalar color) const {
    cv::line(image, top_center, bottom_center, color, 2);
}
Light::Light() : 
    top_center(0, 0),    // 初始化为原点
    bottom_center(0, 0)   // 初始化为原点
{}
cv::Point2f Light::getTopCenter() const { 
    return top_center;
}
cv::Point2f Light::getBottomCenter() const{
    return bottom_center;
}

cv::Point2f Light::getDirection() const{
    cv::Point2f dir = top_center - bottom_center;
    float norm = cv::norm(dir);
    return (norm > 0) ? dir / norm : cv::Point2f(0, 0); // 避免除以零
}


Armor_Detect::Armor_Detect(const Params& params) : params_(params) {}

bool Armor_Detect::areParallel(const Light& light1,const Light& light2,double angle_threshold) const{
    cv::Point2f dir1 = light1.getDirection();
    cv::Point2f dir2 = light2.getDirection();

    // 计算点积和夹角（弧度）
    double dot = dir1.x * dir2.x + dir1.y * dir2.y;
    double angle = std::acos(std::min(std::max(dot, -1.0), 1.0)); // 限制dot在[-1,1]范围内

    // 处理角度周期性（如179°和-179°实际只差2°）
    angle = std::min(angle, CV_PI - angle);

    // 转换为角度制比较
    return angle < (params_.parallel_angle_thresh * CV_PI / 180.0);
}

bool Armor_Detect::isValidAspectRatio(const Light& light1, const Light& light2) const {
    // 计算两个灯条自身的宽度（即灯条的高度）
    float light1_width = cv::norm(light1.top_center - light1.bottom_center);
    float light2_width = cv::norm(light2.top_center - light2.bottom_center);

    // 宽度取较小值（即较短的灯条宽度）
    float width = std::min(light1_width, light2_width);

    // 计算两个灯条对应端点之间的距离（作为可能的长度）
    float dist_top = cv::norm(light1.top_center - light2.top_center);
    float dist_bottom = cv::norm(light1.bottom_center - light2.bottom_center);

    // 长度取较大值（即较长的端点距离）
    float length = std::max(dist_top, dist_bottom);

    // 避免除以零
    if (width == 0 || length == 0) {
        return false;
    }

    // 计算长宽比（确保 ≥ 1）
    float aspect_ratio = std::max(width, length) / std::min(width, length);

    // 检查是否在允许范围内
    return std::abs(aspect_ratio - params_.target_aspect_ratio) <= params_.aspect_ratio_tolerance;
}
std::vector<std::pair<Light, Light>> Armor_Detect::pairLights(const std::vector<Light>& lights) const{
    std::vector<std::pair<Light, Light>> pairs;
    for (size_t i = 0; i < lights.size(); ++i) {
        for (size_t j = i + 1; j < lights.size(); ++j) {
            // 检查平行性和距离 
            if (areParallel(lights[i], lights[j])) {
                float avg_height = (lights[i].getHeight() + lights[j].getHeight()) / 2.0f;
                float dist = cv::norm(lights[i].top_center - lights[j].top_center);
                std::cout<<"Yes!Parallel!"<<std::endl;

                // 检查长宽比
                if (isValidAspectRatio(lights[i], lights[j])) {
                    // 根据X坐标区分左右灯条
                    const Light& left_light = (lights[i].top_center.x < lights[j].top_center.x) ? lights[i] : lights[j];
                    const Light& right_light = (lights[i].top_center.x < lights[j].top_center.x) ? lights[j] : lights[i];
                    pairs.emplace_back(left_light, right_light); // 按(左,右)顺序存储
                    std::cout<<"Yes!"<<std::endl;
                }
            }
        }
    }
    return pairs;
}

    