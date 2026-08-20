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

  private:
    void compileType(bool allowVoid = false);
    void writeToken();
    JackTokenizer& tokenizer;
    std::ofstream output;
};