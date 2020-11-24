#!/bin/bash

# bad arg
echo | ./lab4b --shit &> /dev/null
if [[ $? -ne 1 ]]
then
    echo "bad arg not caught"
fi 

# log working
./lab4b --period=1 --scale="F" --log="logfile" <<-EOF
SCALE=C
STOP
START
SCALE=F
PERIOD=2
OFF
EOF

if [[ $? -ne 0 ]]
then 
    echo "failed logging"
fi 

if [ ! -s logfile ] 
then
    echo "didn't create logfile"
fi

grep "SCALE=C" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log scale"
fi

grep "STOP" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log stop"
fi

grep "START" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log start"
fi

grep "SCALE=F" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log scale"
fi

grep "PERIOD=2" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log period"
fi

grep "OFF" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log off"
fi

grep "SHUTDOWN" LOG &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly log SHUTDOWN"
fi