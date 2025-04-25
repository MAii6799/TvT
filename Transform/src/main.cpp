#include <iostream>
#include "../include/Quaternion_Pose.h"

int main() {
    // 1. 初始化Camera坐标系下的位置和姿态
    Quaternion t_P_camera(0, 1, 2, 3);  // 位置 (x=1, y=2, z=3)
    Euler q_P_camera(0.1, 0.1, 0.1);    // 姿态 (yaw=0.1, pitch=0.1, roll=0.1)

    // 2. 初始化变换参数
    // Camera -> Gimbal 的变换
    Euler q_GimbaltoCamera(0, 0, 0);            // 姿态变换（无旋转）
    Quaternion t_GimbaltoCamera(0, 2, 0, 0);    // 位置变换 (x=2)

    // Gimbal -> Odom 的变换
    Euler q_OdomtoGimbal(-0.1, -0.1, -0.1);     // 姿态变换
    Quaternion t_OdomtoGimbal(0, 0, 0, 0);      // 位置变换（无位移）

    // 3. 计算逆变换
    Euler q_CameratoGimbal = Euler_inverse(q_GimbaltoCamera);
    Euler q_GimbaltoOdom = Euler_inverse(q_OdomtoGimbal);
    Quaternion t_CameratoGimbal = T_inverse(q_CameratoGimbal, t_GimbaltoCamera); 
    Quaternion t_GimbaltoOdom = T_inverse(q_GimbaltoOdom, t_OdomtoGimbal);

    // 4. 从 Camera 到 Gimbal 的坐标变换
    Position_Pose positionPose;  // 创建 Position_Pose 类的实例
    Quaternion t_P_gimbal = positionPose.Position_Transform(q_CameratoGimbal, t_P_camera, t_CameratoGimbal);
    Euler q_P_gimbal = positionPose.Pose_Transform(q_CameratoGimbal, q_P_camera);

    
    // 5. 从 Gimbal 到 Odom 的坐标变换
    Quaternion t_P_odom = positionPose.Position_Transform(q_GimbaltoOdom, t_P_gimbal, t_GimbaltoOdom);
    Euler q_P_odom = positionPose.Pose_Transform(q_GimbaltoOdom, q_P_gimbal); 

    // 6. 打印结果
    std::cout << "Final Position (Odom): (" 
              << t_P_odom.getX() << ", " 
              << t_P_odom.getY() << ", " 
              << t_P_odom.getZ() << ")" << std::endl;
    
    std::cout << "Final Orientation (Odom): (" 
              << q_P_odom.getYaw() << ", " 
              << q_P_odom.getPitch() << ", " 
              << q_P_odom.getRoll() << ")" << std::endl;

    return 0;
}
