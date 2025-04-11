#include <opencv2/opencv.hpp>
#include <iostream>

std::string writePath="../Picture/";

int main(int argc, char** argv){
    ///打开电脑摄像头///
    cv::VideoCapture capture(0);
    if(!capture.isOpened()) {
        std::cerr<<"无法打开摄像头"<<std::endl;
        return -1;
    }

    cv::namedWindow("Camera");
    int i=0;
    
    while(true){
        cv::Mat frame;
        capture>>frame;
        if(frame.empty()) break;

        int key = cv::waitKey(20);
        if (key == 32) { ///空格键拍照///
            std::string name=writePath+std::to_string(i)+".jpg";
            if (cv::imwrite(name,frame)) {
                std::cout<< "保存: "<<name<<std::endl;
                i++;
            }else{
                std::cerr<<"保存失败"<<std::endl;
            }
        } else if(key == 97){ ///'a'键停止///
            break;
        }

        cv::imshow("Camera",frame);
    }
    
    cv::destroyAllWindows();
    return 0;
}