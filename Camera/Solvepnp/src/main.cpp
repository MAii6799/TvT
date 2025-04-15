#include <iostream>
#include <opencv2/opencv.hpp>
#include <vector>

///判断是否为灯条///
bool IsLight(const cv::RotatedRect& minRect,double min_aspect_ratio= 3.0,double max_aspect_ratio = 4.0) {
    // 获取矩形的长和宽
    float width = minRect.size.width;
    float height = minRect.size.height;
    // 计算长宽比
    float aspect_ratio = std::max(width, height) / std::min(width, height);
    // 判断长宽比是否在合理范围内
    return (aspect_ratio >= min_aspect_ratio && aspect_ratio <= max_aspect_ratio);
}

// ///将灯条两两匹配为装甲板///

// // 判断两个灯条是否平行（角度差小于某个阈值）
// bool areParallel(const cv::RotatedRect& rect1, const cv::RotatedRect& rect2, double angle_threshold = 10.0) {
//     // 计算两个矩形的角度差
//     float angle_diff = std::abs(rect1.angle - rect2.angle);
    
//     // 平行的条件是角度差小于阈值（角度差可能超过180度，因此需要取最小差值）
//     if (angle_diff > 180) {
//         angle_diff = 360 - angle_diff;
//     }
    
//     return angle_diff < angle_threshold;
// }

// // 判断大矩形的长宽比是否接近2.45
// bool isValidAspectRatio(const cv::Point2f& top_left, const cv::Point2f& bottom_left, 
//                         const cv::Point2f& top_right, const cv::Point2f& bottom_right, 
//                         double target_aspect_ratio = 2.45, double aspect_ratio_tolerance = 0.2) {
//     // 计算大矩形的宽和高
//     double width = cv::norm(top_left - top_right); // 上边
//     double height = cv::norm(top_left - bottom_left); // 左边
    
//     // 计算长宽比
//     double aspect_ratio = std::max(width, height) / std::min(width, height);
    
//     // 判断长宽比是否接近目标值
//     return std::abs(aspect_ratio - target_aspect_ratio) <= aspect_ratio_tolerance;
// }

// std::vector<cv::RotatedRect> matchLightBars(const std::vector<cv::RotatedRect>& lightBars) {
//     std::vector<cv::RotatedRect> matchedRects;

//     // 遍历每对灯条
//     for (size_t i = 0; i < lightBars.size(); i++) {
//         for (size_t j = i + 1; j < lightBars.size(); j++) {
//             const cv::RotatedRect& rect1 = lightBars[i];
//             const cv::RotatedRect& rect2 = lightBars[j];

//             // 判断两个灯条是否平行
//             if (areParallel(rect1, rect2)) {
//                 // 获取两个灯条的上下顶点
//                 cv::Point2f top_left1 = rect1.center - rect1.size / 2.0f;
//                 cv::Point2f bottom_left1 = rect1.center + rect1.size / 2.0f;
//                 cv::Point2f top_left2 = rect2.center - rect2.size / 2.0f;
//                 cv::Point2f bottom_left2 = rect2.center + rect2.size / 2.0f;

//                 // 计算大矩形的四个顶点
//                 cv::Point2f top_left = cv::Point2f(std::min(top_left1.x, top_left2.x), std::min(top_left1.y, top_left2.y));
//                 cv::Point2f bottom_left = cv::Point2f(std::min(bottom_left1.x, bottom_left2.x), std::min(bottom_left1.y, bottom_left2.y));
//                 cv::Point2f top_right = cv::Point2f(std::max(top_left1.x, top_left2.x), std::max(top_left1.y, top_left2.y));
//                 cv::Point2f bottom_right = cv::Point2f(std::max(bottom_left1.x, bottom_left2.x), std::max(bottom_left1.y, bottom_left2.y));

//                 // 判断大矩形的长宽比是否接近2.45
//                 if (isValidAspectRatio(top_left, bottom_left, top_right, bottom_right)) {
//                     // 符合条件的灯条对，存储大矩形（返回旋转矩形）
//                     cv::RotatedRect bigRect = cv::minAreaRect(cv::Mat(std::vector<cv::Point2f>{top_left, top_right, bottom_right, bottom_left}));
//                     matchedRects.push_back(bigRect); // 存储匹配的矩形
//                 }
//             }
//         }
//     }
    
