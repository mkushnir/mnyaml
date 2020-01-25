Wrappers over libyaml
=====================

This library provides wrappers over
[libyaml](http://pyyaml.org/wiki/LibYAML) YAML parser.  The wrappers allow
to automatically translate YAML constructs to bare C structures.
Providing as much automation as possible, it nevertheless allows a full
flexibility of using complex type designs in C.

Some useful features of the library:

- mapping user defined sets of textual "atoms" to C enumerations;

- validating numeric ranges;

- handling special cases such as "deprecated" settings;

- defining and handling error conditions;

- dynamically map YAML sequences to dynamic arrays.


The library is based on the C structures and algorithms collection
[mncommon](http://github.com/mkushnir/mncommon).

