#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <fstream>
#include <cmath>

double monteCarlo(const std::vector<std::pair<double, double>>& circles, const std::vector<double>& r, int n) {
  int m = 0;
  double min_x = circles[0].first - r[0];
  double max_x = circles[0].first + r[0];
  double min_y = circles[0].second - r[0];
  double max_y = circles[0].second + r[0];

  for (size_t i = 1; i < circles.size(); ++i) {
    min_x = std::min(min_x, circles[i].first - r[i]);
    max_x = std::max(max_x, circles[i].first + r[i]);
    min_y = std::min(min_y, circles[i].second - r[i]);
    max_y = std::max(max_y, circles[i].second + r[i]);
  }

  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> dis_x(min_x, max_x);
  std::uniform_real_distribution<> dis_y(min_y, max_y);

  for (int i = 0; i < n; ++i) {
    double x = dis_x(gen);
    double y = dis_y(gen);
    bool allCircles = true;

    for (size_t j = 0; j < circles.size(); ++j) {
      double dx = x - circles[j].first;
      double dy = y - circles[j].second;
      if (dx * dx + dy * dy > r[j] * r[j]) {
        allCircles = false;
        break;
      }
    }
    if (allCircles) {
      ++m;
    }
  }

  double Srec = (max_x - min_x) * (max_y - min_y);
  return static_cast<double>(m) / n * Srec;
}

int main() {
  std::vector<std::pair<double, double>> circles(3);
  std::vector<double> r(3);
  circles[0].first=1.0;
  circles[0].second=1.0;
  r[0]=1.0;
  circles[1].first=1.5;
  circles[1].second=2;
  r[1]= sqrt(5)/2;
  circles[2].first=2;
  circles[2].second=1.5;
  r[2]= sqrt(5)/2;



  std::ofstream results("results.csv");

  std::vector<int> pointsCount;
  std::vector<double> areas;
  std::vector<double> differences;
  double rightAnswer =0.25 * M_PI + 1.25 * asin(0.8) - 1;
  for (int n = 100; n <= 100000; n += 500) {
    double monteCarloRes = monteCarlo(circles, r, n);
    pointsCount.push_back(n);
    areas.push_back(monteCarloRes);
    differences.push_back(std::abs(monteCarloRes - rightAnswer));
  }

  for (int i = 0; i < pointsCount.size(); ++i) {
    results << pointsCount[i] << ',';
  }
  results << '\n';
  for (int i = 0; i < areas.size(); ++i) {
    results << areas[i] << ',';
  }
  results << '\n';
  for (int i = 0; i < differences.size(); ++i) {
    results << differences[i] << ',';
  }

  results.close();
  return 0;
}
