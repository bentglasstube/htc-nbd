#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct pair {
  char *key;
  int value;
} pair;

typedef struct bucket {
  int count;
  pair *pairs;
} bucket;

typedef struct map {
  int count;
  bucket *buckets;
} map;

void map_init(map *map, const int capacity);
void map_set(map *map, char *key, int value);
int map_get(map *map, char *key);
void map_free(map *map);
