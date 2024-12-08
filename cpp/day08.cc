#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

struct GridSize {
  int rows;
  int cols;
};

class Antenna {
public:
  Antenna(const std::string &content) {
    int rows = std::count(content.begin(), content.end(), '\n');
    int cols = content.find('\n');
    if (cols != std::string::npos)
      cols = cols;
    this->grid = {rows - 1, cols - 1};

    std::istringstream stream(content);
    std::string line;
    int row_idx = 0;

    while (std::getline(stream, line)) {
      for (int col_idx = 0; col_idx < line.size(); ++col_idx) {
        char ch = line[col_idx];
        if (std::isalnum(ch)) {
          frequencies[ch].emplace_back(row_idx, col_idx);
        }
      }
      ++row_idx;
    }
  }

  static std::pair<std::pair<int, int>, std::pair<int, int>>
  new_antenna(const std::pair<int, int> &current_antenna,
              const std::pair<int, int> &other_antenna,
              const std::pair<int, int> &distance) {

    int y = (current_antenna.first < other_antenna.first)
                ? current_antenna.first - distance.first
                : current_antenna.first + distance.first;
    int x = (current_antenna.second < other_antenna.second)
                ? current_antenna.second - distance.second
                : current_antenna.second + distance.second;

    int dy = y - current_antenna.first;
    int dx = x - current_antenna.second;

    return {{y, x}, {dy, dx}};
  }

  std::pair<bool, bool> can_add_antenna(
      const std::pair<int, int> &antenna,
      const std::vector<std::pair<int, int>> &antenna_positions) const {

    if (antenna.first >= 0 && antenna.second >= 0 &&
        antenna.second <= grid.cols && antenna.first <= grid.rows) {
      if (std::find(antenna_positions.begin(), antenna_positions.end(),
                    antenna) != antenna_positions.end()) {
        return {false, false};
      }
      return {false, true};
    }
    return {true, false};
  }

  size_t count_antennas(bool harmony_frequencies) {
    std::vector<std::pair<int, int>> antenna_positions;

    for (const auto &[_, positions] : frequencies) {
      for (size_t i = 0; i < positions.size(); ++i) {
        const auto &pos_i = positions[i];
        for (size_t j = i + 1; j < positions.size(); ++j) {
          const auto &pos_j = positions[j];
          auto distance = std::make_pair(std::abs(pos_i.first - pos_j.first),
                                         std::abs(pos_i.second - pos_j.second));

          if (harmony_frequencies) {
            for (const auto &pos : {pos_i, pos_j}) {
              auto [is_out, can_add] = can_add_antenna(pos, antenna_positions);
              if (is_out) {
                break;
              }
              if (can_add) {
                antenna_positions.push_back(pos);
              }
            }
          }

          auto [first_antenna, distance_1] =
              new_antenna(pos_i, pos_j, distance);
          auto [second_antenna, distance_2] =
              new_antenna(pos_j, pos_i, distance);

          while (true) {
            auto [is_out, can_add] =
                can_add_antenna(first_antenna, antenna_positions);
            if (is_out) {
              break;
            }
            if (can_add) {
              antenna_positions.push_back(first_antenna);
            }
            if (!harmony_frequencies) {
              break;
            }
            first_antenna.first += distance_1.first;
            first_antenna.second += distance_1.second;
          }

          while (true) {
            auto [is_out, can_add] =
                can_add_antenna(second_antenna, antenna_positions);
            if (is_out) {
              break;
            }
            if (can_add) {
              antenna_positions.push_back(second_antenna);
            }
            if (!harmony_frequencies) {
              break;
            }
            second_antenna.first += distance_2.first;
            second_antenna.second += distance_2.second;
          }
        }
      }
    }

    return antenna_positions.size();
  }

private:
  std::unordered_map<char, std::vector<std::pair<int, int>>> frequencies;
  GridSize grid;
};

std::string read_file(const std::string &filename) {
  std::ifstream file(filename);
  if (!file) {
    throw std::runtime_error("Could not open the file");
  }
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main() {
  try {
    std::string message = read_file("crates/day08/input.txt");

    // --- Part One ---
    Antenna checker(message);
    std::cout << "Part One solution: " << checker.count_antennas(false)
              << std::endl;

    // --- Part Two ---
    std::cout << "Part Two solution: " << checker.count_antennas(true)
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
