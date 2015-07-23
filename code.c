#include "code.h"

#include <stdbool.h>
#include <string.h>
#include <malloc.h>

void code_init(code *code) {
  code->pos = code->length = 0;
  code->capacity = 1024;
  code->source = malloc(code->capacity * sizeof(char));
}

void code_append(code *code, char *data, size_t length) {
  if (code->length + length > code->capacity) {
    code->capacity += 1024;
    code->source = realloc(code->source, code->capacity * sizeof(char));
  }

  memcpy(code->source + code->length * sizeof(char), data, length);
  code->length += length;
}

bool _is_whitespace(char c) {
  return c == ' ' || c == '\t' || c == '\n';
}

void code_skip_whitespace(code *code) {
  while (_is_whitespace(code->source[code->pos])) code->pos++;
}

void code_free(code *code) {
  code->pos = code->length = code->capacity = 0;
  free(code->source);
}
