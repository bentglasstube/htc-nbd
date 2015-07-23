#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair {
  char *key;
  long long value;
} pair;

typedef struct bucket {
  long long count;
  pair *pairs;
} bucket;

typedef struct map {
  long long count;
  bucket *buckets;
} map;

void map_init(map *map, const long long capacity);
void map_set(map *map, char *key, long long value);
long long map_get(map *map, char *key);
void map_free(map *map);
