#include "VMWriter.h"

VMWriter::VMWriter(const std::string& outputFilename) : output(outputFilename) {
  if (!output) {
    throw std::runtime_error(
      "Could not create output file: " + outputFilename
    );
  }
}

void VMWriter::writePush(Segment segment, int index) {
  output << "push ";

  switch(segment) {
    case Segment::Const:
      output << "constant ";
      break;
    case Segment::Arg:
      output << "argument ";
      break;
    case Segment::Local:
      output << "local ";
      break;
    case Segment::Static:
      output << "static ";
      break;
    case Segment::This:
      output << "this ";
      break;
    case Segment::That:
      output << "that ";
      break;
    case Segment::Pointer:
      output << "pointer ";
      break;
    case Segment::Temp:
      output << "temp ";
  }

  output << index << "\n";
}

void VMWriter::writePop(Segment segment, int index) {
  output << "pop ";

  switch(segment) {
    case Segment::Const:
      output << "constant ";
      break;
    case Segment::Arg:
      output << "argument ";
      break;
    case Segment::Local:
      output << "local ";
      break;
    case Segment::Static:
      output << "static ";
      break;
    case Segment::This:
      output << "this ";
      break;
    case Segment::That:
      output << "that ";
      break;
    case Segment::Pointer:
      output << "pointer ";
      break;
    case Segment::Temp:
      output << "temp ";
  }

  output << index << "\n";
}

void VMWriter::writeArithmetic(Command command) {
  switch (command) {
    case Command::Add:
      output << "add\n";
      break;
    case Command::Sub:
      output << "sub\n";
      break;
    case Command::Neg:
      output << "neg\n";
      break;
    case Command::Eq:
      output << "eq\n";
      break;
    case Command::Gt:
      output << "gt\n";
      break;
    case Command::Lt:
      output << "lt\n";
      break;
    case Command::And:
      output << "and\n";
      break;
    case Command::Or:
      output << "or\n";
      break;
    case Command::Not:
      output << "not\n";
      break;
  }
}

void VMWriter::writeLabel(const std::string& label) {
  output << "label " << label << "\n";
}

void VMWriter::writeGoto(const std::string& label) {
  output << "goto " << label << "\n";
}

void VMWriter::writeIf(const std::string& label) {
  output << "if-goto " << label << "\n";
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
  output << "call " << name << " " << nArgs << "\n";
}

void VMWriter::writeFunction(const std::string& name, int nLocals) {
  output << "function " << name << " " << nLocals << "\n";
}

void VMWriter::writeReturn() {
  output << "return\n";
}

void VMWriter::close() {
  output.close();
}
