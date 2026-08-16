#ifndef PARSER_H
#define PARSER_H

#include "command.h"

void parser_init(const char *filename);

int parser_has_more_commands(void);
void parser_advance(void);

CommandType parser_command_type(void);

const char *parser_arg1(void);
int parser_arg2(void);

void parser_close();

#endif
