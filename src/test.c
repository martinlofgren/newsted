#include <stdio.h>
#include <stdlib.h>

#include "newsted.h"

int main () {
  json_value_t *obj;
  json_value_t *array;

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
  array = json_new_array();
  json_add_array(array, json_new_string("...And justice for all"));
  json_add_array(array, json_new_string("Metallica"));
  json_add_array(array, json_new_string("Load"));
  json_add_array(array, json_new_string("Reload"));
  json_add_object(obj, "Albums", array);
  json_add_object(obj, "Band", json_new_string("Metallica"));
  json_add_object(obj, "Number of albums", json_new_integer(23));
  json_add_object(obj, "Review", json_new_float(1.22));
  json_add_object(obj, "Great?", json_new_boolean(TRUE));
  printf("Done!\n");

  // Stringify populated json object
  printf("Stringify json object...\n");
  printf("Result: ");
  json_generate(obj, stdout);

  // Clean up
  printf("\nClean up... ");
  json_free(obj);
  printf("Done!\n");

  return 0;
}
