#include <stdlib.h> //malloc
#include <string.h> //strlen
#include <stdio.h>  //printf

#include "json.h"

#define STRING_EXTRA_LENGTH 6 // "\"\":\"\","
#define NUMBER_EXTRA_LENGTH 4 // "\"\":,"

static json_field_t *new_field(char *key);

json_object_t *json_new() {
  json_object_t *obj;

  obj = malloc(sizeof(json_object_t));
  if (obj == NULL)
    return NULL;

  obj->head = NULL;
  obj->string_length = 3; // To hold "{}\0"
  return obj;
}

static json_field_t *new_field(char *key) {
  json_field_t *new;

  // Allocate new json field and strings
  new = malloc(sizeof(json_field_t));
  if (new == NULL)
    return NULL;

  new->key = malloc(strlen(key) * sizeof(char));
  if (new->key == NULL)
    return NULL;

  strcpy(new->key, key);
  new->next = NULL;

  return new;
}

json_field_t *json_new_string(char *key, char *value) {
  json_field_t *new;

  new = new_field(key);
  if (new == NULL)
    return NULL;

  new->value = malloc(strlen(value) * sizeof(char));
  if (new->value == NULL)
    return NULL; 

  // Set values
  new->type = string;
  strcpy(new->value, value);
  
  return new;
}
/*
json_field_t json_new_long(json_object_t *obj, char *key, long long value) {
  json_field_t *new;
  int key_len, value_len;
  long long n;

  n = value;
  value_len = 0;
  if (n < 0)
    value_len++;
  do {
    value_len++;
  } while (n /= 10);

  printf("%lld is %d digits\n", value, value_len);

  key_len = strlen(key);

  new = new_field(obj, key, key_len);
  if (new == NULL)
    return NULL;

  new->value = (json_number_t*) malloc(sizeof(json_number_t));
  if (new->value == NULL)
    return NULL;
  
  // Set values
  new->type = number;
  *((json_number_t*) new->value) = value;
  
  // Update total length of stringified json
  obj->string_length += key_len + value_len + NUMBER_EXTRA_LENGTH;
  
  return new;
}
*/
void json_add(json_object_t *obj, json_field_t *field) {
  json_field_t *ptr;
  
  // If object is empty, create first field, otherwise append to end
  if (obj->head == NULL) {
    obj->head = field;
  }
  else {
    for (ptr = obj->head; ptr && ptr->next; ptr = ptr->next)
      ;
    ptr->next = field;
  }

  // Update total length of stringified json
  //printf("key len: %d, value len: %d\n", (int)field->key_strlen, (int)field->value_strlen);
  obj->string_length += strlen(field->key) + strlen(field->value) + STRING_EXTRA_LENGTH;
}

char* json_stringify (json_object_t *obj) {
  char *format_string, *ret;
  json_field_t *ptr;
  FILE *stream;

  // Allocate return string
  ret = malloc(obj->string_length * sizeof(char));
  
  // Open stream on return string
  stream = fmemopen((char*) ret, obj->string_length, "w");
  if (stream == NULL) {
    perror("error opening stream to return buffer");
    exit(EXIT_FAILURE);
  }

  // Do the stringifying thing ring-a-ding-ding
  fprintf(stream, "{");
  for (ptr = obj->head; ptr; ptr = ptr->next) {

    switch (ptr->type) {
    case string:
      format_string = "\"%s\":\"%s\"";
      fprintf(stream, format_string, ptr->key, ptr->value);
      break;
    case number:
      format_string = "\"%s\":\"%lld\"";
      fprintf(stream, format_string, ptr->key, (json_number_t) ptr->value);
      break;
    case object: 
      break;
    case array:
      break;
    case boolean:
      break;
    case nil:
      break;
    }
    
    if (ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "}");
  
  // Clean up and return
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

