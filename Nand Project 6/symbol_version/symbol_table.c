#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct SymbolEntry {
  char *symbol;
  int address;
  struct SymbolEntry *next;
} SymbolEntry;

#define TABLE_SIZE 211

static SymbolEntry *table[TABLE_SIZE];

static unsigned int hash(const char *symbol) {
  unsigned int hash = 0;

  while (*symbol) {
    hash = hash * 31 + (unsigned char)*symbol;
    symbol++;
  }

  return hash % TABLE_SIZE;
}

void symbol_table_init() {
  for (int i = 0; i < TABLE_SIZE; i++) {
    table[i] = NULL;
  }

  symbol_table_add("SP", 0);
  symbol_table_add("LCL", 1);
  symbol_table_add("ARG", 2);
  symbol_table_add("THIS", 3);
  symbol_table_add("THAT", 4);

  for (int i = 0; i <= 15; i++) {
    char symbol[4];
    snprintf(symbol, sizeof(symbol), "R%d", i);
    symbol_table_add(symbol, i);
  }

  symbol_table_add("SCREEN", 16384);
  symbol_table_add("KBD", 24576);
}

void symbol_table_add(const char *symbol, int address) {
  unsigned int index = hash(symbol);

  SymbolEntry *entry = malloc(sizeof(SymbolEntry));

  entry->symbol = malloc(strlen(symbol) + 1);
  strcpy(entry->symbol, symbol);

  entry->address = address;

  entry->next = table[index];
  table[index] = entry;
}

int symbol_table_contains(const char *symbol) {
  unsigned int index = hash(symbol);

  SymbolEntry *entry = table[index];

  while (entry != NULL) {
    if (strcmp(entry->symbol, symbol) == 0)
      return 1;
    
    entry = entry->next;
  }

  return 0;
}

int symbol_table_get(const char *symbol) {
  unsigned int index = hash(symbol);

  SymbolEntry *entry = table[index];

  while (entry != NULL) {
    if (strcmp(entry->symbol, symbol) == 0)
      return entry->address;
    
    entry = entry->next;
  }

  return -1;
}