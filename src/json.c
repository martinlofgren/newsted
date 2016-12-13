#include <stdlib.h> //malloc
#include <string.h> //strlen
#include <stdio.h>  //printf

#include "json.h"


// Numbers of extra characters used in stringify function

#define BASE_OBJECT_EXTRA_LENGTH 3 // {...}\0
#define KEY_EXTRA_LENGTH    3      // "...":
#define STRING_EXTRA_LENGTH 2      // "..."
#define NUMBER_EXTRA_LENGTH 0      // 
#define OBJECT_EXTRA_LENGTH 2      // {...}


// Static function declarations

static void stringify_key(json_key_t *key, FILE* stream);
static void stringify_string(json_value_t *value, FILE* stream);
static void stringify_integer(json_value_t *value, FILE* stream);
static void stringify_object(json_value_t *value, FILE* stream);
static int integer_strlen(long long value);


// Function definitions

json_object_t *json_newstedt() {
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

  json_key->data = malloc(json_key->len * sizeof(char));
  memcpy(json_key->data, key, json_key->len);

  return json_key;
}

json_value_t *json_new_string(char *value) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = string;
  new->len = strlen(value);
  
  new->data = malloc(strlen(value) * sizeof(char));
  if (new->data == NULL)
    return NULL; 
  memcpy(new->data, value, new->len);

  new->tostring = stringify_string;

  return new;
}

static int integer_strlen(long long value) {
  int n;
  
  n = 0;
  if (value < 0)
    n++;
  do {
    n++;
  } while (value /= 10);

  return n;
}

json_value_t *json_new_integer(long long value) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = num_integer;
  new->len = integer_strlen(value);

  new->data = malloc(sizeof(json_integer_t));
  if (new->data == NULL)
    return NULL;
  *(json_integer_t*) new->data = value;

  new->tostring = stringify_integer;
  
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
  if (obj->head == NULL) {
    obj->head = key;
  }
  else {
    key_ptr = obj->head;
    while (key_ptr && key_ptr->next) 
      key_ptr = key_ptr->next;
    
    key_ptr->next = key;
  }
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

static size_t obj_len(json_object_t *obj) {
  return 0;
}

char* json_stringify (json_object_t *obj) {
  char *ret;
  json_value_t *value;
  FILE *stream;
  size_t string_length;

  string_length = obj_len(obj);

  // Allocate return string
  ret = malloc(string_length * sizeof(char));
  if (ret == NULL)
    return NULL;
  /*  
  // Open stream on return string
  stream = fmemopen((char*) ret, string_length, "w");
  if (stream == NULL) {
    perror("error opening stream to return buffer");
    exit(EXIT_FAILURE);
  }
  */
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
  stream = stdout; // JUST FOR DEBUGGING
  value->tostring(value, stream);

  // Clean up and return
  free(value);
  fclose(stream);
  
  return ret;
}

void json_free(json_object_t *obj) {
  json_key_t *cur, *next;

  cur = obj->head;
  while (cur) {
    next = cur->next;

    free((cur->value)->data);
    free(cur->value);
    free(cur);
    
    cur = next;
  }
  free(obj);
}

