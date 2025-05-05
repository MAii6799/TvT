#ifndef DETECT
#define DETECT

#include <iostream>
#include <opencv2/opencv.hpp>
#include <cmath>

//图像预处理//
cv::Mat ProcessImage(cv::Mat& src);
 
//判断是否为灯条矩形//
bool IsLight(const cv::RotatedRect& minRect,double min_aspect_ratio= 2.5,double max_aspect_ratio = 9.0);

class Light {
public:
    cv::Point2f top_center, bottom_center;  
    // 构造函数
    Light(cv::Point2f top, cv::Point2f bottom);
    //默认构造
    Light();
    //gethight
    double getHeight() const ;
    cv::Point2f getCenter() const;
    //绘制黄色中线
    void draw(cv::Mat& image, cv::Scalar color = cv::Scalar(0, 255, 0)) const;
    // 获取灯条方向向量（从下顶点指向上顶点，单位向量）
    cv::Point2f getDirection() const;

    cv::Point2f getTopCenter() const;
    cv::Point2f getBottomCenter() const;
            
};

class Armor_Detect{
public:
      // 参数结构体定义
    struct Params {
        double parallel_angle_thresh;
        double max_distance_ratio;
        float target_aspect_ratio;
        float aspect_ratio_tolerance;
        
        // 使用构造函数初始化默认值
        Params() : 
            parallel_angle_thresh(5.0),
            target_aspect_ratio(2.45f),
            aspect_ratio_tolerance(0.3f) {}
    };

    // 初始化时传入参数

     // 构造函数声明
    explicit Armor_Detect(const Params& params = Params());

    //判断两个灯条是否平行
    bool areParallel(const Light& light1,const Light& light2,double angle_threshold=5.0) const;
    //判断左右灯条形成的矩形的长宽比是否在2.45左右
    bool isValidAspectRatio(const Light& light1,const Light& light2) const;
    //两两配对灯条
    std::vector<std::pair<Light, Light>> pairLights(const std::vector<Light>& lights) const ;

private:
    Light Left_light;
    Light Right_light;
    Params params_;
};

std::vector<cv::Point2f> detectArmorCenters(cv::Mat& frame);

#endif