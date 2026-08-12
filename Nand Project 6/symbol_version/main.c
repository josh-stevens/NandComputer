#include "parser.h"
#include "code.h"
#include "symbol_table.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

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

  symbol_table_init();

  // First pass, write the symbol table

  int rom_address = 0;
  int next_ram_address = 16;

  while (parser_has_more_commands()) {
    parser_advance();

    CommandType type = parser_command_type();
    if (type == L_COMMAND) {
      const char *symbol = parser_symbol();
      symbol_table_add(symbol, rom_address);
    } else {
      rom_address++;
    }
  }

  // Second pass, write the instructions
  parser_reset();

  FILE *output_file = fopen(output_filename, "w");

  if (output_file == NULL) {
    parser_close();
    return 1;
  }
  int first_instruction = 1;

  while (parser_has_more_commands()) {
    parser_advance();
    CommandType type = parser_command_type();
    if (type == A_COMMAND) {
      const char *symbol = parser_symbol();
      int value;

      if (isdigit((unsigned char)symbol[0])) {
        // convert symbol to int
        value = strtol(symbol, NULL, 10);
      } else if (symbol_table_contains(symbol)) {
        value = symbol_table_get(symbol);
      } else {
          // new variable - assign next available RAM address
          value = next_ram_address;
          symbol_table_add(symbol, next_ram_address);
          next_ram_address++;
      }

      // convert int to 16-bit binary instruction with leading 0
      char instruction[17];
      instruction[16] = '\0';
      for (int i = 15; i >= 1; i--) {
        instruction[i] = (value & 1) ? '1' : '0';
        value >>= 1;
      }
      instruction[0] = '0';

      // add newline if not the first instruction
      if (!first_instruction) {
        fprintf(output_file, "\n");
      }
      first_instruction = 0;
      fprintf(output_file, "%s", instruction);
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

      // add newline if not the first instruction
      if (!first_instruction) {
        fprintf(output_file, "\n");
      }
      first_instruction = 0;

      fprintf(output_file, "%s", instruction);
    }
  }

  parser_close();
  fclose(output_file);

  return 0;
}