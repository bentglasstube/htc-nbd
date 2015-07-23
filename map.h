#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair {
  char *key;
  long value;
} pair;

typedef struct bucket {
  long count;
  pair *pairs;
} bucket;

typedef struct map {
  long count;
  bucket *buckets;
} map;

void map_init(map *map, const long capacity);
void map_set(map *map, char *key, long value);
long map_get(map *map, char *key);
void map_free(map *map);
