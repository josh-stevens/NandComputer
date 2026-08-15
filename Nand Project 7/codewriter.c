#include "codewriter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FILE *output_file;
static char current_filename[256];
static int label_counter = 0;

void codewriter_init(const char *filename) {
  output_file = fopen(filename, "w");
  if (output_file == NULL) {
    printf("Cannot open output file.");
    exit(1);
  }
  label_counter = 0;
}

void codewriter_set_filename(const char *filename) {
  const char *basename = strrchr(filename, '/');

  if (basename != NULL) {
    basename++;
  } else {
      basename = filename;
  }

  const char *dot = strrchr(basename, '.');

  if (dot == NULL) {
    dot = basename + strlen(basename);
  }

  size_t length = dot - basename;

  if (length >= sizeof(current_filename)) {
    length = sizeof(current_filename) - 1;
  }

  memcpy(current_filename, basename, length);
  current_filename[length] = '\0';
}

void codewriter_write_arithmetic(const char *command) {
  if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 ||
      strcmp(command, "and") == 0 || strcmp(command, "or") == 0) {
        // update the stack pointer and store the top two values in D and M
        fprintf(output_file,
                "%s",
                "@SP\n"
                "AM=M-1\n"
                "D=M\n"
                "A=A-1\n");

        // perform the arithmetic operation based on the command
        if (strcmp(command, "add") == 0) {
          fprintf(output_file, "%s", "M=D+M\n");
        } else if (strcmp(command, "sub") == 0) {
          fprintf(output_file, "%s", "M=M-D\n");
        } else if (strcmp(command, "and") == 0) {
          fprintf(output_file, "%s", "M=D&M\n");
        } else if (strcmp(command, "or") == 0) {
          fprintf(output_file, "%s", "M=D|M\n");
        }
  } else if (strcmp(command, "neg") == 0 || strcmp(command, "not") == 0) {
    // leave the stack pointer at the top of the stack and put the top value in M
    fprintf(output_file,
            "%s",
            "@SP\n"
            "A=M-1\n");

    // negate or invert the top value based on the command
    if (strcmp(command, "neg") == 0) {
      fprintf(output_file, "%s", "M=-M\n");
    } else if (strcmp(command, "not") == 0) {
      fprintf(output_file, "%s", "M=!M\n");
    }
  } else {
    // create unique labels for the comparison operations
    char true_label[32];
    char end_label[32];
    snprintf(true_label, sizeof(true_label), "TRUE_%d", label_counter);
    snprintf(end_label, sizeof(end_label), "END_%d", label_counter);
    label_counter++;
  
    // update the stack pointer and store difference of the top two values in D
    fprintf(output_file,
            "%s",
            "@SP\n"
            "AM=M-1\n"
            "D=M\n"
            "A=A-1\n"
            "D=M-D\n");

    // jump to the true label if the comparison is true
    if (strcmp(command, "eq") == 0) {
      fprintf(output_file, "@%s\nD;JEQ\n", true_label);
    } else if (strcmp(command, "gt") == 0) {
      fprintf(output_file, "@%s\nD;JGT\n", true_label);
    } else {
      fprintf(output_file, "@%s\nD;JLT\n", true_label);
    }

    // if the comparison is false, set the top value to 0 and jump to the end label
    fprintf(output_file, "@SP\nA=M-1\nM=0\n@%s\n0;JMP\n", end_label);

    // if the comparison is true, set the top value to -1
    fprintf(output_file, "(%s)\n@SP\nA=M-1\nM=-1\n", true_label);

    // end label
    fprintf(output_file, "(%s)\n", end_label);
  }
}

const char *get_base_segment(const char *segment) {
  if (strcmp(segment, "local") == 0) {
    return "LCL";
  } else if (strcmp(segment, "argument") == 0) {
    return "ARG";
  } else if (strcmp(segment, "this") == 0) {
    return "THIS";
  } else if (strcmp(segment, "that") == 0) {
    return "THAT";
  }
  return NULL;
}

void codewriter_write_push_pop(
  CommandType command,
  const char *segment,
  int index
) {
  if (command == C_PUSH) {
    if (strcmp(segment, "constant") == 0) {
      fprintf(output_file,
              "@%d\n"
              "D=A\n"
              "@SP\n"
              "A=M\n"
              "M=D\n"
              "@SP\n"
              "M=M+1\n",
              index);
    } else if (strcmp(segment, "temp") == 0) {
      fprintf(output_file,
              "@%d\n"
              "D=M\n"
              "@SP\n"
              "A=M\n"
              "M=D\n"
              "@SP\n"
              "M=M+1\n",
              5 + index);
    } else if (strcmp(segment, "pointer") == 0) {
      const char *pointer = (index == 0) ? "THIS" : "THAT";

      fprintf(output_file,
              "@%s\n"
              "D=M\n"
              "@SP\n"
              "A=M\n"
              "M=D\n"
              "@SP\n"
              "M=M+1\n",
              pointer);
    } else if (strcmp(segment, "static") == 0) {
      fprintf(output_file,
              "@%s.%d\n"
              "D=M\n"
              "@SP\n"
              "A=M\n"
              "M=D\n"
              "@SP\n"
              "M=M+1\n",
              current_filename, index);
    } else {
      // Handle local, argument, this, that segments
      const char *base_segment = get_base_segment(segment);

      fprintf(output_file,
              "@%d\n"
              "D=A\n"
              "@%s\n"
              "A=D+M\n"
              "D=M\n"
              "@SP\n"
              "A=M\n"
              "M=D\n"
              "@SP\n"
              "M=M+1\n",
              index,
              base_segment);
    }
  } else if (command == C_POP) {
    if (strcmp(segment, "temp") == 0) {
      fprintf(output_file,
              "@SP\n"
              "AM=M-1\n"
              "D=M\n"
              "@%d\n"
              "M=D\n",
              5 + index);
    } else if (strcmp(segment, "pointer") == 0) {
      const char *pointer = (index == 0) ? "THIS" : "THAT";

      fprintf(output_file,
              "@SP\n"
              "AM=M-1\n"
              "D=M\n"
              "@%s\n"
              "M=D\n",
              pointer);
    } else if (strcmp(segment, "static") == 0) {
      fprintf(output_file,
              "@SP\n"
              "AM=M-1\n"
              "D=M\n"
              "@%s.%d\n"
              "M=D\n",
              current_filename, index);
    } else {
      // Handle local, argument, this, that segments
      const char *base_segment = get_base_segment(segment);

      fprintf(output_file,
              "@%d\n"
              "D=A\n"
              "@%s\n"
              "D=D+M\n"
              "@R13\n"
              "M=D\n"
              "@SP\n"
              "AM=M-1\n"
              "D=M\n"
              "@R13\n"
              "A=M\n"
              "M=D\n",
              index,
              base_segment);
    }
  }
}

void codewriter_close() {
  fclose(output_file);
}
