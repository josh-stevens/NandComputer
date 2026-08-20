#pragma once

#include "JackTokenizer.h"

#include <fstream>
#include <string>

class CompilationEngine {
  public:
    explicit CompilationEngine(JackTokenizer& tokenizer, const std::string& outputFilename);

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    void compileExpressionList();

  private:
    void compileType(bool allowVoid = false);
    void compileSubroutineCall();
    void writeToken();
    JackTokenizer& tokenizer;
    std::ofstream output;
    [[noreturn]] void error(const std::string& message) const;
};