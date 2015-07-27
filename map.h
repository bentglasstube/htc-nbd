#pragma once

#include <gmp.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair {
  char *key;
  mpz_t value;
} pair;

typedef struct bucket {
  uint16_t count;
  pair *pairs;
} bucket;

typedef struct map {
  uint16_t count;
  bucket *buckets;
} map;

void map_init(map *map, const uint16_t capacity);
void map_set(map *map, char *key, mpz_t value, bool add);
void map_get(map *map, char *key, mpz_t value);
void map_free(map *map);
