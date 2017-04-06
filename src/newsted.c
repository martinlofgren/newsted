/*
 * newsted - a JSON library for C.
 *
 * Latest source is available at https://github.com/martinlofgren/newsted
 *
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#include "newsted.h"

// Static function declarations

static json_value_t *new_value (json_type_t new_type_enum,
				void (*new_tostring) (struct json_value *value,
						      FILE *stream),
				size_t (*new_strlen) (struct json_value *value),
				void (*new_free) (struct json_value *value),
				size_t new_data_size);
static json_key_t *new_key(char *key);
static void generate_key(json_key_t *key, FILE* stream);
static void generate_string(json_value_t *value, FILE* stream);
static void generate_integer(json_value_t *value, FILE* stream);
static void generate_float(json_value_t *value, FILE* stream);
static void generate_boolean(json_value_t *value, FILE* stream);
static void generate_object(json_value_t *value, FILE* stream);
static void generate_array(json_value_t *value, FILE* stream);
static size_t strlen_object(json_value_t *value) __attribute__ ((pure));
static size_t strlen_array(json_value_t *value) __attribute__ ((pure));
static size_t strlen_string(json_value_t *value) __attribute__ ((pure));
static size_t strlen_integer(json_value_t *value) __attribute__ ((pure));
static size_t strlen_float(json_value_t *value) __attribute__ ((pure));
static size_t strlen_boolean(json_value_t *value) __attribute__ ((pure));
static void free_object(json_value_t *value);
static void free_array(json_value_t *value);
static void free_key(json_key_t *key);
static void free_simple_value(json_value_t *value);

// Dummy hash function
int hash(char *s) {
  return (int) *s;
}

// ---------------------------------------------------------------------------
// New objects functions
// ---------------------------------------------------------------------------

json_key_t *new_key(char* key) {
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
				void (*new_tostring) (struct json_value *value,
						      FILE *stream),
				size_t (*new_strlen) (struct json_value *value),
				void (*new_free) (struct json_value *value),
				size_t new_data_size) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = new_type_enum;
  new->tostring = new_tostring;
  new->strlen = new_strlen;
  new->free = new_free;

  if (new_data_size) {
    new->data = malloc(new_data_size);
    if (new->data == NULL)
      return NULL;
  }

  return new;
}

json_value_t *json_new_object() {
  json_value_t *new;

  if ((new = new_value(object, generate_object, strlen_object, free_object, 0))) {
    new->data = malloc(sizeof(json_object_t));
    if (new->data == NULL)
      return NULL;

    ((json_object_t *)new->data)->head = NULL;
  }

  return new;
}

json_value_t *json_new_array() {
  json_value_t *new;

  if ((new = new_value(array, generate_array, strlen_array, free_array, 0))) {
    new->data = malloc(sizeof(json_array_t));
    if (new->data == NULL)
      return NULL;

    ((json_array_t *)new->data)->head = NULL;
  }

  return new;
}

json_value_t *json_new_string(char *value) {
  json_value_t *new;
  size_t len;

  len = strlen(value);
  if ((new = new_value(string, generate_string, strlen_string,
		       free_simple_value, (len + 1) * sizeof(json_float_t))))
    memcpy(new->data, value, len + 1);

  return new;
}

json_value_t *json_new_integer(json_integer_t value) {
  json_value_t *new;

  if ((new = new_value(num_integer, generate_integer, strlen_integer,
		       free_simple_value, sizeof(json_integer_t))))
    *(json_integer_t*) new->data = value;
  
  return new;
}

json_value_t *json_new_float(json_float_t value) {
  json_value_t *new;

  if ((new = new_value(num_float, generate_float, strlen_float,
		       free_simple_value, sizeof(json_float_t))))
    *(json_float_t*) new->data = value;

  return new;
}

json_value_t *json_new_boolean(json_boolean_t value) {
  json_value_t *new;

  if ((new = new_value(boolean, generate_boolean, strlen_boolean,
		       free_simple_value, sizeof(json_boolean_t))))
    *(json_boolean_t*) new->data = value;

  return new;
}

// ---------------------------------------------------------------------------
// Functions that add values or name/value pairs to container values
// ---------------------------------------------------------------------------

json_status_t json_add_object(json_value_t *obj, char *key,
			      json_value_t *value) {
  json_key_t *_new_key, *key_ptr;
  json_object_t *object_ptr;

  object_ptr = obj->data;

  // Create new key and associate with value
  if ((_new_key = new_key(key)) == NULL)
    return ERR_MEM_ALLOC;
  _new_key->value = value;
  
  // Empty object -> create first key/value pair
  if (object_ptr->head == NULL)
    object_ptr->head = _new_key;

  // Object does contain some entries
  else {
    key_ptr = object_ptr->head;
    
    // Iterate over object until there are no more entries OR the hash of the
    // key is larger than the prior one
    while (key_ptr->next && (key_ptr->next->hash < _new_key->hash))
      key_ptr = key_ptr->next;

    // Detect duplicate keys
    if (key_ptr->hash == _new_key->hash) {
      if (!strcmp(key_ptr->data, _new_key->data))
	return ERR_KEY_COLLISION;
    }

    // Insert in linked list if not the last entry
    if (key_ptr->next)
      _new_key->next = key_ptr->next;
    key_ptr->next = _new_key;
  }
  
  return SUCCESS;
}

json_status_t json_add_array(json_value_t *array, json_value_t *value) {
  json_array_t *array_ptr;
  json_array_value_t *new_array_value, *array_value_ptr;

  array_ptr = array->data;

  // Create new array value and associate with actual value
  if ((new_array_value = malloc(sizeof(json_array_value_t))) == NULL)
    return ERR_MEM_ALLOC;
  new_array_value->data = value;
  new_array_value->next = NULL;

  // Empty array -> create first value
  if (array_ptr->head == NULL)
    array_ptr->head = new_array_value;

  // Object does contain some entries. Iterate over array until there are no
  // more entries and append to linked list
  else {
    array_value_ptr = array_ptr->head;

    while (array_value_ptr->next)
      array_value_ptr = array_value_ptr->next;

    array_value_ptr->next = new_array_value;
  }
  
  return SUCCESS;
}


// ---------------------------------------------------------------------------
// String length calculation functions
// ---------------------------------------------------------------------------

#define KEY_EXTRA_LENGTH     3 // "...":
#define STRING_EXTRA_LENGTH  2 // "..."
#define NUMBER_EXTRA_LENGTH  0 // 
#define OBJECT_EXTRA_LENGTH  2 // {...}
#define ARRAY_EXTRA_LENGTH   2 // [...]
#define BOOLEAN_EXTRA_LENGTH 0 //

size_t json_strlen(json_value_t *value) {
  return value->strlen(value);
}

static size_t strlen_object(json_value_t *value) {
  json_object_t *obj;
  json_key_t *key_ptr;
  json_value_t *value_ptr;
  size_t ret;

  obj = (json_object_t *) value->data;
  ret = 0;

  for (key_ptr = obj->head; key_ptr; key_ptr = key_ptr->next) {
    ret += key_ptr->len + KEY_EXTRA_LENGTH;
    value_ptr = key_ptr->value;
    ret += value_ptr->strlen(value_ptr);

    if (key_ptr->next)
      ret++;
  }

  return ret + OBJECT_EXTRA_LENGTH;
}

static size_t strlen_array(json_value_t *value) {
  json_array_t *array;
  json_array_value_t *array_value_ptr;
  json_value_t *value_ptr;
  size_t ret;

  array = (json_array_t *) value->data;
  ret = 0;

  for (array_value_ptr = array->head;
       array_value_ptr;
       array_value_ptr = array_value_ptr->next) {
    value_ptr = array_value_ptr->data;
    ret += value_ptr->strlen(value_ptr);

    if (array_value_ptr->next)
      ret++;
  }

  return ret + ARRAY_EXTRA_LENGTH;
}

static size_t strlen_string(json_value_t *value) {
  return strlen((json_string_t) value->data) + STRING_EXTRA_LENGTH;
}

static size_t strlen_integer(json_value_t *value) {
  json_integer_t val;
  size_t n;

  val = (json_integer_t) value->data;
  
  n = 0;
  if (val < 0)
    n++;
  do {
    n++;
  } while (val /= 10);

  return n + NUMBER_EXTRA_LENGTH;
}

static size_t strlen_float(json_value_t *value) {
  char tmp[22];

  return sprintf(tmp, "%.17g", *(json_float_t *) value->data) + NUMBER_EXTRA_LENGTH;
}

static size_t strlen_boolean(json_value_t *value) {
  return ((value->data != FALSE) ? 4 : 5) + BOOLEAN_EXTRA_LENGTH;
}

// ---------------------------------------------------------------------------
// JSON generator functions
// ---------------------------------------------------------------------------

json_status_t json_generate (json_value_t *value, FILE *stream) {
  value->tostring(value, stream);
  
  return SUCCESS;
}

static void generate_key(json_key_t *key, FILE* stream) {
  fprintf(stream, "\"%s\":", key->data);
}

static void generate_string(json_value_t *value, FILE* stream) {
  fprintf(stream, "\"%s\"", (json_string_t) value->data);
}

static void generate_integer(json_value_t *value, FILE* stream) {
  fprintf(stream, "%lld", *(json_integer_t *) value->data);
}

static void generate_float(json_value_t *value, FILE* stream) {
  fprintf(stream, "%.17g", *(json_float_t *) value->data);
}

static void generate_boolean(json_value_t *value, FILE* stream) {
  fprintf(stream, (*(json_boolean_t *) value->data != FALSE) ? "true" : "false");
}

static void generate_object(json_value_t *value, FILE* stream) {
  json_key_t *key_ptr;
  json_value_t *value_ptr;

  fprintf(stream, "{");
  for (key_ptr = ((json_object_t *) value->data)->head;
       key_ptr;
       key_ptr = key_ptr->next) {
    generate_key(key_ptr, stream);
    value_ptr = key_ptr->value;
    value_ptr->tostring(value_ptr, stream);
    
    if (key_ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "}");
}

static void generate_array(json_value_t *value, FILE* stream) {
  json_array_value_t *array_value_ptr;
  json_value_t *value_ptr;

  fprintf(stream, "[");
  for (array_value_ptr = ((json_array_t *) value->data)->head;
       array_value_ptr;
       array_value_ptr = array_value_ptr->next) {
    value_ptr = array_value_ptr->data;
    value_ptr->tostring(value_ptr, stream);
    
    if (array_value_ptr->next)
      fprintf(stream, ",");
  }
  fprintf(stream, "]");
}

// ---------------------------------------------------------------------------
// Freeing functions
// ---------------------------------------------------------------------------

static void free_object(json_value_t *value) {
  json_key_t *cur, *next;

  next = ((json_object_t *) value->data)->head;
  while ((cur = next)) {
    next = cur->next;
    free_key(cur);
  }
  free_simple_value(value);
}

static void free_array(json_value_t *value) {
  json_array_value_t *cur, *next;

  next = ((json_array_t *) value->data)->head;
  while((cur = next)) {
    next = cur->next;
    cur->data->free(cur->data);
    free(cur);
  }
  free_simple_value(value);
}

static void free_key(json_key_t *key) {
  key->value->free(key->value);
  free(key->data);
  free(key);
}

static void free_simple_value(json_value_t *value) {
  free(value->data);
  free(value);
}

void json_free(json_value_t *value) {
  value->free(value);
}

