#pragma once

#include <cstddef>
#include <string>
#include <vector>

enum class TokenType {
  Keyword,
  Symbol,
  Identifier,
  IntConstant,
  StringConstant
};

struct Token {
  TokenType type;
  std::string value;
  size_t line;
  size_t column;
};

class JackTokenizer {
  public:
    explicit JackTokenizer(const std::string& sourceFile);

    bool hasMoreTokens() const;
    void advance();

    TokenType tokenType() const;
    std::string keyword() const;
    char symbol() const;
    std::string identifier() const;
    int intVal() const;
    std::string stringVal() const;
    const std::string& getFilename() const;
    size_t line() const;
    size_t column() const;

  private:
    std::vector<Token> tokens;
    int current;
    std::string filename;
};
