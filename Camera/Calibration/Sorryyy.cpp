#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>

using namespace cv;

int main() {
    ///棋盘格参数设置///
    const int board_w=12;  //棋盘格宽度方向角点数
    const int board_h=8;   //棋盘格高度方向角点数
    const int board_n=board_w*board_h;  //角点总数
    Size board_size(12,8);  //棋盘格角点(宽×高)

    ///图像处理相关变量///
    Mat gray_img;        //灰度图像
    Mat drawn_img;       //存储绘制角点后的图像
    std::vector<Point2f> point_pix_pos_single;  //单幅图像的角点像素坐标点
    std::vector<std::vector<Point2f>> point_pix_pos;  //所有图像的角点像素坐标集合

    ///标定///
    int found;           //是否成功找到棋盘格
    int successes = 0;   //成功检测的棋盘格数量
    Size img_size;       //图像尺寸

    int cnt = 0;         //已处理的图像

    ///处理标定图像///
    for (int i=0;i<62;i++) {
        ///读取图像文件///
        cv::Mat src0=cv::imread("../Picture/"+std::to_string(i)+".jpg");
        
        if (src0.empty()) {
            std::cerr<<"Failed to load image:"<<i<<".jpg"<<std::endl;
            continue;
        }

        ///记录图像尺寸///
        if (!cnt){
            img_size.width=src0.cols;   //图像宽度
            img_size.height=src0.rows;  //图像高度
        }

        ///查找棋盘格角点///
        found=findChessboardCorners(src0,board_size,point_pix_pos_single);//此函数返回值是0或1
        
        // 如果成功找到所有角点
        if (found && point_pix_pos_single.size()==board_n){
            successes++;  //增加成功计数
            
            //转换为灰度图像//
            cvtColor(src0,gray_img,COLOR_BGR2GRAY);
            
            //亚像素级角点精确化?教程说的//
            find4QuadCornerSubpix(gray_img,point_pix_pos_single,Size(5, 5));
            
            //保存当前图像的角点坐标
            point_pix_pos.push_back(point_pix_pos_single);
            
            //在原图上绘制角点
            drawn_img=src0.clone();
            drawChessboardCorners(drawn_img,board_size,point_pix_pos_single,found);
            
            //显示结果
            imshow("corners",drawn_img);
            waitKey(50);
        } else {
            std::cout << "\tbut failed to found all chess board corners in this image" << std::endl;
        }
        
        //清空当前图像的角点
        point_pix_pos_single.clear();
        cnt++; 
    }

    //输出成功检测的棋盘格数量
    std::cout<<successes<<"useful chess boards"<<std::endl;

    //设置棋盘格实际物理尺寸(毫米)
    Size square_size(20,20); 
    
    //准备世界坐标系下的3D点
    std::vector<std::vector<Point3f>> point_world_pos;  // 所有图像的世界坐标点集合
    std::vector<Point3f> point_world_pos_single;          // 单幅图像的世界坐标点缓存
    std::vector<int> point_count;                     // 每幅图像的角点数量

    ///初始化相机参数矩阵///
    Mat camera_matrix(3,3,CV_32FC1,Scalar::all(0));  // 3x3相机内参矩阵
    Mat dist_coeffs(1,5,CV_32FC1,Scalar::all(0));    // 1x5畸变系数矩阵
    std::vector<Mat> rvecs;  // 每幅图像的旋转向量 //rotational
    std::vector<Mat> tvecs;  // 每幅图像的平移向量 //translation

    // 生成世界坐标系下的角点3D坐标
    for (int i=0;i<successes;i++) {
        point_world_pos_single.clear();
        // 按棋盘格行列生成坐标点
        for (int j=0;j<board_h;j++) {
            for (int k=0;k<board_w;k++) {
                Point3f pt;
                pt.x=k*square_size.width;   // X坐标
                pt.y=j*square_size.height;  // Y坐标
                pt.z=0;                       // Z坐标(棋盘格平面设为0)
                point_world_pos_single.push_back(pt);
            }
        }
        point_world_pos.push_back(point_world_pos_single);
        point_count.push_back(board_h*board_w);  // 记录每幅图的角点数
    }

    ///执行相机标定///
    double rms = calibrateCamera(
        point_world_pos,    // 世界坐标系下的3D点
        point_pix_pos,     // 图像坐标系下的2D点
        img_size,         // 图像尺寸
        camera_matrix,    // 输出相机内参矩阵
        dist_coeffs,      // 输出畸变系数
        rvecs,            // 输出旋转向量
        tvecs             // 输出平移向量
    );

    // 输出标定结果
    std::cout<<"Re-projection error: "<<rms<<std::endl;
    std::cout<<"Camera intrinsic matrix:"<<std::endl<<camera_matrix<<std::endl;
    std::cout<<"Distortion coefficients:"<<std::endl<<dist_coeffs<<std::endl;

    return 0;
}