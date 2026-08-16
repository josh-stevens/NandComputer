#include "codewriter.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static FILE *output_file;
static char current_filename[256];
static int label_counter = 0;
static char current_function[256];

void codewriter_init(const char *filename) {
  output_file = fopen(filename, "w");
  if (output_file == NULL) {
    printf("Cannot open output file.");
    exit(1);
  }
  label_counter = 0;
  current_function[0] = '\0';
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

void codewriter_write_init(void) {
  fprintf(output_file,
          "@256\n"
          "D=A\n"
          "@SP\n"
          "M=D\n");
  codewriter_write_call("Sys.init", 0);
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

void codewriter_write_label(const char *label) {
  fprintf(output_file, "(%s%s)\n", current_function, label);
}

void codewriter_write_goto(const char *label) {
  fprintf(output_file, "@%s$%s\n", current_function, label);
  fprintf(output_file, "0;JMP\n");
}

void codewriter_write_if(const char *label) {
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "AM=M-1\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@%s$%s\n", current_function, label);
  fprintf(output_file, "D;JNE\n");
}

void codewriter_write_function(const char *function_name, int num_locals) {
  snprintf(current_function, sizeof(current_function), "%s", function_name);
  fprintf(output_file, "(%s)\n", function_name);
  for (int i = 0; i < num_locals; i++) {
    codewriter_write_push_pop(C_PUSH, "constant", 0);
  }
}

static void push_symbol(const char *symbol) {
  fprintf(output_file, "@%s\n", symbol);
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "A=M\n");
  fprintf(output_file, "M=D\n");
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "M=M+1\n");
}

void codewriter_write_call(const char *function_name, int num_args) {
  char return_label[32];
  snprintf(return_label, sizeof(return_label), "%s$%d", function_name, label_counter++);
  
  // Push return address
  fprintf(output_file, "@%s\n", return_label);
  fprintf(output_file, "D=A\n");
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "A=M\n");
  fprintf(output_file, "M=D\n");
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "M=M+1\n");

  // Save caller's frame
  push_symbol("LCL");
  push_symbol("ARG");
  push_symbol("THIS");
  push_symbol("THAT");

  // Reposition ARG
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@%d\n", num_args + 5);
  fprintf(output_file, "D=D-A\n");
  fprintf(output_file, "@ARG\n");
  fprintf(output_file, "M=D\n");

  // Reposition LCL
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@LCL\n");
  fprintf(output_file, "M=D\n");

  // Transfer control to the called function
  fprintf(output_file, "@%s\n", function_name);
  fprintf(output_file, "0;JMP\n");

  // Declare return label
  fprintf(output_file, "(%s)\n", return_label);
}

void codewriter_write_return(void) {
  // FRAME = LCL
  fprintf(output_file, "@LCL\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@R13\n"); // R13 is FRAME
  fprintf(output_file, "M=D\n");

  // RET = *(FRAME - 5)
  fprintf(output_file, "@5\n");
  fprintf(output_file, "A=D-A\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@R14\n"); // R14 is RET
  fprintf(output_file, "M=D\n");

  // *ARG = pop()
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "AM=M-1\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@ARG\n");
  fprintf(output_file, "A=M\n");
  fprintf(output_file, "M=D\n");

  // SP = ARG + 1
  fprintf(output_file, "@ARG\n");
  fprintf(output_file, "D=M+1\n");
  fprintf(output_file, "@SP\n");
  fprintf(output_file, "M=D\n");

  // THAT = *(FRAME - 1)
  fprintf(output_file, "@R13\n");
  fprintf(output_file, "AM=M-1\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@THAT\n");
  fprintf(output_file, "M=D\n");

  // THIS = *(FRAME - 2)
  fprintf(output_file, "@R13\n");
  fprintf(output_file, "AM=M-1\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@THIS\n");
  fprintf(output_file, "M=D\n");

  // ARG = *(FRAME - 3)
  fprintf(output_file, "@R13\n");
  fprintf(output_file, "AM=M-1\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@ARG\n");
  fprintf(output_file, "M=D\n");

  // LCL = *(FRAME - 4)
  fprintf(output_file, "@R13\n");
  fprintf(output_file, "AM=M-1\n");
  fprintf(output_file, "D=M\n");
  fprintf(output_file, "@LCL\n");
  fprintf(output_file, "M=D\n");

  // goto RET
  fprintf(output_file, "@R14\n");
  fprintf(output_file, "A=M\n");
  fprintf(output_file, "0;JMP\n");
}

void codewriter_close() {
  fclose(output_file);
}
