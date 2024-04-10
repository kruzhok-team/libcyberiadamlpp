# The Cyberida State Machine C++ Library

The C++ library for processing CyberiadaML - the version of GraphML for storing state machine graphs
used by the Cyberiada Project, the Berloga Project games and the Orbita Simulator. 

This is C++ wrapper interface to the libcyberiadaml C library.

The code is distributed under the Lesser GNU Public License (version 3), the documentation -- under
the GNU Free Documentation License (version 1.3).

## Requirements

* libcyberidaml
* libstdc++

## Installation

Run `make` to build the library binaries.

Run `make test` to build the test program.

Use variables:
* `DEBUG=1` debug version of the library
* `DYNAMIC=1` build shared version of the library
