#!/bin/bash
ulimit -c unlimited
killall -9 paycenter
./paycenter -c ./config.xml &
sleep 1
ps -elf | grep --color paycenter | grep -v grep 
