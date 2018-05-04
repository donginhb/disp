#include <stdio.h>
#include <atomic>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#include <boost/property_tree/json_parser.hpp>

#include "MessageQueue.h"

#include "wwtiny.h"
#include "callsupp.h"


//atomic_t VLock=ATOMIC_INIT(0);

int VLock=0;


struct DispRuleStruct R,T;
struct UserStruct *pRAll=NULL,*pTAll=NULL;
char sRefreshDispName[1024];

int iMaxMqCnt=0,iSendCnt=0;;
MessageQueue *ARRAY_MQ[MAX_CLIENT_CNT];


void InsertFreeList(struct UserStruct **pptHead,struct UserStruct *pTemp)
{
	pTemp->pFreeNext=*pptHead;
	*pptHead=pTemp;
}
void LockWorkThread()
{
	//atomic_inc(&VLock);
	VLock=1;
}
void UnLockWorkThread()
{
	//atomic_dec(&VLock);
	VLock=0;
}
int IsWorkThreadLock()
{
	//return atomic_read(&VLock);
	return VLock;
}

/****
#define EVENT_NUM 12

char *event_str[EVENT_NUM] =
{
    "IN_ACCESS",
    "IN_MODIFY",
    "IN_ATTRIB",
    "IN_CLOSE_WRITE",
    "IN_CLOSE_NOWRITE",
    "IN_OPEN",
    "IN_MOVED_FROM",
    "IN_MOVED_TO",
    "IN_CREATE",
    "IN_DELETE",
    "IN_DELETE_SELF",
    "IN_MOVE_SELF"
};
***/

char *GetEventStrInfo(int mask)
{
	char *p;

	switch (mask){
	case 0x01: p=(char*)"IN_ACCESS";	break;
	case 0x02: p=(char*)"IN_MODIFY";       break;
	case 0x04: p=(char*)"IN_ATTRIB";       break;
	case 0x08: p=(char*)"IN_CLOSE_WRITE";  break;
	case 0x10: p=(char*)"IN_CLOSE_NOWRITE";break;
	case 0x20: p=(char*)"IN_OPEN";         break;
	case 0x40: p=(char*)"IN_MOVED_FROM";   break;
	case 0x80: p=(char*)"IN_MOVED_TO";     break;
	case 0x100:p=(char*)"IN_CREATE";       break;
	case 0x200:p=(char*)"IN_DELETE";       break;
	case 0x400:p=(char*)"IN_DELETE_SELF";  break;
	case 0x800:p=(char*)"IN_MOVE_SELF";    break;
	default:break;
	}
	return p;
}

