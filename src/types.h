/*
 * newsted - a JSON library for C.
 *
 * Latest source is available at https://github.com/martinlofgren/newsted
 *
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 *
 * 
 * This header file contains structure definitions, enumerations and type
 * definitions. These are private and should be used only through the api
 * described in newsted.h.
 */

#ifndef NEWSTED_TYPES_H
#define NEWSTED_TYPES_H

typedef enum json_type {
  object,
  array,
  string,
  num_integer,
  num_float,
  boolean,
  nil
} json_type_t;

#define FALSE 0
#define TRUE  1

#define JSON_INDENT 0x10

typedef struct json_value {
  enum json_type type;
  void *data;
  size_t len;
} json_value_t;

typedef struct json_key {
  char *data;
  int hash;
  size_t len;
  struct json_value *value;
  struct json_key *next;
} json_key_t;

typedef struct json_object {
  struct json_key *head;
} json_object_t;

typedef struct json_array_value {
  struct json_value *data;
  struct json_array_value *next;
} json_array_value_t;

typedef struct json_array {
  json_array_value_t *head;
} json_array_t;

typedef char* json_string_t;
typedef long long json_integer_t;
typedef double json_float_t;
typedef int json_boolean_t;

typedef int json_status_t;

#endif //NEWSTED_TYPES_H
