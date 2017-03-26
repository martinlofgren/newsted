/*
 * newstedt - a json library for C.
 *
 * Latest source is available at https://github.com/martinlofgren/newstedt
 *
 * Author: Martin Löfgren <martin.c.lofgren@gmail.com>
 */

#ifndef NEWSTED_H
#define NEWSTED_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "types.h"
#include "error.h"

json_object_t *json_newsted();
/*
 * Create root node.
 *
 * Return value: empty json object.
 */

json_value_t *json_new_object();
/*
 * Create new json object.
 *
 * Return value: json value of type object
 */

json_value_t *json_new_string(char *value);
/*
 * Create new json string.
 *
 * value: char pointer to null-terminated string
 *
 * Return value: json value of type string
 */

json_value_t *json_new_integer(json_integer_t value);
/*
 * Create new json number
 *
 * value: integer number. Internally represented as long long integer and should
 *        thus be casted to this if int or long.
 *
 * Return value: json value of type integer
 */

json_value_t *json_new_float(json_float_t value);
/* DOCUMENT ME */

json_value_t *json_new_boolean(json_boolean_t value);
/* ME TOO */

json_status_t json_add_object(json_value_t *obj, char *key, json_value_t *value);
/*
 * Add key/value pair to an json object.
 *
 * obj:   the object to be populated by the key/value pair
 * key:   json key
 * value: json value
 *
 * Return value: Success/failure (see error.h)
 */

json_status_t json_stringify (json_value_t *value, FILE *stream);
/*
 * Create a string representation of the json object. The returned string has
 * been malloced by the function and should thus be freed by the caller.
 *
 * obj: the object to be stringified
 *
 * Return value: null-terminated char pointer to the string
 */

void json_free(json_value_t *value);
/*
 * Free all allocated resources used by the json object.
 *
 * obj: the json object to be freed
 */

#endif //NEWSTED_H
