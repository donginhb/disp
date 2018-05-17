#!/bin/bash

if [ $# -ne 0 -a $# -ne 1 ]; then
	echo "usage $0 or $0 backupdate(yyyymmdd)"
	exit 1
fi

##У������Ϸ���
if [ $# -eq 1 ]; then
	tmpdate=`date -d "$1" +%Y%m%d`
	if [ "$tmpdate"X != "$1"X ];then
		echo "param1=$1 must in yyyymmdd format e.g $tmpdate"
		echo "usage $0 or $0 backupdate(yyyymmdd)"
		exit 1;
	fi
fi

bakdate=${1:-"`date '+%Y%m%d'`"}

conf_file="$HOME/conf/config.ini"
[ ! -f $conf_file ] && echo "$conf_file is not exist" && exit 1;

. $conf_file

logpath=$HOME/bin/log
logroot=${logroot:-/data}

##�����־·���Ƿ����
if [ ! -d "$logpath" ]; then
	echo "$logpath is not exist"
	exit 1;
fi

#�����-d �����ж�$bakpath�Ƿ����
if [ ! -d "$logroot" ]; then
	mkdir "$logroot"
	if [ $? -ne 0 ]; then
		echo "mkdir $logroot fail"
		exit 1;
	fi
fi

##��zip����־�ļ�������ݵ�ָ���ļ���
echo "`date '+%Y/%m/%d %k:%M:%S'` backup log to $logroot/log_$bakdate.zip BEGIN..."

find  $logpath -name "*$bakdate*" | xargs zip -r $logroot/log_$bakdate.zip

if [ $? -ne 0 ]; then
	echo 'cmd=find  $logpath -name "*$bakdate*" | xargs zip -r $logroot/log_$bakdate.zip'
	echo "`date '+%Y/%m/%d %k:%M:%S'` backup log $logpath $bakdate to $logroot FAIL..."
	exit 1;
fi

echo "`date '+%Y/%m/%d %k:%M:%S'` backup log to $logroot/log_$bakdate.zip OK..."

exit 0
