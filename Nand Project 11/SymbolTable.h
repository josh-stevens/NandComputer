#pragma once
#include <string>
#include <unordered_map>

enum class Symbol {
  Static,
  Field,
  Arg,
  Var,
  None
};

struct SymbolInfo {
  std::string type;
  Symbol kind;
  int index;
};

class SymbolTable {
  public:
    SymbolTable();

    void startSubroutine();

    void define(const std::string& name, const std::string& type, Symbol kind);
    int varCount(Symbol kind) const;

    Symbol kindOf(const std::string& name) const;
    std::string typeOf(const std::string& name) const;
    int indexOf(const std::string& name) const;
  
  private:
    std::unordered_map<std::string, SymbolInfo> classScope;
    std::unordered_map<std::string, SymbolInfo> subroutineScope;

    int staticCounter;
    int fieldCounter;
    int argCounter;
    int varCounter;
};