int WatchFileCloseWriteAndLock(char sFileName[])
{
	int fd,len,i;
	char buf[BUFSIZ],sHostTime[15],sMilli[4];
	struct inotify_event *event;

	if((fd = inotify_init())<0){
		fprintf(stderr, "inotify_init failed\n");
		return -1;
	}

	if(inotify_add_watch(fd, sFileName, IN_ALL_EVENTS)<0){
		fprintf(stderr, "inotify_add_watch %s failed\n", sFileName);
		return -1;
	}

	buf[sizeof(buf) - 1] = 0;
	while( (len = read(fd, buf, sizeof(buf) - 1)) > 0 ){

		printf("-----------------------------3 l=%d.\n",len);

		for(i=0;i<len;i+=sizeof(struct inotify_event)){

			event = (struct inotify_event *)&buf[i];

			GetHostTimeX(sHostTime,sMilli);
			printf("%s:%s %s --- %s\ti=%d,m=%d,mi=%s l=%d\n",sHostTime,sMilli," ", "",
				i,event->mask,GetEventStrInfo(event->mask),len);

			//�������WRITE_CLOSE�¼������
			if((event->mask & 0x8)==0) continue;

			printf("catch WRITE-ON-CLOSE EVENT.\n");

			//��������
			LockWorkThread();
		}
	}
	return 0;
}
void TDF_MARKET_DATA2MktData(MktData &output, const TDF_MARKET_DATA &src)
{
	output.set_szcode(strtoul(src.szCode,nullptr,10));	//600001.SH
	output.set_ntime(src.nTime);			//ʱ��(HHMMSSmmm)
	output.set_nstatus(src.nStatus);		//״̬
	output.set_npreclose(uint32_t(src.nPreClose));	//ǰ���̼�
	output.set_nopen(uint32_t(src.nOpen));			//���̼�
	output.set_nhigh(uint32_t(src.nHigh));			//��߼�
	output.set_nlow(uint32_t(src.nLow));				//��ͼ�
	output.set_nmatch(uint32_t(src.nMatch));			//���¼�
	for (unsigned char i = 0; i < 10; ++i) {
		output.add_naskprice(uint32_t(src.nAskPrice[i]));//������
		output.add_naskvol(src.nAskVol[i]);	//������
		output.add_nbidprice(uint32_t(src.nBidPrice[i]));//�����
		output.add_nbidvol(src.nBidVol[i]);	//������
	}
	output.set_nnumtrades(src.nNumTrades);	//�ɽ�����
	output.set_ivolume(src.iVolume);		//�ɽ�����
	output.set_iturnover(src.iTurnover);	//�ɽ��ܽ��
	output.set_ntotalbidvol(src.nTotalBidVol);//ί����������
	output.set_ntotalaskvol(src.nTotalAskVol);//ί����������
	output.set_nweightedavgbidprice(uint32_t(src.nWeightedAvgBidPrice));//��Ȩƽ��ί��۸�
	output.set_nweightedavgaskprice(uint32_t(src.nWeightedAvgAskPrice));//��Ȩƽ��ί���۸�
	output.set_niopv(src.nIOPV);		//IOPV��ֵ��ֵ
	output.set_nyieldtomaturity(src.nYieldToMaturity);//����������
	output.set_nhighlimited(uint32_t(src.nHighLimited));//��ͣ��
	output.set_nlowlimited(uint32_t(src.nLowLimited));//��ͣ��
	output.set_nsyl1(src.nSyl1);			//��ӯ��1
	output.set_nsyl2(src.nSyl2);			//��ӯ��2
	output.set_nsd2(src.nSD2);				//����2���Ա���һ�ʣ�
}

void TDF_TRANSACTION2Transaction(Transaction &output, const TDF_TRANSACTION &src)
{
	output.set_szcode(strtoul(src.szCode, nullptr, 10));	//600001.SH
	output.set_ntime(src.nTime);			//�ɽ�ʱ��(HHMMSSmmm)
	output.set_nindex(src.nIndex);			//�ɽ����
	output.set_nprice(uint32_t(src.nPrice));//�ɽ��۸�
	output.set_nvolume(src.nVolume);		//�ɽ�����
	output.set_nturnover(src.nTurnover);	//�ɽ����
	output.set_nbsflag(src.nBSFlag);		//��������(��'B', ����'S', ������' ')
	output.set_chorderkind(src.chOrderKind);//�ɽ����
	output.set_chfunctioncode(src.chFunctionCode);//�ɽ�����
	output.set_naskorder(src.nAskOrder);	//������ί�����
	output.set_nbidorder(src.nBidOrder);
}

void TDF_ORDER2Order(Order &output, const TDF_ORDER&src)
{
	output.set_szcode(strtoul(src.szCode, nullptr, 10));	//600001.SH
	output.set_ntime(src.nTime);			//ʱ��(HHMMSSmmm)
	output.set_norder(src.nOrder);		//ί�к�
	output.set_nprice(uint32_t(src.nPrice));//ί�м۸�
	output.set_nvolume(src.nVolume);		//ί������
	output.set_chorderkind(src.chOrderKind);//ί�����
	output.set_chfunctioncode(src.chFunctionCode);//ί�д���('B','S','C')
	output.set_nbroker(src.nBroker);		//�����̱���
	output.set_chstatus(src.chStatus);		//ί��״̬
	output.set_chflag(src.chFlag);
}

