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
#include "IoService.h"
#include "TinyThread.h"

#include "wwtiny.h"

#include "../GTA2TDF/GTA2TDF.h"


#include "public.h"

#define MY_DATE_CEIL_LONG 1000000000L

#define MY_AM_MARKET_BEGIN_TIME	90000000
#define MY_AM_MARKET_OPEN_TIME	93000000
#define MY_AM_MARKET_CLOSE_TIME	113000000
#define MY_PM_MARKET_OPEN_TIME	130000000
#define MY_PM_MARKET_CLOSE_TIME	150000000
#define MY_PM_MARKET_STOP_TIME	153000000
#define MY_DAY_END_TIME		240000000


int iWriteFlag=0,iDelayMilSec=100,iMultiTimes=1,iInfoSec=3;

long lBgnTime=91500000L;
char sDispName[1024],sSourcePath[1024],sWorkRoot[1024],sReplayDate[15];

int iQhCnt=0,iThCnt=0,iAhCnt=0,iQzCnt=0,iTzCnt=0,iOzCnt=0,iDiCnt=0;
int iQhTime,iThTime,iAhTime,iQzTime,iTzTime,iOzTime,iDiTime;

CallBackBase *pCallBack;

void *MainReplayRunQh(void *);
void *MainReplayRunTh(void *);
void *MainReplayRunAh(void *);

void *MainReplayRunQz(void *);
void *MainReplayRunTz(void *);
void *MainReplayRunOz(void *);
void *MainReplayRunD31(void *);

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
	CallBackBase CallbackBase(iWriteFlag,sReplayDate,strWork);
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

	pthread_t pthd_d31;
	pthread_attr_t attr_d31;

	//����oz�ط��߳�
	pthread_attr_init(&attr_d31);
	pthread_attr_setdetachstate(&attr_d31, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_d31, 1024*512);
	pthread_create(&pthd_d31, NULL, MainReplayRunD31, NULL);

	printf("-----------------------------7.\n");

	//����info�ط��߳�
	pthread_t pthd_info;
	pthread_attr_t attr_info;
	pthread_attr_init(&attr_info);
	pthread_attr_setdetachstate(&attr_info, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_info, 1024*512);
	pthread_create(&pthd_info, NULL, MainReplayRunInfo, NULL);

	printf("-----------------------------8.\n");


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
/***
//lBgnTime  ��ָ���طſ�ʼʱ��λ��
//lStartTime: �طų�������ʱ��
//lCurTime  : �طų���ǰ����ʱ��
// *plPickTime :ԭʼ��¼���ʱ��

#define MY_AM_MARKET_BEGIN_TIME	90000000
#define MY_AM_MARKET_OPEN_TIME	93000000
#define MY_AM_MARKET_CLOSE_TIME	113000000
#define MY_PM_MARKET_OPEN_TIME	130000000
#define MY_PM_MARKET_CLOSE_TIME	150000000
#define MY_PM_MARKET_STOP_TIME	153000000
#define MY_DAY_END_TIME		240000000



�ؼ��㣺
	�жϻط�ʱ��lCurTimeΪ��ֵʱ�����������У�
	�жϻط�ʱ���lCurTimeΪ��ֵʱ������ԭʼ��¼���ʱ�䣿
�߼���
	lCurTime==>lPickTime
	lConvCur2ReplayTime: ����ǰʱ��ת��Ϊ�ط�ʱ���߼����£�

	1��ͨ�� ָ���طſ�ʼʱ������������ط�ȫ������Ҫ����ʱ�䣻
	2������طų�������ʱ�䣬��������������ط�������Ҫ�೤ʱ�䣬�� ֱ��ӳ�����ʱ�䣻
	3������طų�������ʱ�䣬������������ʱ�䣬��������ʱ��ε�ӳ�䡣
**/

