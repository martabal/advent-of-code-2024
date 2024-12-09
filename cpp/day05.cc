#include "helpers.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class PrintQueueChecker {
public:
  PrintQueueChecker(const std::string &puzzle) { parsePuzzle(puzzle); }

  int checkPageOrder() const {
    int sum = 0;
    for (const auto &order : pageNumbers) {
      if (checkValid(order)) {
        sum += order[order.size() / 2];
      }
    }
    return sum;
  }

  int checkAndSortPageOrder() {
    int sum = 0;
    for (const auto &order : pageNumbers) {
      if (!checkValid(order)) {
        auto orderClone = order;
        while (!checkValid(orderClone)) {
          bool foundRuleBreak = false;
          int oldIdx = 0;
          int newIdx = 0;
          int replaceVal = 0;

          for (size_t idx = 0; idx < orderClone.size(); ++idx) {
            auto it = rules.find(orderClone[idx]);
            if (it == rules.end())
              continue;

            const auto &rule = it->second;

            for (int checkIdx = orderClone.size() - 1;
                 checkIdx >= static_cast<int>(idx); --checkIdx) {
              if (std::find(rule.begin(), rule.end(), orderClone[checkIdx]) !=
                  rule.end()) {
                newIdx = checkIdx;
                foundRuleBreak = true;
                oldIdx = static_cast<int>(idx);
                replaceVal = orderClone[idx];
                break;
              }
            }
            if (foundRuleBreak)
              break;
          }

          if (foundRuleBreak) {
            orderClone.erase(orderClone.begin() + oldIdx);
            orderClone.insert(orderClone.begin() + newIdx, replaceVal);
          }
        }
        sum += orderClone[orderClone.size() / 2];
      }
    }
    return sum;
  }

private:
  std::unordered_map<int, std::vector<int>> rules;
  std::vector<std::vector<int>> pageNumbers;

  void parsePuzzle(const std::string &puzzle) {
    std::istringstream stream(puzzle);
    std::string line;

    while (std::getline(stream, line) && !line.empty()) {
      auto delimiterPos = line.find('|');
      if (delimiterPos == std::string::npos)
        continue;

      int left = std::stoi(line.substr(0, delimiterPos));
      int right = std::stoi(line.substr(delimiterPos + 1));
      rules[right].push_back(left);
    }

    while (std::getline(stream, line)) {
      if (line.empty())
        continue;
      std::vector<int> numbers;
      std::istringstream numberStream(line);
      std::string number;
      while (std::getline(numberStream, number, ',')) {
        numbers.push_back(std::stoi(number));
      }
      pageNumbers.push_back(numbers);
    }
  }

  bool checkValid(const std::vector<int> &order) const {
    for (size_t idx = 0; idx < order.size(); ++idx) {
      auto it = rules.find(order[idx]);
      if (it == rules.end())
        continue;

      const auto &rule = it->second;
      for (int ruleVal : rule) {
        if (std::find(order.begin() + idx, order.end(), ruleVal) !=
            order.end()) {
          return false;
        }
      }
    }
    return true;
  }
};

int main() {
  try {
    std::string message = readFile("crates/day05/input.txt");

    // --- Part One ---
    PrintQueueChecker checker(message);
    std::cout << "Part One solution: " << checker.checkPageOrder() << std::endl;

    // --- Part Two ---
    std::cout << "Part Two solution: " << checker.checkAndSortPageOrder()
              << std::endl;
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
  }

  return 0;
}
