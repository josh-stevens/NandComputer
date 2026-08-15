#include "parser.h"
#include "codewriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  const char *input_filename = argv[1];
  parser_init(input_filename);

  // Create output filename by replacing .vm with .asm
  char output_filename[256];
  char *dot = strrchr(input_filename, '.');

  if (dot == NULL || strcmp(dot, ".vm") != 0) {
    fprintf(stderr, "Input file must have a .vm extension.\n");
    parser_close();
    exit(1);
  }
  snprintf(output_filename,
           sizeof(output_filename),
           "%.*s.asm",
           (int)(dot - input_filename),
           input_filename);
  codewriter_init(output_filename);
  codewriter_set_filename(input_filename);

  while (parser_has_more_commands()) {
    parser_advance();
    CommandType command_type = parser_command_type();

    if (command_type == C_ARITHMETIC) {
      codewriter_write_arithmetic(parser_arg1());
    } else if (command_type == C_PUSH || command_type == C_POP) {
      codewriter_write_push_pop(command_type, parser_arg1(), parser_arg2());
    }
  }

  parser_close();
  codewriter_close();

  return 0;
}