//����, ָ���طſ�ʼʱ��λ��, ������걾�λط���Ҫ�೤ʱ�䣬��λ�����룩
int GetReplayCostMSec(int nBgnTime)
{
	int iCostMSec=iSubMilliSec(MY_PM_MARKET_STOP_TIME,nBgnTime);

	//�����������ֱ�ӷ���
	if(iCostMSec<=3600*2500) return iCostMSec;

	//����� ����  11:30:00:000 ����ֱ�ӹ���Ϊ����һ��
	if(iCostMSec<3600*4000) return  3600*2500;

	//��������̣���۳��м����ʱ��
	return iCostMSec-3600*1500;
}
//���������󣬵�ǰ��ʣ����ٺ������ʹ��
int GetReplayDayLeftMSec(int nStartTime)
{
	return iSubMilliSec(MY_DAY_END_TIME,nStartTime);
}
//����������Ԥ�Ƴ�����ʲôʱ������ط�
int GetReplayEndTime(int nStartTime,int iCostMSec)
{
	int nEndTime=iAddMilliSec(nStartTime,iCostMSec);

	//���������һ������㣬��ٽ���ʱ�䣬��Զ���ᵽ��
	if(nEndTime<=MY_DAY_END_TIME) return nEndTime;

	return iSubMilliSec(nEndTime,MY_DAY_END_TIME);
}
//���ݼ���ó��ģ���ǰ��Ӧ�Ļط�λ�ã����룺MY_PM_MARKET_STOP_TIME�ĺ���������
//�����ReplayTime����ֵΪ
int GetReplayTimeByLeftMSec(int iEndLeftMSec)
{
	//���ʱ����Ѿ����������磬��ֱ�����ʱ��
	if(iEndLeftMSec<=3600*2500)
		return iSubMilliSec(MY_PM_MARKET_STOP_TIME,iEndLeftMSec);

	//���������磬��ֱ�Ӷ��һ����Сʱ
	return iSubMilliSec(MY_PM_MARKET_STOP_TIME,iEndLeftMSec+3600*1500);
}
//���������е�ǰʱ�䣬ӳ�䵽�ط�λ����ȥ
int nGetReplayTimeByCur(int nCurTime,int nStartTime,int nEndTime,
	int iCostMSec,int iDayLeftMSec)
{
	int iEndLeftMSec,iDay1EndLeftMSec;

	//һ���칻�����ط���������
	if(iDayLeftMSec>=iCostMSec){
		iEndLeftMSec=iSubMilliSec(nEndTime,nCurTime);
		return GetReplayTimeByLeftMSec(iEndLeftMSec);
	}

	//�ط�ʱ��һ�첻����,���죬֧�ֿ�1��
	//��ƻط��ʱ��Ϊ5Сʱ,����ֻ���ܿ�һ��.
	//ʣ��Ϊ�ڶ���ĵĺ�����

	//��ǰʱ��Ȼ�ſ�ʼʱ�仹���ˣ��������
	if(nCurTime<nStartTime){
		iEndLeftMSec=iSubMilliSec(nEndTime,nCurTime);

		return GetReplayTimeByLeftMSec(iEndLeftMSec);
	}

	iDay1EndLeftMSec=iSubMilliSec(MY_DAY_END_TIME,nCurTime);

	iEndLeftMSec=iDay1EndLeftMSec+(iCostMSec-iDayLeftMSec);

	return GetReplayTimeByLeftMSec(iEndLeftMSec);

}

void *MainReplayRunQh(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	SSEL2_Quotation *p=(SSEL2_Quotation*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/gta_qh_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(SSEL2_Quotation))+sizeof(long long);


	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}
		//ҵ��ʱ����ָ���طſ�ʼʱ��֮ǰ����������
		if(p->Time<lBgnTime)continue;
__delay:

		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

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

	printf("qh stock rt=%d,ct=%d cur process count =%d.\n",
		p->Time,nCurTime,iCount);

	return NULL;
}


void *MainReplayRunTh(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	SSEL2_Transaction *p=(SSEL2_Transaction*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/gta_th_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(SSEL2_Transaction))+sizeof(long long);


	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if(p->TradeTime<lBgnTime)continue;

__delay:
		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

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

	printf("th stock rt=%d,ct=%d cur process count =%d.\n",
		p->TradeTime,nCurTime,iCount);
	return NULL;
}

void *MainReplayRunAh(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	SSEL2_Auction *p=(SSEL2_Auction*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/gta_ah_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(SSEL2_Auction))+sizeof(long long);


	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if(p->Time<lBgnTime)continue;

__delay:
		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){			//����ָ����ʱ��
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

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

	printf("ah stock rt=%d,ct=%d cur process count =%d.\n",
		p->Time,nCurTime,iCount);

	return NULL;
}


void *MainReplayRunQz(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	SZSEL2_Quotation *p=(SZSEL2_Quotation*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/gta_qz_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(SZSEL2_Quotation))+sizeof(long long);


	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if((p->Time%MY_DATE_CEIL_LONG)<lBgnTime)continue;

__delay:
		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

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

	printf("qz stock rt=%d,ct=%d cur process count =%d.\n",
		(int)(p->Time%MY_DATE_CEIL_LONG),nCurTime,iCount);

	return NULL;
}


