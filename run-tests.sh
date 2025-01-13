#!/bin/bash

cmake -DCMAKE_BUILD_TYPE=Debug ..
make
if [ $? != 0 ]
then
    echo "make test failed!"
    exit 1
fi

echo
echo "tests ready!"
echo

limit=$1

if [ "$limit" == "" ]
then
    limit="99"
fi

i=-1

for t in $(ls tests/*.test); do
    i=$((i + 1))
    if [ "$i" > "$limit" ]
    then
	break
    fi
    num=$(echo $t | grep -Poe '\d\d')
    echo -n "$num $t... "
    if [ -f "$t-input.graphml" -o -f "tests/$num-output.txt" ]
    then
	$t > "$t.txt"
    else
	$t
    fi
    if [ $? != 0 ]
    then
	echo "test $num run failed!"
	#continue
	exit 1
    fi
    if [ -f "tests/$num-output.txt" ]
    then
	diff "$t.txt" "tests/$num-output.txt"
	if [ $? != 0 ]
	then
	    echo "test $num failed: output didn't match the pattern!"
	    #continue
	    exit 1
	fi
    fi
    if [ -f "tests/$num-output.graphml" ]
    then
	diff "$t.graphml" "tests/$num-output.graphml"
	if [ $? != 0 ]
	then
	    echo "test $num failed: graphml file didn't match the pattern!"
	    #continue
	    exit 1
	fi
    fi
    echo "ok"
done

exit 0
