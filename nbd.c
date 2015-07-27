#include "nbd.h"

#include <gmp.h>
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

void _parse_integer(code *code, mpz_t value) {
  mpz_set_ui(value, 0);

  while (_is_numeric(code->source[code->pos])) {
    mpz_mul_ui(value, value, 10);
    mpz_add_ui(value, value, code->source[code->pos] - '0');
    code->pos++;
  }
}

void _parse_value(code *code, mpz_t value, map* vars) {
  if (_is_numeric(code->source[code->pos])) {
    _parse_integer(code, value);
  } else {
    char *var = malloc(1024 * sizeof(char));
    _parse_variable_name(code, var);
    map_get(vars, var, value);
    free(var);
  }
}

void eval_block(code *block, map* vars) {
  while (true) {
    code_skip_whitespace(block);

    if (block->source[block->pos] == '?') {
      uint8_t brackets = 0;
      size_t start, length = 0;
      mpz_t value;
      mpz_init(value);

      block->pos++;

      code_skip_whitespace(block);
      _parse_value(block, value, vars);
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

      if (mpz_sgn(value) == 0) {
        code subblock;

        code_init(&subblock);
        code_append(&subblock, block->source + start * sizeof(char), length - 2);
        eval_block(&subblock, vars);
        code_free(&subblock);
      }

      mpz_clear(value);
    } else if (block->source[block->pos] == '!') {
      mpz_t value;
      mpz_init(value);

      block->pos++;
      code_skip_whitespace(block);
      _parse_value(block, value, vars);

      mpz_out_str(stdout, 10, value);
      printf("\n");

      mpz_clear(value);
    } else {
      char *var = malloc(1024 * sizeof(char));
      mpz_t value;
      mpz_init(value);

      _parse_variable_name(block, var);

      code_skip_whitespace(block);
      switch (block->source[block->pos]) {
        case '=':
          block->pos++;
          code_skip_whitespace(block);

          _parse_value(block, value, vars);
          map_set(vars, var, value, false);

          break;

        case '+':
          block->pos += 2;
          code_skip_whitespace(block);

          _parse_value(block, value, vars);
          map_set(vars, var, value, true);

          break;

        case '-':
          block->pos += 2;
          code_skip_whitespace(block);

          _parse_value(block, value, vars);
          mpz_neg(value, value);
          map_set(vars, var, value, true);

          break;

        default:
          fprintf(stderr, "Parse error\n");
      }

      free(var);
      mpz_clear(value);
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
