#include "helpers.h"
#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Calibration {
  std::vector<std::pair<uint64_t, std::vector<uint64_t>>> equations;

  static uint64_t add(uint64_t a, uint64_t b) { return a + b; }

  static uint64_t multiply(uint64_t a, uint64_t b) { return a * b; }

  static uint64_t concatenate(uint64_t a, uint64_t b) {
    std::ostringstream oss;
    oss << a << b;
    return std::stoull(oss.str());
  }

  Calibration(const std::string &content) {
    std::istringstream iss(content);
    std::string line;

    while (std::getline(iss, line)) {
      std::istringstream line_stream(line);
      std::string key_str, values_str;

      if (std::getline(line_stream, key_str, ':') &&
          std::getline(line_stream, values_str)) {
        uint64_t key = std::stoull(key_str);
        std::vector<uint64_t> values;
        std::istringstream values_stream(values_str);

        uint64_t value;
        while (values_stream >> value) {
          values.push_back(value);
        }

        equations.emplace_back(key, values);
      }
    }
  }

  uint64_t compute_calibration(
      const std::vector<std::function<uint64_t(uint64_t, uint64_t)>> &operators)
      const {
    uint64_t count = 0;

    for (const auto &[result, equation] : equations) {
      auto is_correct = compute_combinations(equation, operators, result);
      if (is_correct) {
        count += result;
      }
    }

    return count;
  }

  static bool compute_combinations(
      const std::vector<uint64_t> &values,
      const std::vector<std::function<uint64_t(uint64_t, uint64_t)>> &operators,
      const uint64_t &is_result) {
    size_t num_values = values.size();
    assert(num_values > 2);
    size_t num_operators = num_values - 1;
    size_t total_combinations = std::pow(operators.size(), num_operators);

    for (size_t combination = 0; combination < total_combinations;
         combination++) {
      uint64_t result = values[0];
      size_t current_combination = combination;

      for (size_t i = 0; i < num_operators; ++i) {
        size_t operator_index = current_combination % operators.size();
        current_combination /= operators.size();

        result = operators[operator_index](result, values[i + 1]);
      }

      if (result == is_result) {
        return true;
      }
    }

    return false;
  }
};

int main() {
  try {
    std::string message = readFile("crates/day07/input.txt");

    // --- Part One ---
    Calibration checker(message);
    std::vector<std::function<uint64_t(uint64_t, uint64_t)>> operators = {
        Calibration::add, Calibration::multiply};
    uint64_t count = checker.compute_calibration(operators);
    std::cout << "Part One solution: " << count << std::endl;

    // --- Part Two ---
    operators.push_back(Calibration::concatenate);
    count = checker.compute_calibration(operators);
    std::cout << "Part Two solution: " << count << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
