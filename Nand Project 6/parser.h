#ifndef PARSER_H
#define PARSER_H

typedef enum {
    A_COMMAND,
    C_COMMAND,
    L_COMMAND
} CommandType;

void parser_init(const char *filename);
int parser_has_more_commands(void);
void parser_advance(void);

CommandType parser_command_type(void);

const char *parser_symbol(void);
const char *parser_dest(void);
const char *parser_comp(void);
const char *parser_jump(void);

void parser_close(void);

#endif
