#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

std::vector<std::vector<char>> read_file(const std::string &filename) {
  std::ifstream file(filename);
  if (!file.is_open()) {
    throw std::runtime_error("Failed to open the file.");
  }

  std::vector<std::vector<char>> grid;
  std::string line;
  while (std::getline(file, line)) {
    grid.emplace_back(line.begin(), line.end());
  }

  return grid;
}

size_t count_word(const std::vector<std::vector<char>> &grid,
                  const std::string &word) {
  const int rows = grid.size();
  const int cols = grid[0].size();
  const int word_len = word.size();
  size_t count = 0;

  std::vector<std::pair<int, int>> directions = {
      {0, 1}, {0, -1}, {1, 0}, {-1, 0}, {1, 1}, {1, -1}, {-1, 1}, {-1, -1}};

  for (int row = 0; row < rows; ++row) {
    for (int col = 0; col < cols; ++col) {
      for (const auto &[dx, dy] : directions) {
        bool found = true;

        for (int i = 0; i < word_len; ++i) {
          int x = row + i * dx;
          int y = col + i * dy;

          if (x < 0 || y < 0 || x >= rows || y >= cols ||
              grid[x][y] != word[i]) {
            found = false;
            break;
          }
        }

        if (found) {
          ++count;
        }
      }
    }
  }

  return count;
}

size_t count_x_pattern(const std::vector<std::vector<char>> &grid,
                       const std::string &word) {
  const int rows = grid.size();
  if (rows == 0) {
    throw std::invalid_argument("Grid must be a valid grid");
  }

  const int cols = grid[0].size();
  const int word_len = word.size();
  size_t count = 0;

  const int bounded = word_len / 2;
  char middle_char = word[bounded];
  std::string reverse_word = std::string(word.rbegin(), word.rend());

  for (int r = bounded; r < rows - bounded; ++r) {
    for (int c = bounded; c < cols - bounded; ++c) {
      if (grid[r][c] == middle_char) {
        bool first_diagonal = false;
        bool second_diagonal = false;

        // Check forward diagonal '\'
        first_diagonal = true;
        for (int i = 0; i < word_len; ++i) {
          if (grid[r - bounded + i][c - bounded + i] != word[i]) {
            first_diagonal = false;
            break;
          }
        }

        // Check backward diagonal '\'
        if (!first_diagonal) {
          first_diagonal = true;
          for (int i = 0; i < word_len; ++i) {
            if (grid[r - bounded + i][c - bounded + i] != reverse_word[i]) {
              first_diagonal = false;
              break;
            }
          }
        }

        if (first_diagonal) {
          // Check forward diagonal '/'
          second_diagonal = true;
          for (int i = 0; i < word_len; ++i) {
            if (grid[r + bounded - i][c - bounded + i] != word[i]) {
              second_diagonal = false;
              break;
            }
          }

          // Check backward diagonal '/'
          if (!second_diagonal) {
            second_diagonal = true;
            for (int i = 0; i < word_len; ++i) {
              if (grid[r + bounded - i][c - bounded + i] != reverse_word[i]) {
                second_diagonal = false;
                break;
              }
            }
          }

          if (second_diagonal) {
            ++count;
          }
        }
      }
    }
  }

  return count;
}

int main() {
  try {
    auto grid = read_file("crates/day04/input.txt");

    // --- Part One ---
    size_t count = count_word(grid, "XMAS");
    std::cout << count << std::endl;

    // --- Part Two ---
    count = count_x_pattern(grid, "MAS");
    std::cout << count << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }

  return 0;
}
