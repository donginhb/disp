#include <string>
#include <iostream>
#include <time.h>
#include <errno.h>

#include <vector>
#include <map>

#include<signal.h>
#include <time.h>
#include<sys/time.h>

#include "CSemaphore.h"
#include "MessageQueue.h"
#include "TcpSocket.h"


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>

using namespace std;
using namespace boost::property_tree;
using namespace boost;

extern int errno;
TcpSocket TCP_SOCKET; //Tcp������


#include "dat2cli_supp.h"
//#include "dat2cli_t_old.h"

//MessageQueue *MQSYS=NULL;		//���ؽ��̺�����ת������ͨѶ==ϵͳ����
//MessageQueue *MQDATA=NULL;		//����ת�����̽������ݵĶ���==���ݶ���
CSemaphore DATALOCK;			//��д������
ServerInfo_t ServerInfo;		//�������������
time_t		tLmtUserAuth;		//AUTH_PATH�ļ�������޸�ʱ��
MapUserAuth	mapUserAuth;		//�û�Ȩ���б�
ConnectClient_t	CONN;			//�ӽ��̵�ǰ����

int iExitFlag=false,MYPID=0;
char sDispPath[MY_MAX_PATH],sDispLog[MY_MAX_PATH],sUserAuthPath[MY_MAX_PATH];
char sWorkRoot[MY_MAX_PATH],sWriteUser[MY_MAX_PATH];

void *MainProcClientCmd(void *);
int MainData2Cli();

//ֱ��ȡ���ѱ��û���json��Ŀ������tNewRoot�У�д���ļ�
int WriteLogoutJson();
//����ǰCONN�ṹ����Ϣ��д��disp.json�ļ��Ķ�Ӧ�û���
void writeLoginJson();

static void signalProcess(int isignal)
{
	if (isignal == SIGUSR1){
//		iReadSysMq = true;
		return;
	}

	if (isignal == SIGINT||isignal==SIGTERM){
		iExitFlag = true;
		printf_dt("PID=%d CATCH SIG=%d.\n",MYPID,isignal);
		
	//ֻ�����ӽ��̵�����²��˳����������Ҫ���̣���ֱ����ȥexit
		if(getpid()!=MYPID) return;
	}
	exit(0);
}
static void SetProcessSignal()
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

