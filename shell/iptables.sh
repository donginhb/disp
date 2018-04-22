#!/bin/bash
PATH=/bin:/sbin:/usr/bin:/usr/sbin:/usr/local/bin:/usr/local/sbin
export PATH

echo "============================iptables configure============================================"
# Only support CentOS system
# ��ȡSSH�˿�
if grep "^Port" /etc/ssh/sshd_config>/dev/null;then
	sshdport=`grep "^Port" /etc/ssh/sshd_config | sed "s/Port\s//g" `
else
	sshdport=22
fi
# ��ȡDNS������IP
if [ -s /etc/resolv.conf ];then
	nameserver1=`cat /etc/resolv.conf |grep nameserver |awk 'NR==1{print $2 }'`
	nameserver2=`cat /etc/resolv.conf |grep nameserver |awk 'NR==2{print $2 }'`
fi
IPT="/sbin/iptables"
# ɾ�����й���
$IPT --delete-chain
$IPT --flush

# ��ֹ��,�����,����ػ�����
$IPT -P INPUT DROP   
$IPT -P FORWARD DROP 
$IPT -P OUTPUT ACCEPT
$IPT -A INPUT -i lo -j ACCEPT

# �����ѽ����Ļ�������ӵ�ͨ��
$IPT -A INPUT -m state --state RELATED,ESTABLISHED -j ACCEPT
$IPT -A OUTPUT -m state --state RELATED,ESTABLISHED -j ACCEPT

# ����mysql�˿ڵ���IP�����������Ϊ10
$IPT -I INPUT -p tcp --dport 8036 -m connlimit --connlimit-above 10 -j DROP

# �������޵ļ���XX022-ssh, 889X-Ӧ�� 8806 mysql �˿ڵ�����
$IPT -A INPUT -p tcp -m tcp --dport 10022 -j ACCEPT
$IPT -A INPUT -p tcp -m tcp --dport 11022 -j ACCEPT
$IPT -A INPUT -p tcp -m tcp --dport 12022 -j ACCEPT
$IPT -A INPUT -p tcp -m tcp --dport 8895 -j ACCEPT
$IPT -A INPUT -p tcp -m tcp --dport 8896 -j ACCEPT
$IPT -A INPUT -p tcp -m tcp --dport 8897 -j ACCEPT
$IPT -A INPUT -p tcp -m tcp --dport 8806 -j ACCEPT

# ����SSH�˿ڵ�����,�ű��Զ����Ŀǰ��SSH�˿�,����Ĭ��Ϊ22�˿�
$IPT -A INPUT -p tcp -m tcp --dport $sshdport -j ACCEPT

# ����ping
$IPT -A INPUT -p icmp -m icmp --icmp-type 8 -j ACCEPT 
$IPT -A INPUT -p icmp -m icmp --icmp-type 11 -j ACCEPT
# ����DNS
[ ! -z "$nameserver1" ] && $IPT -A OUTPUT -p udp -m udp -d $nameserver1 --dport 53 -j ACCEPT
[ ! -z "$nameserver2" ] && $IPT -A OUTPUT -p udp -m udp -d $nameserver2 --dport 53 -j ACCEPT

# �����������IPTABLES
service iptables save
service iptables restart
echo "============================iptables configure completed============================================"