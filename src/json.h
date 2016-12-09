#ifndef JSON_H
#define JSON_H

typedef enum json_type {
  string,
  number,
  object,
  array,
  boolean,
  nil
} json_type_t;

typedef struct json_field {
  json_type_t type;
  char *key;
  void *value;
  struct json_field *next;
} json_field_t;

typedef struct json_object {
  json_field_t *head;
  size_t string_length;
} json_object_t;

typedef char *json_string_t;
typedef long long json_number_t;

json_object_t *json_new();
json_field_t *json_new_string(char *key, char *value);
void json_add(json_object_t *obj, json_field_t *field);
char* json_stringify (json_object_t *obj);
void json_free(json_object_t *obj);

#endif //JSON_H
