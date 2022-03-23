#!/bin/bash
killall -9 paycenter
sleep 1
ps -elf | grep paycenter | grep -v grep

