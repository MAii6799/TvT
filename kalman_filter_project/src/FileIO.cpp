#include "../include/KalmanFilter.h"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <iostream>

std::vector<std::pair<double, double>> readTimeDistanceData(const std::string& filename) {
    std::vector<std::pair<double, double>> data;
    std::ifstream file(filename);

    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return data;
    }

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        double time, distance;
        
        if (!(iss >> time >> distance)) {
            std::cerr << "Warning: Invalid line format: " << line << std::endl;
            continue;
        }

        data.emplace_back(time, distance);
    }

    file.close();
    return data;
}

void saveFilteredData(const std::vector<double>& times,
                     const std::vector<double>& filtered_distances,
                     const std::string& filename) {
    std::ofstream outfile(filename);
    if (!outfile) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    outfile << std::fixed << std::setprecision(6);
    for (size_t i = 0; i < times.size(); ++i) {
        outfile << times[i] << " " << filtered_distances[i] << "\n";
    }

    outfile.close();
    std::cout << "Filtered data saved to: " << filename << std::endl;
}