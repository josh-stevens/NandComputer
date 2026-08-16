#include "parser.h"
#include "codewriter.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

void translate_file(const char *input_filename) {
  parser_init(input_filename);
  
  codewriter_set_filename(input_filename);
  
  while (parser_has_more_commands()) {
    parser_advance();
    CommandType command_type = parser_command_type();
  
    if (command_type == C_ARITHMETIC) {
      codewriter_write_arithmetic(parser_arg1());
    } else if (command_type == C_PUSH || command_type == C_POP) {
      codewriter_write_push_pop(command_type, parser_arg1(), parser_arg2());
    } else if (command_type == C_LABEL) {
      codewriter_write_label(parser_arg1());
    } else if (command_type == C_GOTO) {
      codewriter_write_goto(parser_arg1());
    } else if (command_type == C_IF) {
      codewriter_write_if(parser_arg1());
    } else if (command_type == C_FUNCTION) {
      codewriter_write_function(parser_arg1(), parser_arg2());
    } else if (command_type == C_CALL) {
      codewriter_write_call(parser_arg1(), parser_arg2());
    } else if (command_type == C_RETURN) {
      codewriter_write_return();
    }
  }
  
  parser_close();
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
    return 1;
  }

  struct stat path_stat;
  if (stat(argv[1], &path_stat) != 0) {
    fprintf(stderr, "Error: Cannot access '%s'\n", argv[1]);
    return 1;
  }

  if (S_ISDIR(path_stat.st_mode)) {
    // directory
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
      fprintf(stderr, "Error: Cannot open directory '%s'\n", argv[1]);
      return 1;
    }

    const char *basename = strrchr(argv[1], '/');
    if (basename != NULL) {
      basename++;
    } else {
      basename = argv[1];
    }

    char output_filename[256];
    snprintf(output_filename,
             sizeof(output_filename),
             "%s/%s.asm",
             argv[1],
             basename);
    
    codewriter_init(output_filename);
    codewriter_write_init();

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      const char *filename = entry->d_name;

      // ignore files that do not end with .vm
      const char *dot = strrchr(filename, '.');

      if (dot == NULL || strcmp(dot, ".vm") != 0) {
        continue;
      }
      char input_filename[256];
      snprintf(input_filename,
               sizeof(input_filename),
               "%s/%s",
               argv[1],
               entry->d_name);
      translate_file(input_filename);
    }

    closedir(dir);
    codewriter_close();
  } else {
    // file
    const char *input_filename = argv[1];
    // Create output filename by replacing .vm with .asm
    char output_filename[256];
    char *dot = strrchr(input_filename, '.');
  
    if (dot == NULL || strcmp(dot, ".vm") != 0) {
      fprintf(stderr, "Input file must have a .vm extension.\n");
      return 1;
    }
    snprintf(output_filename,
             sizeof(output_filename),
             "%.*s.asm",
             (int)(dot - input_filename),
             input_filename);
    codewriter_init(output_filename);
    translate_file(input_filename);
    codewriter_close();
  }
  return 0;
}