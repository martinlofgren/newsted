#include <stdlib.h> //malloc
#include <string.h> //strlen
#include <stdio.h>  //printf

// Types and defines
#define STRING_EXTRA_LENGTH 6 // "\"\":\"\","

#define FALSE 0
#define TRUE  1

typedef unsigned char bool_t;

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
  int string_length;
} json_object_t;

typedef char *json_string_t;
typedef int *json_number_t;

// Functions

json_object_t *json_new() {
  json_object_t *obj;

  obj = malloc(sizeof(json_object_t));
  if (obj == NULL)
    return NULL;

  obj->head = NULL;
  obj->string_length = 3; // To hold "{}\0"
  return obj;
}

bool_t json_add_string(json_object_t *obj, char *key, char *value) {
  json_field_t *new, *ptr;
  int key_len, value_len;

  key_len = strlen(key);
  value_len = strlen(value);

  // Allocate new json field and strings
  new = malloc(sizeof(json_field_t));
  if (new == NULL)
    return FALSE;
  
  new->key = malloc(key_len * sizeof(char));
  if (new->key == NULL)
    return FALSE;
  
  new->value = malloc(value_len * sizeof(char));
  if (new->value == NULL)
    return FALSE;
  
  // Set values
  new->type = string;
  memcpy(new->key, key, key_len);
  memcpy(new->value, value, value_len);
  new->next = NULL;

  // If object is empty, create first field, otherwise append to end
  if (obj->head == NULL) {
    obj->head = new;
  }
  else {
    for (ptr = obj->head; ptr && ptr->next; ptr = ptr->next)
      ;
    ptr->next = new;
  }

  // Update total length of stringified json
  obj->string_length += strlen(key) + strlen(value) + STRING_EXTRA_LENGTH;
  
  return TRUE;
}

char* json_stringify (json_object_t *obj) {
  char *ret;
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

  // Do the stringifying thing
  fprintf(stream, "{");
  for (ptr = obj->head; ptr; ptr = ptr->next) {
    fprintf(stream, "\"%s\":\"%s\"", ptr->key, ptr->value);
    if (ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "}\0");

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

int main () {
  json_object_t *obj;
  char *json_string, buf1[64], buf2[64];
  int i;

  // Create json object
  obj = json_new();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }

  // Stringigy empty json object
  json_string = json_stringify(obj);
  printf("%s\n", json_string);
  free(json_string);
  json_free(obj);

  // Create json object
  obj = json_new();
  if (obj == NULL) {
    perror("error on creating json object");
    exit(EXIT_FAILURE);
  }
  // Populate json object with some string values
  for (i=1; i<5; i++) {
    sprintf(buf1, "key %d", i);
    sprintf(buf2, "value %d", i);
    if (!(json_add_string(obj, buf1, buf2))) {
      perror("error on adding string");
      exit(EXIT_FAILURE);
    };
  }

  // Stringify populated json object
  json_string = json_stringify(obj);
  printf("%s\n", json_string);
  free(json_string);
  json_free(obj);
  
  return 0;
}
