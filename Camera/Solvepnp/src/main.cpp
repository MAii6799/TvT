#include "Detect_Light.hpp"
#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>


int main(){
    ///不管了先输入图像///
    cv::Mat src=cv::imread("../Picture/21.jpg");
    if (src.empty()){
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }
 
    ///图像预处理///
    cv::Mat morph_image=ProcessImage(src);

    cv::imshow("BINARY",morph_image);
    ///查找灯条轮廓///
    cv::Mat edges;
    cv::Canny(morph_image,edges,100,200);//边缘检测

    std::vector<std::vector<cv::Point>> contours;//存储检测到的所有轮廓
    std::vector<Light> lights;//存储检测到的有效灯条

    cv::findContours(edges,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);//查找轮廓

    for(size_t i=0;i<contours.size();i++){
        cv::RotatedRect rect=cv::minAreaRect(contours[i]);

        if (IsLight(rect)) {
            // 如果符合条件，保存该矩形
            cv::Point2f vertices[4];
            rect.points(vertices);
        
            // 排序顶点（按Y坐标）
            std::vector<cv::Point2f> points(vertices, vertices + 4);
            std::sort(points.begin(), points.end(), [](const cv::Point2f& a, const cv::Point2f& b) {
                return a.y < b.y;
            });
        
            // 创建 Light 对象并保存
            cv::Point2f top_center = (points[0] + points[1]) * 0.5f;
            cv::Point2f bottom_center = (points[2] + points[3]) * 0.5f;
            lights.emplace_back(top_center, bottom_center);
            lights.back().draw(src, cv::Scalar(0, 255, 255));
        }
    }//绘制得到的灯条中线

    cv::imshow("Detected Rectangles",src);

    ///匹配灯条///
    Armor_Detect::Params params;
    params.target_aspect_ratio = 2.45f;  // 装甲板长宽比
    Armor_Detect detector(params);
    auto armor_pairs = detector.pairLights(lights);

    //绘制配对结果//
    for (const auto& pair : armor_pairs) {
        const Light& left_light = pair.first;
        const Light& right_light = pair.second;

    // 绘制灯条中线（绿色）
        cv::line(src, left_light.top_center, left_light.bottom_center, cv::Scalar(0, 255, 0), 2);
        cv::line(src, right_light.top_center, right_light.bottom_center, cv::Scalar(0, 255, 0), 2);

        // 定义装甲板四个顶点
    std::vector<cv::Point2f> armor_points = {
        left_light.top_center,     // 左上
        right_light.top_center,    // 右上
        right_light.bottom_center, // 右下
        left_light.bottom_center   // 左下
    };

    // 绘制装甲板外框（红色）
    for (int j = 0; j < 4; j++) {
        cv::line(src, armor_points[j], armor_points[(j + 1) % 4], cv::Scalar(0, 0, 255), 2);
    }

    // 绘制顶点标记（添加这部分新代码）
    std::vector<cv::Scalar> vertex_colors = {
        cv::Scalar(255, 0, 0),    // 左上-蓝色
        cv::Scalar(0, 255, 0),    // 右上-绿色
        cv::Scalar(0, 0, 255),    // 右下-红色
        cv::Scalar(255, 255, 0)   // 左下-青色
    };

    std::vector<std::string> vertex_labels = {"TL", "TR", "BR", "BL"}; // 顶点标签

    for (int j = 0; j < 4; j++) {
        // 绘制顶点圆点
        cv::circle(src, armor_points[j], 8, vertex_colors[j], -1);
        
        // 添加顶点标签
        cv::putText(src, 
                   vertex_labels[j], 
                   armor_points[j] + cv::Point2f(10, -10), 
                   cv::FONT_HERSHEY_SIMPLEX, 
                   0.6, 
                   vertex_colors[j], 
                   2);
    }
    
 
    // 标记装甲板中心（黄色）
        cv::Point2f armor_center = (left_light.top_center + right_light.bottom_center) * 0.5f;
        cv::circle(src, armor_center, 5, cv::Scalar(0, 255, 255), -1);
}

    cv::imshow("Armor Detection", src);
    




///solvepnp///
  
  // 统一使用米为单位（推荐）
const float ARMOR_WIDTH = 0.135f;  // 13.5cm → 0.135m
const float ARMOR_HEIGHT = 0.055f; // 5.5cm → 0.055m

std::vector<cv::Point3f> object_Points = {
    cv::Point3f(0, ARMOR_HEIGHT, 0),    // 左上
    cv::Point3f(0, 0, 0),               // 左下
    cv::Point3f(ARMOR_WIDTH, ARMOR_HEIGHT, 0), // 右上
    cv::Point3f(ARMOR_WIDTH, 0, 0)      // 右下
};

    std::vector<cv::Point2f> image_Points;   // 2D点（图像中的对应点）

    for (const auto& pair : armor_pairs) {
        const Light& left_light = pair.first;
        const Light& right_light = pair.second;

        cv::Point2f top_left = left_light.top_center;
        cv::Point2f top_right = right_light.top_center;
        cv::Point2f bottom_right = left_light.bottom_center;
        cv::Point2f bottom_left = right_light.bottom_center;

    // 对应的2D图像坐标
        image_Points={
            top_left,bottom_left,top_right,bottom_right
        };
        // 打印坐标对应关系
        for(int i=0; i<4; i++){
            std::cout << "3D Point " << object_Points[i] 
                     << " ↔ 2D Point " << image_Points[i] << std::endl;
        }

        cv::Mat cameraMatrix=(cv::Mat_<double>(3,3)<< 
        2065.0580175762857, 0.0, 658.9098266395495, 
        0.0, 2086.886458338243, 531.5333174739342, 
        0.0, 0.0, 1.0);
        cv::Mat distCoeffs=(cv::Mat_<double>(1,5)<<
        -0.051836613762195866, 0.29341513924119095,
        0.001501183796729562, 0.0009386915104617738, 0.0);


        cv::Mat rvec, tvec;
        bool success = cv::solvePnP(object_Points, image_Points, cameraMatrix, distCoeffs, rvec, tvec);

        if (success) {
    // 计算距离（单位：米）
            double distance = cv::norm(tvec);
            std::cout << "Distance to armor: " << distance << " meters" << std::endl;
            std::cout << "rvec:" <<rvec<< std::endl;
            std::cout << "tvec:" <<tvec<< std::endl;
    
            std::string distanceText = "Distance: " + std::to_string(distance) + " meters";
            cv::putText(src, distanceText, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
}

    cv::imshow("Result", src);

}

    cv::waitKey(0);

    return 0;


}