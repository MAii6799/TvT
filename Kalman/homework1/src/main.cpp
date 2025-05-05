#include "../include/KalmanFilter.h"
#include <iostream>
#include <cmath>

int main() {
    auto time_distance_data = readTimeDistanceData("../data/homework_data_4.txt");
    
    const int state_dim = 2;  //距离+速度
    const int measure_dim = 1;  //仅距离
    KalmanFilter kf(state_dim, measure_dim);

    ////初始化参数/////
    double dt = time_distance_data[1].first - time_distance_data[0].first;
    
    Eigen::MatrixXd A(state_dim, state_dim);
    A << 1, dt, 0, 1;
    kf.setTransitionMatrix(A);
    
    Eigen::MatrixXd H(measure_dim, state_dim);
    H << 1, 0;
    kf.setMeasurementMatrix(H);

    Eigen::MatrixXd Q(state_dim, state_dim);
    Q << 0.1, 0, 0, 0.01;
    kf.setProcessNoiseCovariance(Q);

    Eigen::MatrixXd R(measure_dim, measure_dim);
    R <<1;
    kf.setMeasurementNoiseCovariance(R);

    Eigen::VectorXd init_state(state_dim);
    init_state << time_distance_data[0].second, 0;
    Eigen::MatrixXd init_covariance(state_dim, state_dim);
    init_covariance << 1, 0, 0, 1;
    kf.initial(init_state, init_covariance);

    ////滤波////
    std::vector<double> times;
    std::vector<double> filtered_distances;

    for (const auto& [t, s] : time_distance_data) {
        times.push_back(t);
        kf.predict();
        
        if (!std::isnan(s)) {
            Eigen::VectorXd measurement(measure_dim);
            measurement << s;
            kf.update(measurement);
        }
        
        filtered_distances.push_back(kf.getState()(0));
    }

    saveFilteredData(times, filtered_distances, "filtered_results_4.txt");
    return 0;
}