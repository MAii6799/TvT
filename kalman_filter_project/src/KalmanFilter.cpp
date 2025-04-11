#include "../include/KalmanFilter.h"

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