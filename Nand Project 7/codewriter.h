#ifndef CODEWRITER_H
#define CODEWRITER_H

#include "command.h"

void codewriter_init(const char *filename);

void codewriter_set_filename(const char *filename);

void codewriter_arithmetic(const char *command);

void codewriter_write_push_pop(
  CommandType command,
  const char *segment,
  int index);

void codewriter_close();

#endif
