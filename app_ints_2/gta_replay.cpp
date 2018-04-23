//////////////////////////////////////////////////////////////////////////
/// @brief    ����API
///           ʵ�ֲ�ͬ��ʽ�Ķ��ĺ�ȡ�����ļ����ղ�ѯ
//////////////////////////////////////////////////////////////////////////
#include "GTAQTSInterfaceBase.h"
#include "CallBackBase.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "../Common/IoService.h"
#include "../Common/TinyThread.h"

#include "../GTA2TDF/GTA2TDF.h"


#include "../Common/public.h"

#define MY_DATE_CEIL_LONG 1000000000L

int iWriteFlag=0,iDelayMilSec=100,iMultiTimes=1,iInfoSec=3;

long lBgnTime=91500000L;
char sDispName[1024],sSourcePath[1024],sWorkRoot[1024],sReplayDate[16];

int iQhCnt=0,iThCnt=0,iAhCnt=0,iQzCnt=0,iTzCnt=0,iOzCnt=0;
int iQhTime,iThTime,iAhTime,iQzTime,iTzTime,iOzTime;

CallBackBase *pCallBack;

void *MainReplayRunQh(void *);
void *MainReplayRunTh(void *);
void *MainReplayRunAh(void *);

void *MainReplayRunQz(void *);
void *MainReplayRunTz(void *);
void *MainReplayRunOz(void *);

void *MainReplayRunInfo(void *);


int main(int argc, char *argv[])
{

	string strWork;
	strcpy(sDispName,	"./disp.json");
	strcpy(sSourcePath,	"/stock/work");
	strcpy(sWorkRoot,	"/stock/work");


	for (int c; (c = getopt(argc, argv, "d:r:w:t:m:s:o:i:b:?:")) != EOF;){

		switch (c){
		case 'd':
			strncpy(sReplayDate, optarg,8);sReplayDate[8]=0;
			break;
		case 'r':
			strcpy(sDispName, optarg);
			break;
		case 'w':
			iWriteFlag=atoi(optarg);
			if(iWriteFlag!=1&&iWriteFlag!=2) iWriteFlag=0;
			break;
		case 't':
			iDelayMilSec=atoi(optarg);
			break;
		case 'm':
			iMultiTimes=atoi(optarg);
			if(iMultiTimes<=0) iMultiTimes=1;
			break;
		case 's':
			strcpy(sSourcePath, optarg);
			break;
		case 'o':
			strcpy(sWorkRoot, optarg);
			break;
		case 'i':
			iInfoSec=atoi(optarg);
			break;
		case 'b':
			lBgnTime=atoi(optarg);
			if(lBgnTime>=90000000L&&lBgnTime<153000000L)
				break;
			printf("please input correct format -b .\n");
		case '?':
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-d replay-date (yyyymmdd) ]\n");
			printf("   [-r disp-name ]\n");
			printf("   [-w (1,writegta,2 writetdf,other nowrite) ]\n");
			printf("   [-t (query delay mil sec def=100ms) ]\n");
			printf("   [-m (multi_times) ]\n");
			printf("   [-s (source-path) ]\n");
			printf("   [-o work-root-name ]\n");
			printf("   [-i (infosec def=3) ]\n");
			printf("   [-b (begintime def=9:00:00:000,format hhmmssNNN) ]\n");
			exit(1);
			break;
		}
	}

	strWork=string(sWorkRoot);

	//��ʼ��ˢ�����飬�Լ�ˢ���ļ�����ȫ�ֱ���
	InitUserArray(sDispName,&R);

	//ˢ��һ�²���������Ҫ��disp����������������������
	RefreshUserArray(sDispName,&R);

	IoService	ios;
	//������Ϣ�ص���
	CallBackBase CallbackBase(iWriteFlag,strWork);
	CallbackBase.SetIoService(&ios);

	//�����������ݷ���
	ios.Start();


	//����һ���̲߳�ѯ���ݣ��������ݼӵ������߳���
	pCallBack=	&CallbackBase;
		
	pthread_t pthd_qh;
	pthread_attr_t attr_qh;

	//����qh�ط��߳�
	pthread_attr_init(&attr_qh);
	pthread_attr_setdetachstate(&attr_qh, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_qh, 1024*512);
	pthread_create(&pthd_qh, NULL, MainReplayRunQh, NULL);

	printf("-----------------------------1.\n");
	
	pthread_t pthd_th;
	pthread_attr_t attr_th;

	//����th�ط��߳�
	pthread_attr_init(&attr_th);
	pthread_attr_setdetachstate(&attr_th, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_th, 1024*512);
	pthread_create(&pthd_th, NULL, MainReplayRunTh, NULL);

	printf("-----------------------------2.\n");
	
	
	pthread_t pthd_ah;
	pthread_attr_t attr_ah;

	//����ah�ط��߳�
	pthread_attr_init(&attr_ah);
	pthread_attr_setdetachstate(&attr_ah, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_ah, 1024*512);
	pthread_create(&pthd_ah, NULL, MainReplayRunAh, NULL);

	printf("-----------------------------3.\n");
	
	
	pthread_t pthd_qz;
	pthread_attr_t attr_qz;

	//����th�ط��߳�
	pthread_attr_init(&attr_qz);
	pthread_attr_setdetachstate(&attr_qz, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_qz, 1024*512);
	pthread_create(&pthd_qz, NULL, MainReplayRunQz, NULL);

	printf("-----------------------------4.\n");
	
	pthread_t pthd_tz;
	pthread_attr_t attr_tz;

	//����tz�ط��߳�
	pthread_attr_init(&attr_tz);
	pthread_attr_setdetachstate(&attr_tz, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_tz, 1024*512);
	pthread_create(&pthd_tz, NULL, MainReplayRunTz, NULL);

	printf("-----------------------------5.\n");
	
	
	pthread_t pthd_oz;
	pthread_attr_t attr_oz;

	//����oz�ط��߳�
	pthread_attr_init(&attr_oz);
	pthread_attr_setdetachstate(&attr_oz, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_oz, 1024*512);
	pthread_create(&pthd_oz, NULL, MainReplayRunOz, NULL);

	printf("-----------------------------6.\n");

	//����info�ط��߳�
	pthread_t pthd_info;
	pthread_attr_t attr_info;
	pthread_attr_init(&attr_info);
	pthread_attr_setdetachstate(&attr_info, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_info, 1024*512);
	pthread_create(&pthd_info, NULL, MainReplayRunInfo, NULL);

	printf("-----------------------------6.\n");

	
	//ѭ������disp����仯������仯��֪ͨˢ��
	WatchFileCloseWriteAndLock(sDispName);
	
	printf("-----------------------------2.\n");

	//�������벻�����е�����
	while(1){//���̲߳����˳�
		sleep(1);
	}

	//�˳������߳�
	ios.Stop();

	getchar();

	return 0;
}