void TDF_ORDER_QUEUE2Order_queue(Order_queue &output, const TDF_ORDER_QUEUE&src)
{
	output.set_szcode(strtoul(src.szCode, nullptr, 10));	//600001.SH
	output.set_ntime(src.nTime);			//ʱ��(HHMMSSmmm)
	output.set_nside(src.nSide=='B'||src.nSide=='b');//��������('B':Bid 'A':Ask)
	output.set_nprice(uint32_t(src.nPrice));//ί�м۸�
	output.set_norders(src.nOrders);		//��������
	output.set_nabitems(src.nABItems);		//��ϸ����
	for (int i = 0; i < src.nABItems; ++i) {
		output.add_nabvolume(src.nABVolume[i]);
	}
}

MessageQueue *GetMqArray(int iMqId,int *pIndex)
{
	MessageQueue *p;

	*pIndex=-1;

	for(int i=0;i<iMaxMqCnt;i++){
		p=ARRAY_MQ[i];
		if(iMqId==p->m_oriKey){
			*pIndex=i;
			return p;
		}
	}
	return NULL;
}
int AddMqArray(MessageQueue *q)
{
	int iPos=iMaxMqCnt;

	ARRAY_MQ[iPos]=q;

	iMaxMqCnt++;
	return iPos;
}
void InitUserArray(char sDispName[],struct DispRuleStruct *p)
{
/*��ӳ������*/
	for(int i=0;i<MAX_STOCK_CODE;i++){
		p->AMUSER[i]=NULL;
		p->ATUSER[i]=NULL;
		p->AQUSER[i]=NULL;
		p->AOUSER[i]=NULL;
	}
	p->PMALL=p->PTALL=p->PQALL=p->POALL=NULL;
	strcpy(sRefreshDispName,sDispName);
}
void DeleteUserList(struct UserStruct *ptHead)
{
	struct UserStruct *pTemp;
	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pNext;
		delete pTemp;
	}
}
void AssignDispRule(struct DispRuleStruct *p,struct DispRuleStruct *pi)
{
	int i;	
	struct UserStruct *ptHead,*pTemp;
	
	ptHead=pRAll;
	
	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pFreeNext;
		
		
		//����Ʊ����

		if((i=pTemp->iStockCode)!=-1){
			switch(pTemp->iSubscribed){
			case 12:p->AMUSER[i]=NULL;break;
			case 13:p->ATUSER[i]=NULL;break;
			case 14:p->AQUSER[i]=NULL;break;
			case 15:p->AOUSER[i]=NULL;break;
			default:p->AMUSER[i]=NULL;break;
			}
		}
		free(pTemp);
	}
	
	//����ʱ������µ�ʵʱ������
	pRAll=pTAll;
	pTAll=NULL;
	
	//����ʵʱ����������Ӧ����
	ptHead=pRAll;
	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pFreeNext;
		
		if((i=pTemp->iStockCode)!=-1){
			switch(pTemp->iSubscribed){
			case 12:p->AMUSER[i]=pi->AMUSER[i];break;
			case 13:p->ATUSER[i]=pi->ATUSER[i];break;
			case 14:p->AQUSER[i]=pi->AQUSER[i];break;
			case 15:p->AOUSER[i]=pi->AOUSER[i];break;
			default:p->AMUSER[i]=NULL;break;
			}
		}		
	}
	p->PMALL=pi->PMALL;
	p->PTALL=pi->PTALL;
	p->PQALL=pi->PQALL;
	p->POALL=pi->POALL;

}
void FreeDispRule(struct DispRuleStruct *p)
{
	for(int i=0;i<MAX_STOCK_CODE;i++){
		DeleteUserList(p->AMUSER[i]); p->AMUSER[i]=NULL;
		DeleteUserList(p->ATUSER[i]); p->ATUSER[i]=NULL;
		DeleteUserList(p->AQUSER[i]); p->AQUSER[i]=NULL;
		DeleteUserList(p->AOUSER[i]); p->AOUSER[i]=NULL;
	}
	DeleteUserList(p->PMALL);  p->PMALL=NULL;
	DeleteUserList(p->PTALL);  p->PTALL=NULL;
	DeleteUserList(p->PQALL);  p->PQALL=NULL;
	DeleteUserList(p->POALL);  p->POALL=NULL;

}
void RefreshUserArray(char sDispName[],struct DispRuleStruct *p)
{
	int iSubscribed,iStockCode;
	char sHostTime[15],sMilli[4];
	struct UserStruct **AUSER,*pTemp,**PPALL;

	boost::property_tree::ptree tRoot,tMainRoot,t,tSubscribed,tSubcodes;
	string user,mqid;

	GetHostTimeX(sHostTime,sMilli);
	printf("%s:%s refresh BEGIN\n",sHostTime,sMilli);

/*��ȡdisp.json�ļ�,�ȼ��һ�¸�ʽ�Ƿ��������������Ͳ�ˢ��*/
	try {
		boost::property_tree::read_json(sDispName,tMainRoot);
		tRoot=tMainRoot.get_child("users");
	}
	catch (...) {
		printf("��⵽�������� disprule�ļ����ļ���:%s.\n",sDispName);
		return;
	}

/*��ӳ������*/

	InitUserArray(sRefreshDispName,&T);

	for (auto it = tRoot.begin(); it != tRoot.end(); ++it) {

		auto each = it->second;

		try{
			user = 	each.get<string>("user");
			mqid =	each.get<string>("mqid");
			tSubscribed = each.get_child("subscribed");
			tSubcodes = each.get_child("subcodes");
		}
		catch (...){
			//���ĳ���û���Ϣ��ȫ������Ϊ����û�Ϊû�ж���
			GetHostTimeX(sHostTime,sMilli);
			printf("%s:%s watch exception",sHostTime,sMilli);
			continue;
		}

                iStockCode=-1;

		for (auto i = tSubcodes.begin(); i != tSubcodes.end(); ++i) {


			iStockCode=i->second.get_value<int>();

			/*������Ч��֤ȯ����*/
			if(iStockCode<=0||iStockCode>MAX_STOCK_CODE) continue;

			for (auto j = tSubscribed.begin(); j != tSubscribed.end(); ++j) {

				iSubscribed=j->second.get_value<int>();

				GetHostTimeX(sHostTime,sMilli);

	printf("%s:%s user:%s subscribed:%d stockcode:%06d hello world.--------------------------------------------5.zzzz.\n",
		sHostTime,sMilli,user.c_str(),iSubscribed,iStockCode);

				switch(iSubscribed){
				case 12:AUSER=&T.AMUSER[0];break;
				case 13:AUSER=&T.ATUSER[0];break;
				case 14:AUSER=&T.AQUSER[0];break;
				case 15:AUSER=&T.AOUSER[0];break;
				default:AUSER=NULL;
				break;
				}
				//������Ч��������
				if(AUSER==NULL)continue;

				pTemp=new (struct UserStruct);

				if(pTemp==NULL){
					printf("error new struct UserStruct.\n");
					exit(1);
				}

				pTemp->pNext=NULL;
				strncpy(pTemp->sUserName,user.c_str(),sizeof(pTemp->sUserName)-1);
				pTemp->sUserName[sizeof(pTemp->sUserName)-1]=0;

				pTemp->iMqId=	atoi(mqid.c_str());

				pTemp->iMqPos=-1;
				
				pTemp->iStockCode=iStockCode;
				pTemp->iSubscribed=iSubscribed;
				/*���뵽����*/
				pTemp->pNext=AUSER[iStockCode];
				AUSER[iStockCode]=pTemp;
				
				//���뵽pFreeList������
				InsertFreeList(&pTAll,pTemp);
                	}
                }

                if(iStockCode==-1){

			for (auto i = tSubscribed.begin(); i != tSubscribed.end(); ++i) {

				iSubscribed=i->second.get_value<int>();

				GetHostTimeX(sHostTime,sMilli);

	printf("%s:%s user:%s subscribed:%d hello world.--------------------------------------------5.yyyy.\n",
		sHostTime,sMilli,user.c_str(),iSubscribed);

				switch(iSubscribed){
				case 12:PPALL=&T.PMALL;break;
				case 13:PPALL=&T.PTALL;break;
				case 14:PPALL=&T.PQALL;break;
				case 15:PPALL=&T.POALL;break;
				default:PPALL=NULL;
				}
				//������Ч��������
				if(PPALL==NULL)continue;

				pTemp=new (struct UserStruct);

				if(pTemp==NULL){
					printf("error new struct UserStruct.\n");
					exit(1);
				}

				pTemp->pNext=NULL;
				strncpy(pTemp->sUserName,user.c_str(),sizeof(pTemp->sUserName)-1);
				pTemp->sUserName[sizeof(pTemp->sUserName)-1]=0;

				pTemp->iMqId=	atoi(mqid.c_str());
				pTemp->iMqPos=-1;
				
				pTemp->iStockCode=-1;
				pTemp->iSubscribed=iSubscribed;

				/*���뵽����*/
				pTemp->pNext=*PPALL;
				*PPALL=pTemp;
				
				//���뵽pFreeList������
				InsertFreeList(&pTAll,pTemp);
			}
                }

	}

	GetHostTimeX(sHostTime,sMilli);
	printf("%s:%s refresh ASSGIN BEGIN\n",sHostTime,sMilli);

	AssignDispRule(p,&T);

	GetHostTimeX(sHostTime,sMilli);
	printf("%s:%s refresh END\n",sHostTime,sMilli);
}
int SendMsg2Mq(string &str,struct UserStruct *pCli)
{
	MessageQueue *mq;
	char sHostTime[15],sMilli[4];

	if(pCli->iMqPos==-1){//���û��MQ��ַ�򵽻�������

		if((mq=GetMqArray(pCli->iMqId,&pCli->iMqPos))==NULL){

			//��������û�����MQ����NEWһ����OPEN�������浽��������
			if((mq=new MessageQueue(pCli->iMqId))==NULL){
				printf("new MessageQueue error.\n");
				exit(1);
			}
			mq->open(false,false,512,15000);
			pCli->iMqPos=AddMqArray(mq);
		}
	}
	else
		mq=	ARRAY_MQ[pCli->iMqPos];

	if(mq->send(str,0)==(int)(str.length())) iSendCnt++;
	else{
		GetHostTimeX(sHostTime,sMilli);
		printf("%s:%s send error user=%s pos =%d.\n",
			sHostTime,sMilli,pCli->sUserName,iSendCnt);
	}

	if((iSendCnt%50000)==0){
		GetHostTimeX(sHostTime,sMilli);		
		printf("%s:%s user=%s send count =%d.\n",
			sHostTime,sMilli,pCli->sUserName,iSendCnt);

	}

	return 0;
}
void SendMsg2Cli(int iStockCode,char cType,string& str)
{
	char sBuffer[4];
	unsigned int len,l0,l1;
	struct UserStruct *pAll,*pUser;

	string str1;

//	printf("hello world.--------------------------------------------3.5.\n");

	if(IsWorkThreadLock()){

		printf("hello world.--------------------------------------------4.\n");
		RefreshUserArray(sRefreshDispName,&R);
		UnLockWorkThread();
	}

	/*ȡ�ֽڳ��ȣ����ж������ֵ*/
	len=str.length()+1; if(len>10230) len=10230;

	l0=len%256;l1=len/256;

	((unsigned char*)sBuffer)[0]=l1;
	((unsigned char*)sBuffer)[1]=l0;

	switch (cType){
	case 'M': pUser=R.AMUSER[iStockCode];pAll=R.PMALL;sBuffer[2]=12;break;
	case 'T': pUser=R.ATUSER[iStockCode];pAll=R.PTALL;sBuffer[2]=13;break;
	case 'Q': pUser=R.AQUSER[iStockCode];pAll=R.PQALL;sBuffer[2]=14;break;
	case 'O': pUser=R.AOUSER[iStockCode];pAll=R.POALL;sBuffer[2]=15;break;
	default:  pUser=R.AQUSER[iStockCode];pAll=R.PQALL;sBuffer[2]=14;break;
	break;
	}

	str1=string(sBuffer,3)+str;

//	strncpy(sBuffer+3,str.c_str(),str.length());

	while(pUser!=NULL){
		SendMsg2Mq(str1,pUser);
		pUser=pUser->pNext;
	}

	while(pAll!=NULL){
		SendMsg2Mq(str1,pAll);
		pAll=pAll->pNext;
	}
}
