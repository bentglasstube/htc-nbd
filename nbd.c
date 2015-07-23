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

  fprintf(stderr, "Got variable name: %s\n", name);
}

void _parse_integer(code *code, uint64_t* value) {
  uint64_t v = 0;
  while (_is_numeric(code->source[code->pos])) {
    v = v * 10 + code->source[code->pos] - '0';
    code->pos++;
  }

  *value = v;

  fprintf(stderr, "Got integer value: %lu\n", v);
}

void _parse_value(code *code, uint64_t* value, map* vars) {
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
      uint8_t brackets = 0;
      uint64_t value = 0;
      size_t start, length = 0;

      fprintf(stderr, "If zero block\n");
      block->pos++;

      code_skip_whitespace(block);
      _parse_value(block, &value, vars);
      code_skip_whitespace(block);

      fprintf(stderr, "Value is %lu\n", value);

      start = block->pos + 1;

      fprintf(stderr, "Parsing block\n");

      while (true) {
        length++;

        if (block->source[block->pos] == '{') {
          brackets++;
          block->pos++;
          fprintf(stderr, "Open brace (%u)\n", brackets);
        } else if (block->source[block->pos] == '}') {
          brackets--;
          block->pos++;
          fprintf(stderr, "Close brace (%u)\n", brackets);
          if (brackets == 0) break;
        } else {
          block->pos++;
        }
      }

      if (value == 0) {
        code subblock;
        fprintf(stderr, "Running conditional\n");

        code_init(&subblock);
        code_append(&subblock, block->source + start * sizeof(char), length - 2);
        eval_block(&subblock, vars);
        code_free(&subblock);
      }
    } else if (block->source[block->pos] == '!') {
      uint64_t value = 0;

      fprintf(stderr, "Println block\n");

      block->pos++;
      code_skip_whitespace(block);
      _parse_value(block, &value, vars);

      fprintf(stderr, "Printing value %lu\n", value);

      printf("%lu\n", value);
    } else {
      uint64_t value;
      char *var = malloc(1024 * sizeof(char));

      _parse_variable_name(block, var);

      code_skip_whitespace(block);
      switch (block->source[block->pos]) {
        case '=':
          fprintf(stderr, "Assignment\n");

          block->pos++;
          code_skip_whitespace(block);

          _parse_value(block, &value, vars);
          map_set(vars, var, value);

          break;

        case '+':
          fprintf(stderr, "Addition\n");

          block->pos += 2;
          code_skip_whitespace(block);

          _parse_value(block, &value, vars);
          map_set(vars, var, map_get(vars, var) + value);

          break;

        case '-':
          fprintf(stderr, "Subtraction\n");

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