void *MainReplayRunQh(void *)
{
	int iCount=0;
	char sInFileName[1024],sBuffer[10240];
	UTIL_Time sStartTime;
	long lItemLen=0;
	long long lStartTime,lDiffTime,lCurTime;
	
	FILE *fpIn;
	
	SSEL2_Quotation *p=(SSEL2_Quotation*)(sBuffer+sizeof(long long));	

	sprintf(sInFileName,"%s/gta_qh_%s.dat",sSourcePath,sReplayDate);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}
	
	lItemLen=(sizeof(SSEL2_Quotation))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  lBgnTime;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}		
		if(p->Time<lBgnTime)continue;
__delay:
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

		if(p->Time>(lCurTime%MY_DATE_CEIL_LONG-lDiffTime)){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){
				                        	
			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SSEL2_Quotation;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SSEL2_Quotation));
                        	
			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SSEL2_Quotation,pCallBack, subdata)));
                        	
			pCallBack->m_ios->Post(task);
			
//			if((++iCount)%10000==0)
//				printf("qh stock rt=%d,ct=%lld cur process count =%d.\n",
//					p->Time,lCurTime%MY_DATE_CEIL_LONG,iCount);

			iQhCnt=++iCount;
			iQhTime=p->Time;
		}
		
		if(feof(fpIn)) break;

	}

	fclose(fpIn);
	
	printf("qh stock rt=%d,ct=%lld cur process count =%d.\n",
		p->Time,lCurTime%MY_DATE_CEIL_LONG,iCount);

	return NULL;
}


