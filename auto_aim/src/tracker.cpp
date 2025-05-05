#include "tracker.hpp"

KalmanFilter::KalmanFilter(int state_dim, int measure_dim) 
    : state_dim(state_dim), measure_dim(measure_dim),
      A(Eigen::MatrixXd::Identity(state_dim, state_dim)),
      H(Eigen::MatrixXd::Zero(measure_dim, state_dim)),
      Q(Eigen::MatrixXd::Identity(state_dim, state_dim)),
      R(Eigen::MatrixXd::Identity(measure_dim, measure_dim)),
      P(Eigen::MatrixXd::Identity(state_dim, state_dim)),
      X(Eigen::VectorXd::Zero(state_dim)) {}

void KalmanFilter::initial(const Eigen::VectorXd& init_state, const Eigen::MatrixXd& init_covariance) {
    X = init_state;
    P = init_covariance;
}

void KalmanFilter::setTransitionMatrix(const Eigen::MatrixXd& A) {
    this->A = A;
}

void KalmanFilter::setProcessNoiseCovariance(const Eigen::MatrixXd& Q) {
    this->Q = Q;
}

void KalmanFilter::setMeasurementMatrix(const Eigen::MatrixXd& H) {
    this->H = H;
}

void KalmanFilter::setMeasurementNoiseCovariance(const Eigen::MatrixXd& R) {
    this->R = R;
}

void KalmanFilter::predict() {
    X = A * X;
    P = A * P * A.transpose() + Q;
}

void KalmanFilter::update(const Eigen::VectorXd& measurement) {
    Eigen::MatrixXd temp = H * P * H.transpose() + R;
    K = P * H.transpose() * temp.inverse();
    X = X + K * (measurement - H * X);
    Eigen::MatrixXd I = Eigen::MatrixXd::Identity(state_dim, state_dim);
    P = (I - K * H) * P;
}

Eigen::VectorXd KalmanFilter::getState() const {
    return X;
}

std::vector<cv::Point2f> kalmanTrackRotationCenters(const std::vector<cv::Point2f>& observed_centers) {
    std::vector<cv::Point2f> predicted_positions;
    
    if (observed_centers.empty()) {
        return predicted_positions;
    }

    // 初始化卡尔曼滤波器 (4状态: x,y,vx,vy; 2测量: x,y)
    KalmanFilter kf(4, 2);
    
    // 设置状态转移矩阵 (匀速运动模型)
    Eigen::Matrix4d A;
    A << 1, 0, 1, 0,
         0, 1, 0, 1,
         0, 0, 1, 0,
         0, 0, 0, 1;
    kf.setTransitionMatrix(A);
    
    // 设置测量矩阵
    Eigen::MatrixXd H(2, 4);
    H << 1, 0, 0, 0,
         0, 1, 0, 0;
    kf.setMeasurementMatrix(H);
    
    // 设置过程噪声和测量噪声
    kf.setProcessNoiseCovariance(Eigen::Matrix4d::Identity() * 1e-2);
    kf.setMeasurementNoiseCovariance(Eigen::Matrix2d::Identity() * 1e-1);
    
    // 初始化状态 (第一个观测点，速度设为0)
    Eigen::Vector4d init_state;
    init_state << observed_centers[0].x, observed_centers[0].y, 0, 0;
    kf.initial(init_state, Eigen::Matrix4d::Identity());
    
    // 处理每个观测点
    for (size_t i = 0; i < observed_centers.size(); ++i) {
        // 预测步骤
        kf.predict();
        
        // 获取预测状态
        Eigen::VectorXd state = kf.getState();
        cv::Point2f pred_point(state(0), state(1));
        predicted_positions.push_back(pred_point);
        
        // 如果有新的观测数据，则更新
        if (i < observed_centers.size() - 1) {
            Eigen::Vector2d measurement;
            measurement << observed_centers[i+1].x, observed_centers[i+1].y;
            kf.update(measurement);
        }
    }
    
    return predicted_positions;
}