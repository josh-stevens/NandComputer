#ifndef CODEWRITER_H
#define CODEWRITER_H

#include "command.h"

void codewriter_init(const char *filename);

void codewriter_set_filename(const char *filename);

void codewriter_write_init(void);

void codewriter_write_arithmetic(const char *command);

void codewriter_write_push_pop(
  CommandType command,
  const char *segment,
  int index);

void codewriter_write_label(const char *label);

void codewriter_write_goto(const char *label);

void codewriter_write_if(const char *label);

void codewriter_write_call(const char *function_name, int num_args);

void codewriter_write_return(void);

void codewriter_write_function(const char *function_name, int num_locals);

void codewriter_close(void);

#endif