//     return matchedRects;
// }
////实现不了，暂时放弃了///

int main(){
    ///不管了先输入图像///
    cv::Mat src=cv::imread("../Picture/1.jpg");
    if (src.empty()){
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }

    ///图像预处理///
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

    cv::imshow("BINARY",morph_image);
    ///查找灯条轮廓///
    cv::Mat edges;
    cv::Canny(morph_image,edges,100,200);//边缘检测

    std::vector<std::vector<cv::Point>> contours;//存储检测到的所有轮廓
    std::vector<cv::RotatedRect> validRects;//存储检测到的有效灯条

    cv::findContours(edges,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);//查找轮廓

    for(size_t i=0;i<contours.size();i++){
        cv::RotatedRect rect=cv::minAreaRect(contours[i]);

        if (IsLight(rect)) {
            // 如果符合条件，保存该矩形
            // 如果符合条件，保存该矩形
        validRects.push_back(rect);
        cv::Point2f rect_points[4];
        rect.points(rect_points);
        for (int j = 0; j < 4; j++) {
            cv::line(src, rect_points[j], rect_points[(j + 1) % 4], cv::Scalar(0, 255, 0), 2);
            std::string label = std::to_string(j);  // 标号是0, 1, 2, 3
            cv::putText(src, label, rect_points[j], cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 0, 255), 2);     
        }
        }
    }//绘制最小矩形

    cv::imshow("Detected Rectangles",src);

    // ///调用匹配函数///
    // std::vector<cv::RotatedRect> matchedRects = matchLightBars(validRects);

    // for (const auto& rect : matchedRects) {
    //     cv::Point2f rect_points[4];
    //     rect.points(rect_points);
    //     std::vector<cv::Point> poly_points;
    //     for (int i = 0; i < 4; i++) {
    //         poly_points.push_back(rect_points[i]);
    //     }
    //     cv::polylines(src, poly_points, true, cv::Scalar(0, 255, 0), 2);  // 绘制大矩形
    // }

    // // 显示绘制结果
    // cv::imshow("Detected Armor Plates", src);

///solvepnp///
  
    std::vector<cv::Point3f> object_Points = {
        cv::Point3f(0,5.5, 0),   // 左灯条上顶点
        cv::Point3f(0,0,0),    // 左灯条下顶点
        cv::Point3f(13.5,5.5, 0),    // 右灯条上顶点
        cv::Point3f(13.5, 0, 0)      // 右灯条下顶点
    };

    std::vector<cv::Point2f> image_Points;   // 2D点（图像中的对应点）

    for (size_t i = 0; i < validRects.size(); i++) {
    const auto& rect = validRects[i];
    
    // 提取旋转矩形的四个顶点
    cv::Point2f rect_points[4];
    rect.points(rect_points);  // 获取旋转矩形的四个顶点

    // 假设顺序是：左上，右上，右下，左下（根据旋转矩形的顺序）
    cv::Point2f top_left = rect_points[0];
    cv::Point2f top_right = rect_points[1];
    cv::Point2f bottom_right = rect_points[2];
    cv::Point2f bottom_left = rect_points[3];

    // 计算上顶点（左上和右上的中点）
    cv::Point2f upper_mid = (top_left + top_right) / 2.0f;

    // 计算下顶点（左下和右下的中点）
    cv::Point2f lower_mid = (bottom_left + bottom_right) / 2.0f;

    // 对应的2D图像坐标
    image_Points.push_back(upper_mid);  // 图像中的上顶点
    image_Points.push_back(lower_mid);  // 图像中的下顶点
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
        double distance = cv::norm(tvec) / 100.0; //世界坐标单位是厘米
        std::cout << "Distance to armor: " << distance << " meters" << std::endl;
        std::cout << "rvec:" <<rvec<< std::endl;
        
        std::string distanceText = "Distance: " + std::to_string(distance) + " meters";
        cv::putText(src, distanceText, cv::Point(50, 50), cv::FONT_HERSHEY_SIMPLEX, 1, cv::Scalar(0, 255, 0), 2);
    }

    cv::imshow("Result", src);
    cv::waitKey(0);

    return 0;


}