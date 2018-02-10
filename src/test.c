#include <stdio.h>
#include <stdlib.h>

#include "newsted.h"

int main () {
  json_value_t *obj;
  json_value_t *array;

  // Create JSON object
  fprintf(stderr, "Create json object... ");
  obj = json_new_object();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }
  fprintf(stderr, "Done!\n");

  // Populate JSON object
  fprintf(stderr, "Populate json object... ");
  array = json_new_array();
  json_add_array(array, json_new_string("...And justice for all"));
  json_add_array(array, json_new_string("Metallica"));
  json_add_array(array, json_new_string("Load"));
  json_add_array(array, json_new_string("Reload"));
  json_add_object(obj, "Band", json_new_string("Metallica"));
  json_add_object(obj, "Number of albums", json_new_integer(23)); 
  json_add_object(obj, "Review", json_new_float(1.22));
  json_add_object(obj, "Great?", json_new_boolean(TRUE));
  json_add_object(obj, "Albums", array);
  fprintf(stderr, "Done!\n");

  // Get string length
  fprintf(stderr, "Get string length... %d\n", (int) json_strlen(obj));

  // Generate populated JSON object string
  fprintf(stderr, "Generate string from json object...\n");
  fprintf(stderr, "Result: ");
  json_generate(obj, JSON_INDENT | 2, stderr);

  // Clean up
  fprintf(stderr, "\nClean up... ");
  json_free(obj);
  fprintf(stderr, "Done!\n");


  // Do some parsing
  char* str = "[true, false, \"Jag är en fisk\"]";
  json_value_t* j_val;

  fprintf(stderr, "\n*** PARSING ***\n\nParse a string... ");

  if ((j_val = json_parse(str)))
    fprintf(stderr, "Done!\n");
  else {
    fprintf(stderr, "error on parsing string: %s\n", str);
    exit(EXIT_FAILURE);
  }
  
  // Get string length
  fprintf(stderr, "Get string length... %d\n", (int) json_strlen(j_val));

  // Generate populated JSON object string
  fprintf(stderr, "Generate string from json object...\n");
  fprintf(stderr, "Result: ");
  json_generate(j_val, JSON_INDENT | 2, stderr);

  // Clean up
  fprintf(stderr, "\nClean up... ");
  json_free(j_val);
  fprintf(stderr, "Done!\n");

  return 0;
}
