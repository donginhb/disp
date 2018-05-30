#!/bin/bash

pidof_bin="/usr/sbin/pidof"

[ ! -f $pidof_bin ] && echo "$pidof_bin is not exist" && exit 1;

export TERM=${TERM:-xterm}

ethn=$1

pid_str=""
pnd_str="ints_gta|ints_tdf|replay_gta|replay_tdf|agentcli|dat2cli|COMMAND"

#����ֻȡǰ20������ID��
#����top: pid limit (20) exceeded ����
get_pid_str()
{
	my_str="`$pidof_bin ints_gta ints_tdf replay_gta replay_tdf agentcli dat2cli`"
	
	j=0
	new_str=""

	for i in $my_str
	do
		if [ $j -eq 0 ];then
			new_str="$i"
		else
			new_str="$new_str $i"
		fi

		j=`echo $j+1|bc`

		if [ $j -ge 20 ];then
			break;
		fi	
	done
	
	new_str=${new_str:-0}
	
	new_str=`echo $new_str | sed 's/[0-9]*/-p&/g'`
	echo "$new_str"
}

RX_cmd="cat /proc/net/dev |  awk '\$1==\"$ethn:\"{print \$2}'"
TX_cmd="cat /proc/net/dev |  awk '\$1==\"$ethn:\"{print \$10}'"

while true
do
	##09��15��֮ǰ��15��15��֮��11��45�ֵ�12��45�ֲ����
	my_date=`date "+%k%M%S"`
	if [ $my_date -gt "153000" ] ||
		[ "$my_date" -lt "090000" ] ||
		[ "$my_date" -gt "114500" -a "$my_date" -lt "124500" ]; then
		echo -e "`date '+%Y/%m/%d %k:%M:%S'` this time no monitor"
		sleep 30
		continue;
	fi

	RX_pre=$(eval $RX_cmd)
	TX_pre=$(eval $TX_cmd)
	sleep 1
	RX_next=$(eval $RX_cmd)
	TX_next=$(eval $TX_cmd)

	RX=$((${RX_next}-${RX_pre}))
	TX=$((${TX_next}-${TX_pre}))

	if [[ $RX -lt 1024 ]];then
		RX="${RX}B/s"
	elif [[ $RX -gt 1048576 ]];then
		RX=$(echo $RX | awk '{print $1/1048576 "MB/s"}')
	else
		RX=$(echo $RX | awk '{print $1/1024 "KB/s"}')
	fi

	if [[ $TX -lt 1024 ]];then
		TX="${TX}B/s"
	elif [[ $TX -gt 1048576 ]];then
		TX=$(echo $TX | awk '{print $1/1048576 "MB/s"}')
	else
		TX=$(echo $TX | awk '{print $1/1024 "KB/s"}')
	fi

	echo -e " `date '+%Y/%m/%d %k:%M:%S'`	RX=$RX \t TX=$TX"

	sudo netstat -nap | egrep "ints_gta|ints_tdf|dat2cli|Address"
	
	##���ӽ���ռ�õ�cpu���

	pid_str=`get_pid_str`
	top -b -n1 -H `echo $pid_str` | egrep `echo $pnd_str` | head -12
	##������Ϣ���м��
	ipcs -q
done
