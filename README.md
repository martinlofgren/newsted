newsted
=======

newsted is a json library written in C89. It has no dependencies other than the C standard library. newsted is intended to be simple, lightweight and portable. 

Installation
------------

Build with `make`. To include in your project, simply include the header `newsted.h` and include the `newsted.c` source file directory.

Usage
-----

Building json data consists of creating a json object and populate it with key/value pairs. This object can then be stringified. Following is a basic example, omitting error checking:

```c
json_object_t *obj = json_newsted();
json_add_object(obj, json_new_key("Band"), json_new_string("Metallica"));
char *string = json_stringify(obj);
json_free(obj);
free(string);
```

A complete working example is to be found in [`test.c`](src/test.c). For a detailed description of the library API, see the [`hewsted.h`](src/newsted.h) header file. 
