#!/bin/bash

echo | ./lab4b --shit &> /dev/null
if [[ $? -ne 1 ]]
then
    echo "bad arg not caught"
fi 