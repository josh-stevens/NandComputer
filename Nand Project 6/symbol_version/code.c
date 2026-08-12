#include "code.h"
#include <string.h>

const char *code_dest(const char *dest) {
  if (strcmp(dest, "M") == 0)
    return "001";
  if (strcmp(dest, "D") == 0)
    return "010";
  if (strcmp(dest, "MD") == 0)
    return "011";
  if (strcmp(dest, "A") == 0)
    return "100";
  if (strcmp(dest, "AM") == 0)
    return "101";
  if (strcmp(dest, "AD") == 0)
    return "110";
  if (strcmp(dest, "AMD") == 0)
    return "111";
  
  return "000";
}

const char *code_comp(const char *comp) {
  if (strcmp(comp, "0") == 0)
    return "0101010";
  if (strcmp(comp, "1") == 0)
    return "0111111";
  if (strcmp(comp, "-1") == 0)
    return "0111010";
  if (strcmp(comp, "D") == 0)
    return "0001100";
  if (strcmp(comp, "A") == 0)
    return "0110000";
  if (strcmp(comp, "M") == 0)
    return "1110000";
  if (strcmp(comp, "!D") == 0)
    return "0001101";
  if (strcmp(comp, "!A") == 0)
    return "0110001";
  if (strcmp(comp, "!M") == 0)
    return "1110001";
  if (strcmp(comp, "-D") == 0)
    return "0001111";
  if (strcmp(comp, "-A") == 0)
    return "0110011";
  if (strcmp(comp, "-M") == 0)
    return "1110011";
  if (strcmp(comp, "D+1") == 0)
    return "0011111";
  if (strcmp(comp, "A+1") == 0)
    return "0110111";
  if (strcmp(comp, "M+1") == 0)
    return "1110111";
  if (strcmp(comp, "D-1") == 0)
    return "0001110";
  if (strcmp(comp, "A-1") == 0)
    return "0110010";
  if (strcmp(comp, "M-1") == 0)
    return "1110010";
  if (strcmp(comp, "D+A") == 0)
    return "0000010";
  if (strcmp(comp, "D+M") == 0)
    return "1000010";
  if (strcmp(comp, "D-A") == 0)
    return "0010011";
  if (strcmp(comp, "D-M") == 0)
    return "1010011";
  if (strcmp(comp, "A-D") == 0)
    return "0000111";
  if (strcmp(comp, "M-D") == 0)
    return "1000111";
  if (strcmp(comp, "D&A") == 0)
    return "0000000";
  if (strcmp(comp, "D&M") == 0)
    return "1000000";
  if (strcmp(comp, "D|A") == 0)
    return "0010101";
  if (strcmp(comp, "D|M") == 0)
    return "1010101";
  return "0000000";
}

const char *code_jump(const char *jump) {
  if (strcmp(jump, "JGT") == 0)
    return "001";
  if (strcmp(jump, "JEQ") == 0)
    return "010";
  if (strcmp(jump, "JGE") == 0)
    return "011";
  if (strcmp(jump, "JLT") == 0)
    return "100";
  if (strcmp(jump, "JNE") == 0)
    return "101";
  if (strcmp(jump, "JLE") == 0)
    return "110";
  if (strcmp(jump, "JMP") == 0)
    return "111";
  return "000";
}