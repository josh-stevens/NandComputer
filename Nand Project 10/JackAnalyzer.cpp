#include "JackTokenizer.h"

#include <fstream>
#include <iostream>
#include <string>
#include <filesystem>

int main(int argc, char* argv[]) {
  if (argc != 2) {
    std::cerr << "Usage: JackAnalyzer <file.jack>\n";
    return 1;
  }

  std::string inputFilename = argv[1];
  std::filesystem::path inputPath(inputFilename);
  std::filesystem::path outputPath =
    inputPath.parent_path() /
    (inputPath.stem().string() + "T.xml");

  JackTokenizer tokenizer(inputFilename);
  std::ofstream output(outputPath);

  if (!output) {
    std::cerr << "Could not create output file: "
              << outputPath << '\n';
    return 1;
  }

  output << "<tokens>\n";

  while (tokenizer.hasMoreTokens()) {
    tokenizer.advance();


  }

  output << "</tokens>\n";
  return 0;
}