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

json_object_t *json_newsted();
/*
 * Create root node.
 *
 * Return: empty json object.
 */

json_key_t *json_new_key(char *key);
/*
 * Create new json key.
 *
 * key: char pointer to null-terminated string
 *
 * Return: json key
 */

json_value_t *json_new_object();
/*
 * Create new json object.
 *
 * Return: json value of type object
 */

json_value_t *json_new_string(char *value);
/*
 * Create new json string.
 *
 * value: char pointer to null-terminated string
 *
 * Return: json value of type string
 */

json_value_t *json_new_integer(json_integer_t value);
/*
 * Create new json number
 *
 * value: integer number. Internally represented as long long integer and should
 *        thus be casted to this if int or long.
 *
 * Return: json value of type integer
 */

void json_add_object(json_object_t *obj, json_key_t *key, json_value_t *value);
/*
 * Add key/value pair to an json object.
 *
 * obj:   the object to be populated by the key/value pair
 * key:   json key
 * value: json value
 */

char* json_stringify (json_object_t *obj);
/*
 * Create a string representation of the json object. The returned string has
 * been malloced by the function and should thus be freed by the caller.
 *
 * obj: the object to be stringified
 *
 * Return: null-terminated char pointer to the string
 */

void json_free(json_object_t *obj);
/*
 * Free all allocated resources used by the json object.
 *
 * obj: the json object to be freed
 */

#endif //NEWSTED_H