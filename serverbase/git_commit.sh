#!/bin/bash
if [ $# != 1 ]; then
    echo -e "Syntax: \033[44;31m$0 \"commit message\"\033[0m"
    exit 1;
fi

git_lines=`git status --porcelain | sort -u| awk -F ' ' '{printf("%s#%s\n", $1,$2)}'`
for line in $git_lines;
do
    op=`echo "$line" | awk -F '#' '{print $1}'`
    file=`echo "$line" | awk -F '#' '{print $2}'`
    if [ "$op" = "M" ]; then
        echo "git add $file"
        git add $file
    elif [ "$op" = "D" ]; then
        echo "git rm -f $file"
        git rm -f $file
    else
        echo "git add $file"
        git add $file
    fi
done
git commit -m "$1"