bool ValidHost(char sIp[],MapUserAuth &map)
{
	//sIp��֤
	for (auto it = map.begin(); it != map.end(); ++it){
		if (strcmp(sIp,it->second.strIp.c_str())==0)
			return true;
	}
	return false;
}
void PrintUsage(char *argv[])
{
	printf("Usage: %s \n", argv[0]);
	printf("   [-p cfg-path ]\n");
	printf("   [-r disp-path ]\n");
	printf("   [-u user-privilegepath ]\n");
	printf("   [-o work-root-path ]\n");
	printf("   [-w write-user ]\n");
	printf("   [-c 1-Check IP;0-UnCheck IP ]\n");
	printf("   [-k 1:set socket buffer len 0: do not set socket buffer len]\n");
	printf("   [-n 0 parent process 1 child process ]\n");
}
int main(int argc, char *argv[])
{
	
	int iCheckIp=0,iSetSocketFlag=0,iPid,ret;
	char sCfgPath[MY_MAX_PATH];
	
	struct sockaddr_in sAddr;

	//��¼������ID
	MYPID=getpid();
	
	strcpy(sCfgPath,	"../conf/cfg.json");
	strcpy(sDispPath,	"../conf/disp.json");
	strcpy(sDispLog,	"../conf/disp.json.log");

	strcpy(sUserAuthPath,	"../conf/user_privilege.json");
	strcpy(sWorkRoot,	"/stock/work");
	strcpy(sWriteUser,	"");

	for (int c; (c = getopt(argc, argv, "c:p:k:r:u:o:w:?:")) != EOF;){
		switch (c)
		{
		case 'c':
			iCheckIp = atoi(optarg);
			break;
		case 'k':
			iSetSocketFlag = atoi(optarg);
			break;
		case 'p':
			strcpy(sCfgPath, optarg);
			break;
		case 'r':
			strcpy(sDispPath, optarg);
			sprintf(sDispLog,"%s.log",sDispPath);
			break;
		case 'u':
			strcpy(sUserAuthPath, optarg);
			break;
		case 'o':
			strcpy(sWorkRoot, optarg);
			break;
		case 'w':
			strcpy(sWriteUser, optarg);
			break;
		case '?':
		default:
			PrintUsage(argv);
			exit(1);
			break;
		}
	}

	if(argc==1){
		PrintUsage(argv);
		exit(1);
	}

	SetProcessSignal();
		
	if(ReadServerInfo(sCfgPath,ServerInfo)<0){
		printf_dt("��ʼ������������ʧ��. file=%s.\n",sCfgPath);
		return -1;
	}
	
	char sSemName[32];
	sprintf(sSemName, "%d", ServerInfo.iSemLockKey);
	//����Ҫ�ж��Ƿ�ɹ�
	DATALOCK.getSem(sSemName, 1, 1);

	tLmtUserAuth=tFileModifyTime(sUserAuthPath);
	if(ReadUserAuth2Map(sUserAuthPath,mapUserAuth)<0){
		printf_dt("�����û�Ȩ���ļ� %s ����.\n",sUserAuthPath);
		return -1;
	}
	
	if (signal(SIGCHLD, SIG_DFL) != SIG_ERR)
		signal(SIGCHLD, SIG_IGN);

	CONN.iPid=	0;
	CONN.iMqId=	0;
	CONN.lRecvCnt=	0;
	CONN.lSendCnt=	0;

	while (true){

		ret = TCP_SOCKET.listen(ServerInfo.iPort,sAddr);
		if (ret < 0 && ret != -2) return -1;

		sleep(1);

		TCP_SOCKET.CloseListenSocket();

		//�������USER_AUTH_PATH�ļ��޸��ˣ������¼���
		time_t tLmtTemp=tFileModifyTime(sUserAuthPath);
		
		if(tLmtUserAuth<tLmtTemp){
			
			MapUserAuth	mapTemp;
			
			if(ReadUserAuth2Map(sUserAuthPath,mapTemp)<0){
				printf_dt("�����û�Ȩ���ļ� %s ����,�������ͻ����ٵ�¼.\n",sUserAuthPath);
				TCP_SOCKET.Close();
				continue;
			}
			
			//��վ�MAP,����ʱ����������ݷŵ���map��
			mapUserAuth.clear();
			tLmtUserAuth=	tLmtTemp;
			
			//mapUserAuth=	mapTemp;
			//��mapTemp�����ݣ�ͳͳ���뵽mapUserAuth��
			mapUserAuth.insert(mapTemp.begin(),mapTemp.end());
		}

		CONN.iPort=	ntohs(sAddr.sin_port);
		strcpy(CONN.sDestIp,inet_ntoa(sAddr.sin_addr));
		CONN.strIp=	string(CONN.sDestIp);

		//У�����IP�Ƿ�Ϸ�
		if (iCheckIp){
			if (ValidHost(CONN.sDestIp,mapUserAuth)==false){

				printf_dt("δ֪IP:%s;�ܾ�����.\n",CONN.sDestIp);

				TCP_SOCKET.Close();
				continue;
			}
		}
		printf_dt("IP:%s;����ɹ�.\n",CONN.sDestIp);

		if (iSetSocketFlag){
			if (ServerInfo.iSocketSendLen > 0)
				TCP_SOCKET.setSocketBufferLen(true, (int&)ServerInfo.iSocketSendLen);

			if (ServerInfo.iSocketRecvLen > 0)
				TCP_SOCKET.setSocketBufferLen(false,(int&) ServerInfo.iSocketRecvLen);
		}
		//���forkʧ������ʾ��Ϣ�˳�
		if((iPid=fork())<0){
			printf_dt("������fork �����ӽ���ʧ��.\n");
			return -1;
		}
		//����Ǹ�����,�ر�socket����������IP�Ͷ˿�
		if(iPid>0){
			TCP_SOCKET.Close();
			continue;
		}
		
		//�õ��ӽ��̺�
		CONN.iPid=getpid();

		printf_dt("IP:���ӳɹ�%s:%d connected.\n",CONN.sDestIp,CONN.iPort);
		
		//��������ͻ�����Ϣ�߳�
		pthread_t pthd_cmd;
		pthread_attr_t attr_cmd;
		pthread_attr_init(&attr_cmd);
		pthread_attr_setdetachstate(&attr_cmd, PTHREAD_CREATE_DETACHED);
		pthread_attr_setstacksize(&attr_cmd, 1024*512);
		pthread_create(&pthd_cmd, NULL, MainProcClientCmd, NULL);

		//������MQ��������ת�����ͻ��˵ĺ���
		MainData2Cli();
		WriteLogoutJson();
		
		char sInfo[256],sErrMsg[256];
	
		sprintf(sInfo,"RECVMQ EXIT user=%s(%s:%d)",CONN.sUserName,CONN.sDestIp,CONN.iPort);
	
		printf_dt("%s.\n",sInfo);

		if(LogDispJson(sInfo,(char*)"NULL",sDispPath,sDispLog,sErrMsg)==false){
			printf_dt("LOG LOGOUT JSON ERROR user=%s(%s:%d) msg=%s.\n",
				CONN.sUserName,CONN.sDestIp,CONN.iPort,sErrMsg);
			exit(1);
		}

		//���������ֹ����ֱ���˳�
		break;		
	}

	TCP_SOCKET.Close();

	return 0;
}
/***
����һ������������Ϣ��
        {
            "user": "moni_user",
            "mqid": "22099",
            "pid": "128170",
            "subscribed": "",
            "subcodes": ""
        }
***/
void GenDispJsonItem(ConnectClient_t &c,ptree &t)
{
	ptree tSubscribed,tSubsCode;

	t.put("user",	c.strUserName);
	t.put<int>("mqid", c.iMqId);
	t.put<int>("pid", c.iPid);

	for (auto it = c.setSubscribed.begin(); it != c.setSubscribed.end(); ++it)
		tSubscribed.push_back(std::make_pair("", ptree(to_string(*it))));
	
	for (auto it = c.setSubsCode.begin(); it != c.setSubsCode.end(); ++it)
		tSubsCode.push_back(std::make_pair("", ptree(to_string(*it))));
	
	
	t.put_child("subscribed",tSubscribed);
	t.put_child("subcodes",	tSubsCode);

//	out.push_back(std::make_pair("", t));
}
//����ǰCONN�ṹ����Ϣ��д��disp.json�ļ��Ķ�Ӧ�û���
void writeLoginJson()
{
	ptree tNewItem,tRoot,tNewRoot,tNewChild;
	
	read_json(sDispPath, tRoot);
	
	//����һ��������Ϣ
	GenDispJsonItem(CONN,tNewItem);
	
	if(tRoot.count("users")==0){
		tNewChild.push_back(std::make_pair("", tNewItem));
		tNewRoot.put_child("users", tNewChild);
//		tNewRoot.put_child("users", tNewItem);

		DATALOCK.P();
		write_json(sDispPath, tNewRoot);
		DATALOCK.V();
		return;
	}
	
	//��һ��ѭ���������Ǳ��û������÷ŵ�tNewChild��
	for(auto t : tRoot.get_child("users")){

		if (CONN.strUserName!=t.second.get < string > ("user"))
			tNewChild.push_back(t);
	}
	
	tNewChild.push_back(std::make_pair("", tNewItem));
	tNewRoot.put_child("users",tNewChild);	
	
	DATALOCK.P();
	write_json(sDispPath, tNewRoot);
	DATALOCK.V();
}
//ֱ��ȡ���ѱ��û���json��Ŀ������tNewRoot�У�д���ļ�
int WriteLogoutJson()
{
//	int i=0;
	int iFoundFlag=false;
	
	ptree tRoot,tNewRoot,tNewChild;
	
	read_json(sDispPath, tRoot);
	
	//��һ��ѭ���������Ǳ��û������÷ŵ�tNewChild��
	for(auto t : tRoot.get_child("users")){

		if (CONN.strUserName!=t.second.get < string > ("user")){
//			printf("xxxx.\n");
			tNewChild.push_back(t);
		}
		else	iFoundFlag=true;

//		printf("----%ld-i=%d,u1=%s,u2=%s.\n",
//			tNewChild.size(),i++,
//			CONN.strUserName.c_str(),
//			t.second.get < string > ("user").c_str());
	}
	
	tNewRoot.put_child("users",tNewChild);	

	//ֻ����disp.json�ļ��д��ڵ�ǰ�û��ļ�¼����Ҫ����
	if(iFoundFlag==true){	
		DATALOCK.P();
		write_json(sDispPath, tNewRoot);
		DATALOCK.V();
	}

	return iFoundFlag;
}
//�ж��Ƿ��Ѿ���¼���������¼�����ܵ�¼
bool IsLogined(string strUserName)
{
	int iPid;
	ptree tRoot;
	
	read_json(sDispPath, tRoot);

	if(tRoot.count("users")==0) return false;
	
	//��һ��ѭ���������Ǳ��û������÷ŵ�tNewChild��
	for(auto t : tRoot.get_child("users")){

		if (strUserName==t.second.get < string > ("user")){

			iPid = t.second.get<int>("pid",0);
			//�������Ч���̣����ʾδ��¼
			if(iPid==0)	break;
			//json�ļ���¼��pid������{���÷����ź�ʧ�����ж�}��
			//Ҳ��ʾδ��¼
			if(kill(iPid,0)!=0) break;
				
			return true;
		}
	}
	
	return false;
}
bool setSubscrible(const SubscribeRequest &req)
{
	if(CONN.strUserName.size() == 0) return false;


	const auto it = mapUserAuth.find(CONN.strUserName);
	const int numSub = req.sub_size();
	BizCode bizCodeTmp;

	CONN.setSubscribed.clear();

	for (int i = 0; i < numSub; ++i){

		auto t=it->second.setAuth;

		bizCodeTmp = BizCode(req.sub(i));
			
		//���Ȩ���ҵ�,����D31�Ķ������ֱ��Ҹ�������Ŀ
		if(bizCodeTmp==D31_ITEM){
			if(t.find(180)!=t.cend())	CONN.setSubscribed.insert(180);
			if(t.find(183)!=t.cend())	CONN.setSubscribed.insert(183);
			if(t.find(185)!=t.cend())	CONN.setSubscribed.insert(185);
			if(t.find(18)!=t.cend())	CONN.setSubscribed.insert(18);
		}
		else{
			if(t.find(bizCodeTmp)!=t.cend())CONN.setSubscribed.insert((int)bizCodeTmp);
		}
	}

	writeLoginJson();
	return true;
}

