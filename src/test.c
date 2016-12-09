#include <stdio.h>
#include <stdlib.h>

#include "json.h"

int main () {
  json_object_t *obj;
  json_field_t *field;
  char *json_string, buf1[64], buf2[64];
  int i;

  // Create json object
  obj = json_new();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }

  // Stringify empty json object
  json_string = json_stringify(obj);
  printf("%s\n", json_string);
  free(json_string);
  json_free(obj);

  // Create json object
  obj = json_new();

  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }

  // Populate json object with some string values
  for (i=1; i<5; i++) {
    sprintf(buf1, "key %d", i);
    sprintf(buf2, "value %d", i);
    field = json_new_string(buf1, buf2);
    json_add(obj, field);
  }

  // Stringify populated json object
  json_string = json_stringify(obj);
  printf("%s\n", json_string);
  free(json_string);
  json_free(obj);
    
  // Create json object
  obj = json_new();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }

  // Populate json object with some number values
  for (i=1; i<5; i++) {
    sprintf(buf1, "key %d", i);
    field = json_new_long(buf1, (long)i*i*i+(13*i)-1);
    json_add(obj, field);
  }

  // Stringify populated json object
  json_string = json_stringify(obj);
  printf("%s\n", json_string);
  free(json_string);
  json_free(obj);
  
  return 0;
}
