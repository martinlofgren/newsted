newsted
=======

newsted is a json library written in C89. It has no dependencies other than the C standard library. newsted is intended to be simple, lightweight and portable. 

**This is a work in progress; don't expect anything to be stable, nor are all functionality implemented yet.**

Installation
------------

Build with `make`. To include in your project, simply include the header `newsted.h` and include the files in the source directory.

Usage
-----

Building json data consists of creating a json object and populate it with key/value pairs. This object can then be stringified. Following is a basic example, omitting error checking:

```c
// Create new base object
json_value_t *obj;
obj = json_new_object();

// Add some key/value pairs
json_add_object(obj, "Band", json_new_string("Metallica"));

// Generate JSON string
json_generate(obj, stdout);

// Free all allocated resources
json_free(obj);
```

A complete working example is to be found in [`test.c`](src/test.c). For a detailed description of the library API, see the [`newsted.h`](src/newsted.h) header file. 
