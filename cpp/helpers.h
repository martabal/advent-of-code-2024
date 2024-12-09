#include <fstream>
#include <sstream>
#include <string>

std::string readFile(const std::string &filePath) {
  std::ifstream file(filePath);
  if (!file) {
    throw std::runtime_error("Could not open file");
  }
  return std::string((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
}