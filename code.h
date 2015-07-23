#pragma once

#include <stddef.h>

typedef struct code {
  size_t pos, length, capacity;
  char * source;
} code;

void code_init(code *code);
void code_append(code *code, char *data, size_t length);
void code_skip_whitespace(code *code);
void code_free(code *code);
