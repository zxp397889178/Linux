#!/bin/bash

process_ver=`uname -p`
kernel_ver=`uname -r | awk -F '.' '{print $(NF-1)}'`
lib_list='BaseCode Database DbAdapter NetWork Security ProtoBase Utility QWFrame'

cur_dir=$(pwd)
bin_dir=$cur_dir/bin
lib_dir=$cur_dir/lib

cd $bin_dir
for f in $lib_list
do
    newf=`ls -t lib$f.so.* | awk 'NR==1'`
#    echo $newf
    if [ -f "$newf" ]; then
        ln -s -f $newf ./lib$f.so
    fi
done
echo "build bin link ok"
ls -lh $bin_dir

cd $lib_dir
for f in $lib_list
do
    if [ -f "lib$f.$kernel_ver.$process_ver.a" ]; then
        ln -s -f lib$f.$kernel_ver.$process_ver.a ./lib$f.a
    fi
done
echo "build lib link ok"
ls -lh $lib_dir

