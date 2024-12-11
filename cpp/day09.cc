#include "helpers.h"
#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

enum class File { Free, Taken };

struct TakenFile {
  uint32_t id;
};

class FileSystem {
private:
  std::vector<std::optional<TakenFile>> files;
  std::vector<std::optional<TakenFile>> defragmentedFiles;

public:
  FileSystem(const std::string &content) {
    uint32_t id = 0;
    for (const auto &line : split(content, '\n')) {
      for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (std::isdigit(c)) {
          uint32_t digit = c - '0';
          if (i % 2 == 0) {
            for (uint32_t j = 0; j < digit; ++j) {
              files.emplace_back(TakenFile{id});
            }
            id++;
          } else {
            for (uint32_t j = 0; j < digit; ++j) {
              files.emplace_back(std::nullopt);
            }
          }
        }
      }
    }
    defragmentedFiles = files;
  }

  static size_t
  countFreeFiles(const std::vector<std::optional<TakenFile>> &files,
                 size_t index) {
    size_t count = 0;
    for (size_t i = index; i < files.size(); ++i) {
      if (files[i].has_value()) {
        break;
      }
      count++;
    }
    return count;
  }

  static std::optional<std::pair<size_t, size_t>>
  detectBlock(const std::vector<std::optional<TakenFile>> &files,
              size_t indexEnd) {
    std::optional<std::pair<uint32_t, size_t>> currentBlock;

    for (size_t i = indexEnd; i-- > 0;) {
      if (!files[i].has_value()) {
        if (currentBlock.has_value()) {
          return std::make_pair(i + 1, currentBlock->second);
        }
        currentBlock.reset();
      } else if (files[i]->id) {
        if (!currentBlock.has_value()) {
          currentBlock = std::make_pair(files[i]->id, 1);
        } else if (files[i]->id != currentBlock->first) {
          return std::make_pair(i + 1, currentBlock->second);
        } else {
          currentBlock->second++;
        }
      }
    }

    return std::nullopt;
  }

  void fragmentFiles() {
    size_t last_index = defragmentedFiles.size();
    for (size_t i = 0; i < defragmentedFiles.size(); i++) {
      if (!defragmentedFiles[i].has_value()) {
        std::optional<size_t> check_index;

        for (size_t j = last_index; j > i; j--) {
          if (defragmentedFiles[j - 1].has_value()) {
            check_index = j - 1;
            break;
          }
        }
        if (check_index.has_value()) {
          last_index = check_index.value();
          std::iter_swap(defragmentedFiles.begin() + i,
                         defragmentedFiles.begin() + last_index);
        }
      }
    }
  }

  void fragmentFilesBlock() {
    size_t endVec = files.size();
    while (auto block = detectBlock(files, endVec)) {
      size_t startIndex = block->first;
      size_t size = block->second;
      bool followingFreeFile = false;
      for (size_t i = 0; i < files.size(); ++i) {
        if (!files[i].has_value()) {
          if (!followingFreeFile) {
            followingFreeFile = true;
            size_t numberFreeFiles = countFreeFiles(files, i);
            if (numberFreeFiles >= size && i < startIndex) {
              for (size_t j = 0; j < size; ++j) {
                std::swap(files[i + j], files[startIndex + j]);
              }
              break;
            }
          }
        } else {
          followingFreeFile = false;
        }
      }
      endVec = startIndex;
    }
  }

  static uint64_t
  calculateChecksum(const std::vector<std::optional<TakenFile>> &files) {
    uint64_t checksum = 0;
    for (size_t i = 0; i < files.size(); ++i) {
      if (files[i].has_value()) {
        checksum += i * files[i]->id;
      }
    }
    return checksum;
  }

  const std::vector<std::optional<TakenFile>> &getDefragmentedFiles() const {
    return defragmentedFiles;
  }

  const std::vector<std::optional<TakenFile>> &getFiles() const {
    return files;
  }

private:
  static std::vector<std::string> split(const std::string &str,
                                        char delimiter) {
    std::vector<std::string> tokens;
    size_t start = 0, end = 0;
    while ((end = str.find(delimiter, start)) != std::string::npos) {
      tokens.push_back(str.substr(start, end - start));
      start = end + 1;
    }
    tokens.push_back(str.substr(start));
    return tokens;
  }
};

int main() {
  try {
    std::string message = readFile("crates/day09/input.txt");

    // --- Part One ---
    FileSystem checker(message);
    checker.fragmentFiles();
    std::cout << "Part One solution: "
              << FileSystem::calculateChecksum(checker.getDefragmentedFiles())
              << std::endl;

    // --- Part Two ---
    checker.fragmentFilesBlock();
    std::cout << "Part Two solution: "
              << FileSystem::calculateChecksum(checker.getFiles()) << std::endl;

  } catch (const std::exception &e) {
    std::cerr << "Error: " << e.what() << std::endl;
    return 1;
  }
  return 0;
}
