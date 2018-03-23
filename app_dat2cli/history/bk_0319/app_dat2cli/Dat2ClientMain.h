#ifndef _DAT2CLIENT_H_
#define _DAT2CLIENT_H_

//#include <ace/OS.h>
//#include <ace/Get_Opt.h>
#include <vector>
#include <map>
//#include "diameter_parser_api.h"
#include "TcpSocket.h"

#include <sstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bind.hpp>
#include <thread>
#include <time.h>
#include<sys/time.h>
#include "CSemaphore.h"
#include "MessageQueue.h"
#include "mktdata.pb.h"

#include<signal.h>
#include "TimerController.h"

using namespace std;
using namespace boost::property_tree;

#define ERROR_TRANSPORT -1;
#define SUCC 0
#define SOCKET_DATALEN 8192
#define MAX_PATH 512

struct Privalige
{
	string m_sIp;
	string m_sUser;
	int m_iMqID;
	string password;
	set<BizCode> privl;
};
class Dat2Client
{
public:
	enum
	{
		MAX_MSG_LENGTH = 2048, MAX_MSG_BLOCK = 10
	};

public:

	Dat2Client();
	~Dat2Client();
	int run(int argc, char *argv[]); //�̳н��̿�ܵ�run����

private:

	char m_sRecvBuffer[SOCKET_DATALEN]; //���ͽ��ջ�����
	char m_sSendBuffer[SOCKET_DATALEN]; //���ͻ�����

	int m_iPid; //�շ����̼�¼�������Ӧ��PID �����̼�¼Ϊ0

	char m_sIpAddr[30];
	int m_iDebugFlag;
	int m_iSetSocketFlag;

	int m_iSemLockKey;
	char m_sUserPrivJsonPath[MAX_PATH];
	char m_sOutDispJsonPath[MAX_PATH];
	char m_sCfgJsonPath[MAX_PATH];
	int m_iPort; //�����˿�
	int m_iSocketSendLen;
	int m_iSocketRecvLen;
	int m_iSysMqKey;
	int m_iSysMqMaxLen;
	int m_iSysMqMaxNum;


	int Init();
	int diffTimeStr(char *curr_time_str, const char *last_time_str);

	int m_iCheckIp; //�Ƿ�У��IP 1-У�� 0-��У��

	//OperationLoad m_cOperationLoad;
private:
	map<string, Privalige> m_mapPrivl; //key:�û���
	unsigned int m_secHeartbeat;
	string m_sUserName;

	vector<uint32_t> m_allCodes; //���������ģ��ͬ������

	set<BizCode> m_Subscribed;

	//ÿ���ͻ�����������Ӧ�ĳ�ʱ�����
	////TimerHeartbeat m_Timer;
	int m_iMqID;
	bool m_subSomeCodes = false;
	set<uint32_t> m_codes; //ֻ������Щ֤ȯ����

	TimerController *m_pTimerController;
	boost::asio::io_service m_io;
	std::thread *m_pThread;
#ifdef DEBUG_ONE
	map<BizCode,long> m_mapBizStat;
	long m_lRecvNum;
	long m_lSendNum;
#endif

	CSemaphore *m_poDataLock;

private:
	bool isValidHost(const char *sIp); //�жϽ����Զ�������Ƿ��������ε�����IP
	int RecvCliSockRequestAndProcess();
	int RecvMqAndDisp2Cli();

	MessageQueue *m_poSysMQ; //�ܿض���
	MessageQueue *m_poDataMQ; //���ݶ���

	void dealCommand(string &msg);

	void setSubscrible(const SubscribeRequest &req);
	void addReduceCodes(const vector<uint32_t> &codes, const bool addReduce);
	int writeDispJson();
	
	void logout();
	bool isLogined(string sUserName);
	void handleTimeOut();
	void SetProcessSignal();
	void GetSysDate(char sSysDate[]);
public:
	static bool m_bReadSysMq; //�Ƿ��ȡ�ܿض���
};

#endif

