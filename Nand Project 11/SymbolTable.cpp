#include "SymbolTable.h"

SymbolTable::SymbolTable()
  : staticCounter(0),
    fieldCounter(0),
    argCounter(0),
    varCounter(0) {
}

void SymbolTable::startSubroutine() {
  subroutineScope.clear();
  argCounter = 0;
  varCounter = 0;
}

void SymbolTable::define(
  const std::string& name,
  const std::string& type,
  Symbol kind) {
  if (kind == Symbol::Static || kind == Symbol::Field) {
    int& index = kind == Symbol::Static ? staticCounter : fieldCounter;
    classScope.emplace(name, SymbolInfo{type, kind, index});
    index++;
  } else if (kind == Symbol::Arg || kind == Symbol::Var) {
    int& index = kind == Symbol::Arg ? argCounter : varCounter;
    subroutineScope.emplace(name, SymbolInfo{type, kind, index});
    index++;
  }
}

int SymbolTable::varCount(Symbol kind) const {
  switch(kind) {
    case Symbol::Static:
      return staticCounter;
    case Symbol::Field:
      return fieldCounter;
    case Symbol::Arg:
      return argCounter;
    case Symbol::Var:
      return varCounter;
    case Symbol::None:
    default:
      return 0;
  }
}

Symbol SymbolTable::kindOf(const std::string& name) const {
  auto it = subroutineScope.find(name);

  if (it != subroutineScope.end()) {
    return it->second.kind;
  }

  it = classScope.find(name);

  if (it != classScope.end()) {
    return it->second.kind;
  }

  return Symbol::None;
}

std::string SymbolTable::typeOf(const std::string& name) const {
  auto it = subroutineScope.find(name);

  if (it != subroutineScope.end()) {
    return it->second.type;
  }

  it = classScope.find(name);

  if (it != classScope.end()) {
    return it->second.type;
  }

  return "";
}

int SymbolTable::indexOf(const std::string& name) const {
  auto it = subroutineScope.find(name);

  if (it != subroutineScope.end()) {
    return it->second.index;
  }

  it = classScope.find(name);

  if (it != classScope.end()) {
    return it->second.index;
  }

  return -1;
}