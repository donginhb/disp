#!/bin/bash

#1.����Ƿ��н��̣��������澯�� 8:50�ּ��;			check_app_stop
#2.���ϵͳ��������־����9:01�ּ�飬���û�������������ʼ���	check_app_start_log
#7.��� �����б��еĽ��� �Ƿ���ڣ�������			check_app_pid_exist
#	1��
#	c���� ���� $8 ~ ������
#	shell�ű� ���� $9 ~������
#	2��
#	$1 Ϊ���û�
#	3)
#	$3 Ϊ1
#5.��鱾�û���mq���У������еĶѻ�used-bytes���������õ��ֽ���ʱ����澯��ÿ���Ӹ澯һ��;
#	�ҵ���Ӧ��
#		dat2cli����id���Է��û�������������ʱ�� �Է�IP	check_app_mq_jam
#8.ͬʱ���ӵ��û���dat2cli����������� �������ޣ��� �澯��	check_app_conn_cnt
#3.���9:02�ֿ�ʼ������յ������ݣ��������û�յ���澯��
# 	ÿ���Ӽ��һ�� 						check_app_gen_data
#4.���д��̿ռ��飬������̿ռ䳬��85%,��澯��ÿ���Ӽ��һ�Ρ���һ�����忪��ʱ�䣻
#6.���netstat -nap ��һ�¶��У�ĳ�����̵Ľ��� ���� ���ն�����ֵ ���Ͷ��� ���� ���Ͷ�����ֵ���򣬸���
#	д�����̺ţ�������������������ʱ�䡿 ������id�������������Զ˵�IP��
#	�����dat2cli���̣��� $HOME/conf/disp.json ���ҵ���Ӧ���û�����mq���ơ�
#9.����ڴ����������ڴ�ﵽ�ܴ��ˣ���top��������� ��ǰ5���Ľ��̳�����
#	��������id ��������������id����������������������ʱ�䡿���澯������
#10.�������CPU���������ﵽĳ��ֵ����top�澯������
#	Ӳ��		4
#	�ڴ�(SWAP�ռ�)	9
#	����		6
#	CPU		10
#	Ӧ��		{1,2,7,3,5,8}

