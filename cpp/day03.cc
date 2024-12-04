#include <cctype>
#include <fstream>
#include <iostream>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

struct Problem {
  std::string program;

  explicit Problem(const std::string &input) : program(input) {}

  static Problem fromString(const std::string &input) { return Problem(input); }
};

class ProgramParser {
public:
  explicit ProgramParser(const std::string &program)
      : iterator(program.begin()), end(program.end()) {}

  int parse() {
    int result = 0;

    while (iterator != end) {
      if (parse_mul()) {
        if (peek() == '(') {
          next();

          auto a = parseNumber();
          if (a && peek() == ',') {
            next();

            auto b = parseNumber();
            if (b && peek() == ')') {
              next();
              result += (*a) * (*b);
            }
          }
        }
      } else {
        next();
      }
    }

    return result;
  }

private:
  std::string::const_iterator iterator;
  std::string::const_iterator end;

  char peek() const { return (iterator != end) ? *iterator : '\0'; }

  void next() {
    if (iterator != end) {
      iterator++;
    }
  }

  bool parse_mul() {
    static const std::string target = "mul";
    for (char ch : target) {
      if (peek() != ch) {
        return false;
      }
      next();
    }
    return true;
  }

  std::optional<int> parseNumber() {
    std::string digits;
    while (digits.size() < 3 && std::isdigit(peek())) {
      digits.push_back(peek());
      next();
    }
    return digits.empty() ? std::nullopt
                          : std::make_optional(std::stoi(digits));
  }
};

std::vector<size_t> findAllIndexes(const std::string &haystack,
                                   const std::string &needle) {
  std::vector<size_t> indexes;
  size_t start = 0;

  while ((start = haystack.find(needle, start)) != std::string::npos) {
    indexes.push_back(start);
    start += needle.size();
  }

  return indexes;
}

std::optional<size_t> closest_greater_than(size_t x,
                                           const std::vector<size_t> &values) {
  std::optional<size_t> closest;
  for (size_t value : values) {
    if (value > x && (!closest || value < *closest)) {
      closest = value;
    }
  }
  return closest;
}

int count_mul(const std::string &message) {
  Problem problem = Problem::fromString(message);
  ProgramParser parser(problem.program);
  return parser.parse();
}

int parseWithRules(const std::string &message) {
  const std::string doSubstring = "do()";
  const std::string dontSubstring = "don't()";

  auto doPositions = findAllIndexes(message, doSubstring);
  auto dontPositions = findAllIndexes(message, dontSubstring);

  if (doPositions.empty() && dontPositions.empty()) {
    return count_mul(message);
  }

  std::string firstSubstring =
      message.substr(0, doPositions[0] + doSubstring.size());
  int count = count_mul(firstSubstring);
  std::optional<size_t> nextDont;

  for (size_t i : doPositions) {
    if (!nextDont || i > *nextDont) {
      auto nextDontOpt = closest_greater_than(i, dontPositions);
      if (nextDontOpt) {
        size_t nextDontValue = *nextDontOpt;
        std::string substring = message.substr(i, nextDontValue - i);
        count += count_mul(substring);
        nextDont = nextDontValue;
      }
    }
  }

  return count;
}

std::string readFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file.is_open()) {
    throw std::runtime_error("Unable to open file: " + filePath);
  }

  std::ostringstream content;
  content << file.rdbuf();
  return content.str();
}

int main() {

  std::string message = readFile("crates/day03/input.txt");

  // --- Part One ---
  int count = count_mul(message);
  std::cout << "Part One solution: sum is " << count << std::endl;

  // --- Part Two ---
  count = parseWithRules(message);
  std::cout << "Part Two solution: sum is " << count << std::endl;

  return 0;
}
