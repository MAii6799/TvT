#include <opencv2/opencv.hpp>
#include <iostream>
#include <vector>
///一个超绝大失败。。。尝试用hsv和inrange提取颜色识别灯带的版本///
int main(){
    cv::Mat src=cv::imread("../armors/21.jpg");

    if (src.empty()) {
        std::cerr << "Error: Could not load image " << std::endl;
        return -1;
    }

    std::cout << "Image loaded successfully. Size: " << src.size() << std::endl;

    cv::Mat hsv;
    cv::cvtColor(src,hsv,cv::COLOR_BGR2HSV);//先转换颜色空间

    ///用inrange创建mask提取蓝色区域，二值化///
    cv::Scalar lower_blue(90,70,200);    
    cv::Scalar upper_blue(120,150,255);
    
    cv::Mat mask;
    cv::inRange(hsv,lower_blue,upper_blue,mask);

    ///去噪///
    cv::Mat kernel=cv::getStructuringElement(cv::MORPH_RECT,cv::Size(5,5));
    cv::morphologyEx(mask,mask,cv::MORPH_OPEN,kernel);//开运算去噪
    cv::morphologyEx(mask,mask,cv::MORPH_CLOSE,kernel);//闭运算填充

    ///查找轮廓///
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(mask,contours,cv::RETR_EXTERNAL,cv::CHAIN_APPROX_SIMPLE);
    ///筛选+绘制///
    for(const auto& contour:contours){
        double area=cv::contourArea(contour);
        cv::Rect rect=cv::boundingRect(contour);
        cv::rectangle(src, rect, cv::Scalar(0, 255, 0), 2);
    }
    cv::imshow("Original Image", src);
    cv::imshow("Mask", mask);
   
    cv::waitKey(0);

    return 0;
    
    
}