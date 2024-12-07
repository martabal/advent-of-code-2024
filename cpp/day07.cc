#include <algorithm>
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

  // Static methods for operations
  static uint64_t add(uint64_t a, uint64_t b) { return a + b; }

  static uint64_t multiply(uint64_t a, uint64_t b) { return a * b; }

  static uint64_t concatenate(uint64_t a, uint64_t b) {
    std::ostringstream oss;
    oss << a << b;
    return std::stoull(oss.str());
  }

  // Constructor to parse input
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

  // Compute calibration
  uint64_t compute_calibration(
      const std::vector<std::function<uint64_t(uint64_t, uint64_t)>> &operators)
      const {
    uint64_t count = 0;

    for (const auto &[result, equation] : equations) {
      auto results = compute_combinations(equation, operators);
      if (std::find(results.begin(), results.end(), result) != results.end()) {
        count += result;
      }
    }

    return count;
  }

  // Compute combinations
  static std::vector<uint64_t> compute_combinations(
      const std::vector<uint64_t> &values,
      const std::vector<std::function<uint64_t(uint64_t, uint64_t)>>
          &operators) {
    size_t num_values = values.size();
    size_t num_operators = num_values - 1;
    size_t total_combinations = std::pow(operators.size(), num_operators);

    std::vector<uint64_t> results;

    for (size_t combination = 0; combination < total_combinations;
         ++combination) {
      uint64_t result = values[0];
      size_t current_combination = combination;

      for (size_t i = 0; i < num_operators; ++i) {
        size_t operator_index = current_combination % operators.size();
        current_combination /= operators.size();

        result = operators[operator_index](result, values[i + 1]);
      }

      results.push_back(result);
    }

    return results;
  }
};

// Utility function to read a file
std::string read_file(const std::string &file_path) {
  std::ifstream file(file_path);
  if (!file) {
    throw std::runtime_error("Could not open file: " + file_path);
  }

  std::ostringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main() {
  try {
    std::string message = read_file("crates/day07/input.txt");

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
