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
    error(
      "Could not create output file: " + outputFilename
    );
  }
}

void CompilationEngine::compileClass() {
  output << "<class>\n";

  // class
  if (tokenizer.tokenType() != TokenType::Keyword ||
      tokenizer.keyword() != "class") {
    error("Expected 'class'");
  }
  writeToken();

  // class name
  if (tokenizer.tokenType() != TokenType::Identifier) {
    error("Expected class name");
  }
  writeToken();

  // {
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '{') {
    error("Expected '{'");
  }
  writeToken();
  
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
    error("Expected '}'");
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
    error("Expected variable name");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      (tokenizer.symbol() != ';' && tokenizer.symbol() != ',')) {
    error("Expected semicolon");
  }

  while (tokenizer.symbol() == ',') {
    // comma
    writeToken();

    // identifier
    if (tokenizer.tokenType() != TokenType::Identifier) {
      error("Expected variable name");
    }
    writeToken();
  }

  // semicolon
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    error("Expected semicolon");
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
    error("Expected subroutine name");
  }

  writeToken();

  // (
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '(') {
    error("Expected '('");
  }

  writeToken();

  compileParameterList();

  // )
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ')') {
    error("Expected ')'");
  }

  writeToken();
  output << "<subroutineBody>\n";

  // {
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '{') {
    error("Expected '{'");
  }

  writeToken();


  while(tokenizer.tokenType() == TokenType::Keyword &&
        tokenizer.keyword() == "var") {
    compileVarDec();
  }

  compileStatements();

  // } (compileStatements exits with this as the token)
  writeToken();

  output << "</subroutineBody>\n";
  output << "</subroutineDec>\n";
}

void CompilationEngine::compileParameterList() {
  output << "<parameterList>\n";

  // empty parameter list
  if (tokenizer.tokenType() == TokenType::Symbol &&
      tokenizer.symbol() == ')') {
    output << "</parameterList>\n";
    return;
  }

  compileType();

    // identifier
  if (tokenizer.tokenType() != TokenType::Identifier) {
    error("Expected variable name");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      (tokenizer.symbol() != ',' && tokenizer.symbol() != ')')) {
    error("Expected ')'");
  }

  while (tokenizer.symbol() == ',') {
    // comma
    writeToken();

    compileType();

    // identifier
    if (tokenizer.tokenType() != TokenType::Identifier) {
      error("Expected variable name");
    }
    writeToken();
  }

  output << "</parameterList>\n";
}

void CompilationEngine::compileVarDec() {
  output << "<varDec>\n";

  // var
  writeToken();

  compileType();

  // identifier
  if (tokenizer.tokenType() != TokenType::Identifier) {
    error("Expected variable name");
  }
  writeToken();

  if (tokenizer.tokenType() != TokenType::Symbol ||
      (tokenizer.symbol() != ';' && tokenizer.symbol() != ',')) {
    error("Expected semicolon");
  }

  while (tokenizer.symbol() == ',') {
    // comma
    writeToken();

    // identifier
    if (tokenizer.tokenType() != TokenType::Identifier) {
      error("Expected variable name");
    }
    writeToken();
  }

  // semicolon
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    error("Expected semicolon");
  }
  writeToken();

  output << "</varDec>\n";
}

void CompilationEngine::compileStatements() {
  output << "<statements>\n";

  while (tokenizer.tokenType() != TokenType::Symbol || 
         tokenizer.symbol() != '}') {
    if (tokenizer.tokenType() != TokenType::Keyword) {
      error("Expected statement");
    }

    if (tokenizer.keyword() == "return") {
      compileReturn();
    } else if (tokenizer.keyword() == "let") {
      compileLet();
    } else if (tokenizer.keyword() == "do") {
      compileDo();
    } else if (tokenizer.keyword() == "while") {
      compileWhile();
    } else if (tokenizer.keyword() == "if") {
      compileIf();
    } else {
      error("Expected statement");
    }
  }

  output << "</statements>\n";
}

void CompilationEngine::compileDo() {
  output << "<doStatement>\n";

  // do
  writeToken();

  compileSubroutineCall();

  // ;
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    error("Expected ';'");
  }
  writeToken();

  output << "</doStatement>\n";
}

void CompilationEngine::compileLet() {
  output << "<letStatement>\n";

  // let
  writeToken();

  // varName
  if (tokenizer.tokenType() != TokenType::Identifier) {
    error("Expected variable name");
  }
  writeToken();

  // optional [expression]
  if (tokenizer.tokenType() == TokenType::Symbol &&
      tokenizer.symbol() == '[') {
    writeToken();

    compileExpression();

    if (tokenizer.tokenType() != TokenType::Symbol ||
        tokenizer.symbol() != ']') {
      error("Expected ']'");
    }
    writeToken();
  }

  // =
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '=') {
    error("Expected '='");
  }
  writeToken();

  // expression
  compileExpression();

  // ;
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    error("Expected ';'");
  }
  writeToken();

  output << "</letStatement>\n";
}

void CompilationEngine::compileWhile() {
  output << "<whileStatement>\n";

  // while
  writeToken();

  // (
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '(') {
    error("Expected '('");
  }
  writeToken();

  // expression
  compileExpression();

  // )
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ')') {
    error("Expected ')'");
  }
  writeToken();

  // {
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '{') {
    error("Expected '{'");
  }
  writeToken();

  compileStatements();

  // }
  writeToken();

  output << "</whileStatement>\n";
}

