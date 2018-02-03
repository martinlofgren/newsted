/*
 * newsted - a JSON library for C.
 *
 * Latest source is available at https://github.com/martinlofgren/newsted
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

json_value_t *json_new_object();
/*
 * Create new JSON object.
 *
 * Return value: JSON value of type object.
 */

json_value_t *json_new_array();
/* 
 * Create new JSON array.
 *
 * Return value: JSON value of type array.
 */

json_value_t *json_new_string(char *value);
/*
 * Create new JSON string.
 *
 * value: char pointer to null-terminated string
 *
 * Return value: JSON value of type string
 */

json_value_t *json_new_integer(json_integer_t value);
/*
 * Create new JSON number of type integer
 *
 * value: integer number. Internally represented as long long integer and should
 *        thus be casted to this if int or long (or, cast to the typedef
 *        json_integer_t).
 *
 * Return value: JSON value of type integer
 */

json_value_t *json_new_float(json_float_t value);
/*
 * Create new JSON number of type float
 *
 * value: float number. Internally represented as double and should
 *        thus be casted to this if int or long (or, cast to the typedef
 *        json_float_t).
 *
 * Return value: JSON value of type float
 */

json_value_t *json_new_boolean(json_boolean_t value);
/*
 * Create new JSON boolean
 *
 * value: boolean value. True corresponds to 1 and false to 0 (integer values),
 *        these are defined as TRUE and FALSE in the types.h include file.
 *
 * Return value: JSON value of type boolean
 */

json_status_t json_add_object(json_value_t *obj, char *key, json_value_t *value);
/*
 * Add key/value pair to an JSON object.
 *
 * obj:   the object to be populated by the key/value pair
 * key:   JSON key
 * value: JSON value
 *
 * Return value: Success/failure (see error.h)
 */

json_status_t json_add_array(json_value_t *array, json_value_t *value);
/*
 * Append value to an JSON array.
 *
 * array: the array to be populated by the value
 * value: JSON value
 *
 * Return value: Success/failure (see error.h)
 */

size_t json_strlen(json_value_t *value);
/*
 * Get the length of generated JSON string, as it is generated by json_generate.
 *
 * value: the value to calculate.
 *
 * Return value: the length of generated JSON string, not including terminating 
 *               null character.
 */

json_value_t *json_parse (char *string);
/*
 * Parse a string and create a new json value if the provided string is valid
 * json.
 *
 * string: the string to be parsed
 *
 * Return value: json value if succesfull, NULL otherwise.
 */

json_status_t json_generate (json_value_t *value, unsigned char opt, FILE *stream);
/* 
 * Generate JSON string.
 *
 * value:  the value from which the string should be generated.
 * stream: the stream to which the generated string should be sent.
 *
 * Return value: Success/failure (see error.h)
 */

void json_free(json_value_t *value);
/*
 * Free all allocated resources for the value. If value is a container type
 * (object or array), all contained values will be freed. Typical use case is
 * to instantiate a container (object or array), populate with keys/values or
 * values, and after use free the top container.
 *
 * value: the JSON value to be freed
 */

#endif //NEWSTED_H
