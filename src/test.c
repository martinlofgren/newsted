#include <stdio.h>
#include <stdlib.h>

#include "newsted.h"

int main () {
  json_object_t *obj;
  char *json_string;

  // Create json object
  printf("Create json object...");
  obj = json_newsted();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }
  printf("Done!\n");

  // Populate json object
  printf("Populate json object... ");
  json_key_t *key;
  json_value_t *val;
  key = json_new_key("Band");
  val = json_new_string("Metallica");
  json_add_object(obj, key, val);
  printf("Done!\n");

  // Stringify populated json object
  printf("Stringify json object... ");
  json_string = json_stringify(obj);
  printf("Done!\n");
  printf("Result: %s\n", json_string);

  // Clean up
  printf("Clean up... ");
  free(json_string);
  json_free(obj);
  printf("Done!\n");

  return 0;
}
