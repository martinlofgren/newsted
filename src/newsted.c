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
				size_t new_data_size);
static json_key_t *new_key(char *key);
static void indent (int indent, FILE *stream);
static void generate_key(json_key_t *key, FILE* stream);
static void free_key(json_key_t *key);

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
				size_t new_data_size) {
  json_value_t *new;

  new = malloc(sizeof(json_value_t));
  if (new == NULL)
    return NULL;

  new->type = new_type_enum;

  if (new_data_size) {
    new->data = malloc(new_data_size);
    if (new->data == NULL)
      return NULL;
  }

  return new;
}

json_value_t *json_new_object() {
  json_value_t *new;

  if ((new = new_value(object, 0))) {
    new->data = malloc(sizeof(json_object_t));
    if (new->data == NULL)
      return NULL;

    ((json_object_t *)new->data)->head = NULL;
  }

  return new;
}

json_value_t *json_new_array() {
  json_value_t *new;

  if ((new = new_value(array, 0))) {
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
  if ((new = new_value(string, (len + 1) * sizeof(json_float_t))))
    memcpy(new->data, value, len + 1);

  return new;
}

json_value_t *json_new_integer(json_integer_t value) {
  json_value_t *new;

  if ((new = new_value(num_integer, sizeof(json_integer_t))))
    *(json_integer_t*) new->data = value;
  
  return new;
}

json_value_t *json_new_float(json_float_t value) {
  json_value_t *new;

  if ((new = new_value(num_float, sizeof(json_float_t))))
    *(json_float_t*) new->data = value;

  return new;
}

json_value_t *json_new_boolean(json_boolean_t value) {
  json_value_t *new;

  if ((new = new_value(boolean, sizeof(json_boolean_t))))
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
  size_t ret;
  json_object_t *obj;
  json_key_t *key_ptr;
  json_array_t *arr;
  json_array_value_t *arr_val_ptr;
  json_integer_t val;
  char tmp[22];

  switch (value->type) {
  case object:
    obj = (json_object_t *) value->data;
    ret = OBJECT_EXTRA_LENGTH;

    for (key_ptr = obj->head; key_ptr; key_ptr = key_ptr->next) {
      ret += key_ptr->len + KEY_EXTRA_LENGTH;
      ret += json_strlen(key_ptr->value);

      if (key_ptr->next)
	ret++;
    }
    break;

  case array:
    arr = (json_array_t *) value->data;
    ret = ARRAY_EXTRA_LENGTH;

    for (arr_val_ptr = arr->head; arr_val_ptr; arr_val_ptr = arr_val_ptr->next) {
      ret += json_strlen(arr_val_ptr->data);

      if (arr_val_ptr->next)
	ret++;
    }
    break;

  case string:
    ret = strlen((json_string_t) value->data) + STRING_EXTRA_LENGTH;
    break;

  case num_integer:
    val = *((json_integer_t*) value->data);
  
    ret = NUMBER_EXTRA_LENGTH;
    if (val < 0)
      ret++;
    do {
      ret++;
    } while (val /= 10);
    
    break;

  case num_float:
    ret = sprintf(tmp, "%.17g", *(json_float_t *) value->data) + NUMBER_EXTRA_LENGTH;
    break;

  case boolean:
    ret = ((value->data != FALSE) ? 4 : 5) + BOOLEAN_EXTRA_LENGTH;
    break;

  case nil:
    ret = 4;
    break;
  }

  return ret;
}


// ---------------------------------------------------------------------------
// JSON generator functions
// ---------------------------------------------------------------------------

void indent (int indent, FILE *stream) {
  int i;
  
  fprintf(stream, "\n");
  for (i = indent; i > 0; i--)
    fprintf(stream, " ");
}

json_status_t json_generate (json_value_t *value, unsigned char opt, FILE *stream) {
  json_key_t *key_ptr;
  json_array_value_t *array_value_ptr;
  char indent_spaces = opt & 0xF;
  char opt_indent = opt & JSON_INDENT;

  static int ind_level = 0;
  static int obj_val = FALSE;
  
  if (opt_indent && !obj_val)
    indent(ind_level * indent_spaces, stream);
  
  switch (value->type) {
  case object:
    fprintf(stream, "{");
    ind_level++;

    for (key_ptr = ((json_object_t *) value->data)->head;
	 key_ptr;
	 key_ptr = key_ptr->next) {
      if (opt_indent) 
	indent(ind_level * indent_spaces, stream);

      generate_key(key_ptr, stream);

      if (opt_indent)
	fprintf(stream, " ");
      
      obj_val = TRUE;
      json_generate(key_ptr->value, opt, stream);
      obj_val = FALSE;
    
      if (key_ptr->next) {
	fprintf(stream, ",");
      }
    }

    ind_level--;
    
    if (opt_indent)
      indent(ind_level * indent_spaces, stream);
    
    fprintf(stream, "}");
    break;

  case array:
    obj_val = FALSE;
    fprintf(stream, "[");
    ind_level++;

    for (array_value_ptr = ((json_array_t *) value->data)->head;
	 array_value_ptr;
	 array_value_ptr = array_value_ptr->next) {
      json_generate(array_value_ptr->data, opt, stream);
    
      if (array_value_ptr->next)
	fprintf(stream, ",");
    }

    ind_level--;
    if (opt_indent)
      indent(ind_level * indent_spaces, stream);

    fprintf(stream, "]");
    obj_val = TRUE;
    break;

  case string:
    fprintf(stream, "\"%s\"", (json_string_t) value->data);
    break;

  case num_integer:
    fprintf(stream, "%lld", *(json_integer_t *) value->data);
    break;

  case num_float:
    fprintf(stream, "%.17g", *(json_float_t *) value->data);
    break;

  case boolean:
    fprintf(stream, (*(json_boolean_t *) value->data != FALSE) ? "true" : "false");
    break;

  case nil:
    fprintf(stream, "null");
    break;
  }
  return SUCCESS;
}

static void generate_key(json_key_t *key, FILE* stream) {
  fprintf(stream, "\"%s\":", key->data);
}


// ---------------------------------------------------------------------------
// Freeing functions
// ---------------------------------------------------------------------------

static void free_key(json_key_t *key) {
  json_free(key->value);
  free(key->data);
  free(key);
}

void json_free(json_value_t *value) {
  json_key_t *cur_key, *next_key;
  json_array_value_t *cur_arr, *next_arr;

  switch (value->type) {
  case object:
    next_key = ((json_object_t *) value->data)->head;
    while ((cur_key = next_key)) {
      next_key = cur_key->next;
      free_key(cur_key);
    }
    free(value);
    break;

  case array:
    next_arr = ((json_array_t *) value->data)->head;
    while ((cur_arr = next_arr)) {
      next_arr = cur_arr->next;
      json_free(cur_arr->data);
      free(cur_arr);
    }
    free(value);
    break;

  case string:          // Fall through
  case num_integer:     //      |
  case num_float:       //      |
  case boolean:         //      v
    free(value->data);
  case nil:             //      v
    free(value);
    break;
  }
}

