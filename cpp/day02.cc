#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <vector>

bool has_small_difference(const std::vector<int> &vec) {
  for (size_t i = 0; i < vec.size() - 1; i++) {
    int diff = std::abs(vec[i + 1] - vec[i]);
    if (diff < 1 || diff > 3) {
      return false;
    }
  }
  return true;
}

bool is_safe(const std::vector<int> &e) {
  bool test = has_small_difference(e);
  bool all_non_increasing =
      std::is_sorted(e.begin(), e.end(), std::greater<int>());
  bool all_non_decreasing = std::is_sorted(e.begin(), e.end());
  return (all_non_increasing || all_non_decreasing) && test;
}

std::vector<std::vector<int>> read_file(const std::string &input) {
  std::vector<std::vector<int>> report;
  std::ifstream file(input);
  std::string line;

  while (std::getline(file, line)) {
    std::istringstream iss(line);
    std::vector<int> row;
    int num;
    while (iss >> num) {
      row.push_back(num);
    }
    report.push_back(row);
  }

  return report;
}

int main() {
  // --- Part One ---
  auto report = read_file("crates/day02/input.txt");
  unsigned int safe = 0;

  for (const auto &e : report) {
    if (is_safe(e)) {
      safe++;
    }
  }
  std::cout << "Part One solution: sum is " << safe << std::endl;

  // --- Part Two ---
  safe = 0;
  for (const auto &e : report) {
    if (is_safe(e)) {
      safe++;
    } else {
      for (size_t pos = 0; pos < e.size(); pos++) {
        std::vector<int> new_e = e;
        new_e.erase(new_e.begin() + pos);

        if (is_safe(new_e)) {
          safe++;
          break;
        }
      }
    }
  }
  std::cout << "Part Two solution: sum is " << safe << std::endl;

  return 0;
}
