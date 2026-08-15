#include "codewriter.h"
#include <stdio.h>

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
  snprintf(
    current_filename,
    sizeof(current_filename),
    "%s",
    filename);
}

void codewriter_write_arithmetic(const char *command) {
  if (strcmp(command, "add") == 0 || strcmp(command, "sub") == 0 ||
      strcmp(command, "and") == 0 || strcmp(command, "or") == 0) {
        // update the stack pointer and store the top two values in D and M
        fprintf(output_file, "%s", "@SP\nAM=M-1\nD=M\nA=A-1\n");

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
    fprintf(output_file, "%s", "@SP\nA=M-1\n");

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
    fprintf(output_file, "%s", "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n");

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