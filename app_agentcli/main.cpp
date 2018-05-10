// agentcli : ������:һ��dat2cli -> agentcli ->����dat2cli
//���ܣ�agentcli�����Կͻ�����ݽ���һ�������ȡ���ĵ����е�ҵ����Ϣ�������ݶ��������Ͽͻ��˵Ľ������ת�����ݵ����������Ŀͻ����ϡ�
//ʹ��˵����һ��������������agentcli�����û���������Ϣ���뱾����(���-pָ����cfg.json)ע����û�������һ����������������롣
#include <exception>
#include <iostream>
#include <stdio.h>
#include <atomic>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>
#include <vector>
#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include<signal.h>
#include <thread>
#include <time.h>
#include "MessageQueue.h"
#include "marketInterface.h"
#include "Client.h"
#include "domain2ipPort.h"
#include "CSemaphore.h"

#include "wwtiny.h"

#include "callsupp.h"


using namespace std;
using namespace boost::property_tree;
using namespace boost;

IpDomain g_strServer; //һ��������IP
unsigned int g_iPort; //һ������������˿�
string g_strPassword; //��һ��ע������
string g_strUsername; //��һ��ע���û���
int st_interface;

string g_sMoniDispFilePath; //����disp.json�ļ�·�������ڼ��·�ɱ仯��Ϣ
CSemaphore *g_poDataLock = NULL; //��ֹdisp.json���ڶ�д������
int g_iSemLockKey = 0;
bool g_locked = false;
int g_iSysMqMaxLen = 512;
int g_iSysMqMaxNum = 15000;
vector<MessageQueue*> g_vMqList;//���Ͷ����б�

void GetSysDate(char sSysDate[])
{
	GetHostTime(sSysDate);
}

//���̳�ʼ������
bool init(const char *cfgfile,char sDispFile[])
{
	//logInit("asio_client");
	boost::property_tree::ptree tree;

	boost::property_tree::read_json(cfgfile, tree);
	g_strServer.ip = tree.get < string > ("server");
	g_iPort = tree.get<unsigned>("port");
	g_strUsername = tree.get < string > ("username");
	g_strPassword = tree.get < string > ("password");
	st_interface = tree.get("interface", 1);

	g_sMoniDispFilePath = tree.get < string > ("MonitorDispJsonFile");
	g_iSemLockKey = tree.get<int>("SemLockKey");
	g_iSysMqMaxLen = tree.get<int>("SysMqMaxLen");
	g_iSysMqMaxNum = tree.get<int>("SysMqMaxNum");
	
	if(strlen(sDispFile)>0)
		g_sMoniDispFilePath=string(sDispFile);

	if (g_strServer.ip[0] <= '0' || g_strServer.ip[0] > '9')
	{
		g_strServer.fromDomain = true;
		auto endp = domain2ipPort::trans(g_strServer.ip, g_iPort);
		g_strServer.ip = endp->address().to_string();
	}
	if (!g_poDataLock)
	{
		char sSemName[MAX_PATH];
		sprintf(sSemName, "%d", g_iSemLockKey);
		g_poDataLock = new CSemaphore();
		g_poDataLock->getSem(sSemName, 1, 1);
	}

	cout << __FUNCTION__ << " complete" << endl;
	return true;
}
void signalProcess(int signal)
{
	if (signal == SIGUSR1){
		printf("signal SIGUSR1...\n");
		return;
	}
	marketInterfaceClose();
	exit(0);
}

void SetProcessSignal()
{
	// �����źŴ���
	signal(SIGQUIT, signalProcess);
	signal(SIGTERM, signalProcess);
	signal(SIGINT, signalProcess);
	signal(SIGSEGV, signalProcess);
	signal(SIGILL, signalProcess);
	signal(SIGABRT, signalProcess);
	signal(SIGFPE, signalProcess);
	signal(SIGPIPE, signalProcess);
	signal(SIGUSR1, signalProcess);
}

int main(int argc, char *argv[])
{
	char m_sCfgJsonPath[MAX_PATH],m_sDispJsonPath[MAX_PATH];//���������ļ�·��
	
	strcpy(m_sCfgJsonPath,"");
	strcpy(m_sDispJsonPath,"");

	SetProcessSignal();
	for (int c; (c = getopt(argc, argv, "p:r:?:")) != EOF;)
	{
		switch (c)
		{
		case 'p':
			strcpy(m_sCfgJsonPath, optarg);
			break;
		case 'r':
			strcpy(m_sDispJsonPath, optarg);
			break;
		case '?':
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-p cfg-path ]\n");
			printf("   [-r disp-json-path ]\n");
			exit(1);
			break;
		}
	}

	init(m_sCfgJsonPath,m_sDispJsonPath);

	char errmsg[256];
	bool connected = m_connect(errmsg);
	if (connected)
	{
		cerr << __FUNCTION__ << ' ' << errmsg << endl;
		return 1;
	}
	bool blogin = login(errmsg);
	if (blogin)
	{
		cerr << __FUNCTION__ << ' ' << errmsg << endl;
		return 1;
	}
	//ȫ������
	subscribe(true, true, true, true);

	WatchFileCloseWriteAndLock((char*)g_sMoniDispFilePath.c_str());

	if (g_poDataLock)
	{
		delete g_poDataLock;
		g_poDataLock = NULL;
	}
	marketInterfaceClose();

	return 0;
}

