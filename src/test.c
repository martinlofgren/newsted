#include <stdio.h>
#include <stdlib.h>

#include "json.h"

int main () {
  json_object_t *obj;
  json_field_t *field;
  char *json_string;

  // Create json object
  printf("Create json object...");
  obj = json_init();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }
  printf("Done!\n");

  // Populate json object
  printf("Populate json object... ");
  json_add(obj, json_new_string("Band", "Metallica"));
  field = json_new_object("Members");
  json_add(obj, field);
  json_add(field->value, json_new_string("Vocalist", "James Hetfield"));
  json_add(field->value, json_new_string("Guitarist", "Kirk Hammet"));
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