void *MainReplayRunTh(void *)
{
	int iCount=0;
	char sInFileName[1024],sBuffer[10240];
	UTIL_Time sStartTime;
	long lItemLen=0;
	long long lStartTime,lDiffTime,lCurTime;
	
	FILE *fpIn;
	
	SSEL2_Transaction *p=(SSEL2_Transaction*)(sBuffer+sizeof(long long));	

	sprintf(sInFileName,"%s/gta_th_%s.dat",sSourcePath,sReplayDate);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}
	
	lItemLen=(sizeof(SSEL2_Transaction))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  lBgnTime;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if(p->TradeTime<lBgnTime)continue;

__delay:
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

		if(p->TradeTime>(lCurTime%MY_DATE_CEIL_LONG-lDiffTime)){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){
				                        	
			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SSEL2_Transaction;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SSEL2_Transaction));
                        	
			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SSEL2_Transaction,pCallBack, subdata)));
                        	
			pCallBack->m_ios->Post(task);
			
//			if((++iCount)%20000==0)
//				printf("th stock rt=%d,ct=%lld cur process count =%d.\n",
//					p->TradeTime,lCurTime%MY_DATE_CEIL_LONG,iCount);
			iThCnt=++iCount;
			iThTime=p->TradeTime;

		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);
	
	printf("th stock rt=%d,ct=%lld cur process count =%d.\n",
			p->TradeTime,lCurTime%MY_DATE_CEIL_LONG,iCount);
	return NULL;
}

void *MainReplayRunAh(void *)
{
	int iCount=0;
	char sInFileName[1024],sBuffer[10240];
	UTIL_Time sStartTime;
	long lItemLen=0;
	long long lStartTime,lDiffTime,lCurTime;
	
	FILE *fpIn;
	
	SSEL2_Auction *p=(SSEL2_Auction*)(sBuffer+sizeof(long long));	

	sprintf(sInFileName,"%s/gta_ah_%s.dat",sSourcePath,sReplayDate);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}
	
	lItemLen=(sizeof(SSEL2_Auction))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  lBgnTime;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}
		
		if(p->Time<lBgnTime)continue;

__delay:
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

		if(p->Time>(lCurTime%MY_DATE_CEIL_LONG-lDiffTime)){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){
				                        	
			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SSEL2_Auction;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SSEL2_Auction));
                        	
			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SSEL2_Auction,pCallBack, subdata)));
                        	
			pCallBack->m_ios->Post(task);
			
//			if((++iCount)%10000==0)
//				printf("ah stock rt=%d,ct=%lld cur process count =%d.\n",
//					p->Time,lCurTime%MY_DATE_CEIL_LONG,iCount);
					
			iAhCnt=++iCount;
			iAhTime=p->Time;

		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);
	
	printf("ah stock rt=%d,ct=%lld cur process count =%d.\n",
		p->Time,lCurTime%MY_DATE_CEIL_LONG,iCount);

	return NULL;
}


void *MainReplayRunQz(void *)
{
	int iCount=0;
	char sInFileName[1024],sBuffer[10240];
	UTIL_Time sStartTime;
	long lItemLen=0;
	long long lStartTime,lDiffTime,lCurTime;
	
	FILE *fpIn;
	
	SZSEL2_Quotation *p=(SZSEL2_Quotation*)(sBuffer+sizeof(long long));	

	sprintf(sInFileName,"%s/gta_qz_%s.dat",sSourcePath,sReplayDate);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}
	
	lItemLen=(sizeof(SZSEL2_Quotation))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  lBgnTime;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}
		
		if((p->Time%MY_DATE_CEIL_LONG)<lBgnTime)continue;

__delay:
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

		if((p->Time%MY_DATE_CEIL_LONG)>(lCurTime%MY_DATE_CEIL_LONG-lDiffTime)){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){
				                        	
			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SZSEL2_Quotation;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SZSEL2_Quotation));
                        	
			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SZSEL2_Quotation,pCallBack, subdata)));
                        	
			pCallBack->m_ios->Post(task);
			
