#ifndef JSON_H
#define JSON_H

#include <stdlib.h> //malloc
#include <string.h> //strlen
#include <stdio.h>  //printf

// Type definitions

typedef enum json_type {
  object,
  array,
  string,
  num_integer,
  num_float,
  boolean,
  nil
} json_type_t;

struct json_key;
struct json_value;
struct json_object;
struct json_array;

typedef struct json_key {
  char *data;
  size_t len;
  struct json_value *value;
  struct json_key *next;
} json_key_t;

typedef struct json_value {
  enum json_type type;
  void *data;
  size_t len;;
  void (*tostring) (struct json_value *value, FILE *stream);
  struct json_value *next;
} json_value_t;

typedef struct json_object {
  struct json_key *head;
} json_object_t;

typedef struct json_array {
  json_value_t *head;
} json_array_t;

typedef char* json_string_t;
typedef long long json_integer_t;
typedef double json_float_t;


// Function declarations

json_object_t *json_newstedt();
json_key_t *json_new_key();
json_value_t *json_new_object();
json_value_t *json_new_string(char *value);
json_value_t *json_new_integer(json_integer_t value);
void json_add_object(json_object_t *obj, json_key_t *key, json_value_t *value);
char* json_stringify (json_object_t *obj);
void json_free(json_object_t *obj);

#endif //JSON_H
