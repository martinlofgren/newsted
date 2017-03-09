/*
 * newstedt - a json library for C.
 *
 * Latest source is available at https://github.com/martinlofgren/newstedt
 *
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#include "newsted.h"
#include "math.h"


// Numbers of extra characters used in stringify function

#define KEY_EXTRA_LENGTH     3 // "...":
#define STRING_EXTRA_LENGTH  2 // "..."
#define NUMBER_EXTRA_LENGTH  0 // 
#define OBJECT_EXTRA_LENGTH  2 // {...}
#define BOOLEAN_EXTRA_LENGTH 0 // 


// Static function declarations

static void stringify_key(json_key_t *key, FILE* stream);
static void stringify_string(json_value_t *value, FILE* stream);
static void stringify_integer(json_value_t *value, FILE* stream);
static void stringify_float(json_value_t *value, FILE* stream);
static void stringify_boolean(json_value_t *value, FILE* stream);
static void stringify_object(json_value_t *value, FILE* stream);
static size_t integer_strlen(json_integer_t value)  __attribute__ ((pure));
static size_t float_strlen(json_float_t value)  __attribute__ ((pure));
static size_t obj_strlen(json_object_t *value) __attribute__ ((pure));

// Function definitions

json_object_t *json_newsted() {
  json_object_t *obj;

  obj = malloc(sizeof(json_object_t));
  if (obj == NULL)
    return NULL;

  obj->head = NULL;

  return obj;
}

json_key_t *json_new_key(char* key) {
  json_key_t *json_key;

  json_key = malloc(sizeof(json_key_t));
  if (json_key == NULL)
    return NULL;

  json_key->len = strlen(key);
  json_key->next = NULL;

  json_key->data = malloc((json_key->len + 1) * sizeof(char));
  memcpy(json_key->data, key, json_key->len + 1);

  return json_key;
}

json_value_t *json_new_object() {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = object;
  new->tostring = stringify_object;

  new->data = json_newsted();
  if (new->data == NULL)
    return NULL;

  return new;
}

json_value_t *json_new_string(char *value) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = string;
  new->tostring = stringify_string;
  new->len = strlen(value);
  
  new->data = malloc((new->len + 1) * sizeof(char));
  if (new->data == NULL)
    return NULL; 
  memcpy(new->data, value, new->len + 1);

  return new;
}

json_value_t *json_new_integer(json_integer_t value) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = num_integer;
  new->tostring = stringify_integer;
  new->len = integer_strlen(value);

  new->data = malloc(sizeof(json_integer_t));
  if (new->data == NULL)
    return NULL;
  *(json_integer_t*) new->data = value;
  
  return new;
}

json_value_t *json_new_float(json_float_t value) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = num_float;
  new->tostring = stringify_float;
  new->len = float_strlen(value);

  new->data = malloc(sizeof(json_float_t));
  if (new->data == NULL)
    return NULL;
  *(json_float_t*) new->data = value;

  return new;
}

json_value_t *json_new_boolean(json_boolean_t value) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = boolean;
  new->tostring = stringify_boolean;
  new->len = 5;

  new->data = malloc(sizeof(json_boolean_t));
  if (new->data == NULL)
    return NULL;
  *(json_boolean_t*) new->data = value;

  return new;
}

void json_add_object(json_object_t *obj, json_key_t *key, json_value_t *value) {
  json_key_t *key_ptr;

  // TODO: Should return error indication somehow. Change from void to
  //       json_object_t and return NULL on error, obj otherwise?
  if (key == NULL)
    return;
  
  // Associate key with value
  key->value = value;
  
  // If object is empty, create first value, else append to end of linked list
  if (obj->head == NULL) { // Is this clause really necessary?
    obj->head = key;
  }
  else {
    key_ptr = obj->head;
    while (key_ptr && key_ptr->next) 
      key_ptr = key_ptr->next;
    
    key_ptr->next = key;
  }
}

static size_t obj_strlen(json_object_t *obj) {
  json_key_t *key_ptr;
  json_value_t *value_ptr;
  size_t ret;

  ret = 0;

  for (key_ptr = obj->head; key_ptr; key_ptr = key_ptr->next) {
    ret += key_ptr->len + KEY_EXTRA_LENGTH;
    value_ptr = key_ptr->value;

    switch (value_ptr->type) {
    case object:
      ret += obj_strlen((json_object_t *) value_ptr->data);
      break;
    case array:
      break;
    case string:
      ret += value_ptr->len + STRING_EXTRA_LENGTH;
      break;
    case num_integer:
      ret += value_ptr->len + NUMBER_EXTRA_LENGTH;
      break;
    case num_float:
      ret += value_ptr->len + NUMBER_EXTRA_LENGTH;
      break;
    case boolean:
      ret += value_ptr->len + BOOLEAN_EXTRA_LENGTH;
      break;
    case nil:
      break;
    }

    if (key_ptr->next)
      ret++;
  }

  return ret + OBJECT_EXTRA_LENGTH;
}

static size_t integer_strlen(json_integer_t value) {
  size_t n;

  n = 0;
  if (value < 0)
    n++;
  do {
    n++;
  } while (value /= 10);

  return n;
}

static size_t float_strlen(json_float_t value) {
  char tmp[22];

  return sprintf(tmp, "%.17g", value);
}

static void stringify_key(json_key_t *key, FILE* stream) {
  fprintf(stream, "\"%s\":", key->data);
}

static void stringify_string(json_value_t *value, FILE* stream) {
  fprintf(stream, "\"%s\"", (json_string_t) value->data);
}

static void stringify_integer(json_value_t *value, FILE* stream) {
  fprintf(stream, "%lld", *(json_integer_t *) value->data);
}

static void stringify_float(json_value_t *value, FILE* stream) {
  fprintf(stream, "%.17g", *(json_float_t *) value->data);
}

static void stringify_boolean(json_value_t *value, FILE* stream) {
  fprintf(stream, (*(json_float_t *) value->data != 0) ? "true" : "false");
}

static void stringify_object(json_value_t *value, FILE* stream) {
  json_key_t *key_ptr;
  json_value_t *value_ptr;

  fprintf(stream, "{");
  for (key_ptr = ((json_object_t *) value->data)->head; key_ptr; key_ptr = key_ptr->next) {
    stringify_key(key_ptr, stream);
    value_ptr = key_ptr->value;
    value_ptr->tostring(value_ptr, stream);
    
    if (key_ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "}");
}

char* json_stringify (json_object_t *obj) {
  char *ret;
  json_value_t *value;
  FILE *stream;
  size_t string_length;

  string_length = obj_strlen(obj) + 1;

  // Allocate return string
  ret = malloc(string_length * sizeof(char));
  if (ret == NULL)
    return NULL;

  // Open stream on return string
  stream = fmemopen((char*) ret, string_length, "w");
  if (stream == NULL) {
    perror("error opening stream to return buffer");
    exit(EXIT_FAILURE);
  }

  // Encapsulate object in value struct to adapt type to function
  value = malloc(sizeof(json_value_t));
  if (value == NULL)
    return NULL;
  
  value->type = object;
  value->data = obj;
  value->len = 0;
  value->tostring = stringify_object;
  value->next = NULL;

  // Do the stringifying
  value->tostring(value, stream);
  ret[string_length - 1] = 0;
  
  // Clean up and return
  free(value);
  fclose(stream);
  
  return ret;
}

void json_free(json_object_t *obj) {
  json_key_t *cur, *next;

  next = obj->head;
  while ((cur = next)) {
    next = cur->next;

    free((cur->value)->data);
    free(cur->value);
    free(cur->data);
    free(cur);
  }
  free(obj);
}

