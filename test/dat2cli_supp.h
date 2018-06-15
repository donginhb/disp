#ifndef __DAT2CLI_SUPP_H__
#define __DAT2CLI_SUPP_H__

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <set>
#include <map>
#include <string>
#include <sys/stat.h>
#include <stdarg.h>

using namespace std;

#define MY_MAX_PATH 512

#include "mktdata.pb.h"


typedef struct ServerInfoStruct
{
	uint32_t iSleep;
	uint32_t iSemLockKey;
	uint32_t iPort; //�����˿�
	uint32_t iSocketSendLen;
	uint32_t iSocketRecvLen;
	uint32_t iSysMqKey;
	uint32_t iSysMqMaxLen;
	uint32_t iSysMqMaxNum;
	uint32_t iHeartBeat;
} ServerInfo_t;

//������û�Ȩ�ޱ�
typedef struct UserAuthStruct
{
	string  strIp;			//�ͻ���IP����
	string  strUserName;		//�ͻ��˵�¼�û���
	string  strPassword;		//�ͻ��˵�¼����
	int	iMqId;			//�ͻ��˶�Ӧ��MQID
	set <int>	setAuth;	//�ͻ��˶�Ӧ��ҵ��Ȩ�޼�
} UserAuth_t;
//���ӿͻ�����Ϣ��
typedef struct ConnectClientStruct {
	char	sUserName[32];		//���ӿͻ����û���
	char	sDestIp[32];		//�ַ�����ʽ�����ӿͻ���IP
	int	iPort;			//���ӿͻ��˶˿ں�
	int	iPid;			//�ͻ��˷�����̵�pid
	string  strUserName;		//�ͻ����û���
	string	strIp;			//���ӿͻ���IP
	int	iMqId;			//�ͻ��˶�Ӧ�ĺ�̨��mqid
	set<int>	setAllSubs;	//���пɹ����ĵ�ҵ��
	set<int>	setSubscribed;	//��ǰ�Ѷ�����ҵ��
	set<int>	setSubsCode;	//��ǰ�Ѷ����Ĺ�Ʊ����
	map<int,long>	mapSubsStat;	//��ҵ��ͳ�Ʒ��ͼ�¼��
	long		lRecvCnt;	//�ܽ�����
	long		lSendCnt;	//�ܷ�����
} ConnectClient_t;

typedef std::map<string, UserAuth_t> MapUserAuth;
typedef std::map<string, ConnectClient_t> MapConnectClient;


int ReadServerInfo(char sServerInfoPath[], ServerInfo_t &Info);
//��user_privilege.json�ļ�����map�ṹ��
int ReadUserAuth2Map(char sUserAuthPath[], MapUserAuth &mapUserAuth);

//��bizCode��pbmsg��Ϣ�����Ϊ�� ��ʽ��2�ֽڳ���+1�ֽ�bizCode+pb->SerializeToString���������Ϊ 1�ֽ�+���л���
void addBizcode(string &output, const google::protobuf::Message &pbmsg, BizCode bizCode);
//��msgInput�ĵ�һ���ֶ�ȡ������ΪbizCode����Ĵ���Ϊpbmsg
void getBizcode(BizCode &bizCode, string &pbmsg, const string &msgInput);
void MyBin2HexStr(char *buf,int len,char sTemp[]);
int GetHostTimeX(char sHostTime[15],char sMiniSec[4]);
void PrintHexBuf(char *buf, int size);

bool LogDispJson(char sInfo[],char sMsg[],char sDispPath[],char sDispLog[],char sErrInfo[]);
int UserInWriteUser(char sUserName[],char sWriteUser[]);
int MyWrite2CliFile(char sWorkRoot[],char sUserName[],string &str);

long lFileSize(char sFileName[]);//��ȡ�ļ���Ϊfilename���ļ���С��
time_t tFileModifyTime(char sFileName[]);

void printf_dt(const char *fmt,...);

extern int (*Write2CliFile)(char sWorkRoot[],char sUserName[],string &str);


#endif
