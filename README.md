# The Cyberida State Machine C++ Library

The C++ library for processing CyberiadaML - the version of GraphML for storing state machine graphs
used by the Cyberiada Project, the Berloga Project games and the Orbita Simulator. 

This is C++ wrapper interface to the libcyberiadaml C library.

The code is distributed under the Lesser GNU Public License (version 3), the documentation -- under
the GNU Free Documentation License (version 1.3).

## Requirements

* libcyberidaml (and its dependencies)
* libstdc++
* cmake (version 3.21+)

## Installation

Create `build` directory: `mkdir build && cd build`

Run `cmake ..` to build the library binaries and the test program.

Run `make install` to install the library.

Use CMake parameters to change the build type / installation prefix / etc.

## Testing

The tests are run by CTest. From the `build` directory:

`cmake .. && make && ctest`

Configure with `-DMEMCHECK=ON` to run the tests under valgrind memcheck.

The wrapper scripts build the library and run the tests in one step
(from the `build` directory): `run-tests.sh` for the plain suite,
`run-mem-tests.sh` for the memcheck suite. The optional argument of both
scripts is a regular expression to filter the tests.
