#!/bin/bash

DEBUG=1 make
DEBUG=1 make tests
if [ $? != 0 ]
then
    echo "make test failed!"
    exit 1
fi

echo
echo "tests ready!"
echo

for t in $(ls tests/*.test); do
    num=$(echo $t | grep -Poe '\d+')
    echo -n "$num $t... "
    if [ -f "$t-input.graphml" ]
    then
	$t > "$t.txt"
    else
	$t
    fi
    if [ $? != 0 ]
    then
	echo "test run failed!"
	exit 2
    fi
    if [ -f "tests/$num-output.graphml" ]
    then
	diff "$t.graphml" "tests/$num-output.graphml"
	if [ $? != 0 ]
	then
	    echo "test failed: graphml file didn't match the pattern!"
	    exit 3
	fi
    fi
    if [ -f "tests/$num-output.txt" ]
    then
	diff "$t.txt" "tests/$num-output.txt"
	if [ $? != 0 ]
	then
	    echo "test failed: output didn't match the pattern!"
	    exit 3
	fi
    fi
    echo "ok"
done

exit 0