if [ $# -ne 0 -a $# -ne 2 ]; then
	echo "usage $0 or $0 or $0 type(disk mem net cpu app) cleandate(yyyymmdd)"
	exit 1
fi

conf_file="$HOME/src/conf/checksys.ini"
[ ! -f $conf_file ] && echo "$conf_file is not exist" && exit 1;

. $conf_file

commons_file="$HOME/src/shell/commons"
[ ! -f $commons_file ] && echo "$commons_file is not exist" && exit 1;

. $commons_file


mytype=""
myparam=""

if [ $# -eq 0 ];then
	moniall=${moniall:-"disk:85,mem:81"}

	##ɾ����","���ַ�������м����ֶ�
	##��д��
	#tmpstr=${moniall//[^,]/}
	tmpstr=`echo $moniall | sed 's/[^,]//g'`

	tmpstr=${tmpstr:-""}
	mylen=$((${#tmpstr}+1))

	for((i=1;i<=mylen;i++))
	{
		tmpcmd="echo $moniall | awk -F\",\" '{print \$$i}'"

#		#echo cmd=$tmpcmd

		tmp=`eval $tmpcmd`

		mytype=`echo $tmp | awk -F":" '{print $1}'`
		myparam=`echo $tmp | awk -F":" '{print $2}'`

#		echo "tmp=$tmp mytype=$mytype myparam=$myparam 0=$0"

		tmpstr=`$0 $mytype $myparam`

		if [ $? -eq 0 ]; then
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` OK GOOD $tmpstr"
		else
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` FOUND ABNORMAL $tmpstr"
		fi
	}
	exit 0;
fi



mytype=$1
myparam=$2

check_disk()
{
	mypercent=$1

#	dfcmd="df -h | sed 's/\%//g' | awk '\$5>$mypercent&&\$5<100'"

	#���ﹲ�õĲ�����������ˣ�����95%�����Ի�����Ҫ�����ϸ��أ�����85%
	dfcmd="df -h | sed -e '1d;s/\%//g'"

	IFS_old=$IFS
	IFS=$'\n'
	for dfstr in `eval $dfcmd`
	do
		mytol=`echo $dfstr | awk '{print $2}'`
		myleft=`echo $dfstr | awk '{print $4}'`
		myper=`echo $dfstr | awk '{print $5}'`
		mydir=`echo $dfstr | awk '{print $6}'`
		max_per_var_name="max_per"$mydir

#echo "-------$max_per_var_name new hello world"

		#��Ŀ¼б���滻Ϊ�»��ߣ�ת��Ϊ������
		max_per_var_name=`echo $max_per_var_name | sed 's/\\//_/g'`
		#������������ĸ�ֵ���
		assigncmd="$max_per_var_name=\${$max_per_var_name:-$mypercent}"
		#ִ�������ֵ��������������и���
		eval $assigncmd
		#���ɴ�ӡ������ֵ����䣬���ں��������ñ���ֵ
		prtvalcmd="echo \$$max_per_var_name"

#echo "-------$max_per_var_name new hello world val=`eval $prtvalcmd`------"

		##�����ǰʹ�ÿռ䣬С�������õ�ר�ÿռ�������ֵ������Ե�ǰ��¼
		if [ $myper -lt `eval $prtvalcmd` ];then
			continue;
		fi

		echo "`date '+%Y/%m/%d %k:%M:%S.%N'` DIR=$mydir OVERLOAD percent d=$myper(`eval $prtvalcmd`) TOTAL=$mytol LEFT=$myleft"
	done
	IFS=$IFS_old
}
check_disk1()
{
	echo "hello world"
}
check_net1()
{
	echo "hello world"
}
check_net()
{
	net_max_sendq=${net_max_sendq:-10000}
	net_max_recvq=${net_max_recvq:-10000}

	IFS_old=$IFS
	IFS=$'\n'
	#ȡtcp��udpЭ�飬���ͻ���ն��з�0��״̬Ϊ������״̬������ͳ����Ϣ
	for netstr in `netstat -nap 2>/dev/null| awk '($1~"tcp"||$1~"udp")&&($2>0||$3>0)&&$6=="ESTABLISHED"'`
	do
		myrecvq=`echo $netstr | awk '{print $2}'`
		mysendq=`echo $netstr | awk '{print $3}'`

#		echo "$mysendq($net_max_sendq) $myrecvq($net_max_recvq) netstr=$netstr"

##����������ж�С��ָ������ֵ���������˼�¼
		if [ $mysendq -lt $net_max_sendq -a $myrecvq -lt $net_max_recvq ];then
#			echo $netstr
			continue;
		fi
##��ʾ�澯����
		mymsg=""
		if [ $mysendq -ge $net_max_sendq ];then
			mymsg="SENDQ=$mysendq"
		fi
		if [ $myrecvq -ge $net_max_recvq ];then
			if [ ${#mymsg} -lt 0 ];then
				mymsg="$mymsg RECVQ=$myrecvq"
			else
				mymsg="RECVQ=$myrecvq"
			fi
		fi

		myopp=`echo $netstr | awk '{print $5}'`
		myapp=`echo $netstr | awk '{print $7}'`
		mypid=`echo $myapp | sed 's/[^0-9]//g'`

#		if [ $myopp = "220.200.58.245:18336" ]; then
#			echo "$mysendq($net_max_sendq) $myrecvq($net_max_recvq) netstr1=$netstr"
#		fi

##����ҵ���pid���򽫽��̵���ϸ��Ϣ����
		if [ -n "$mypid" ];then
			myuser=`belong_user_mypid $mypid`
			mycmd=`belong_cmd_mypid $mypid`
			myppid=`belong_ppid_mypid $mypid`
			mystime=`belong_stime_mypid $mypid`

			#���������dat2cli���̣���ͨ�����ӽ����ҵ���Ӧ�Ŀͻ����˻���CLI��MQ
			if [ `basename $mycmd` -eq "dat2cli" ];then

				disp_file="$HOME/conf/disp.json"
				[ ! -f $disp_file ] && echo "$disp_file is not exist" && exit 1;

				user_list=`cat $disp_file | awk -F":" '$1~"user"&&$1!~"users"{print $2}'`
				user_list=`echo $user_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

				mqid_list=`cat $disp_file | awk -F":" '$1~"mqid"{print $2}'`
				mqid_list=`echo $mqid_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

				pid_list=`cat $disp_file | awk -F":" '$1~"pid"{print $2}'`
				pid_list=`echo $pid_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

				j=1
				for pidstr in `echo $pid_list`
				do
					new_ppidcmd="ps -ef | awk '\$2==$pidstr{print $3}'"
					newppid=`eval $new_ppidcmd`
					if [ $newppid -eq $mypid ];then

						new_usercmd="echo $user_list | awk '{print \$$j}'"
						new_mqidcmd="echo $mqid_list | awk '{print \$$j}'"

						mymsg="$mymsg CLI=`eval new_usercmd` MQ=`eval new_mqidcmd`"
						break;
					fi
					((j++))
				done
			fi

			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` OPP=$myopp ALERT $mymsg USR=$myuser APP=$mycmd PID=$mypid PPID=$myppid STIME=$mystime"
		else
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` OPP=$myopp ALERT $mymsg"
		fi

	done
	IFS=$IFS_old
}

check_cpu()
{
	myused=$1
	myidle=$((100-$myused))

	#������ʾ�ĵ�һ��cpuռ��ͳ�Ʋ�׼ȷ��ȡ�ڶ���ͳ��
	#�����ʵ��ʹ�ý���ʵ��ռ�õ�cpu������λС������ȡ��
	#tmpidle=`top -bn2 | grep Cpu | tail -1 | awk '{printf "%0.0f",$8+0.49}'`
	#�����idle��ֵ����Ҫ����ȡ����ֱ�����½ضϣ�ȷ��used��ֵ����ȡ��OK
	tmpidle=`top -bn2 | grep Cpu | tail -1 | awk '{printf "%0.0f",$8}'`
	tmpused=$((100-$tmpidle))

	if [ $tmpused -ge $myused ];then
		mymsg="CPU OVERLOAD percent c=$((100-$tmpidle))($myused)"
##�ҵ�����3������
		IFS_old=$IFS
		IFS=$'\n'
		for cpustr in `top -bn1 |head -20 | sed -n '/COMMAND/,+3p' | grep -v "COMMAND"`
		do
			mypid=`echo $cpustr | awk '{print $1}'`
			mymem=`echo $cpustr | awk '{print $6}'`
			mycpu=`echo $cpustr | awk '{print $9}'`
			myusr=`echo $cpustr | awk '{print $2}'`
			mycmd=`echo $cpustr | awk '{print $12}'`

			#���һ�½����Ƿ���ڣ��������������Խ���
			pidexistcmd="ps -ef | awk '\$2==$mypid' | wc -l"
			if [ `eval $pidexistcmd` -eq 0 ];then
				continue;
			fi

			myppid=`belong_ppid_mypid $mypid`
			mystime=`belong_stime_mypid $mypid`

			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` CPU=$mycpu ALERT $mymsg MEM=$mymem USR=$myuser APP=$mycmd PID=$mypid PPID=$myppid STIME=$mystime"
		done
		IFS=$IFS_old
	fi
}
check_mem()
{
	mypercent=$1

	mem_max_used_per=${mem_max_used_per:-85}
	swap_max_used_per=${swap_max_used_per:-95}

	memstr=`free -m | grep Mem`
	swapstr=`free -m | grep Swap`

	memtol=`echo $memstr | awk '{print $2}'`
	memavail=`echo $memstr | awk '{print $7}'`
	swaptol=`echo $swapstr | awk '{print $2}'`
	swapavail=`echo $swapstr | awk '{print $4}'`

	memper=$((100-(memavail*100/memtol)))
	swapper=$((100-(swapavail*100/swaptol)))

#	echo "`date '+%Y/%m/%d %k:%M:%S.%N'` meminfo=$memper $swapper $mem_max_used_per $swap_max_used_per"

	if [ $memper -ge $mem_max_used_per -o $swapper -ge $swap_max_used_per ];then
		if [ $memper -ge $mem_max_used_per ];then
			mymsg="MEM OVERLOAD percent m=$memper($mem_max_used_per) s=$swapper($swap_max_used_per)"
		else
			mymsg="SWAP OVERLOAD percent m=$swapper($swap_max_used_per) m=$memper($mem_max_used_per)"
		fi
##�ҵ�����5������
		IFS_old=$IFS
		IFS=$'\n'
		for tmpstr in `top -bn1 |head -20 | sed -n '/COMMAND/,+5p' | grep -v "COMMAND"`
		do
#			echo tmpstr=$tmpstr
			mypid=`echo $tmpstr | awk '{print $1}'`
			mymem=`echo $tmpstr | awk '{print $6}'`
			mycpu=`echo $tmpstr | awk '{print $9}'`
			myusr=`echo $tmpstr | awk '{print $2}'`
			mycmd=`echo $tmpstr | awk '{print $12}'`

			#���һ�½����Ƿ���ڣ��������������Խ���
			pidexistcmd="ps -ef | awk '\$2==$mypid' | wc -l"
			if [ `eval $pidexistcmd` -eq 0 ];then
				continue;
			fi

			myppid=`belong_ppid_mypid $mypid`
			mystime=`belong_stime_mypid $mypid`

			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` MEM=$mymem ALERT $mymsg CPU=$mycpu USR=$myuser APP=$mycmd PID=$mypid PPID=$myppid STIME=$mystime"
		done
		IFS=$IFS_old
	fi
}
#myparam=stop
check_app_stop1()
{
	echo "hello world"
}
check_app_stop()
{
	app_list=${app_list:-"intf_tdf,dat2cli,moni.sh"}
	app_list=`echo $app_list | sed 's/,/ /g'`

	mypids=`$pidof_bin -x $app_list`
	mypid=`check_mypid_exist $mypids`

	if [ $mypid -ne 0 ];then
		belong_user=`belong_user_mypid $mypid`
		belong_cmd=`belong_cmd_mypid $mypid`
		echo "`date '+%Y/%m/%d %k:%M:%S.%N'` pid $mypid cmd=$belong_cmd user=$belong_user is running"
	fi
}
#myparam=startlog
check_app_start_log1()
{
	echo "start_log demo"
}
check_app_start_log()
{
	start_log=${start_log:-"startup.log"}
	base_log=`basename $start_log`

	if [ $start_log = $base_log ];then
		start_log="$HOME/bin/log/$base_log"
	fi
	[ ! -f $start_log ] && echo "$start_log is not exist" && exit 1;

	okcnt=`tail -1 $start_log | grep "system" | grep "startup" | grep "OK" | wc -l`

#	echo $okcnt $start_log
	#���û���ҵ������ɹ���¼
	if [ $okcnt -eq 0 ];then
		bgnline=`tail -20 $start_log | awk '$0~"BEGIN"{print NR}' | tail -1`
		bgnline=${bgnline:-0}
#		echo "$okcnt $bgnline hello world"

		if [ $bgnline -ne 0 ];then
			##����BEGIN����
			((bgnline++))
			sedcmd="tail -20 $start_log | sed -n '$bgnline,\$p'"

#			echo "$okcnt $bgnline $sedcmd"
			eval $sedcmd
		fi
	fi
}
#myparam=pidexist
check_app_pid_exist1()
{
	echo "hello world"
}
check_app_pid_exist()
{
	app_list=${app_list:-"intf_tdf,dat2cli,moni.sh"}
	app_list=`echo $app_list | sed 's/,/ /g'`

	my_name=`whoami`
	my_name=${my_name:-$USER}

	for appstr in `echo $app_list`
	do
		##����ǽű���ȡ�ھŸ�����������ȡ�ڰ˸�����
		if [ $appstr != ${appstr//.sh/} ];then
			pscmd="ps -ef | awk '\$1==\"$my_name\"&&\$3==1&&\$9~\"$appstr\"' | wc -l"
		else
			pscmd="ps -ef | awk '\$1==\"$my_name\"&&\$3==1&&\$8~\"$appstr\"' | wc -l"
		fi

		runcnt=`eval $pscmd`

		if [ $runcnt -eq 0 ];then
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` APP=$appstr USR=$my_name PPID=1 NOT EXIST"
		elif [ $runcnt -ne 1 ];then
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` APP=$appstr USR=$my_name PPID=1 HAVE $runcnt INSTANCE"
		else	:
		fi
#		echo app_name=$appstr app1=${appstr//.sh/} pscmd=$pscmd

	done
}
#myparam=mqjam
check_app_mq_jam1()
{
	echo "hello world"
}
check_app_mq_jam()
{
	max_mq_mb=${max_mq_mb:-10}

	my_name=`whoami`
	my_name=${my_name:-$USER}

	IFS_old=$IFS
	IFS=$'\n'

	ipcscmd="ipcs -q | awk '\$3==\"$my_name\"'"

	#ȡ������ǰ�û�����Ϣ������Ϣ
	for ipcsstr in `eval $ipcscmd`
	do
		mybytes=`echo $ipcsstr | awk '{print $5}'`
		myMb=$(($mybytes/(1024*1024)))

		#Ϊ��ģ�����ݣ�����һ���������
		#�����������ȡ���������0��ͷ�����֣�%N�ڰ���8��9�������򱨸����ƴ���
		#value too great for base (error token is "045807647")
#		myMb=$((`date +%N`%30))
#		echo "$myMb($max_mq_mb) ipcsstr=$ipcsstr"

		##���MQ���ж�С��ָ������ֵ���������˼�¼
		if [ $myMb -lt $max_mq_mb ];then
			continue;
		fi

		disp_file="$HOME/conf/disp.json"
		[ ! -f $disp_file ] && echo "$disp_file is not exist" && exit 1;

		##�õ�MQID����ǰ���ַ���ȥ��
		mymqid=`echo $ipcsstr | awk '{print $1}' | sed 's/^0x//;s/^0*//g'`

		##����ֶ�ôʲô�ã�Ҫȥ��������Ҳ��Ҫ��¼��̨ͨ��key�ҵ����ID����
##		mymsqid=`echo $ipcsstr | awk '{print $2}'`

		user_list=`cat $disp_file | awk -F":" '$1~"user"&&$1!~"users"{print $2}'`
		user_list=`echo $user_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

		mqid_list=`cat $disp_file | awk -F":" '$1~"mqid"{print $2}'`
		mqid_list=`echo $mqid_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

		pid_list=`cat $disp_file | awk -F":" '$1~"pid"{print $2}'`
		pid_list=`echo $pid_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

#		pricmd="echo $mqid_list"

		mymsg=""
		j=1

		#���ѭ����IFS�޸�Ϊ�� $'\n' ������Ҫ��ʱΪ�ڲ�ѭ�����ÿո�Ϊ�ָ���
		IFS_pre=$IFS
		IFS=$' '
		for mqidstr in `echo $mqid_list`
		do
#			echo "mymqid=$mymqid mqidstr=$mqidstr"

			##�������mqid�ҵ������cliname��mypid�����ȡ�������Ϣ
			if [ $mymqid -eq $mqidstr ];then

				new_usercmd="echo $user_list | awk '{print \$$j}'"
				new_pidcmd="echo $pid_list | awk '{print \$$j}'"

				mypid=`eval $new_pidcmd`
				myppid=`belong_ppid_mypid $mypid`

				netcmd="netstat -nap 2>/dev/null| grep ^tcp | grep ESTABLISHED| awk '\$7~\"dat2cli\"&&\$7~\"$myppid\"'"
				myopp=`eval $netcmd | awk '{print $5}'`
				mystime=`belong_stime_mypid $myppid`

				mymsg="CLI=`eval $new_usercmd` ADDR=$myopp CTIME=$mystime PID=($mypid,$myppid)"
				break;
			fi
			((j++))
		done

		IFS=$IFS_pre

		echo "`date '+%Y/%m/%d %k:%M:%S.%N'` MQ OVERLOAD MQ=$mymqid JAM(Mb)=$myMb($max_mq_mb) $mymsg"

	done
}
#myparam=conncnt
check_app_conn_cnt1()
{
	echo "hello world"
}
check_app_conn_cnt()
{
	#Ĭ��֧��20������
	max_conn_cnt=${max_conn_cnt:-20}

	my_name=`whoami`
	my_name=${my_name:-$USER}

	pscmd="ps -ef | awk '\$1==\"$my_name\"&&\$3!=1&&\$8~\"dat2cli\"' | wc -l"

	conncnt=$((`eval $pscmd`/2))

	if [ $conncnt -gt $max_conn_cnt ];then

		echo "`date '+%Y/%m/%d %k:%M:%S.%N'` APP CLICNT OVERLOAD c=$conncnt($max_conn_cnt)"

		disp_file="$HOME/conf/disp.json"
		[ ! -f $disp_file ] && echo "$disp_file is not exist" && exit 1;

		#�����滻����sed��Ч
		#user_list=`cat $disp_file | awk -F":" '$1~"user"&&$1!~"users"{print $2}'`
		#echo user_list0=$user_list
		#user_list=${user_list//\"/}
		#echo user_list1=$user_list
		#user_list=${user_list//\\n/}
		#echo user_list2=$user_list
		#user_list=${user_list//,/ }
		#echo user_list3=$user_list

		#����sed�滻
		user_list=`cat $disp_file | awk -F":" '$1~"user"&&$1!~"users"{print $2}'`
		user_list=`echo $user_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

		mqid_list=`cat $disp_file | awk -F":" '$1~"mqid"{print $2}'`
		mqid_list=`echo $mqid_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

		pid_list=`cat $disp_file | awk -F":" '$1~"pid"{print $2}'`
		pid_list=`echo $pid_list | sed 's/[\"\n\r ]//g;s/,$//;s/,/ /g'`

#		echo "user=$user_list,mqid=$mqid_list,pid=$pid_list. conncnt=$conncnt newcnt=$newcnt"

		newcnt=`echo $user_list | awk '{print NF}'`
		if [ $conncnt -eq $newcnt ];then
			for((j=1;j<=newcnt;j++)){
				awk_usercmd="echo $user_list | awk '{print \$$j}'"
				awk_mqidcmd="echo $mqid_list | awk '{print \$$j}'"
				awk_pidcmd="echo $pid_list | awk '{print \$$j}'"

				mycli=`eval $awk_usercmd`
				mymqid=`eval $awk_mqidcmd`
				mypid=`eval $awk_pidcmd`
				myppid=`belong_ppid_mypid $mypid`

				netcmd="netstat -nap 2>/dev/null| grep ^tcp | grep ESTABLISHED| awk '\$7~\"dat2cli\"&&\$7~\"$myppid\"'"
				myopp=`eval $netcmd | awk '{print $5}'`
				mystime=`belong_stime_mypid $myppid`

#				echo "j=$j cli=$mycli mqid=$mymqid pid=$mypid ppid=$myppid myopp=$myopp netcmd=$netcmd"
#				echo "`date '+%Y/%m/%d %k:%M:%S.%N'` APP CLICNT OVERLOAD c=$conncnt($max_conn_cnt)"
				echo "`date '+%Y/%m/%d %k:%M:%S.%N'` CLI$j USER=$mycli ADDR=$myopp CTIME=$mystime MQ=$mymqid PID=($mypid,$myppid)"
			}
		fi
	fi
#	echo "hello world conncnt=$conncnt max_conn_cnt=$max_conn_cnt pscmd=$pscmd"
}
#myparam=gendata
check_app_gen_data1()
{
	echo "hello world"
}
check_app_gen_data()
{
	moni_serv_list=${moni_serv_list:-"192.168.1.224:8896:moni_user:moni!%#"}
	moni_serv_list=${moni_serv_list//,/ }

	agentcli_t="$HOME/bin/agentcli_t"
	[ ! -f $agentcli_t ] && echo "$agentcli_t is not exist" && exit 1;

	for servstr in `echo $moni_serv_list`
	do
		myip=`echo $servstr | awk -F":" '{print $1}'`
		myport=`echo $servstr | awk -F":" '{print $2}'`
		myuser=`echo $servstr | awk -F":" '{print $3}'`
		mypass=`echo $servstr | awk -F":" '{print $4}'`

#		echo "IP=$myip PORT=$myport CLI=$myuser PASS=$mypass servstr=$servstr"

		gen_rec_cnt=${gen_rec_cnt:-1000}
		gen_sleep_sec=${gen_sleep_sec:-2}
		gen_time_out_sec=${gen_time_out_sec:-4}

		tmpcmd="$agentcli_t -I$myip -P$myport -u$myuser -p$mypass -c$gen_rec_cnt -s$gen_sleep_sec -m$gen_time_out_sec"
		tmpstr=`eval $tmpcmd`

		if [ $? -ne 0 ];then
			cmdinfo=`echo $tmpstr | tail -l`
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` APP IP=$myip PORT=$myport NO-DATA INFO=$cmdinfo"
			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` cmd=$tmpcmd"
		else
			:
#			echo "`date '+%Y/%m/%d %k:%M:%S.%N'` CMD=$tmpcmd"
		fi

	done
}

case $mytype in
disk)
	echo "check DISK $myparam"
#	check_disk1 $myparam
	check_res=`check_disk $myparam`
;;
mem)
	echo "check MEM $myparam"
	check_res=`check_mem`
;;
net)
	echo "check NET $myparam"
#	check_net1
	check_res=`check_net`
;;
cpu)
	echo "check CPU $myparam"
	check_res=`check_cpu $myparam`
;;
app)
	echo "check APP $myparam"

	pidof_bin="/usr/sbin/pidof"
	[ ! -f $pidof_bin ] && echo "$pidof_bin is not exist" && exit 1;

	case $myparam in
	stop)
#		check_app_stop1
		check_res=`check_app_stop`
	;;
	startlog)
#		check_app_start_log1
		check_res=`check_app_start_log`
	;;
	pidexist)
#		check_app_pid_exist1
		check_res=`check_app_pid_exist`
	;;
	mqjam)
#		check_app_mq_jam1
		check_res=`check_app_mq_jam`
	;;
	conncnt)
#		check_app_conn_cnt1
		check_res=`check_app_conn_cnt`
	;;
	gendata)
#		check_app_gen_data1
		check_res=`check_app_gen_data`
	;;
	*)
		echo "mytype=app param=$myparam ERROR must in [stop,startlog,pidexist,gendata,mqjam,mqjam,conncnt]"
	;;
	esac
;;
*)
	echo "$mytype ERROR must in [disk,mem,net,cpu,app]"
	exit 1
;;
esac

check_res=${check_res:-"0"}

#echo "check_res=$check_res type=$mytype"

if [ ${#check_res} -gt 1 ];then
	echo "$check_res"
	exit 1;
fi

exit 0
