#include <stdio.h>
#include <atomic>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/inotify.h>

#include <boost/property_tree/json_parser.hpp>

#include "MessageQueue.h"

#include "callsupp.h"


//atomic_t VLock=ATOMIC_INIT(0);

int VLock=0;


struct DispRuleStruct R,T;
char sRefreshDispName[1024];

int iMaxMqCnt=0,iSendCnt=0;;
MessageQueue *ARRAY_MQ[MAX_CLIENT_CNT];

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

int WatchFileCloseWriteAndLock(char sFileName[])
{
	int fd,len,i;
	char buf[BUFSIZ];
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
			
			fprintf(stdout, "%s --- %s\ti=%d,m=%d,l=%d\n"," ", "",i,event->mask,len);

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
	for(int i=0;i<MAX_STOCK_CODE;i++){
		DeleteUserList(p->AMUSER[i]);p->AMUSER[i]=pi->AMUSER[i];
		DeleteUserList(p->ATUSER[i]);p->ATUSER[i]=pi->ATUSER[i];
		DeleteUserList(p->AQUSER[i]);p->AQUSER[i]=pi->AQUSER[i];
		DeleteUserList(p->AOUSER[i]);p->AOUSER[i]=pi->AOUSER[i];
	}
	DeleteUserList(p->PMALL);p->PMALL=pi->PMALL;
	DeleteUserList(p->PTALL);p->PTALL=pi->PTALL;
	DeleteUserList(p->PQALL);p->PQALL=pi->PQALL;
	DeleteUserList(p->POALL);p->POALL=pi->POALL;

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
	struct UserStruct **AUSER,*pTemp,**PPALL;

	boost::property_tree::ptree tRoot,tMainRoot,t,tSubscribed,tSubcodes;
	string user,mqid;

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
			continue;
		}

                iStockCode=-1;

		for (auto i = tSubcodes.begin(); i != tSubcodes.end(); ++i) {


			iStockCode=i->second.get_value<int>();

			/*������Ч��֤ȯ����*/
			if(iStockCode<=0||iStockCode>MAX_STOCK_CODE) continue;

			for (auto j = tSubscribed.begin(); j != tSubscribed.end(); ++j) {

				iSubscribed=j->second.get_value<int>();

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
				/*���뵽����*/
				pTemp->pNext=AUSER[iStockCode];
				AUSER[iStockCode]=pTemp;
                	}
                }

                if(iStockCode==-1){

			for (auto i = tSubscribed.begin(); i != tSubscribed.end(); ++i) {

				iSubscribed=i->second.get_value<int>();

	printf("hello world.--------------------------------------------5.yyyy.\n");

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

				/*���뵽����*/
				pTemp->pNext=*PPALL;
				*PPALL=pTemp;
			}
                }

	}
	
	AssignDispRule(p,&T);

	printf("hello world.--------------------------------------------5.8.\n");

}
int SendMsg2Mq(string &str,struct UserStruct *pCli)
{
	MessageQueue *mq;
	
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

	if((iSendCnt%50000)==0)
		printf("send count =%d.\n",iSendCnt);

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
