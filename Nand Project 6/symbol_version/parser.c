#include "parser.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

static FILE *input_file;
static char next_line[256];
static char current_line[256];

void parser_init(const char *filename) {
  input_file = fopen(filename, "r");
  if (input_file == NULL) {
    printf("Cannot open input file.");
    exit(1);
  }
}

static void remove_comment(char *line) {
  char *comment = strstr(line, "//");

  if (comment != NULL) {
    *comment = '\0';
  }
}

static void trim(char *line) {
  // Remove leading whitespace
  char *start = line;

  while (isspace((unsigned char)*start)) {
    start++;
  }

  if (start != line) {
    memmove(line, start, strlen(start) + 1);
  }

  // Remove trailing whitespace
  size_t length = strlen(line);

  while (length > 0 && isspace((unsigned char)line[length - 1])) {
    line[length - 1] = '\0';
    length--;
  }
}

int parser_has_more_commands() {
  while(fgets(next_line, sizeof(next_line), input_file) != NULL) {
    remove_comment(next_line);
    trim(next_line);

    if (next_line[0] != '\0') {
      return 1;
    }
  }

  return 0;
}

void parser_advance() {
  strcpy(current_line, next_line);
}

CommandType parser_command_type() {
  if (current_line[0] == '@')
    return A_COMMAND;
  if (current_line[0] == '(')
    return L_COMMAND;
  return C_COMMAND;
}

const char *parser_symbol() {
  // just remove leading @ of A command
  if (current_line[0] == '@')
    return current_line + 1;

  // remove opening and closing parens of L command
  current_line[strlen(current_line) - 1] = '\0';
  return current_line + 1;
}

const char *parser_dest() {
  static char dest[4];
  char *equal = strchr(current_line, '=');

  if (equal == NULL) {
    // no equal sign, no dest, return empty string
    dest[0] = '\0';
    return dest;
  }

  size_t length = equal - current_line;

  strncpy(dest, current_line, length);
  dest[length] = '\0';
  return dest;
}

const char *parser_comp() {
  static char comp[4];

  // Find '=' in command and move start pointer to its index + 1
  // else start pointer points to beginning of line
  char *start = strchr(current_line, '=');
  if (start != NULL)
    start++;
  else
    start = current_line;

  // Find ';' in command and calculate comp mnemonic length from end - start
  // else comp mnemonic length is length of the rest of the string
  char *end = strchr(start, ';');
  size_t length;

  if (end != NULL)
    length = end - start;
  else
    length = strlen(start);

  strncpy(comp, start, length);
  comp[length] = '\0';

  return comp;
}

const char *parser_jump() {
  char *semicolon = strchr(current_line, ';');

  if (semicolon == NULL) {
    return "";
  }

  return semicolon + 1;
}

void parser_close() {
  fclose(input_file);
}

void parser_reset() {
  rewind(input_file);
}