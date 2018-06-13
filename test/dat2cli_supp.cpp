#include "dat2cli_supp.h"
#include <arpa/inet.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
namespace BTREE = boost::property_tree;


//��bizCode��pbmsg��Ϣ�����Ϊ�� ��ʽ��2�ֽڳ���+1�ֽ�bizCode+pb->SerializeToString���������Ϊ 1�ֽ�+���л���
void addBizcode(string &output, const google::protobuf::Message &pbmsg, BizCode bizCode)
{
	string msg;

	pbmsg.SerializeToString(&msg);

	char strBizCode[2] = { 0 };

	strBizCode[0] = bizCode;
	output = string(strBizCode) + msg;

	uint16_t networkLenBody = uint16_t(htons(output.size()));

	char sLen[3]={0};

	sLen[0] = ((char*)&networkLenBody)[0];
	sLen[1] = ((char*)&networkLenBody)[1];
	output.insert(0,sLen,2);
}

//��msgInput�ĵ�һ���ֶ�ȡ������ΪbizCode����Ĵ���Ϊpbmsg
void getBizcode(BizCode &bizCode, string &pbmsg, const string &msgInput)
{
	bizCode = BizCode((unsigned char) (msgInput[0]));
	pbmsg = msgInput.substr(1, string::npos);
}

void MyBin2HexStr(char *buf,int len,char sTemp[])
{
	int l=0;

	for(int i=0;i<len;i++){
		if(l==0)
			l+=sprintf(sTemp,"%02X",buf[i] & 0xff);
		else	l+=sprintf(sTemp+l," %02X",buf[i] & 0xff);
		if(l>=253) break;
	}
}
int GetHostTimeX(char sHostTime[15],char sMiniSec[4])
{
	struct tm *tm;
	struct timeval t;
	time_t tHostTime;

	strcpy(sMiniSec,"");

	if(gettimeofday(&t,NULL)==-1) return -1;

	tHostTime=(time_t)(t.tv_sec);
	if((tm=(struct tm*)localtime(&tHostTime))==NULL) return -1;

	if(strftime(sHostTime,15,"%Y%m%d%H%M%S",tm)==(size_t)0)	return -1;

	sprintf(sMiniSec,"%03ld",t.tv_usec/1000);

	return 0;
}
void PrintHexBuf(char *buf, int size)
{
	int i;

	printf("buf [%d] start:", size);
	for (i = 0; i < size; i++){
		if (i % 16 == 0) printf("\n");
		printf("%02X ", buf[i] & 0xff);
	}
	printf("\nbuf end\n\n");
}
long lFileSize(char sFileName[])//��ȡ�ļ���Ϊfilename���ļ���С��
{
	struct stat t;
	if (stat(sFileName, &t) != 0) return -1;
	return t.st_size;//�����ļ���С��
}
time_t tFileModifyTime(char sFileName[])
{
	struct stat t;
	if (stat(sFileName, &t) != 0) return -1;

	return t.st_mtime;
}
#define MY_MAX_JSON_LEN	16*1024
bool LogDispJson(char sInfo[],char sMsg[],char sDispPath[],char sDispLog[],char sErrInfo[])
{
	FILE *fpOut,*fpIn;
	long lSize;
	char sBuffer[MY_MAX_JSON_LEN],sLogTime[15],sMSec[4];

	strcpy(sErrInfo,"");

	lSize=lFileSize(sDispPath);
	if(lSize>=MY_MAX_JSON_LEN){
		sprintf(sErrInfo,"FILE %s LEN GT %d",sDispPath,MY_MAX_JSON_LEN);
		return false;
	}
	if((fpIn= fopen(sDispPath,"r"))==NULL){
		sprintf(sErrInfo,"OPEN %s FILE FOR READ ERROR",sDispPath);
		return false;
	}

	if(fread((void*)sBuffer,lSize,1,fpIn)!=1){
		fclose(fpIn);
		sprintf(sErrInfo,"READ %s FILE ERROR",sDispPath);
		return false;
	}
	fclose(fpIn);

	//���ļ�ĩβ��������������⽫��������д����־�ļ�
	sBuffer[lSize]=0;

	if((fpOut= fopen(sDispLog,"ab+"))==NULL){
		sprintf(sErrInfo,"OPEN %s FILE FOR WRITE ERROR",sDispLog);
		return false;
	}

	//�õ�ϵͳʱ�䣬������Ϣд��DISPLOG�ļ���
	GetHostTimeX(sLogTime,sMSec);
	fprintf(fpOut,"%s.%s INFO:%s MSG:%s\nDISPJSON=%s ::------------END-----------\n",
		sLogTime,sMSec,sInfo,sMsg,sBuffer);

	fclose(fpOut);

	return true;
}