void *MainReplayRunTz(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	SZSEL2_Transaction *p=(SZSEL2_Transaction*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/gta_tz_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(SZSEL2_Transaction))+sizeof(long long);


	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if((p->TradeTime%MY_DATE_CEIL_LONG)<lBgnTime)continue;
__delay:
		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);


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

	printf("tz stock rt=%d,ct=%d cur process count =%d.\n",
		(int)(p->TradeTime%MY_DATE_CEIL_LONG),nCurTime,iCount);

	return NULL;
}

void *MainReplayRunOz(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	SZSEL2_Order *p=(SZSEL2_Order*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/gta_oz_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(SZSEL2_Order))+sizeof(long long);


	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if((p->Time%MY_DATE_CEIL_LONG)<lBgnTime)continue;

__delay:
		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

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

	printf("oz stock rt=%d,ct=%d cur process count =%d.\n",
		(int)(p->Time%MY_DATE_CEIL_LONG),nCurTime,iCount);

	return NULL;
}

void *MainReplayRunD31(void *)
{	
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	struct D31ItemStruct *p=(struct D31ItemStruct*)(sBuffer+sizeof(long long));

	sprintf(sInFileName,"%s/d31_gt_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(struct D31ItemStruct))+sizeof(long long);

	//ȷ����ʼʱ��
	nStartTime=	nGetHostCurTime();
	iCostMSec=	GetReplayCostMSec((int)lBgnTime);
	iDayLeftMSec=	GetReplayDayLeftMSec(nStartTime);
	nEndTime=	GetReplayEndTime(nStartTime,iCostMSec);

	//D31��ʽר�õ������ֶ�
	int nT0;
	char sTime[15];

	while(1){

		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		sFormatTime((time_t)p->nTradeTime,sTime);
		nT0=atoi(sTime+8)*1000;

		if(nT0<lBgnTime)continue;

__delay:
		nCurTime=nGetHostCurTime();
		nPickTime=(int)((*plPickTime)%MY_DATE_CEIL_LONG);
		nReplayTime=nGetReplayTimeByCur(nCurTime,nStartTime,nEndTime,iCostMSec,iDayLeftMSec);

		//���ʵ�����ݲɼ���ʱ��picktime
		//�ȵ�ǰϵͳʱ���Ӧ��Ӧ���طŵ���ʱ�����ˣ����̵߳ȴ�
		if(nPickTime>nReplayTime){
			//����ָ����ʱ��
			usleep(iDelayMilSec*1000);
			goto __delay;
		}

		//����multi-times������
		for(int j=0;j<iMultiTimes;j++){

			UTIL_Time stTime;
			PUTIL_GetLocalTime(&stTime);
			long long lCurTime=PUTIL_SystemTimeToDateTime(&stTime);

			//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
			SubData *subdata = new SubData;
			subdata->msgType = Msg_SZSEL2_Order;
			subdata->cur_time = lCurTime;
			subdata->data.assign((const char*)p, sizeof(SZSEL2_Order));

			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_D31Item,pCallBack, subdata)));

			pCallBack->m_ios->Post(task);

			iDiCnt=++iCount;
			iDiTime=(int)(nT0%MY_DATE_CEIL_LONG);
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);

	printf("di stock rt=%d,ct=%d cur process count =%d.\n",
		nT0,nCurTime,iCount);

	return NULL;
}

void *MainReplayRunInfo(void *)
{
	int iCount=0,nCurTime;
	while(1){
		sleep(iInfoSec);

		nCurTime=nGetHostCurTime();

/*		printf("qh=%-9dqht=%-10dth=%-9dtht=%-10dah=%-9daht=%-10d\n\
qz=%-9dqzt=%-10dtz=%-9dtzt=%-10doz=%-9dozt=%-10dt=%lld\n",
			iQhCnt,iQhTime,iThCnt,iThTime,iAhCnt,iAhTime,
			iQzCnt,iQzTime,iTzCnt,iTzTime,iOzCnt,iOzTime,lCurTime/1000);
*/
		if(iCount++%10==0)
			printf("qh(%-9d)\tth(%-9d)\tah(%-9d)\tqz(%-9d)\ttz(%-9d)\toz(%-9d)\tdi(%-9d)\tcurtime\n",
				 iQhTime,iThTime,iAhTime,iQzTime,iTzTime,iOzTime,iDiTime);
		printf("%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%d\n",
			iQhCnt,iThCnt,iAhCnt,iQzCnt,iTzCnt,iOzCnt,iDiCnt,nCurTime/1000);

	}
	return NULL;
}
