#ifndef TRACKER
#define TRACKER

#include <Eigen/Dense>
#include <vector>
#include <utility>
#include <opencv2/opencv.hpp>

class KalmanFilter {
public:
    KalmanFilter(int state_dim, int measure_dim);
    void initial(const Eigen::VectorXd& init_state, const Eigen::MatrixXd& init_covariance);

    void setTransitionMatrix(const Eigen::MatrixXd& A);
    void setProcessNoiseCovariance(const Eigen::MatrixXd& Q);
    void setMeasurementMatrix(const Eigen::MatrixXd& H);
    void setMeasurementNoiseCovariance(const Eigen::MatrixXd& R);

    void predict();
    void update(const Eigen::VectorXd& measurement);
    Eigen::VectorXd getState() const;

private:
    Eigen::MatrixXd A, H, Q, R, P, K;
    Eigen::VectorXd X;
    int state_dim, measure_dim;
};


std::vector<cv::Point2f> kalmanTrackRotationCenters(const std::vector<cv::Point2f>& observed_centers);

#endif 