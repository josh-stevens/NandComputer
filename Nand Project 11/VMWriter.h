#pragma once

#include <fstream>
#include <string>

enum class Segment {
  Const,
  Arg,
  Local,
  Static,
  This,
  That,
  Pointer,
  Temp
};

enum class Command {
  Add,
  Sub,
  Neg,
  Eq,
  Gt,
  Lt,
  And,
  Or,
  Not
};

class VMWriter {
  public:
    explicit VMWriter(const std::string& outputFilename);

    void writePush(Segment segment, int index);
    void writePop(Segment segment, int index);
    void writeArithmetic(Command command);

    void writeLabel(const std::string& label);
    void writeGoto(const std::string& label);
    void writeIf(const std::string& label);

    void writeCall(const std::string& name, int nArgs);
    void writeFunction(const std::string& name, int nLocals);
    void writeReturn();
    void close();

  private:
    std::ofstream output;
};