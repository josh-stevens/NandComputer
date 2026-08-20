#include "CompilationEngine.h"

#include <stdexcept>

namespace {
  std::string escape(char c) {
    switch(c) {
      case '<':
        return "&lt;";
      case '>':
        return "&gt;";
      case '&':
        return "&amp;";
      default:
        return std::string(1, c);
    }
  }
}

CompilationEngine::CompilationEngine(
  JackTokenizer& tokenizer,
  const std::string& outputFilename
)
  : tokenizer(tokenizer),
    output(outputFilename) {
  if (!output) {
    throw std::runtime_error(
      "Could not create output file: " + outputFilename
    );
  }
}

void CompilationEngine::compileClass() {
  output << "<class>\n";

  // class
  if (tokenizer.tokenType() != TokenType::Keyword ||
      tokenizer.keyword() != "class") {
    throw std::runtime_error("Expected 'class'");
  }
  writeToken();

  // class name
  if (tokenizer.tokenType() != TokenType::Identifier) {
    throw std::runtime_error("Expected class name");
  }
  writeToken();

  // {
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '{') {
    throw std::runtime_error("Expected '{'");
  }
  writeToken();

  tokenizer.advance();
  
  while (tokenizer.tokenType() == TokenType::Keyword &&
      (tokenizer.keyword() == "static" ||
      tokenizer.keyword() == "field")) {
    compileClassVarDec();
  }

  while (tokenizer.tokenType() == TokenType::Keyword &&
      (tokenizer.keyword() == "constructor" ||
      tokenizer.keyword() == "function" ||
      tokenizer.keyword() == "method")) {
    compileSubroutine();
  }

  // }
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '}') {
    throw std::runtime_error("Expected '}'");
  }
  writeToken();

  output << "</class>\n";
}

void CompilationEngine::compileClassVarDec() {
  output << "<classVarDec>\n";

  // static | field
  writeToken();

  // type
  compileType();

  // identifier
  if (tokenizer.tokenType() != TokenType::Identifier) {
    throw std::runtime_error("Expected variable name");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      (tokenizer.symbol() != ';' && tokenizer.symbol() != ',')) {
    throw std::runtime_error("Expected semicolon");
  }

  while (tokenizer.symbol() == ',') {
    // comma
    writeToken();

    // identifier
    if (tokenizer.tokenType() != TokenType::Identifier) {
      throw std::runtime_error("Expected variable name");
    }
    writeToken();
  }

  // semicolon
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    throw std::runtime_error("Expected semicolon");
  }
  writeToken();

  output << "</classVarDec>\n";
}

void CompilationEngine::compileSubroutine() {
  output << "<subroutineDec>\n";

  // constructor | function | method
  writeToken();

  // return type
  compileType(true);

  // subroutine name
  if (tokenizer.tokenType() != TokenType::Identifier) {
    throw std::runtime_error("Expected subroutine name");
  }

  writeToken();

  // (
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '(') {
    throw std::runtime_error("Expected '('");
  }

  writeToken();

  compileParameterList();

  // )
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ')') {
    throw std::runtime_error("Expected ')'");
  }

  writeToken();

  // {
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '{') {
    throw std::runtime_error("Expected '{'");
  }

  writeToken();

  while(tokenizer.tokenType() == TokenType::Keyword &&
        tokenizer.keyword() == "var") {
    compileVarDec();
  }
  compileStatements(); // not yet implemented

  // }
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '}') {
    throw std::runtime_error("Expected '}'");
  }

  writeToken();

  output << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
  // empty parameter list
  if (tokenizer.tokenType() == TokenType::Symbol &&
      tokenizer.symbol() == ')') {
    return;
  }

  compileType();

    // identifier
  if (tokenizer.tokenType() != TokenType::Identifier) {
    throw std::runtime_error("Expected variable name");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      (tokenizer.symbol() != ',' && tokenizer.symbol() != ')')) {
    throw std::runtime_error("Expected ')'");
  }

  while (tokenizer.symbol() == ',') {
    // comma
    writeToken();

    compileType();

    // identifier
    if (tokenizer.tokenType() != TokenType::Identifier) {
      throw std::runtime_error("Expected variable name");
    }
    writeToken();
  }
}

void CompilationEngine::compileVarDec() {
  output << "<varDec>\n";

  // var
  writeToken();

  compileType();

  // identifier
  if (tokenizer.tokenType() != TokenType::Identifier) {
    throw std::runtime_error("Expected variable name");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      (tokenizer.symbol() != ';' && tokenizer.symbol() != ',')) {
    throw std::runtime_error("Expected semicolon");
  }

  while (tokenizer.symbol() == ',') {
    // comma
    writeToken();

    // identifier
    if (tokenizer.tokenType() != TokenType::Identifier) {
      throw std::runtime_error("Expected variable name");
    }
    writeToken();
  }

  // semicolon
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    throw std::runtime_error("Expected semicolon");
  }
  writeToken();

  output << "</varDec>\n";
}

void CompilationEngine::compileStatements() {

}

void CompilationEngine::compileDo() {

}

void CompilationEngine::compileLet() {

}

void CompilationEngine::compileWhile() {

}

void CompilationEngine::compileReturn() {
  if (tokenizer.tokenType() != TokenType::Keyword ||
      tokenizer.keyword() != "return") {
    throw std::runtime_error("Function does not return");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    throw std::runtime_error("Expected ';'");
  }
  writeToken();
}

void CompilationEngine::compileIf() {

}

void CompilationEngine::writeToken() {
  switch(tokenizer.tokenType()) {
    case TokenType::Keyword:
      output << "<keyword> "
              << tokenizer.keyword()
              << " </keyword>\n";
      break;

    case TokenType::Symbol:
      output << "<symbol> "
            << escape(tokenizer.symbol())
            << " </symbol>\n";
      break;

    case TokenType::Identifier:
      output << "<identifier> "
            << tokenizer.identifier()
            << " </identifier>\n";
      break;

    case TokenType::IntConstant:
      output << "<integerConstant> "
            << tokenizer.intVal()
            << " </integerConstant>\n";
      break;

    case TokenType::StringConstant:
      output << "<stringConstant> "
            << tokenizer.stringVal()
            << " </stringConstant>\n";
      break;
  }

  if (tokenizer.hasMoreTokens()) {
    tokenizer.advance();
  }
}

void CompilationEngine::compileType(bool allowVoid) {
  if (tokenizer.tokenType() == TokenType::Keyword) {
    if (tokenizer.keyword() != "int" &&
        tokenizer.keyword() != "char" &&
        tokenizer.keyword() != "boolean" &&
        !(allowVoid && tokenizer.keyword() == "void")) {
      throw std::runtime_error("Expected type");
    }
  } else if (tokenizer.tokenType() != TokenType::Identifier) {
    throw std::runtime_error("Expected type");
  }

  writeToken();
}