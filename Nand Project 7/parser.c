#include "parser.h"
#include <stdlib.h>
#include <stdio.h>

static FILE *input_file;
static char next_line[256];
static char current_line[256];
static char *tokens[3];
static int token_count;

void parser_init(const char *filename) {
  input_file = fopen(filename, "r");
  if (input_file == NULL) {
    printf("Cannot open input file.");
    exit(1);
  }
  next_line[0] = '\0';
  current_line[0] = '\0';
  tokens[0] = NULL;
  tokens[1] = NULL;
  tokens[2] = NULL;
  token_count = 0;
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
  token_count = 0;
  tokens[0] = NULL;
  tokens[1] = NULL;
  tokens[2] = NULL;
  char *token = strtok(current_line, " \t");
  while (token != NULL && token_count < 3) {
    tokens[token_count] = token;
    token_count++;
    token = strtok(NULL, " \t");
  }
}

CommandType parser_command_type(void) {
  if (strcmp(tokens[0], "add") == 0 ||
    strcmp(tokens[0], "sub") == 0 ||
    strcmp(tokens[0], "neg") == 0 ||
    strcmp(tokens[0], "eq") == 0 ||
    strcmp(tokens[0], "gt") == 0 ||
    strcmp(tokens[0], "lt") == 0 ||
    strcmp(tokens[0], "and") == 0 ||
    strcmp(tokens[0], "or") == 0 ||
    strcmp(tokens[0], "not") == 0)
{
    return C_ARITHMETIC;
}
  if (strcmp(tokens[0], "push") == 0) {
    return C_PUSH;
  }
  if (strcmp(tokens[0], "pop") == 0) {
    return C_POP;
  }
  if (strcmp(tokens[0], "label") == 0) {
    return C_LABEL;
  }
  if (strcmp(tokens[0], "goto") == 0) {
    return C_GOTO;
  }
  if (strcmp(tokens[0], "if-goto") == 0) {
    return C_IF;
  }
  if (strcmp(tokens[0], "function") == 0) {
    return C_FUNCTION;
  }
  if (strcmp(tokens[0], "return") == 0) {
    return C_RETURN;
  }
  if (strcmp(tokens[0], "call") == 0) {
    return C_CALL;
  }
}

const char *parser_arg1(void) {
  if (parser_command_type() == C_ARITHMETIC) {
    return tokens[0];
  }
  
  return tokens[1];
}

int parser_arg2(void) {
  return atoi(tokens[2]);
}

void parser_close() {
  fclose(input_file);
  input_file = NULL;
}
