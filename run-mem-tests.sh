#!/bin/bash
#
# Build the library and run the test suite under valgrind memcheck.
# The optional argument is a regular expression to filter the tests
# (ctest -R). Memory errors and leaks fail the tests.

cmake -DCMAKE_BUILD_TYPE=Debug -DMEMCHECK=ON ..
make
if [ $? != 0 ]
then
    echo "make test failed!"
    exit 1
fi

echo
echo "memcheck tests ready!"
echo

if [ -n "$1" ]
then
    ctest --output-on-failure -R "$1"
else
    ctest --output-on-failure
fi
