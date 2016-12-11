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

static json_field_t *new_field(char *key);
static void stringify_key(json_field_t *field, FILE* stream);
static void stringify_string(json_field_t *field, FILE* stream);
static void stringify_number(json_field_t *field, FILE* stream);
static void stringify_object(json_field_t *field, FILE* stream);


// Function definitions

json_object_t *json_init() {
  json_object_t *obj;

  obj = malloc(sizeof(json_object_t));
  if (obj == NULL)
    return NULL;

  obj->head = NULL;
  obj->parent = NULL;
  obj->string_length = BASE_OBJECT_EXTRA_LENGTH;

  return obj;
}

static json_field_t *new_field(char *key) {
  json_field_t *new;

  // Allocate new json field and strings
  new = malloc(sizeof(json_field_t));
  if (new == NULL)
    return NULL;

  if (key) {
    new->key = malloc(strlen(key) * sizeof(char));
    if (new->key == NULL)
      return NULL;
    strcpy(new->key, key);
  }
  else {
    new->key = NULL;
  }

  new->next = NULL;

  return new;
}

json_field_t *json_new_object(char* key) {
  json_field_t *new;

  new = new_field(key);
  if (new == NULL)
    return NULL;

  new->type = object;
  new->stringifier = stringify_object;

  new->value = malloc(sizeof(json_object_t));
  if (new->value == NULL)
    return NULL;

  json_object_t *obj;
  obj = (json_object_t *) new->value;
  
  obj->head = NULL;
  obj->parent = NULL;
  obj->string_length = 0;
  return new;
}

json_field_t *json_new_string(char *key, char *value) {
  json_field_t *new;

  new = new_field(key);
  if (new == NULL)
    return NULL;
  
  new->type = string;
  new->stringifier = stringify_string;
  
  new->value = malloc(strlen(value) * sizeof(char));
  if (new->value == NULL)
    return NULL; 
  strcpy(new->value, value);
  
  return new;
}

json_field_t *json_new_long(char *key, json_number_t value) {
  json_field_t *new;

  new = new_field(key);
  if (new == NULL)
    return NULL;

  new->type = number;
  new->stringifier = stringify_number;

  new->value = malloc(sizeof(json_number_t));
  if (new->value == NULL)
    return NULL;
  
  *((json_number_t*) new->value) = value;
  
  return new;
}

void json_add(json_object_t *obj, json_field_t *field) {
  json_field_t *field_ptr;
  json_object_t *obj_ptr;
  
  // If object is empty, create first field, otherwise append to end
  if (obj->head == NULL) {
    obj->head = field;
  }
  else {
    for (field_ptr = obj->head; field_ptr && field_ptr->next; field_ptr = field_ptr->next)
      ;
    field_ptr->next = field;
  }

  // Get the top object to hold the string length
  for (obj_ptr = obj; obj_ptr->parent; obj_ptr = obj_ptr->parent)
    ;

  // Update total length of stringified json
  obj_ptr->string_length++;
  
  if (field->key)
    obj_ptr->string_length += strlen(field->key) + KEY_EXTRA_LENGTH;
  
  long long n; 
  switch (field->type) {
  case string:
    obj_ptr->string_length += strlen(field->value) + STRING_EXTRA_LENGTH;
    break;
  case number:
    n = *((long long *) field->value);
    if (n < 0)
      obj_ptr->string_length++;
    do {
      obj_ptr->string_length++;
    } while (n /= 10);
    obj_ptr->string_length += NUMBER_EXTRA_LENGTH;
    break;
  case object:
    ((json_object_t *) field->value)->parent = obj;
    obj_ptr->string_length += obj->string_length + OBJECT_EXTRA_LENGTH;
    break;
  case array:
    break;
  case boolean:
    break;
  case nil:
    break;
  }
}

static void stringify_key(json_field_t *field, FILE* stream) {
  fprintf(stream, "\"%s\":", field->key);
}

static void stringify_string(json_field_t *field, FILE* stream) {
  fprintf(stream, "\"%s\"", (json_string_t) field->value);
}

static void stringify_number(json_field_t *field, FILE* stream) {
  fprintf(stream, "%lld", *(json_number_t *) field->value);
}

static void stringify_object(json_field_t *field, FILE* stream) {
  json_field_t *ptr;

  fprintf(stream, "{");
  for (ptr = ((json_object_t *) field->value)->head; ptr; ptr = ptr->next) {
    if (ptr->key)
      stringify_key(ptr, stream);
    ptr->stringifier(ptr, stream);
    
    if (ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "}");
}

char* json_stringify (json_object_t *obj) {
  char *ret;
  json_field_t *field;
  FILE *stream;

  // Allocate return string
  ret = malloc((obj->string_length + 1) * sizeof(char));
  
  // Open stream on return string
  stream = fmemopen((char*) ret, obj->string_length, "w");
  if (stream == NULL) {
    perror("error opening stream to return buffer");
    exit(EXIT_FAILURE);
  }

  // Encapsulate object in field struct to adapt type to function
  field = malloc(sizeof(json_field_t));
  field->type = object;
  field->key = NULL;
  field->value = obj;
  field->next = NULL;
  field->stringifier = stringify_object;

  // Do the stringifying 
  field->stringifier(field, stream);

  // Clean up and return
  free(field);
  fclose(stream);
  
  return ret;
}

void json_free(json_object_t *obj) {
  json_field_t *cur, *next;

  cur = obj->head;
  while (cur) {
    next = cur->next;

    free(cur->key);
    free(cur->value);
    free(cur);
    
    cur = next;
  }
  free(obj);
}

