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
};

class JackTokenizer {
  public:
    explicit JackTokenizer(const std::string& filename);

    bool hasMoreTokens() const;
    void advance();

    TokenType tokenType() const;
    std::string keyword() const;
    char symbol() const;
    std::string identifier() const;
    int intVal() const;
    std::string stringVal() const;

  private:
    std::vector<Token> tokens;
    int current;
};
