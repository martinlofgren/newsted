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
  enum json_type type;
  char *key;
  void *value;
  struct json_field *next;
  void (*stringifier)(struct json_field *field, FILE *stream);
} json_field_t;

typedef struct json_object {
  struct json_object *parent;
  struct json_field *head;
  size_t string_length;
} json_object_t;

typedef char *json_string_t;
typedef long long json_number_t;

json_object_t *json_init();
json_field_t *json_new_object(char* key);
json_field_t *json_new_string(char *key, char *value);
json_field_t *json_new_long(char *key, json_number_t value);
void json_add(json_object_t *obj, json_field_t *field);
char* json_stringify (json_object_t *obj);
void json_free(json_object_t *obj);

#endif //JSON_H
