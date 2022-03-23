#!/bin/bash
# 函数参数信息
# https://www.runoob.com/linux/linux-shell-func.html

process_ver=`uname -p`
kernel_ver=`uname -r | awk -F '.' '{print $(NF-1)}'`
lib_list='BaseCode Database DbAdapter NetWork Security ProtoBase Utility QWFrame'

dir_base=/data/work_git/serverbase
dir_all='$dir_base'

mkdir -p $dir_base/bin $dir_base/lib $dir_base/include

# 定义发布到指定目录的函数
dist_to_dir() {
    if [ -d $1 ]; then
        if [ ! -d $1/bin ]; then mkdir -p $1/bin; fi;
        if [ ! -d $1/lib ]; then mkdir -p $1/lib; fi;
        \cp -r include/ $1/;
        for f in $lib_list
        do
            nf=`ls -t ./bin/lib$f.so.* | awk 'NR==1'`
            rm -f $1/bin/lib$f.so.*
            \cp -f $nf $1/bin
            \cp -f ./lib/lib$f.$kernel_ver.$process_ver.a $1/lib
        done;
        \cp -f ./build_ln.sh $1;
        echo "dist devel library to $1";
        #$1/build_ln.sh;
    fi;
}

dist_to_dir $dir_base

# 定义对链接进行排序的函数
sort_dist_dir() {
    if [ -d $1 ]; then
        cd $1/bin;
        for f in $lib_list
        do
            nf=`ls -t lib$f.so.* | awk 'NR==1'`
            ln -s -f $nf ./lib$f.so
        done;
        cd $1/lib;
        for f in $lib_list
        do
            ln -s -f lib$f.$kernel_ver.$process_ver.a ./lib$f.a
        done;
        echo "make $1 lib & bin link"
    fi;
}

sort_dist_dir $dir_base

