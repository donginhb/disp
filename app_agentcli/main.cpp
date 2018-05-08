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

using namespace std;
using namespace boost::property_tree;
using namespace boost;

void loaddispjson();

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
/*	struct tm *tim;
	time_t ltim;
	time(&ltim);
	tim = localtime(&ltim);
	sprintf(sSysDate,"%04d%02d%02d%02d%02d%02d",tim->tm_year+1900,tim->tm_mon+1,tim->tm_mday,
	        tim->tm_hour,tim->tm_min,tim->tm_sec);
*/
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
	loaddispjson();

//	getTodayInit();
	cout << __FUNCTION__ << " complete" << endl;
	return true;
}

//����ҵ����Ϣ�������ͻ��˶���
void sendMsg2Client(string &msg)
{
#ifdef DEBUG_ONE
		static long lTotal = 0;
		static map<int,long> mapStat;
		lTotal++;
#endif
	if (g_locked)
	{
		loaddispjson();
		g_locked = false;
	}
	for (int i = 0; i < g_vMqList.size(); i++)
	{
#ifdef DEBUG_ONE
		mapStat[g_vMqList[i]->m_oriKey]++;
#endif
		g_vMqList[i]->send(msg, 0);
	}
#ifdef DEBUG_ONE
	if(lTotal % 30000 == 0)
	{
		char sSysDate[15];
		GetSysDate(sSysDate);
		cout<<"["<<sSysDate<<"] Recv Msg Num:["<<lTotal<<"]"<<endl;
		cout<<setw(10)<<"MqID"<<setw(16)<<"Recv Num"<<endl;
		cout<<"---------------------------"<<endl;
		for (int i = 0; i < g_vMqList.size(); i++)
		{
			cout<<setw(10)<<g_vMqList[i]->m_oriKey<<setw(16)<<mapStat[g_vMqList[i]->m_oriKey]<<endl;
		}
		cout<<"---------------------------"<<endl<<endl;
	}
#endif
}

//��ؿͻ��˵�½��ע��·���ļ�
int WatchFileCloseWriteAndLock(const char* sFileName)
{
	int fd, len, i;
	char buf[BUFSIZ];
	struct inotify_event *event;

	if ((fd = inotify_init()) < 0)
	{
		fprintf(stderr, "inotify_init failed\n");
		return -1;
	}

	if (inotify_add_watch(fd, sFileName, IN_CLOSE_WRITE) < 0)
	{
		fprintf(stderr, "inotify_add_watch %s failed\n", sFileName);
		return -1;
	}

	buf[sizeof(buf) - 1] = 0;
	while ((len = read(fd, buf, sizeof(buf) - 1)) > 0)
	{
#ifdef DEBUG_ONE
		char sSysDate[15] = {0};
		GetSysDate(sSysDate);
		cout << "["<<sSysDate<<"]:"<<sFileName<<" recv close event!"<<endl;
#endif
		g_locked = true;

	}
	return 0;
}

//���¼���·���ļ�--����ȫ���ķ�ʽ
void loaddispjson()
{
#ifdef DEBUG_ONE
		char sSysDate[15] = {0};
		GetSysDate(sSysDate);
		cout << "["<<sSysDate<<"]: reload disp.json!"<<endl;
#endif
	for (int i = 0; i < g_vMqList.size(); i++)
	{
		delete g_vMqList[i];
	}
	g_vMqList.clear();

	g_poDataLock->P();

	ptree root, readPt, items, userparam, itemsub, itemsubcodes, root_1;
	read_json(g_sMoniDispFilePath.c_str(), readPt);
	if (readPt.count("users"))
	{
		ptree ptChildRead = readPt.get_child("users");
		for (auto pos : ptChildRead) //��������
		{
			ptree p = pos.second;
			int iPid = p.get<int>("pid", 0);
			int iKey = p.get<int>("mqid",0);
			if (iKey != 0 && 0 == kill(iPid, 0)) //��½�����л��������д����ظ���½��Ϣ����ʱĬ��δ��ȫ��½�򲻴���
			{
				MessageQueue *pMq = new MessageQueue(iKey);
				pMq->open(false, true, g_iSysMqMaxLen, g_iSysMqMaxNum);
				g_vMqList.push_back(pMq);
			}
		}
	}
	g_poDataLock->V();
}

void signalProcess(int isignal)
{
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

	WatchFileCloseWriteAndLock(g_sMoniDispFilePath.c_str());

	if (g_poDataLock)
	{
		delete g_poDataLock;
		g_poDataLock = NULL;
	}
	for (int i = 0; i < g_vMqList.size(); i++)
	{
		delete g_vMqList[i];
	}
	g_vMqList.clear();

	marketInterfaceClose();

	return 0;
}

