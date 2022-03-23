#!/bin/bash

process_ver=`uname -p`
kernel_ver=`uname -r | awk -F '.' '{print $(NF-1)}'`
lib_list='BaseCode Database DbAdapter NetWork Security ProtoBase Utility'

cur_dir=$(pwd)
bin_dir=$cur_dir/bin
lib_dir=$cur_dir/lib

cd $bin_dir
for f in $lib_list
do
    newf=`ls -t lib$f.so.* | awk 'NR==1'`
#    echo $newf
    ln -s -f $newf ./lib$f.so
done
echo "build bin link ok"
ls -lh $bin_dir

cd $lib_dir
for f in $lib_list
do
    ln -s -f lib$f.$kernel_ver.$process_ver.a ./lib$f.a
done
echo "build lib link ok"
ls -lh $lib_dir