int ReadServerInfo(char sServerInfoPath[], ServerInfo_t &Info)
{
	BTREE::ptree tRoot;

	BTREE::read_json(sServerInfoPath, tRoot);

	Info.iSleep =		tRoot.get<int>("sleep");
	Info.iSemLockKey =	tRoot.get<int>("SemLockKey");
	Info.iPort =		tRoot.get<int>("Port");
	Info.iSocketSendLen =	tRoot.get<int>("SocketSendLen");
	Info.iSocketRecvLen =	tRoot.get<int>("SocketRecvLen");
	Info.iSysMqKey =	tRoot.get<int>("SysMqID");
	Info.iSysMqMaxLen =	tRoot.get<int>("SysMqMaxLen");
	Info.iSysMqMaxNum =	tRoot.get<int>("SysMqMaxNum");
	Info.iHeartBeat =	tRoot.get<int>("heartbeat");

	return 0;
}

//��user_privilege.json�ļ�����map�ṹ��
int ReadUserAuth2Map(char sUserAuthPath[], MapUserAuth &mapUserAuth)
{
	UserAuth_t u;
	BTREE::ptree tRoot, t;

	BTREE::read_json(sUserAuthPath, tRoot);

	for (auto it = tRoot.begin(); it != tRoot.end(); ++it) {

		auto e = it->second;

		u.strIp = e.get < string >("ip");
		u.strUserName = e.get < string >("user");
		u.strPassword = e.get < string >("pswd");
		u.iMqId = e.get<int>("mqid");

		t = e.get_child("privl");
		for (auto jt = t.begin(); jt != t.end(); ++jt)
			u.setAuth.insert(jt->second.get_value<int>());

		mapUserAuth.insert({ u.strUserName, u });
	}

	return 0;
}

int UserInWriteUser(char sUserName[],char sWriteUser[])
{
	char sTmpUser[MY_MAX_PATH];
	char *pBgn,*pCur;

	//���û������б������
	if(sWriteUser[0]==0) return 0;

	strcpy(sTmpUser,sWriteUser);

	pBgn=sTmpUser;

	pCur=strchr(pBgn,':');

	while(1){
		if(pCur==NULL) break;

		*pCur=0;
		if(strcmp(sUserName,pBgn)==0) return true;

		//����':'��
		pBgn=pCur+1;
		pCur=strchr(pBgn,':');
	}

	//�ϸ���ȫƥ��ѽ
	if(strcmp(sUserName,pBgn)==0) return true;
	return false;
}
FILE *fpOut;
char sOutFileName[MY_MAX_PATH];

int MyWrite2CliFile(char sWorkRoot[],char sUserName[],string &str)
{
	//ֻ����linux�ˣ�long long ̫�ѿ���
	long lCurTime;
	static int iFirstFlag=true;

	char sHostTime[15],sMiniSec[4];

	GetHostTimeX(sHostTime,sMiniSec);

	if(iFirstFlag){
		iFirstFlag=false;
		char sTmpDate[9];
		strncpy(sTmpDate,sHostTime,8);sTmpDate[8]=0;

		sprintf(sOutFileName,"%s/%s_%s.dat",sWorkRoot,sUserName,sTmpDate);

		fpOut=	fopen(sOutFileName,"ab+");

		if(fpOut==NULL){
			printf("open file %s for write error.\n",sOutFileName);
			return -1;
		}
	}
	lCurTime=atol(sHostTime)*1000+atol(sMiniSec);

	if(fwrite((const void*)&lCurTime,sizeof(lCurTime),1,fpOut)!=1){
		printf("write file %s error pos1.\n",sOutFileName);
		return -1;
	}
	if(fwrite((const void*)str.c_str(),str.size(),1,fpOut)!=1){
		printf("write file %s error pos2.\n",sOutFileName);
		return -1;
	}

	fflush(fpOut);

	return 0;
}

void printf_dt(const char *fmt,...)
{
	va_list	argptr;
	char sMsg[8192],sHostTime[15],sMSec[4];
	
	GetHostTimeX(sHostTime,sMSec);

	va_start(argptr, fmt);
	vsprintf(sMsg,fmt, argptr);
	va_end(argptr);

	printf("%s.%s(%d) %s",sHostTime,sMSec,getpid(),sMsg);
}
