#!/bin/bash

# bad arg
echo | ./lab4b --shit &> /dev/null
if [[ $? -ne 1 ]]
then
    echo "bad arg not caught"
fi 

# logfile working
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
    echo "failed loging"
fi 

if [ ! -s logfile ] 
then
    echo "didn't create logfile"
fi

grep "SCALE=C" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile scale"
fi

grep "STOP" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile stop"
fi

grep "START" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile start"
fi

grep "SCALE=F" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile scale"
fi

grep "PERIOD=2" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile period"
fi

grep "OFF" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile off"
fi

grep "SHUTDOWN" logfile &> /dev/null; \
if [[ $? -ne 0 ]]
then
	echo "didn't properly logfile SHUTDOWN"
fi