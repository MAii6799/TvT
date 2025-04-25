#ifndef QUATERNIONPOSE_H
#define QUATERNIONPOSE_H

#include <iostream>
#include <cmath>

class Euler {
public:
    Euler(double yaw = 0.0, double pitch = 0.0, double roll = 0.0);
    double getYaw() const;
    double getPitch() const;
    double getRoll() const;
private:
    double yaw, pitch, roll;
};

class Quaternion {
public:
    Quaternion(double w = 1.0, double x = 0.0, double y = 0.0, double z = 0.0);
    Quaternion(const Quaternion& other); // 拷贝构造函数

    Quaternion operator+(const Quaternion& other) const; // 四元数加法
    Quaternion operator-(const Quaternion& other) const; // 四元数减法
    Quaternion operator*(const Quaternion& other) const; // 四元数乘法
    Quaternion operator-() const; //四元数取负
    double norm() const; // 四元数的模
    Quaternion conjugate() const; // 四元数的共轭
    Quaternion inverse() const; // 四元数的逆

    double getW() const;
    double getX() const; 
    double getY() const;
    double getZ() const;

private:
    double w, x, y, z;
};

// 欧拉角转换到四元数
Quaternion EulerToQuaternion(const Euler& euler);

// 四元数转换到欧拉角
Euler QuaternionToEuler(const Quaternion& q);

// 平移项的逆
Quaternion T_inverse(const Euler& q_BA, const Quaternion& t_AB);

// 姿态逆变换
Euler Euler_inverse(const Euler& e);

class Position_Pose {
public:
    // 位置变换
    Quaternion Position_Transform(const Euler& q_BA,const Quaternion& t_PA, const Quaternion& t_BA);

    // 姿态变换
    Euler Pose_Transform(const Euler& q_BA, const Euler& q_PA);

private:
    Quaternion t_PA; // 物体在A坐标系下的位置
    Quaternion t_BA; // A相对于B的平移向量
    Euler q_BA; // A相对于B的姿态
    Euler q_PA; // 物体在A坐标系下的姿态
};

#endif
