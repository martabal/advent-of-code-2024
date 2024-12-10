#include "helpers.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

const std::vector<std::pair<int, int>> DIRECTIONS = {
    {0, 1},  // Right
    {1, 0},  // Down
    {0, -1}, // Left
    {-1, 0}  // Up
};

class TrailHeads {
public:
  TrailHeads(const std::string &content) {
    std::istringstream stream(content);
    std::string line;
    int row = 0;

    while (std::getline(stream, line)) {
      for (int col = 0; col < line.size(); ++col) {
        char ch = line[col];
        if (isdigit(ch)) {
          int height = ch - '0';
          auto coords = std::make_pair(row, col);
          if (height == 0) {
            trailheads.push_back(coords);
          }
          grid[coords] = height;
        }
      }
      row++;
    }
  }

  size_t count_trailheads() const {
    size_t total = 0;
    for (const auto &head : trailheads) {
      total += count_trail_unique(head, 0).size();
    }
    return total;
  }

  size_t count_distinct_trailheads() const {
    size_t total = 0;
    for (const auto &head : trailheads) {
      total += count_trail_all(head, 0);
    }
    return total;
  }

private:
  struct pair_hash {
    template <class T1, class T2>
    std::size_t operator()(const std::pair<T1, T2> &pair) const {
      auto hash1 = std::hash<T1>{}(pair.first);
      auto hash2 = std::hash<T2>{}(pair.second);
      return hash1 ^ hash2;
    }
  };
  std::vector<std::pair<int, int>> trailheads;
  std::unordered_map<std::pair<int, int>, int, pair_hash> grid;

  std::unordered_set<std::pair<int, int>, pair_hash>
  count_trail_unique(const std::pair<int, int> &position, int height) const {
    std::unordered_set<std::pair<int, int>, pair_hash> res;

    for (const auto &[row_dir, col_dir] : DIRECTIONS) {
      auto new_dir =
          std::make_pair(position.first + row_dir, position.second + col_dir);
      auto it = grid.find(new_dir);
      if (it != grid.end()) {
        int new_pos = it->second;
        if (height + 1 != new_pos) {
          continue;
        }

        if (new_pos == 9) {
          res.insert(new_dir);
        } else {
          auto unique_trails = count_trail_unique(new_dir, new_pos);
          res.insert(unique_trails.begin(), unique_trails.end());
        }
      }
    }
    return res;
  }

  size_t count_trail_all(const std::pair<int, int> &position,
                         int height) const {
    size_t res = 0;
    for (const auto &[row_dir, col_dir] : DIRECTIONS) {
      auto new_dir =
          std::make_pair(position.first + row_dir, position.second + col_dir);
      auto it = grid.find(new_dir);
      if (it != grid.end()) {
        int new_pos = it->second;
        if (height + 1 != new_pos) {
          continue;
        }

        if (new_pos == 9) {
          res++;
        } else {
          res += count_trail_all(new_dir, new_pos);
        }
      }
    }
    return res;
  }
};

int main() {
  try {
    std::string message = readFile("crates/day10/input.txt");
    TrailHeads checker(message);

    // --- Part One ---
    std::cout << "Part One solution: " << checker.count_trailheads()
              << std::endl;

    // --- Part Two ---
    std::cout << "Part Two solution: " << checker.count_distinct_trailheads()
              << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
  }

  return 0;
}
