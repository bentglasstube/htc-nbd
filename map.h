#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair {
  char *key;
  unsigned long long value;
} pair;

typedef struct bucket {
  unsigned long long count;
  pair *pairs;
} bucket;

typedef struct map {
  unsigned long long count;
  bucket *buckets;
} map;

void map_init(map *map, const unsigned long long capacity);
void map_set(map *map, char *key, unsigned long long value);
unsigned long long map_get(map *map, char *key);
void map_free(map *map);