bool addReduceCodes(const vector<uint32_t> &codes, const bool addFlag)
{
	if(CONN.strUserName.size() == 0)
		return false;

	for (const auto code : codes){
		if(addFlag)
			CONN.setSubsCode.insert(code);
		else	CONN.setSubsCode.erase(code);
	}

	writeLoginJson();

	return true;
}

bool DealCommand(string &msg)
{
	bool bRes = true;
	BizCode iBizCode;
	string msgProtobuf;

	getBizcode(iBizCode, msgProtobuf, msg);

	switch (iBizCode){

	case LOGIN_REQ:
	{
		Reply rep;
		LoginRequest req;
		ErrCode errcode = ErrCode::OTHER;

		string msgRep,msgBody,sPasswd;


		req.ParseFromString(msgProtobuf);
		sPasswd = req.password();
			
		auto itAuth = mapUserAuth.find(req.name());
		
		CONN.strUserName = req.name();
		strcpy(CONN.sUserName,req.name().c_str());

		printf_dt("Recv LOGIN_REQ user=%s(%s:%d) passwd=%s.\n",
			CONN.sUserName,CONN.sDestIp,CONN.iPort,sPasswd.c_str());
		PrintHexBuf((char*)(msg.c_str()),msg.size());

		rep.set_err(errcode);
	
		//�Ȳ�ѯ�û����Ƿ���Ч,��Ч�򷵻ؿͻ���
		if (itAuth == mapUserAuth.cend()){
			rep.set_desc("user_invalid");
			addBizcode(msgBody, rep, BizCode::LOGIN_REP);
			TCP_SOCKET.send((unsigned char *) msgBody.data(), msgBody.size());
			bRes = false;
			printf_dt("LOGIN_REP invalid user=%s(%s:%d).\n",
				CONN.sUserName,CONN.sDestIp,CONN.iPort);

			break;
		}
		
		CONN.iMqId = itAuth->second.iMqId;

		//����Ѿ���¼�򣬻ظ��Ѿ���¼��Ϣ
		if(IsLogined(CONN.strUserName)){
			rep.set_desc("user_have_logined");
			addBizcode(msgBody, rep, BizCode::LOGIN_REP);
			TCP_SOCKET.send((unsigned char *) msgBody.data(), msgBody.size());
			bRes = false;
			printf_dt("LOGIN_REP user logined user=%s(%s:%d).\n",
				CONN.sUserName,CONN.sDestIp,CONN.iPort);
			break;
		}
		//������������������Ϣ���ؿͻ���
		if(itAuth->second.strPassword != req.password()){
			rep.set_desc("pswd_error");
			addBizcode(msgBody, rep, BizCode::LOGIN_REP);
			TCP_SOCKET.send((unsigned char *) msgBody.data(), msgBody.size());
			bRes = false;

			printf_dt("LOGIN_REP passwd error user=%s(%s:%d).\n",
				CONN.sUserName,CONN.sDestIp,CONN.iPort);
			break;
		}

		//�û���������֤ͨ��

		//���������֤Ҳͨ�����������������
/**
		if(!m_pTimerController){
			//������ʱ��
			m_pTimerController  = new TimerController(m_io,
				boost::bind(&Dat2Client::handleTimeOut,this),ServerInfo.iHeartBeat*2);
			m_pThread = new std::thread(startIosThread,&m_io);
			m_pThread->detach();
		}
		else{
			m_pTimerController->resetTimer(ServerInfo.iHeartBeat*2);
		}
**/
		//���������֤Ҳͨ�������ڷ��سɹ����ٷ��ؿͻ���allsubs��Ϣ��pb��ʽ��
		rep.set_err(ErrCode::SUCCESS);

		msgRep=to_string(ServerInfo.iHeartBeat);

		rep.set_desc(msgRep);
		addBizcode(msgBody, rep, BizCode::LOGIN_REP);

		TCP_SOCKET.send((unsigned char *) msgBody.data(), msgBody.size());

		PbCodesBroadcast bc;
		for (const auto code : CONN.setAllSubs)
			bc.add_codes(code);

		addBizcode(msgBody, bc, BizCode::CODES_BROADCAST);
		TCP_SOCKET.send((unsigned char *) msgBody.data(), msgBody.size());
		
		printf_dt("LOGIN_REP login SUCCESS user=%s(%s:%d).\n",
			CONN.sUserName,CONN.sDestIp,CONN.iPort);
	}
	break;
	case SUBSCRIBLE:
	{
		char sInfo[256],sMsg[256],sErrMsg[256];
	
		SubscribeRequest req;
		req.ParseFromString(msgProtobuf);
		bRes = setSubscrible(req);

		sprintf(sInfo,"Recv SUBSCRIBLE user=%s(%s:%d) Msg:Sub_Size=%d",
			CONN.sUserName,CONN.sDestIp,CONN.iPort,req.sub_size());

		printf_dt("%s.\n",sInfo);

		PrintHexBuf((char*)(msg.c_str()),msg.size());
		
		//���ｫ�ı�֮����ļ���ȫ��д�뵽 disp.json.log�ļ���
		MyBin2HexStr((char*)(msg.c_str()),msg.size(),sMsg);
		if(LogDispJson(sInfo,sMsg,sDispPath,sDispLog,sErrMsg)==false){
			printf_dt("SUBSCRIBLE %s user=%s(%s:%d).\n",
				sErrMsg,CONN.sUserName,CONN.sDestIp,CONN.iPort);
			bRes = false;
		}
	}
		break;
	case HEART_BEAT:
	{
#ifdef DEBUG_ONE

		printf_dt("Recv HEART_BEAT MSG user=%s(%s:%d).\n",
			CONN.sUserName,CONN.sDestIp,CONN.iPort);

		PrintHexBuf((char*)(msg.c_str()),msg.size());
#endif
//		m_pTimerController->resetTimer(ServerInfo.iHeartBeat*2);
	}
		break;
	case CODES_SUB:
	{
		
		char sInfo[256],sMsg[256],sErrMsg[256];

		PbCodesSub req;

		req.ParseFromString(msgProtobuf);
		const int sz = req.codes_size();

		vector<uint32_t> codes(sz);

		const auto reqCodes = req.codes();

		for (int i = 0; i < sz; ++i)
		{
			codes[i] = reqCodes.operator[](i);
		}
		bRes = addReduceCodes(codes, req.add_red());

		sprintf(sInfo,"Recv CODES_SUB user=%s(%s:%d) Msg:codes_size=%d",
			CONN.sUserName,CONN.sDestIp,CONN.iPort,sz);

		printf_dt("%s.\n",sInfo);

		PrintHexBuf((char*)(msg.c_str()),msg.size());
		
		//���ｫ�ı�֮����ļ���ȫ��д�뵽 disp.json.log�ļ���
		MyBin2HexStr((char*)(msg.c_str()),msg.size(),sMsg);
		if(LogDispJson(sInfo,sMsg,sDispPath,sDispLog,sErrMsg)==false){
			printf_dt("CODES_SUB %s user=%s(%s:%d).\n",
				sErrMsg,CONN.sUserName,CONN.sDestIp,CONN.iPort);
			bRes = false;
		}
	}
		break;
	default:
	{
		bRes = false;
		
		printf_dt("Unknow Msg user=%s(%s:%d).\n",
			CONN.sUserName,CONN.sDestIp,CONN.iPort);
		PrintHexBuf((char*)(msg.c_str()),msg.size());

		break;
	}
	}
	return bRes;
}

