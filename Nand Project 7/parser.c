#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

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
  //tokenize current_line
}

CommandType parser_command_type(void) {
  char command[32];
  int i = 0;

  while (current_line[i] != '\0' &&
          !isspace((unsigned char)current_line[i])) {

      command[i] = current_line[i];
      i++;
  }

  command[i] = '\0';
  if (strcmp(command, "add") == 0 ||
    strcmp(command, "sub") == 0 ||
    strcmp(command, "neg") == 0 ||
    strcmp(command, "eq") == 0 ||
    strcmp(command, "gt") == 0 ||
    strcmp(command, "lt") == 0 ||
    strcmp(command, "and") == 0 ||
    strcmp(command, "or") == 0 ||
    strcmp(command, "not") == 0)
{
    return C_ARITHMETIC;
}
  if (strcmp(command, "push") == 0) {
    return C_PUSH;
  }
  if (strcmp(command, "pop") == 0) {
    return C_POP;
  }
  if (strcmp(command, "label") == 0) {
    return C_LABEL;
  }
  if (strcmp(command, "goto") == 0) {
    return C_GOTO;
  }
  if (strcmp(command, "if-goto") == 0) {
    return C_IF;
  }
  if (strcmp(command, "function") == 0) {
    return C_FUNCTION;
  }
  if (strcmp(command, "return") == 0) {
    return C_RETURN;
  }
  if (strcmp(command, "call") == 0) {
    return C_CALL;
  }
}

const char *parser_arg1(void) {
  return NULL;
}

int parser_arg2(void) {
  return 0;
}

void parser_close() {
  
}