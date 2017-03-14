/*
 * newstedt - a json library for C.
 *
 * Latest source is available at https://github.com/martinlofgren/newstedt
 *
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#include "newsted.h"
//#include <math.h>


// Numbers of extra characters used in stringify function

#define KEY_EXTRA_LENGTH     3 // "...":
#define STRING_EXTRA_LENGTH  2 // "..."
#define NUMBER_EXTRA_LENGTH  0 // 
#define OBJECT_EXTRA_LENGTH  2 // {...}
#define BOOLEAN_EXTRA_LENGTH 0 // 


// Static function declarations

static json_value_t *new_value (json_type_t new_type_enum,
				void (*new_tostring) (struct json_value *value, FILE *stream),
				size_t new_strlen,
				size_t new_data_size);
static json_key_t *json_new_key(char *key);
static void stringify_key(json_key_t *key, FILE* stream);
static void stringify_string(json_value_t *value, FILE* stream);
static void stringify_integer(json_value_t *value, FILE* stream);
static void stringify_float(json_value_t *value, FILE* stream);
static void stringify_boolean(json_value_t *value, FILE* stream);
static void stringify_object(json_value_t *value, FILE* stream);
static size_t integer_strlen(json_integer_t value)  __attribute__ ((pure));
static size_t float_strlen(json_float_t value)  __attribute__ ((pure));
static size_t obj_strlen(json_object_t *value) __attribute__ ((pure));

// Dummy hash function
int hash(char *s) {
  return (int) *s;
}

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
  json_key->hash = hash(key);

  json_key->data = malloc((json_key->len + 1) * sizeof(char));
  memcpy(json_key->data, key, json_key->len + 1);

  return json_key;
}

static json_value_t *new_value (json_type_t new_type_enum,
				void (*new_tostring) (struct json_value *value, FILE *stream),
				size_t new_strlen,
				size_t new_data_size) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = new_type_enum;
  new->tostring = new_tostring;
  new->strlen = new_strlen;

  if (new_data_size) {
    new->data = malloc(new_data_size);
    if (new->data == NULL)
      return NULL;
  }

  return new;
}

json_value_t *json_new_object() {
  json_value_t *new;

  if ((new = new_value(object, stringify_object, 0, 0))) {
    new->data = json_newsted();
    if (new->data == NULL)
      return NULL;
  }

  return new;
}

json_value_t *json_new_string(char *value) {
  json_value_t *new;
  size_t len;

  len = strlen(value);
  if ((new = new_value(string, stringify_string, len, (len + 1) * sizeof(json_float_t)))) {
    memcpy(new->data, value, new->strlen + 1);
  }

  return new;
}

json_value_t *json_new_integer(json_integer_t value) {
  json_value_t *new;

  if ((new = new_value(num_integer, stringify_integer, integer_strlen(value), sizeof(json_integer_t)))) {
    *(json_integer_t*) new->data = value;
  }
  
  return new;
}

json_value_t *json_new_float(json_float_t value) {
  json_value_t *new;

  if ((new = new_value(num_float, stringify_float, float_strlen(value), sizeof(json_float_t)))) {
    *(json_float_t*) new->data = value;
  }

  return new;
}

json_value_t *json_new_boolean(json_boolean_t value) {
  json_value_t *new;

  if ((new = new_value(boolean, stringify_boolean, 5, sizeof(json_boolean_t)))) {
    *(json_boolean_t*) new->data = value;
  }

  return new;
}

json_status_t json_add_object(json_object_t *obj, char *key, json_value_t *value) {
  json_key_t *new_key, *key_ptr;

  // Create new key and associate with value
  if ((new_key = json_new_key(key)) == NULL)
    return ERR_MEM_ALLOC;
  new_key->value = value;
  
  // Empty object -> create first key/value pair
  if (obj->head == NULL)
    obj->head = new_key;

  // Object does contain some entries
  else {
    key_ptr = obj->head;
    
    // Iterate over object until there are no more entries OR the hash of the
    // key is larger than the prior one
    while (key_ptr->next && (key_ptr->next->hash < new_key->hash))
      key_ptr = key_ptr->next;

    // Detect duplicate keys
    if (key_ptr->hash == new_key->hash) {
      if (!strcmp(key_ptr->data, new_key->data))
	return ERR_KEY_COLLISION;
    }

    // Insert in linked list if not the last entry
    if (key_ptr->next)
      new_key->next = key_ptr->next;
    key_ptr->next = new_key;
  }
  
  return SUCCESS;
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
      ret += value_ptr->strlen + STRING_EXTRA_LENGTH;
      break;
    case num_integer:
      ret += value_ptr->strlen + NUMBER_EXTRA_LENGTH;
      break;
    case num_float:
      ret += value_ptr->strlen + NUMBER_EXTRA_LENGTH;
      break;
    case boolean:
      ret += value_ptr->strlen + BOOLEAN_EXTRA_LENGTH;
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
  fprintf(stream, (*(json_boolean_t *) value->data != FALSE) ? "true" : "false");
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

static void stringify_array(json_value_t *value, FILE* stream) {
  json_value_t *value_ptr;

  fprintf(stream, "[");
  for (value_ptr = ((json_array_t *) value)->head; value_ptr; value_ptr = value_ptr->next) {
    value_ptr->tostring(value_ptr, stream);
    
    if (value_ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "]");
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
  value->strlen = 0;
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

