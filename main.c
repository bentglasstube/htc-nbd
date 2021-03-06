#include <stdio.h>

#include "code.h"
#include "map.h"
#include "nbd.h"

int main() {
  code code;
  map vars;
  char *buffer;

  code_init(&code);
  buffer = malloc(1024 * sizeof(char));

  while (fgets(buffer, 1024, stdin)) {
    code_append(&code, buffer, strlen(buffer));
  }

  free(buffer);

  map_init(&vars, 1024);

  eval_block(&code, &vars);

  map_free(&vars);
  code_free(&code);

  return 0;
}
