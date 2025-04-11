#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <Eigen/Dense>
#include "../include/KalmanFilter.h"

using namespace std;
using namespace Eigen;

int main() {
    //// 读取文件 /////
    string input_filename = "stock_prices.csv";
    string output_filename = "predicted_prices.csv";
    ifstream input_file(input_filename);
    ofstream output_file(output_filename);

    if (!input_file.is_open()) {
        cerr << "Unable to open input file: " << input_filename << endl;
        return 1;
    }

    if (!output_file.is_open()) {
        cerr << "Unable to open output file: " << output_filename << endl;
        return 1;
    }

    vector<int> days; // 存储日期（整数）
    vector<double> prices; // 存储价格
    string line;

    // 读取CSV文件
    while (getline(input_file, line)) {
        stringstream ss(line);
        string day_str, price_str;

        if (getline(ss, day_str, ',') && getline(ss, price_str)) {
            try {
                int day = stoi(day_str);
                double price = stod(price_str);
                days.push_back(day);
                prices.push_back(price);
            } catch (...) {
                cerr << "Error parsing line: " << line << endl;
            }
        }
    }
    input_file.close();

    // 检查数据是否足够
    if (days.size() < 2) {
        cerr << "Insufficient data points (need at least 2)" << endl;
        return 1;
    }

    ///// kalman滤波预测 //////
    const int state_dim = 2;  // 状态维度：价格+变化率
    const int measure_dim = 1;  // 测量维度：价格
    KalmanFilter kf(state_dim, measure_dim);

    // 初始化参数
    double dt = days[1] - days[0]; // 时间间隔
    
    // 状态转移矩阵
    Eigen::MatrixXd A(state_dim, state_dim);
    A << 1, dt, 
         0, 1;
    kf.setTransitionMatrix(A);
    
    // 测量矩阵
    Eigen::MatrixXd H(measure_dim, state_dim);
    H << 1, 0;
    kf.setMeasurementMatrix(H);

    // 过程噪声协方差
    Eigen::MatrixXd Q(state_dim, state_dim);
    Q << 0.1, 0, 
         0, 0.01;
    kf.setProcessNoiseCovariance(Q);

    // 测量噪声协方差
    Eigen::MatrixXd R(measure_dim, measure_dim);
    R << 0.5;
    kf.setMeasurementNoiseCovariance(R);

    // 初始状态：第一个价格，初始变化率为0
    Eigen::VectorXd init_state(state_dim);
    init_state << prices[0], 0;
    
    // 初始协方差矩阵
    Eigen::MatrixXd init_covariance(state_dim, state_dim);
    init_covariance << 1, 0, 
                      0, 1;
    kf.initial(init_state, init_covariance);

    // 写入表头
    output_file << "day,predicted_price" << endl;

    // 处理数据
    for (size_t i = 0; i < prices.size(); ++i) {
        kf.predict(); // 预测步骤
        
        // 保存预测值到CSV
        output_file << days[i] << "," << kf.getState()(0) << endl;
        
        // 更新步骤
        Eigen::VectorXd measurement(measure_dim);
        measurement << prices[i];
        kf.update(measurement);
    }

    output_file.close();
    cout << "Prediction completed. Results saved to " << output_filename << endl;

    return 0;
}