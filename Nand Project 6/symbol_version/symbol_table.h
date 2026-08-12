#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

void symbol_table_init(void);

void symbol_table_add(const char *symbol, int address);

int symbol_table_contains(const char *symbol);

int symbol_table_get(const char *symbol);

#endif