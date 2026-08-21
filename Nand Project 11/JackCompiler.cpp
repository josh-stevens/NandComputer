#include "JackTokenizer.h"
#include "CompilationEngine.h"

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

void analyzeFile(const std::filesystem::path& inputPath) {
  std::filesystem::path outputPath =
    inputPath.parent_path() /
    (inputPath.stem().string() + ".xml");

  JackTokenizer tokenizer(inputPath.string());
  CompilationEngine engine(tokenizer, outputPath.string());

  if (tokenizer.hasMoreTokens()) {
    tokenizer.advance();
    engine.compileClass();
  }

  if (tokenizer.hasMoreTokens()) {
    throw std::runtime_error("Expected one class per file");
  }
}

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: JackAnalyzer <file.jack | directory>\n";
    return 1;
  }

  std::string inputFilename = argv[1];
  std::filesystem::path inputPath(inputFilename);
  
  if (std::filesystem::is_regular_file(inputPath)) {
    analyzeFile(inputPath);
  } else if (std::filesystem::is_directory(inputPath)) {
    for (const auto& entry :
         std::filesystem::directory_iterator(inputPath)) {
      if (entry.is_regular_file() &&
          entry.path().extension() == ".jack") {
        analyzeFile(entry.path());
      }
    }
  } else {
    std::cerr << "Input is not a file or directory\n";
    return 1;
  }

  return 0;
}