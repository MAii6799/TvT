#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
#include <algorithm>

int main(){

    cv::Mat src=cv::imread("../Picture/21.jpg");
    if (src.empty()){
        std::cerr << "Error: Image not found!" << std::endl;
        return -1;
    }

    cv::Mat gray, mask;
    cv::cvtColor(src,gray,cv::COLOR_BGR2GRAY);
    
    ///自适应阈值处理///
    cv::threshold(gray,mask,200,255,cv::THRESH_BINARY);
    cv::imshow("Mask",mask);

    ///开闭运算///
    cv::Mat kernel=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));
    cv::morphologyEx(mask,mask,cv::MORPH_CLOSE,kernel); 
    cv::morphologyEx(mask,mask, cv::MORPH_OPEN,kernel); 

    ///canny检测///
    cv::Mat edges;
    cv::Canny(mask,edges,50,150);
    cv::imshow("Edges",edges);

    ///查找轮廓///
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(edges,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);

    for (const auto& contour: contours){
        double area = cv::contourArea(contour);
        ///删除面积不在[300, 600]之间的轮廓 ///刚好可以把这个头顶上的灯带去掉。。嘿嘿///
        contours.erase(
        std::remove_if(
            contours.begin(), 
            contours.end(),
            [](const std::vector<cv::Point>& contour) {
                double area = cv::contourArea(contour);
                return (area < 300) || (area > 600);  // 删除不满足条件的轮廓
            }
        ),
        contours.end()
    );
    }
    cv::imshow("Result", src);

    std::vector<cv::Point2f> allVertices; // 存储所有4个顶点（左上下，右上下）
    if (contours.size() == 2) {
        // 处理两个灯条
        for (int i = 0; i < 2; ++i) {
            cv::RotatedRect rect = cv::minAreaRect(contours[i]);
            cv::Point2f vertices[4];
            rect.points(vertices);
            
            // 绘制灯条矩形框
            for (int j = 0; j < 4; ++j) {
                cv::line(src, vertices[j], vertices[(j+1)%4], cv::Scalar(0, 255, 255), 2);
            }
            
            // 按y坐标排序顶点，确定上下顶点
            std::sort(vertices, vertices + 4, [](const cv::Point2f& a, const cv::Point2f& b) {
                return a.y < b.y;
            });
            
            // 计算上下顶点（取y最小和最大的两个点的中点）
            cv::Point2f top = (vertices[0] + vertices[1]) * 0.5f;
            cv::Point2f bottom = (vertices[2] + vertices[3]) * 0.5f;
            
            // 存储顶点，左灯条在前，右灯条在后
            allVertices.push_back(top);
            allVertices.push_back(bottom);
            
            // 为当前灯条绘制顶点和编号
            cv::Scalar color = (i == 0) ? cv::Scalar(0, 0, 255) : cv::Scalar(255, 0, 0); // 左红右蓝
            cv::circle(src, top, 6, color, -1);
            cv::circle(src, bottom, 6, color, -1);
            
            // 添加编号文本 (1-2为左灯条，3-4为右灯条)
            int topId = i * 2 + 1;
            int bottomId = i * 2 + 2;
            cv::putText(src, std::to_string(topId), top + cv::Point2f(5, 5), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
            cv::putText(src, std::to_string(bottomId), bottom + cv::Point2f(5, 5), 
                       cv::FONT_HERSHEY_SIMPLEX, 0.6, cv::Scalar(255, 255, 255), 2);
        }
    }
    cv::imshow("Result", src);

    
    //世界坐标系三维点坐标
    std::vector<cv::Point3f> objectPoints = {
        cv::Point3f(0,5.5, 0),   // 左灯条上顶点
        cv::Point3f(0,0,0),    // 左灯条下顶点
        cv::Point3f(13.5,5.5, 0),    // 右灯条上顶点
        cv::Point3f(13.5, 0, 0)      // 右灯条下顶点
    };

///solvepnp///
    cv::Mat cameraMatrix=(cv::Mat_<double>(3,3)<< 
    2065.0580175762857, 0.0, 658.9098266395495, 
    0.0, 2086.886458338243, 531.5333174739342, 
    0.0, 0.0, 1.0);
    cv::Mat distCoeffs=(cv::Mat_<double>(1,5)<<
    -0.051836613762195866, 0.29341513924119095,
     0.001501183796729562, 0.0009386915104617738, 0.0);
    
    cv::Mat rvec, tvec;
    bool success = cv::solvePnP(objectPoints, allVertices, cameraMatrix, distCoeffs, rvec, tvec);

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