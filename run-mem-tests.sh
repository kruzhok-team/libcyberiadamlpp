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

MEMTEST="valgrind --tool=memcheck"

for t in $(ls tests/*.test); do
    i=$((i + 1))
    if [ "$i" == "$limit" ]
    then
	break
    fi
    num=$(echo $t | grep -Poe '\d\d')
    echo -n "$num $t... "
    cmd="$MEMTEST $t"
    if [ -f "$t-input.graphml" -o -f "tests/$num-output.txt" ]
    then
	$cmd > "$t.txt" 2>/dev/null
    else
	$cmd 2>/dev/null
    fi
    if [ $? != 0 ]
    then
	echo "memcheck test $num run failed!"
	#continue
	exit 1
    fi
    echo "ok"
done

exit 0
