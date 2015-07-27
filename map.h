#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair {
  char *key;
  uint64_t value;
} pair;

typedef struct bucket {
  uint64_t count;
  pair *pairs;
} bucket;

typedef struct map {
  uint64_t count;
  bucket *buckets;
} map;

void map_init(map *map, const uint64_t capacity);
void map_set(map *map, char *key, uint64_t value);
uint64_t map_get(map *map, char *key);
void map_free(map *map);
