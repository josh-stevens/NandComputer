#include "JackTokenizer.h"

#include <fstream>
#include <stdexcept>
#include <unordered_set>

const std::unordered_set<char> symbols = {
  '{', '}', '(', ')', '[', ']', '.', ',', ';',
  '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'
};

const std::unordered_set<std::string> keywords = {
  "class", "constructor", "function", "method",
  "field", "static", "var", "int", "char", "boolean",
  "void", "true", "false", "null", "this",
  "let", "do", "if", "else", "while", "return"
};

JackTokenizer::JackTokenizer(const std::string& filename)
  : current(-1) {
    std::ifstream file(filename);

    if (!file) {
      throw std::runtime_error("Could not open file: " + filename);
    }

    // Read the entire input stream into a string
    std::string source(
      (std::istreambuf_iterator<char>(file)),
      std::istreambuf_iterator<char>()
    );

    size_t position = 0;

    while (position < source.length()) {
      char c = source[position];

      // skip whitespace
      if (std::isspace(static_cast<unsigned char>(c))) {
        position++;
        continue;
      }

      // skip comments
      if (c == '/' && position + 1 < source.length()) {
        if (source[position + 1] == '/') {
          // line comment
          position += 2;

          while (position < source.length() && source[position] != '\n') {
            position++;
          }

          continue;
        } else if (source[position + 1] == '*') {
          // block comment
          position += 2;

          while (position + 1 < source.length() &&
                !(source[position] == '*' && source[position + 1] == '/')) {
            position++;
          }

          if (position + 1 >= source.length()) {
            throw std::runtime_error("Unterminated block comment");
          }

          position += 2;
          continue;
        }
      }

      // symbols
      if (symbols.contains(c)) {
        tokens.push_back({TokenType::Symbol, std::string(1, c)});
        position++;
        continue;
      }

      // strings
      if (c == '"') {
        position++;

        std::string value;

        while (position < source.length() && source[position] != '"') {
          value += source[position];
          position++;
        }

        if (position >= source.length()) {
          throw std::runtime_error("Unterminated string constant");
        }

        position++;

        tokens.push_back({TokenType::StringConstant, value});
        continue;
      }

      // integers
      if (std::isdigit(static_cast<unsigned char>(c))) {
        std::string value;

        while (position < source.length() &&
               std::isdigit(static_cast<unsigned char>(source[position]))) {
          value += source[position];
          position++;
        }

        tokens.push_back({TokenType::IntConstant, value});
        continue;
      }

      // keywords and identifiers
      if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        std::string value;

        while (position < source.length() &&
               (std::isalnum(static_cast<unsigned char>(source[position])) ||
               source[position] == '_')) {
          value += source[position];
          position++;
        }

        if (keywords.contains(value)) {
          tokens.push_back({TokenType::Keyword, value});
        } else {
          tokens.push_back({TokenType::Identifier, value});
        }

        continue;
      }

      throw std::runtime_error(
        "Unexpected character: '" + std::string(1, c) +
        "' at position " + std::to_string(position)
      );
    }
    
  }

bool JackTokenizer::hasMoreTokens() const {
  return current + 1 < static_cast<int>(tokens.size());
}

void JackTokenizer::advance() {
  if (!hasMoreTokens()) {
    throw std::runtime_error("Unexpected end of file");
  }
  current++;
}

TokenType JackTokenizer::tokenType() const {
  return tokens[current].type;
}

std::string JackTokenizer::keyword() const {
  return tokens[current].value;
}

char JackTokenizer::symbol() const {
  return tokens[current].value[0];
}

std::string JackTokenizer::identifier() const {
  return tokens[current].value;
}

int JackTokenizer::intVal() const {
  return std::stoi(tokens[current].value);
}

std::string JackTokenizer::stringVal() const {
  return tokens[current].value;
}