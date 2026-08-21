#!/bin/bash
#
# Build the library and run the test suite. The optional argument
# is a regular expression to filter the tests (ctest -R).

cmake -DCMAKE_BUILD_TYPE=Debug -DMEMCHECK=OFF ..
make
if [ $? != 0 ]
then
    echo "make test failed!"
    exit 1
fi

echo
echo "tests ready!"
echo

if [ -n "$1" ]
then
    ctest --output-on-failure -R "$1"
else
    ctest --output-on-failure
fi