//����ͻ�����Ϣ���̣߳�������
void *MainProcClientCmd(void *)
{
	
	string msg;
	int recv_len, msg_len, ret,iResult=0;
	unsigned char sBuffer[8192];
	unsigned char *data = sBuffer;


	printf_dt("CLICMD thread start opp=(%s:%d).\n",CONN.sDestIp,CONN.iPort);

	memset((void*)sBuffer, 0, sizeof(sBuffer));

	while(1){

		msg_len = 2;
		recv_len =  0;

		//��һ��ѭ�����ӿͻ���socket�ж�ȡ��Ϣ����data��
		while(recv_len < msg_len){
			if ((ret = TCP_SOCKET.read((data + recv_len), msg_len - recv_len)) <= 0){
				printf_dt("user=%s(%s:%d) READ ERROR code=%d.\n",
					CONN.sUserName,CONN.sDestIp,CONN.iPort,ret);
				iResult=1;
				goto next_end_cli;
			}
			recv_len += ret;
		}

		msg_len = data[0] * 256 + data[1];

		//У����Ϣ���ȵĺϷ��ԣ�����Ƿ���Ͽ�
		if (msg_len <= 0||msg_len > 8190){ //��Ϣ��������
			printf_dt("��Ϣ��������msg_len��%d!\n",msg_len);
			iResult=2;
			break;
		}


#ifdef DEBUG_ONE
		printf_dt("user=%s(%s:%d) Recv BizCode[%d]Msg_Len[%d].\n",
			CONN.sUserName,CONN.sDestIp,CONN.iPort,(int)data[3],msg_len);
#endif

		//�õ���Ч����Ϣ���Ⱥ����ȡ������Ϣ
		while(recv_len - 2 < msg_len){
			if ((ret = TCP_SOCKET.read(data + recv_len, msg_len)) <= 0){
				
				
				printf_dt("TCP_SOCKET��ȡʧ��.\n");
				iResult=3;
				goto next_end_cli;
			}
			recv_len += ret;
		}

		msg.assign((char*)data+2, msg_len);

		if(DealCommand(msg)==false){
			printf_dt("user=%s(%s:%d) PROCESS CMD ERROR.\n",
				CONN.sUserName,CONN.sDestIp,CONN.iPort);

			iResult=4;
			break;
		}

	}
next_end_cli:
	
	int iWriteFlag=WriteLogoutJson();

	char sInfo[256],sErrMsg[256];
	
	sprintf(sInfo,"CLICMD EXIT user=%s(%s:%d)",CONN.sUserName,CONN.sDestIp,CONN.iPort);
	
	printf_dt("%s.\n",sInfo);

	//ֻ��ȷʵ��дdisp.json�ļ�ʱ���ŵ���logdispjson�����鵵
	if(iWriteFlag==true){
		if(LogDispJson(sInfo,(char*)"NULL",sDispPath,sDispLog,sErrMsg)==false){
			printf_dt("LOG LOGOUT JSON ERROR user=%s(%s:%d) msg=%s.\n",
				CONN.sUserName,CONN.sDestIp,CONN.iPort,sErrMsg);
			exit(1);
		}
	}
	
	if(iResult>0) exit(iResult);
	
	return NULL;
}