void CompilationEngine::compileReturn() {
  output << "<returnStatement>\n";

  if (tokenizer.tokenType() != TokenType::Keyword ||
      tokenizer.keyword() != "return") {
    error("Function does not return");
  }
  writeToken();

  // optional expression
  if (tokenizer.tokenType() == TokenType::Identifier) {
    compileExpression();
  }

  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ';') {
    error("Expected ';'");
  }
  writeToken();

  output << "</returnStatement>\n";
}

void CompilationEngine::compileIf() {
  output << "<ifStatement>\n";

  // if
  writeToken();

  // (
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '(') {
    error("Expected '('");
  }
  writeToken();

  // expression
  compileExpression();

  // )
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ')') {
    error("Expected ')'");
  }
  writeToken();

  // {
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '{') {
    error("Expected '{'");
  }
  writeToken();

  compileStatements();

  // }
  writeToken();

  // optional else
  if (tokenizer.tokenType() == TokenType::Keyword &&
      tokenizer.keyword() == "else") {
    writeToken();

    // {
    if (tokenizer.tokenType() != TokenType::Symbol ||
        tokenizer.symbol() != '{') {
      error("Expected '{'");
    }
    writeToken();

    compileStatements();

    // }
    writeToken();
  }

  output << "</ifStatement>\n";
}

void CompilationEngine::compileExpression() {
  output << "<expression>\n";

  compileTerm();

  while (tokenizer.tokenType() == TokenType::Symbol &&
       (tokenizer.symbol() == '+' ||
        tokenizer.symbol() == '-' ||
        tokenizer.symbol() == '*' ||
        tokenizer.symbol() == '/' ||
        tokenizer.symbol() == '&' ||
        tokenizer.symbol() == '|' ||
        tokenizer.symbol() == '<' ||
        tokenizer.symbol() == '>' ||
        tokenizer.symbol() == '=')) {
    writeToken();
    compileTerm();
  }

  output << "</expression>\n";
}

void CompilationEngine::compileTerm() {
  output << "<term>\n";

  // integerConstant
  if (tokenizer.tokenType() == TokenType::IntConstant) {
    writeToken();

  // stringConstant
  } else if (tokenizer.tokenType() == TokenType::StringConstant) {
    writeToken();

  // keywordConstant
  } else if (tokenizer.tokenType() == TokenType::Keyword &&
             (tokenizer.keyword() == "true" ||
              tokenizer.keyword() == "false" ||
              tokenizer.keyword() == "null" ||
              tokenizer.keyword() == "this")) {
    writeToken();

  // varName | varName '[' expression ']' | subroutineCall
  } else if (tokenizer.tokenType() == TokenType::Identifier) {
    writeToken();

    // '[' expression ']'
    if (tokenizer.tokenType() == TokenType::Symbol &&
        tokenizer.symbol() == '[') {
      writeToken();

      compileExpression();

      if (tokenizer.tokenType() != TokenType::Symbol ||
          tokenizer.symbol() != ']') {
        error("Expected ']'");
      }
      writeToken();

    // '(...)' or '.method(...)'
    } else if (tokenizer.tokenType() == TokenType::Symbol &&
               (tokenizer.symbol() == '(' ||
                tokenizer.symbol() == '.')) {
      compileSubroutineCall();
    }

  // '(' expression ')'
  } else if (tokenizer.tokenType() == TokenType::Symbol &&
             tokenizer.symbol() == '(') {
    writeToken();

    compileExpression();

    if (tokenizer.tokenType() != TokenType::Symbol ||
        tokenizer.symbol() != ')') {
      error("Expected ')'");
    }
    writeToken();

  // unaryOp term
  } else if (tokenizer.tokenType() == TokenType::Symbol &&
             (tokenizer.symbol() == '-' ||
              tokenizer.symbol() == '~')) {
    writeToken();
    compileTerm();

  } else {
    error("Expected term");
  }

  output << "</term>\n";
}

void CompilationEngine::compileExpressionList() {
  output << "<expressionList>\n";

  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ')') {
    compileExpression();

    while (tokenizer.tokenType() == TokenType::Symbol &&
           tokenizer.symbol() == ',') {
      writeToken();
      compileExpression();
    }
  }

  output << "</expressionList>\n";
}

void CompilationEngine::compileSubroutineCall() {
  // subroutineName | className
  if (tokenizer.tokenType() != TokenType::Identifier) {
    error("Expected subroutine or class name");
  }
  writeToken();

  // optional .subroutineName
  if (tokenizer.tokenType() == TokenType::Symbol &&
      tokenizer.symbol() == '.') {
    writeToken();

    if (tokenizer.tokenType() != TokenType::Identifier) {
      error("Expected subroutine name");
    }
    writeToken();
  }

  // (
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != '(') {
    error("Expected '('");
  }
  writeToken();

  // expressionList
  compileExpressionList();

  // )
  if (tokenizer.tokenType() != TokenType::Symbol ||
      tokenizer.symbol() != ')') {
    error("Expected ')'");
  }
  writeToken();
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
      error("Expected type");
    }
  } else if (tokenizer.tokenType() != TokenType::Identifier) {
    error("Expected type");
  }

  writeToken();
}

[[noreturn]]
void CompilationEngine::error(const std::string& message) const {
  throw std::runtime_error(
    tokenizer.getFilename() + ":" +
    std::to_string(tokenizer.line()) + ":" +
    std::to_string(tokenizer.column()) + ": " +
    message
  );
}