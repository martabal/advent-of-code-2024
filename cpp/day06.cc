#include <fstream>
#include <iostream>
#include <optional>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

using namespace std;

const char OBJECT = '#';

enum class Direction { Up, Down, Left, Right };

class Lab {
public:
  pair<size_t, size_t> pos_init;
  vector<vector<char>> grid;
  pair<size_t, size_t> right_bottom_position;

  Lab(const string &content) {
    grid = parseGrid(content);
    pos_init = findInitialPosition(content);
    right_bottom_position = {grid.size() - 1, grid[0].size() - 1};
  }

  size_t findPath() {
    Direction direction = Direction::Up;
    auto current_position = pos_init;
    set<pair<size_t, size_t>> already_explored_pos;
    already_explored_pos.insert(pos_init);

    if (pos_init.second == 0 ||
        (pos_init.second > 0 &&
         grid[pos_init.second][pos_init.first] == OBJECT)) {
      direction = changeDirection(direction);
    }

    bool second_try = false;

    while (true) {
      auto [is_out, pos, _] = changePosition(direction, current_position);
      if (is_out) {
        break;
      }
      if (pos.has_value()) {
        current_position = pos.value();
        already_explored_pos.insert(current_position);
        if (second_try) {
          second_try = false;
        }
      } else {
        direction = changeDirection(direction);
        if (second_try) {
          break;
        }
        second_try = true;
      }
    }

    return already_explored_pos.size();
  }

  size_t findIfStuck() {
    vector<vector<char>> original_grid = grid;
    size_t count = 0;

    for (size_t row = 0; row < grid.size(); ++row) {
      for (size_t col = 0; col < grid[row].size(); ++col) {
        if (OBJECT != grid[row][col]) {
          grid[row][col] = OBJECT;
          Direction direction = Direction::Up;
          auto current_position = pos_init;
          set<tuple<pair<size_t, size_t>, pair<size_t, size_t>, string>>
              detect_loop;

          if (pos_init.second == 0 ||
              (pos_init.second > 0 &&
               grid[pos_init.second][pos_init.first] == OBJECT)) {
            direction = changeDirection(direction);
          }

          bool second_try = false;

          while (true) {
            auto [is_out, pos, block] =
                changePosition(direction, current_position);
            if (is_out) {
              break;
            }
            if (pos.has_value()) {
              current_position = pos.value();
              if (second_try) {
                second_try = false;
              }
            } else if (block.has_value()) {
              string str_direction = to_string(static_cast<int>(direction));
              auto blocking =
                  make_tuple(current_position, block.value(), str_direction);
              direction = changeDirection(direction);
              if (detect_loop.count(blocking)) {
                count += 1;
                break;
              }
              detect_loop.insert(blocking);
              second_try = true;
            }
          }

          grid[row][col] = original_grid[row][col];
        }
      }
    }

    return count;
  }

private:
  static vector<vector<char>> parseGrid(const string &content) {
    vector<vector<char>> grid;
    stringstream ss(content);
    string line;

    while (getline(ss, line)) {
      vector<char> row(line.begin(), line.end());
      for (char &c : row) {
        if (c == '^') {
          c = '.';
        }
      }
      grid.push_back(row);
    }
    return grid;
  }

  static pair<size_t, size_t> findInitialPosition(const string &content) {
    stringstream ss(content);
    string line;
    size_t row_idx = 0;

    while (getline(ss, line)) {
      size_t col_idx = line.find('^');
      if (col_idx != string::npos) {
        return {row_idx, col_idx};
      }
      row_idx++;
    }
    throw runtime_error("initial position not detected");
  }

  tuple<bool, optional<pair<size_t, size_t>>, optional<pair<size_t, size_t>>>
  changePosition(Direction direction, pair<size_t, size_t> position) {
    auto [y, x] = position;

    switch (direction) {
    case Direction::Up:
      if (y == 0) {
        return {true, nullopt, nullopt};
      }
      if (grid[y - 1][x] != OBJECT) {
        return {false, make_optional(make_pair(y - 1, x)), nullopt};
      }
      return {false, nullopt, make_optional(make_pair(y - 1, x))};

    case Direction::Left:
      if (x == 0) {
        return {true, nullopt, nullopt};
      }
      if (grid[y][x - 1] != OBJECT) {
        return {false, make_optional(make_pair(y, x - 1)), nullopt};
      }
      return {false, nullopt, make_optional(make_pair(y, x - 1))};

    case Direction::Down:
      if (y + 1 > right_bottom_position.first) {
        return {true, nullopt, nullopt};
      }
      if (grid[y + 1][x] != OBJECT) {
        return {false, make_optional(make_pair(y + 1, x)), nullopt};
      }
      return {false, nullopt, make_optional(make_pair(y + 1, x))};

    case Direction::Right:
      if (x + 1 > right_bottom_position.second) {
        return {true, nullopt, nullopt};
      }
      if (grid[y][x + 1] != OBJECT) {
        return {false, make_optional(make_pair(y, x + 1)), nullopt};
      }
      return {false, nullopt, make_optional(make_pair(y, x + 1))};
    }

    return {false, nullopt, nullopt};
  }

  static Direction changeDirection(Direction direction) {
    switch (direction) {
    case Direction::Up:
      return Direction::Right;
    case Direction::Right:
      return Direction::Down;
    case Direction::Down:
      return Direction::Left;
    }
    return Direction::Up;
  }
};

string readFile(const string &filename) {
  ifstream file(filename);
  if (!file.is_open()) {
    throw runtime_error("Could not open file");
  }
  stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

int main() {
  try {
    string message = readFile("crates/day06/input.txt");
    Lab grid(message);
    // --- Part One ---
    cout << "Part One solution: " << grid.findPath() << endl;
    // --- Part Two ---
    cout << "Part Two solution: " << grid.findIfStuck() << endl;
  } catch (const exception &e) {
    cerr << e.what() << endl;
  }
  return 0;
}
