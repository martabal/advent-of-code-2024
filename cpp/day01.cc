#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::pair<std::vector<int>, std::vector<int>>
read_file(const std::string &input) {
  std::vector<int> left_column, right_column;

  std::ifstream file(input);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file");
  }

  std::string line;
  while (std::getline(file, line)) {
    std::istringstream iss(line);
    int left, right;
    if (iss >> left >> right) {
      left_column.push_back(left);
      right_column.push_back(right);
    }
  }

  return {left_column, right_column};
}

int main() {
  // --- Part One ---
  auto [left_column, right_column] = read_file("crates/day01/input.txt");
  unsigned int sum = 0;

  if (left_column.size() != right_column.size()) {
    throw std::runtime_error("Column sizes do not match");
  }

  std::sort(left_column.begin(), left_column.end());
  std::sort(right_column.begin(), right_column.end());

  for (size_t i = 0; i < left_column.size(); i++) {
    sum +=
        (static_cast<unsigned int>(std::abs(left_column[i] - right_column[i])));
  }

  std::cout << "Part One solution: sum is " << sum << std::endl;

  // --- Part Two ---
  sum = 0;
  std::vector<unsigned int> appear;
  for (const auto &e : left_column) {
    unsigned int count =
        std::count(right_column.begin(), right_column.end(), e);
    sum += static_cast<unsigned int>(e) * count;
  }
  std::cout << "Part Two solution: sum is " << sum << std::endl;

  return 0;
}
