#!/bin/bash
if (( $# < 2 ))
then
    echo error
    exit
fi

mkisofs -o $2 $1 &> /dev/null
if [ $? != 0 ]
then    
    echo error
    exit
fi

if [ -n "$3" ]
then
    chown $3 $2 &> /dev/null
    if [ $? != 0 ]
    then
        echo error
        exit
    fi
fi
if [ -n "$4" ]
then
    chown $3:$4 $2 &> /dev/null
    if [ $? != 0 ]
    then
        echo error
        exit
    fi
fi

echo done
