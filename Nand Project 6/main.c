#include "parser.h"
#include "code.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("Usage: assembler file.asm\n");
    return 1;
  }

  const char *input_filename = argv[1];
  parser_init(input_filename);

  char output_filename[256];
  strcpy(output_filename, input_filename);

  char *extension = strrchr(output_filename, '.');

  if (extension != NULL) {
    strcpy(extension, ".hack");
  }

  FILE *output_file = fopen(output_filename, "w");

  if (output_file == NULL) {
    parser_close();
    return 1;
  }

  int first_instruction = 1;

  while (parser_has_more_commands()) {
    // add newline if not the first instruction
    if (!first_instruction) {
      fprintf(output_file, "\n");
    }
    first_instruction = 0;

    parser_advance();
    CommandType type = parser_command_type();
    if (type == A_COMMAND) {
      const char *symbol = parser_symbol();

      // convert symbol to int
      long value = strtol(symbol, NULL, 10);

      // convert int to 16-bit binary instruction with leading 0
      char instruction[17];
      instruction[16] = '\0';
      for (int i = 15; i >= 1; i--) {
        instruction[i] = (value & 1) ? '1' : '0';
        value >>= 1;
      }

      // write 0 + address to output
      instruction[0] = '0';

      fprintf(output_file, "%s", instruction);
    }
    if (type == L_COMMAND) {
      // do nothing for now
    }
    if (type == C_COMMAND) {
      const char *dest = parser_dest();
      const char *comp = parser_comp();
      const char *jump = parser_jump();

      char instruction[17];

      strcpy(instruction, "111");
      strcat(instruction, code_comp(comp));
      strcat(instruction, code_dest(dest));
      strcat(instruction, code_jump(jump));

      fprintf(output_file, "%s", instruction);
    }
  }

  parser_close();
  fclose(output_file);

  return 0;
}