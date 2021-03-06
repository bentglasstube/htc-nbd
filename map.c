#include "map.h"

static pair * get_pair(bucket *bucket, const char *key) {
  size_t i;
  pair *pair;

  for (i = 0, pair = bucket->pairs; i < bucket->count; i++, pair++) {
    if (pair->key != NULL && strcmp(pair->key, key) == 0) return pair;
  }

  return NULL;
}

static uint16_t hash(const char *str) {
  uint16_t hash = 2839;
  char c;

  while (c = *str++) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

void map_init(map *map, const uint16_t capacity) {
  map->count = capacity;
  map->buckets = malloc(map->count * sizeof(bucket));
  memset(map->buckets, 0, map->count * sizeof(bucket));
}

void map_set(map *map, char *key, mpz_t value, bool add) {
  uint16_t index;
  bucket *bucket;
  pair *pair;

  index = hash(key) % map->count;
  bucket = &(map->buckets[index]);

  if ((pair = get_pair(bucket, key)) == NULL) {
    if (bucket->count == 0) {
      bucket->pairs = malloc(sizeof(*pair));
      bucket->count = 1;
    } else {
      bucket->pairs = realloc(bucket->pairs, (bucket->count + 1) * sizeof(*pair));
      bucket->count++;
    }

    pair = &(bucket->pairs[bucket->count - 1]);

    pair->key = malloc((strlen(key) + 1) * sizeof(char));
    strcpy(pair->key, key);

    mpz_init(pair->value);
  }

  if (add) {
    mpz_add(pair->value, pair->value, value);
  } else {
    mpz_set(pair->value, value);
  }
}

void map_get(map *map, char *key, mpz_t value) {
  size_t index;
  bucket *bucket;
  pair *pair;

  index = hash(key) % map->count;
  bucket = &(map->buckets[index]);
  pair = get_pair(bucket, key);

  if (pair == NULL) return;
  mpz_set(value, pair->value);
}

void map_free(map *map) {
  size_t i, j;

  for (i = 0; i < map->count; ++i) {
    bucket *bucket = &(map->buckets[i]);
    for (j = 0; j < bucket->count; ++j) {
      pair *pair = &(bucket->pairs[j]);
      free(pair->key);
      mpz_clear(pair->value);
    }
    free(bucket->pairs);
  }
  free(map->buckets);
  map->count = 0;
}
