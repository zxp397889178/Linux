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
    echo_Blue " usage: sh $0 [-x 目标SVN库访问地址] [-s 指定开始revision号] "   
	echo_Blue "              [-e 指定结束revision号] [-u SVN账号] [-p SVN密码] "  
    echo_Blue " usage: sh $0 [-h ] 获取帮助"  
    echo_Blue "        其中-s,-e参数 revision号格式为纯数字，例如1000 "
} 

if [ $# -lt 3 ]; then
    usage
    exit 1 
fi

while getopts "x:s:e:u:p:h" option; do  
    case $option in  
        x) target=$OPTARG;;  
        s) start_parameter=$OPTARG;;  
        e) end_parameter=$OPTARG;;  
        u) user=$OPTARG;;  
        p) passwd=$OPTARG;;  
        h) usage; exit 1;;  
    esac  
done  

if [ -z $target ]; then  
    echo "请输入SVN库访问地址!"  
    exit 1  
fi  

if [ -z $start_parameter ]; then  
    echo "请输入开始revision号!"  
    exit 1  
fi

if [ -z $end_parameter ]; then  
    echo "请输入结束revision号!"  
    exit 1  
fi

if [ -z $user ]; then  
    echo "请输入SVN账号!"  
    exit 1  
fi

if [ -z $passwd ]; then  
    echo "请输入SVN密码!"  
    exit 1  
fi

if [ $start_parameter -gt $end_parameter ]; then  
    echo "所输参数，开始revision号必须比结束revision号小"  
    exit 1  
fi


TEMPFILE=temp.log
USERNAME=${user}
PASSWD=${passwd}
arrValidRow=()

latest_version=`svn info $target --username $USERNAME --password $PASSWD |awk 'NR==6 {print $2}'`

if [[ $start_parameter -le $latest_version && $end_parameter -le $latest_version ]]; then
	SubmitRevsion=`svn log $target -r$start_parameter:$end_parameter --username $USERNAME --password $PASSWD |awk '{print $1}' |grep -P "^r[1-9]\d*$" |sed 's/ //g' |tr -d "r"`
	SubmitAuthor=`svn log $target -r$start_parameter:$end_parameter --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $3}' |tr -s '\n'`
	SubmitDate=`svn log $target -r$start_parameter:$end_parameter --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $5}' |tr -s '\n'`
	SubmitTime=`svn log $target -r$start_parameter:$end_parameter --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $6}' |tr -s '\n'`
elif [[ $start_parameter -gt $latest_version && $end_parameter -gt $latest_version ]]; then
	SubmitRevsion=`svn log $target -rHEAD --username $USERNAME --password $PASSWD |awk '{print $1}' |grep -P "^r[1-9]\d*$" |sed 's/ //g' |tr -d "r"`
	SubmitAuthor=`svn log $target -rHEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $3}' |tr -s '\n'`
	SubmitDate=`svn log $target -rHEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $5}' |tr -s '\n'`
	SubmitTime=`svn log $target -rHEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $6}' |tr -s '\n'`
elif [ $start_parameter -gt $latest_version ]; then
	SubmitRevsion=`svn log $target -r$end_parameter:HEAD --username $USERNAME --password $PASSWD |awk '{print $1}' |grep -P "^r[1-9]\d*$" |sed 's/ //g' |tr -d "r"`
	SubmitAuthor=`svn log $target -r$end_parameter:HEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $3}' |tr -s '\n'`
	SubmitDate=`svn log $target -r$end_parameter:HEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $5}' |tr -s '\n'`
	SubmitTime=`svn log $target -r$end_parameter:HEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $6}' |tr -s '\n'`
elif [ $end_parameter -gt $latest_version ]; then
	SubmitRevsion=`svn log $target -r$start_parameter:HEAD --username $USERNAME --password $PASSWD |awk '{print $1}' |grep -P "^r[1-9]\d*$" |sed 's/ //g' |tr -d "r"`
	SubmitAuthor=`svn log $target -r$start_parameter:HEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $3}' |tr -s '\n'`
	SubmitDate=`svn log $target -r$start_parameter:HEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $5}' |tr -s '\n'`
	SubmitTime=`svn log $target -r$start_parameter:HEAD --username $USERNAME --password $PASSWD |grep "^r[0-9]" |awk '{print $6}' |tr -s '\n'`
fi


for rev in ${SubmitRevsion}
do  
	last_rev=$((rev-1))
	
	svn diff $target -r${last_rev}:${rev} --username $USERNAME --password $PASSWD > $TEMPFILE
	
	CountAdd=`codeline $TEMPFILE |awk '{print $1}'`
	
	rm -rf $TEMPFILE
	
	#echo "$rev"
	#echo "$last_rev"
	#echo $CountAdd
	#echo "------"
	lengthtemp0=${#arrValidRow[@]}
	#echo "lengthtemp = $lengthtemp0, CountAdd = $CountAdd"
	arrValidRow[lengthtemp0]=$CountAdd
done

arrSubmitRevsion=(${SubmitRevsion//\n/ })
arrSubmitAuthor=(`echo $SubmitAuthor | tr '\n' ' '`)
arrSubmitDate=(${SubmitDate//\n/ })
arrSubmitTime=(${SubmitTime//\n/ })

length=${#arrSubmitRevsion[@]}
# echo "length = $length"

for ((n=$length;n>-1;n--))
do
	echo "${arrSubmitRevsion[n]} ${arrSubmitAuthor[n]} ${arrValidRow[n]} ${arrSubmitDate[n]} ${arrSubmitTime[n]}"
done
