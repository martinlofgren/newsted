#include <stdio.h>
#include <stdlib.h>

#include "newsted.h"

int main () {
  json_value_t *obj;

  // Create json object
  printf("Create json object... ");
  obj = json_new_object();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }
  printf("Done!\n");

  // Populate json object
  printf("Populate json object... ");
  json_add_object(obj, "Band", json_new_string("Metallica"));
  json_add_object(obj, "Number of albums", json_new_integer(23));
  json_add_object(obj, "Review", json_new_float(1.22));
  json_add_object(obj, "Great?", json_new_boolean(TRUE));
  printf("Done!\n");

  // Stringify populated json object
  printf("Stringify json object...\n");
  printf("Result: ");
  json_stringify(obj, stdout);

  // Clean up
  printf("Clean up... ");
  json_free(obj);
  printf("Done!\n");

  return 0;
}