int MainData2Cli()
{
	string strRecv;
	char sTemp[256],sBuffer[8192];
	unsigned char *data = (unsigned char*)sBuffer;
	int iRet,iSubs,msg_len,num,l;

	MessageQueue *mqData=NULL;

	//��һ��ѭ���ȴ���¼�߳��õ�MQID
	usleep(10*1000);
	while(CONN.iMqId==0){
		usleep(200*1000);
		printf_dt("RECVMQ process wait CLICMD.\n");
	}

	printf_dt("RECVMQ process user=%s(%s:%d).\n",
		CONN.sUserName,CONN.sDestIp,CONN.iPort);

	//�������򿪴������ݵ�MQ����
	if((mqData=new MessageQueue(CONN.iMqId))==NULL){
		printf_dt("��ʼ�� mq ���� key=%d.\n",CONN.iMqId);
		return -1;
	}
	mqData->open(false, true, ServerInfo.iSysMqMaxLen, ServerInfo.iSysMqMaxNum);
	
	//�����ǰ�û��������־�û��б��У����д�ͻ�������ļ�����
	if(UserInWriteUser(CONN.sUserName,sWriteUser))
		Write2CliFile=MyWrite2CliFile;
	else	Write2CliFile=NULL;

	while (iExitFlag==false){

		//������յ���ϢΪ�գ�û����Ϣ��������0.1ms��������
		if((iRet = mqData->receive(strRecv, 0))<=0){
			usleep(100);
			continue;
		}

		//��һ��ͳ����Ϣ������ն�
		
		CONN.lRecvCnt++;

		iSubs=(int)((unsigned char) (strRecv[2]));
		
		CONN.mapSubsStat[iSubs]++;

		if((CONN.lRecvCnt)%3000==0){

			l=0;

			for(auto it=CONN.mapSubsStat.begin(); it!=CONN.mapSubsStat.end(); it++){
				if(l==0)
					l+=sprintf(sTemp,"[%d:%ld]",	it->first,it->second);
				else	l+=sprintf(sTemp+l,",[%d:%ld]",it->first,it->second);
			}

			printf_dt("user=%s(%s:%d) mqid=%d,sendcnt=%ld{%s}\n",
				CONN.sUserName,	CONN.sDestIp,CONN.iPort,
				mqData->m_oriKey,CONN.lRecvCnt,sTemp);
		}

		num = 0;
		msg_len = strRecv.size();

		memcpy((void*)data, strRecv.c_str(), strRecv.size());

		while(num < msg_len){
			if((iRet = TCP_SOCKET.write((data + num), msg_len - num))<0){
				printf_dt("write socket error count=%d.\n",CONN.lRecvCnt);
				return -1;
			}
			num += iRet;
		}

		if(Write2CliFile!=NULL){
			if(Write2CliFile(sWorkRoot,(char*)CONN.sUserName,strRecv)<0)
				return -1;
		}
	}
	return 0;
}
