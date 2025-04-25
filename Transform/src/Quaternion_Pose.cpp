#include "../include/Quaternion_Pose.h"

Quaternion::Quaternion(double w, double x, double y, double z)
    : w(w), x(x), y(y), z(z) {
}
Quaternion::Quaternion(const Quaternion& other){
    w = other.w;
    x = other.x;
    y = other.y;
    z = other.z;
}

Quaternion Quaternion::operator+(const Quaternion& other) const{
    double new_w=this->w+other.w;
    double new_x=this->x+other.x;
    double new_y=this->y+other.y;
    double new_z=this->z+other.z;

    return Quaternion(new_w,new_x,new_y,new_z);
}
Quaternion Quaternion::operator-(const Quaternion& other) const{
    double new_w=this->w-other.w;
    double new_x=this->x-other.x;
    double new_y=this->y-other.y;
    double new_z=this->z-other.z;

    return Quaternion(new_w,new_x,new_y,new_z);
}
Quaternion Quaternion::operator*(const Quaternion& other) const{
    double new_w=w*other.w-x*other.x-y*other.y-z*other.z;
    double new_x=w*other.x+x*other.w+y*other.z-z*other.y;
    double new_y=w*other.y-x*other.z+y*other.w+z*other.x;
    double new_z=w*other.z+x*other.y-y*other.x+z*other.w;
        
    return Quaternion(new_w,new_x,new_y,new_z);
}
Quaternion Quaternion::operator-() const {
    return Quaternion(w, -x, -y, -z);
}
double Quaternion::norm() const{
    double norm;
    norm=std::sqrt(w*w+x*x+y*y+z*z);
    return norm;
}
Quaternion Quaternion::conjugate() const{
    return Quaternion(w,-x,-y,-z);
}
Quaternion Quaternion::inverse() const{
    double norm_sq=norm()*norm();
    Quaternion q_conjugate=conjugate();
    return Quaternion(q_conjugate.w/norm_sq,q_conjugate.x/norm_sq,q_conjugate.y/norm_sq,q_conjugate.z/norm_sq);
}
double Quaternion::getW() const{
    return this->w;
}
double Quaternion::getX() const{
    return this->x;
}
double Quaternion::getY() const{
    return this->y;
}
double Quaternion::getZ() const{
    return this->z;
}



Quaternion EulerToQuaternion(const Euler& e){
     // 计算各个角度的 cos 和 sin
     double c_roll = std::cos(e.getRoll() / 2);
     double s_roll = std::sin(e.getRoll() / 2);
     double c_pitch = std::cos(e.getPitch() / 2);
     double s_pitch = std::sin(e.getPitch() / 2);
     double c_yaw = std::cos(e.getYaw() / 2);
     double s_yaw = std::sin(e.getYaw() / 2);
 
     // 计算四元数的各个分量
     double w = c_roll * c_pitch * c_yaw + s_roll * s_pitch * s_yaw;
     double x = s_roll * c_pitch * c_yaw - c_roll * s_pitch * s_yaw;
     double y = c_roll * s_pitch * c_yaw + s_roll * c_pitch * s_yaw;
     double z = c_roll * c_pitch * s_yaw - s_roll * s_pitch * c_yaw;
 
     // 返回计算出来的四元数
     return Quaternion(w, x, y, z);
}
Euler QuaternionToEuler(const Quaternion& q){
    double roll,yaw,pitch; 

    double w = q.getW();
    double x = q.getX();
    double y = q.getY();
    double z = q.getZ();

    // 计算滚转角（roll）
    double sinr_cosp = 2.0 * (w * x + y * z);
    double cosr_cosp = 1.0 - 2.0 * (x * x + y * y);
    roll = std::atan2(sinr_cosp, cosr_cosp);

    // 计算俯仰角（pitch）
    double sinp = 2.0 * (w * y - z * x);
    if (std::fabs(sinp) >= 1)
        pitch = std::copysign(M_PI / 2, sinp);  // 夹在 [-π/2, π/2] 之间
    else
        pitch = std::asin(sinp);

    // 计算偏航角（yaw）
    double siny_cosp = 2.0 * (w * z + x * y);
    double cosy_cosp = 1.0 - 2.0 * (y * y + z * z);
    yaw = std::atan2(siny_cosp, cosy_cosp);

    return Euler(yaw,pitch,roll);
}
Quaternion T_inverse(const Euler& q_BA,const Quaternion& t_AB){
    Quaternion t_BA,qBA;
    qBA=EulerToQuaternion(q_BA);
    t_BA =-( qBA*t_AB);
    return t_BA;
}
Euler Euler_inverse(const Euler& e){
    Quaternion E,E_inverse;
    E=EulerToQuaternion(e);
    E_inverse=E.conjugate();
    return QuaternionToEuler(E_inverse);
}

Euler::Euler(double yaw, double pitch, double roll)
    : yaw(yaw), pitch(pitch), roll(roll) {
}
double Euler::getYaw() const{
    return yaw;
}
double Euler::getRoll() const{
    return roll;
}
double Euler::getPitch() const{
    return pitch;
}

Quaternion Position_Pose::Position_Transform(const Euler& q_BA,const Quaternion& t_PA,const Quaternion& t_BA){
    Quaternion qBA=EulerToQuaternion(q_BA);
    Quaternion qBA_inverse=qBA.inverse();
    Quaternion t_PB;
    t_PB=qBA*t_PA*qBA_inverse+t_BA;
    std::cout << "t_PB: (" << t_PB.getX() << ", " << t_PB.getY() << ", " << t_PB.getZ() << ")" << std::endl;
    return t_PB;
}
Euler Position_Pose::Pose_Transform(const Euler& q_BA,const Euler& q_PA){
    Quaternion qBA,qPA,qPB;
    Euler q_PB;
    qBA=EulerToQuaternion(q_BA);
    qPA=EulerToQuaternion(q_PA);
    qPB=qBA*qPA;
    q_PB=QuaternionToEuler(qPB);
    std::cout << "q_PB: (" << q_PB.getYaw() << ", " << q_PB.getPitch() << ", " << q_PB.getRoll() << ")" << std::endl;
    return q_PB;
}