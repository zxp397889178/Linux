#!/bin/bash 

Color_Text()
{
    echo -e " \e[0;$2m$1\e[0m"
}

echo_Blue()
{
    echo $(Color_Text "$1" "34")
}

usage() {
    echo_Blue " usage: sh $0 [-x 目标SVN库访问地址] [-s 指定开始时间] [-e 指定结束时间]"   
    echo_Blue " usage: sh $0 [-h ] 获取帮助"  
    echo_Blue "        其中参数-s 日期格式为{2020-01-01}或{20200101}"
} 

if [ $# -lt 3 ]; then
    usage
    exit 1 
fi

while getopts "x:s:e:h" option; do  
    case $option in  
        x) target=$OPTARG;;  
        s) start_parameter=$OPTARG;;  
        e) end_parameter=$OPTARG;;     
        h) usage; exit 1;;  
    esac  
done  

if [ -z $target ]; then  
    echo "请输入SVN库访问地址!"  
    exit 1  
fi  

if [ -z $start_parameter ]; then  
    echo "请输入开始时间!"  
    exit 1  
fi

if [ -z $end_parameter ]; then  
    echo "请输入结束时间!"  
    exit 1  
fi


TEMPFILE=temp.log

#定义空数组
arrValidRow=()

#-----------------通过查询日期第一个版本对比-1的版本号，直到结束--------------------
revs=`svn log $target -r$start_parameter:$end_parameter |awk '{print $1,$3}'|grep "^r[0-9]" | awk '{print $1}'`
	
for rev in ${revs}
do  
	rev=${rev:1}  
	last_rev=$((rev-1))
	
	svn diff $target -r${last_rev}:${rev} > $TEMPFILE
         
    # 去掉含有注释的代码行，只算增加的有效行数
    CountAdd=`grep '^+' $TEMPFILE | grep -v '^+++' |sed 's/^.//'|sed s/[[:space:]]//g |sed '/^$/d' |grep -v '^#' |grep -v '^//' |sed '/^[ \t]*\/\*/,/.*\*\//d' |wc -l`
	
	#echo "$rev"
	#echo "$last_rev"
	#echo $CountAdd
	#echo "------"
	lengthtemp0=${#arrValidRow[@]}
	#echo "lengthtemp = $lengthtemp0, CountAdd = $CountAdd"
	arrValidRow[lengthtemp0]=$CountAdd

	rm -rf $TEMPFILE
done

#----------------显示数组遍历结果---------------------
SubmitDate=`svn log $target -r$start_parameter:$end_parameter |grep "^r[0-9]" |awk '{print $1}' |sed 's/ //g'`
SubmitAuthor=`svn log $target -r$start_parameter:$end_parameter |grep "^r[0-9]" |awk '{print $3}' |sed 's/ //g'`
SubmitRevsion=`svn log $target -r$start_parameter:$end_parameter |grep "^r[0-9]" |awk '{print $5"_"$6}' |sed 's/ //g'`

#字符串转数组
arrSubmitRevsion=(${SubmitRevsion//\n/ })
arrSubmitAuthor=(`echo $SubmitAuthor | tr '\n' ' '`)
arrSubmitDate=(${SubmitDate//\n/ })

#循环遍历数组
length=${#arrSubmitRevsion[@]}
# echo "length = $length"

for((n=0;n<$length;n++));
do
    #echo "----- 序号$n -----"
    echo "${arrSubmitDate[n]} ${arrSubmitAuthor[n]} ${arrValidRow[n]} ${arrSubmitRevsion[n]}"
done
