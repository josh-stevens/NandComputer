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

JackTokenizer::JackTokenizer(const std::string& sourceFile)
  : current(-1), filename(sourceFile) {
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
    size_t line = 1;
    size_t column = 1;

    // line/column handling
    auto consume = [&]() {
      if (source[position] == '\n') {
        line++;
        column = 1;
      } else {
        column++;
      }

      position++;
    };

    while (position < source.length()) {
      char c = source[position];

      size_t tokenLine = line;
      size_t tokenColumn = column;

      // skip whitespace
      if (std::isspace(static_cast<unsigned char>(c))) {
        consume();
        continue;
      }

      // skip comments
      if (c == '/' && position + 1 < source.length()) {
        if (source[position + 1] == '/') {
          // line comment
          consume();
          consume();

          while (position < source.length() && source[position] != '\n') {
            consume();
          }

          continue;
        } else if (source[position + 1] == '*') {
          // block comment
          consume();
          consume();

          while (position + 1 < source.length() &&
                !(source[position] == '*' && source[position + 1] == '/')) {
            consume();
          }

          if (position + 1 >= source.length()) {
            throw std::runtime_error("Unterminated block comment");
          }

          consume();
          consume();
          continue;
        }
      }

      // symbols
      if (symbols.contains(c)) {
        tokens.push_back({
          TokenType::Symbol,
          std::string(1, c),
          tokenLine,
          tokenColumn
        });
        consume();
        continue;
      }

      // strings
      if (c == '"') {
        consume();

        std::string value;

        while (position < source.length() && source[position] != '"') {
          value += source[position];
          consume();
        }

        if (position >= source.length()) {
          throw std::runtime_error("Unterminated string constant");
        }

        consume();

        tokens.push_back({
          TokenType::StringConstant,
          value,
          tokenLine,
          tokenColumn
        });
        continue;
      }

      // integers
      if (std::isdigit(static_cast<unsigned char>(c))) {
        std::string value;

        while (position < source.length() &&
               std::isdigit(static_cast<unsigned char>(source[position]))) {
          value += source[position];
          consume();
        }

        tokens.push_back({
          TokenType::IntConstant,
          value,
          tokenLine,
          tokenColumn
        });
        continue;
      }

      // keywords and identifiers
      if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
        std::string value;

        while (position < source.length() &&
               (std::isalnum(static_cast<unsigned char>(source[position])) ||
               source[position] == '_')) {
          value += source[position];
          consume();
        }

        if (keywords.contains(value)) {
          tokens.push_back({
            TokenType::Keyword,
            value,
            tokenLine,
            tokenColumn
          });
        } else {
          tokens.push_back({
            TokenType::Identifier,
            value,
            tokenLine,
            tokenColumn
          });
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

const std::string& JackTokenizer::getFilename() const {
  return filename;
}

size_t JackTokenizer::line() const {
  return tokens[current].line;
}

size_t JackTokenizer::column() const {
  return tokens[current].column;
}