//			if((++iCount)%10000==0)
//				printf("qz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);

			iQzCnt=++iCount;
			iQzTime=(int)(p->Time%MY_DATE_CEIL_LONG);
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);
	
	printf("qz stock rt=%d,ct=%lld cur process count =%d.\n",
		(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);

	return NULL;
}


void *MainReplayRunTz(void *)
{
	int iCount=0;
	char sInFileName[1024],sBuffer[10240];
	UTIL_Time sStartTime;
	long lItemLen=0;
	long long lStartTime,lDiffTime,lCurTime;
	
	FILE *fpIn;
	
	SZSEL2_Transaction *p=(SZSEL2_Transaction*)(sBuffer+sizeof(long long));	

	sprintf(sInFileName,"%s/gta_tz_%s.dat",sSourcePath,sReplayDate);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}
	
	lItemLen=(sizeof(SZSEL2_Transaction))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  lBgnTime;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}
		
		if((p->TradeTime%MY_DATE_CEIL_LONG)<lBgnTime)continue;
__delay:
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

		if((p->TradeTime%MY_DATE_CEIL_LONG)>(lCurTime%MY_DATE_CEIL_LONG-lDiffTime)){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){
				                        	
			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SZSEL2_Transaction;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SZSEL2_Transaction));
                        	
			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SZSEL2_Transaction,pCallBack, subdata)));
                        	
			pCallBack->m_ios->Post(task);
			
//			if((++iCount)%50000==0)
//				printf("tz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->TradeTime%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);

			iTzCnt=++iCount;
			iTzTime=(int)(p->TradeTime%MY_DATE_CEIL_LONG);
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);
	
	printf("tz stock rt=%d,ct=%lld cur process count =%d.\n",
		(int)(p->TradeTime%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);

	return NULL;
}

void *MainReplayRunOz(void *)
{
	int iCount=0;
	char sInFileName[1024],sBuffer[10240];
	UTIL_Time sStartTime;
	long lItemLen=0;
	long long lStartTime,lDiffTime,lCurTime;
	
	FILE *fpIn;
	
	SZSEL2_Order *p=(SZSEL2_Order*)(sBuffer+sizeof(long long));	

	sprintf(sInFileName,"%s/gta_oz_%s.dat",sSourcePath,sReplayDate);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}
	
	lItemLen=(sizeof(SZSEL2_Order))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  lBgnTime;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}
		
		if((p->Time%MY_DATE_CEIL_LONG)<lBgnTime)continue;

__delay:
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

		if((p->Time%MY_DATE_CEIL_LONG)>(lCurTime%MY_DATE_CEIL_LONG-lDiffTime)){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){
				                        	
			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SZSEL2_Order;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SZSEL2_Order));
                        	
			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SZSEL2_Order,pCallBack, subdata)));
                        	
			pCallBack->m_ios->Post(task);
			
//			if((++iCount)%50000==0)
//				printf("oz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);
			iOzCnt=++iCount;
			iOzTime=(int)(p->Time%MY_DATE_CEIL_LONG);
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);
	
	printf("oz stock rt=%d,ct=%lld cur process count =%d.\n",
		(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);

	return NULL;
}
void *MainReplayRunInfo(void *)
{
	int iCount=0;
	while(1){
		sleep(iInfoSec);
		
		UTIL_Time stTime;
		PUTIL_GetLocalTime(&stTime);
		long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

/*		printf("qh=%-9dqht=%-10dth=%-9dtht=%-10dah=%-9daht=%-10d\n\
qz=%-9dqzt=%-10dtz=%-9dtzt=%-10doz=%-9dozt=%-10dt=%lld\n",
			iQhCnt,iQhTime,iThCnt,iThTime,iAhCnt,iAhTime,
			iQzCnt,iQzTime,iTzCnt,iTzTime,iOzCnt,iOzTime,lCurTime/1000);
*/
		if(iCount++%10==0)
			printf("qh(%-9d)\tth(%-9d)\tah(%-9d)\tqz(%-9d)\ttz(%-9d)\toz(%-9d)\tcurtime\n",
				 iQhTime,iThTime,iAhTime,iQzTime,iTzTime,iOzTime);
		printf("%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%lld\n",
			iQhCnt,iThCnt,iAhCnt,iQzCnt,iTzCnt,iOzCnt,lCurTime/1000);

	}
	return NULL;
}
