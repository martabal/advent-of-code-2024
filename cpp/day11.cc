#include "helpers.h"
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

const uint8_t BLINK_NUMBER_PART_1 = 25;
const uint8_t BLINK_NUMBER_PART_2 = 75;

class Stones {
private:
  std::unordered_map<uint64_t, size_t> stones;

  static std::unordered_map<uint64_t, size_t>
  rules(const std::unordered_map<uint64_t, size_t> &stones) {
    std::unordered_map<uint64_t, size_t> new_stones;

    for (const auto &[stone, amount] : stones) {
      std::string stone_str = std::to_string(stone);

      if (stone == 0) {
        new_stones[1] += amount;
      } else if (stone_str.length() % 2 == 0) {
        size_t mid = stone_str.length() / 2;
        uint64_t first_half = std::stoull(stone_str.substr(0, mid));
        uint64_t second_half = std::stoull(stone_str.substr(mid));
        new_stones[first_half] += amount;
        new_stones[second_half] += amount;
      } else {
        new_stones[stone * 2024] += amount;
      }
    }

    return new_stones;
  }

public:
  explicit Stones(const std::string &content) {
    std::istringstream iss(content);
    uint64_t stone;
    while (iss >> stone) {
      stones[stone]++;
    }
  }

  size_t blink(uint8_t blink_number) {
    auto new_stones = stones;
    for (uint8_t i = 0; i < blink_number; i++) {
      new_stones = rules(new_stones);
    }

    size_t sum = 0;
    for (const auto &[_, amount] : new_stones) {
      sum += amount;
    }
    return sum;
  }
};

int main() {
  try {
    std::string message = readFile("crates/day11/input.txt");
    Stones stones(message);

    // --- Part One ---
    std::cout << "Part One solution: " << stones.blink(BLINK_NUMBER_PART_1)
              << std::endl;

    // --- Part Two ---
    std::cout << "Part Two solution: " << stones.blink(BLINK_NUMBER_PART_2)
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
