#include "nbd.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

bool _is_numeric(char c) {
  return c >= '0' && c <= '9';
}

bool _is_alphanumeric(char c) {
  return (c >= '0' && c <= '9')
      || (c >= 'a' && c <= 'z')
      || (c >= 'A' && c <= 'Z');
}

void _parse_variable_name(code *code, char* name) {
  size_t length = 0;
  while (_is_alphanumeric(code->source[code->pos + length])) length++;

  memcpy(name, code->source + code->pos * sizeof(char), length);
  code->pos += length;
  name[length] = 0;
}

void _parse_integer(code *code, unsigned long long * value) {
  unsigned long long v = 0;
  while (_is_numeric(code->source[code->pos])) {
    v = v * 10 + code->source[code->pos] - '0';
    code->pos++;
  }

  *value = v;
}

void _parse_value(code *code, unsigned long long * value, map* vars) {
  if (_is_numeric(code->source[code->pos])) {
    _parse_integer(code, value);
  } else {
    char *var = malloc(1024 * sizeof(char));
    _parse_variable_name(code, var);
    *value = map_get(vars, var);
    free(var);
  }
}

void eval_block(code *block, map* vars) {
  while (true) {
    code_skip_whitespace(block);

    if (block->source[block->pos] == '?') {
      unsigned long long brackets = 0, value = 0;
      size_t start, length = 0;

      block->pos++;

      code_skip_whitespace(block);
      _parse_value(block, &value, vars);
      code_skip_whitespace(block);

      start = block->pos + 1;

      while (true) {
        length++;

        if (block->source[block->pos] == '{') {
          brackets++;
          block->pos++;
        } else if (block->source[block->pos] == '}') {
          brackets--;
          block->pos++;
          if (brackets == 0) break;
        } else {
          block->pos++;
        }
      }

      if (value == 0) {
        code subblock;

        code_init(&subblock);
        code_append(&subblock, block->source + start * sizeof(char), length - 2);
        eval_block(&subblock, vars);
        code_free(&subblock);
      }
    } else if (block->source[block->pos] == '!') {
      unsigned long long value = 0;

      block->pos++;
      code_skip_whitespace(block);
      _parse_value(block, &value, vars);

      printf("%ld\n", value);
      fprintf(stderr, "%ld\n", value);
    } else {
      unsigned long long value;
      char *var = malloc(1024 * sizeof(char));

      _parse_variable_name(block, var);

      code_skip_whitespace(block);
      switch (block->source[block->pos]) {
        case '=':
          block->pos++;
          code_skip_whitespace(block);

          _parse_value(block, &value, vars);
          map_set(vars, var, value);

          break;

        case '+':
          block->pos += 2;
          code_skip_whitespace(block);

          _parse_value(block, &value, vars);
          map_set(vars, var, map_get(vars, var) + value);

          break;

        case '-':
          block->pos += 2;
          code_skip_whitespace(block);

          _parse_value(block, &value, vars);
          map_set(vars, var, map_get(vars, var) - value);

          break;

        default:
          fprintf(stderr, "Parse error\n");
          return;
      }

      free(var);
    }

    code_skip_whitespace(block);

    if (block->pos >= block->length) return;

    if (block->source[block->pos] == ';') {
      block->pos++;
    } else {
      fprintf(stderr, "Missing ;\n");
      return;
    }
  }
}
