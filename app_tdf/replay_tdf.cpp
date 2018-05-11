//////////////////////////////////////////////////////////////////////////
/// @brief    ����API
///           ʵ�ֲ�ͬ��ʽ�Ķ��ĺ�ȡ�����ļ����ղ�ѯ
//////////////////////////////////////////////////////////////////////////
#include "CallBackBase.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/inotify.h>
#include <unistd.h>
#include "IoService.h"
#include "TinyThread.h"

#include "wwtiny.h"

#include "d31_item.h"

#include "public.h"

#include "callsupp.h"


#define MY_DATE_CEIL_LONG 1000000000L

int iWriteFlag=0,iDelayMilSec=100,iMultiTimes=1,iInfoSec=3,iFlag=1;

long lBgnTime=91500000L;
char sDispName[1024],sSourcePath[1024],sWorkRoot[1024],sReplayDate[15];

int iMktCnt=0,iTraCnt=0,iOrdCnt=0,iQueCnt=0,iDiCnt=0;
int iMktTime,iTraTime,iOrdTime,iQueTime,iDiTime;

CallBackBase *pCallBack;

void *MainReplayRunMkt(void *);
void *MainReplayRunTra(void *);
void *MainReplayRunOrd(void *);
void *MainReplayRunQue(void *);

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
		case 'f':
			iFlag=atoi(optarg);
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
			printf("   [-w (2 writetdf,other nowrite) ]\n");
			printf("   [-t (query delay mil sec def=100ms) ]\n");
			printf("   [-m (multi_times) ]\n");
			printf("   [-s (source-path) ]\n");
			printf("   [-o work-root-name ]\n");
			printf("   [-i (infosec def=3) ]\n");
			printf("   [-f (flag  def=1 mk,or,tr,qu; 2 mkt,ord,tra,que) ]\n");
			printf("   [-b (begintime def=9:15:00:000,format hhmmssNNN) ]\n");
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

	pthread_t pthd_mkt;
	pthread_attr_t attr_mkt;

	//����mkt�ط��߳�
	pthread_attr_init(&attr_mkt);
	pthread_attr_setdetachstate(&attr_mkt, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_mkt, 1024*512);
	pthread_create(&pthd_mkt, NULL, MainReplayRunMkt, NULL);

	printf("-----------------------------1.\n");
	
	pthread_t pthd_tra;
	pthread_attr_t attr_tra;

	//����tra�ط��߳�
	pthread_attr_init(&attr_tra);
	pthread_attr_setdetachstate(&attr_tra, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_tra, 1024*512);
	pthread_create(&pthd_tra, NULL, MainReplayRunTra, NULL);
	
	printf("-----------------------------2.\n");

	pthread_t pthd_ord;
	pthread_attr_t attr_ord;

	//����ord�ط��߳�
	pthread_attr_init(&attr_ord);
	pthread_attr_setdetachstate(&attr_ord, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_ord, 1024*512);
	pthread_create(&pthd_ord, NULL, MainReplayRunOrd, NULL);
	
	printf("-----------------------------3.\n");

	pthread_t pthd_que;
	pthread_attr_t attr_que;

	//����que�ط��߳�
	pthread_attr_init(&attr_que);
	pthread_attr_setdetachstate(&attr_que, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr_que, 1024*512);
	pthread_create(&pthd_que, NULL, MainReplayRunQue, NULL);

	printf("-----------------------------4.\n");

	pthread_t pthd_d31;
	pthread_attr_t attr_d31;

	//����D31�ط��߳�
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

void *MainReplayRunMkt(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	TDF_MARKET_DATA *p=(TDF_MARKET_DATA*)(sBuffer+sizeof(long long));

	if(iFlag==1)
		sprintf(sInFileName,"%s/tdf_mk_%s.dat",sSourcePath,sReplayDate);
	else	sprintf(sInFileName,"%s/tdf_mkt_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(TDF_MARKET_DATA))+sizeof(long long);


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

		if(p->nTime<lBgnTime)continue;

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
			MySubData *d = new MySubData;
			d->nItemCnt = 1;
			d->cur_time = lCurTime;
			d->data.assign((const char*)p, sizeof(TDF_MARKET_DATA));

			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_TdfMkt,pCallBack, d)));

			pCallBack->m_ios->Post(task);

//			if((++iCount)%50000==0)
//				printf("oz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);
			iMktCnt=++iCount;
			iMktTime=p->nTime;
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);

	printf("mkt stock rt=%d,ct=%d cur process count =%d.\n",
		(int)p->nTime,nCurTime,iCount);

	return NULL;
}
void *MainReplayRunTra(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	TDF_TRANSACTION *p=(TDF_TRANSACTION*)(sBuffer+sizeof(long long));

	if(iFlag==1)
		sprintf(sInFileName,"%s/tdf_tr_%s.dat",sSourcePath,sReplayDate);
	else	sprintf(sInFileName,"%s/tdf_tra_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(TDF_TRANSACTION))+sizeof(long long);


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

		if(p->nTime<lBgnTime)continue;

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
			MySubData *d = new MySubData;
			d->nItemCnt = 1;
			d->cur_time = lCurTime;
			d->data.assign((const char*)p, sizeof(TDF_TRANSACTION));

			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_TdfTra,pCallBack, d)));

			pCallBack->m_ios->Post(task);

//			if((++iCount)%50000==0)
//				printf("oz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);
			iTraCnt=++iCount;
			iTraTime=p->nTime;
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);

	printf("tra stock rt=%d,ct=%d cur process count =%d.\n",
		(int)p->nTime,nCurTime,iCount);

	return NULL;
}

void *MainReplayRunOrd(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	TDF_ORDER *p=(TDF_ORDER*)(sBuffer+sizeof(long long));

	if(iFlag==1)
		sprintf(sInFileName,"%s/tdf_or_%s.dat",sSourcePath,sReplayDate);
	else	sprintf(sInFileName,"%s/tdf_ord_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(TDF_ORDER))+sizeof(long long);


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

		if(p->nTime<lBgnTime)continue;

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
			MySubData *d = new MySubData;
			d->nItemCnt = 1;
			d->cur_time = lCurTime;
			d->data.assign((const char*)p, sizeof(TDF_ORDER));

			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_TdfOrd,pCallBack, d)));

			pCallBack->m_ios->Post(task);

//			if((++iCount)%50000==0)
//				printf("oz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);
			iOrdCnt=++iCount;
			iOrdTime=p->nTime;
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);

	printf("ord stock rt=%d,ct=%d cur process count =%d.\n",
		(int)p->nTime,nCurTime,iCount);

	return NULL;
}

void *MainReplayRunQue(void *)
{
	int iCount=0,nStartTime,nEndTime,nCurTime,nPickTime,nReplayTime,iCostMSec,iDayLeftMSec;
	char sInFileName[1024],sBuffer[10240];
	long lItemLen=0;
	long long *plPickTime=(long long*)sBuffer;

	FILE *fpIn;

	TDF_ORDER_QUEUE *p=(TDF_ORDER_QUEUE*)(sBuffer+sizeof(long long));

	if(iFlag==1)
		sprintf(sInFileName,"%s/tdf_qu_%s.dat",sSourcePath,sReplayDate);
	else	sprintf(sInFileName,"%s/tdf_que_%s.dat",sSourcePath,sReplayDate);

	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return NULL;
	}

	lItemLen=(sizeof(TDF_ORDER_QUEUE))+sizeof(long long);


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

		if(p->nTime<lBgnTime)continue;

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
			MySubData *d = new MySubData;
			d->nItemCnt = 1;
			d->cur_time = lCurTime;
			d->data.assign((const char*)p, sizeof(TDF_ORDER_QUEUE));

			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_TdfQue,pCallBack, d)));

			pCallBack->m_ios->Post(task);

//			if((++iCount)%50000==0)
//				printf("oz stock rt=%d,ct=%lld cur process count =%d.\n",
//					(int)(p->Time%MY_DATE_CEIL_LONG),lCurTime%MY_DATE_CEIL_LONG,iCount);
			iQueCnt=++iCount;
			iQueTime=p->nTime;
		}
		if(feof(fpIn)) break;
	}

	fclose(fpIn);

	printf("que stock rt=%d,ct=%d cur process count =%d.\n",
		(int)p->nTime,nCurTime,iCount);

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

	sprintf(sInFileName,"%s/d31_t3_%s.dat",sSourcePath,sReplayDate);

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
			MySubData *d = new MySubData;
			d->nItemCnt = 1;
			d->cur_time = lCurTime;
			d->data.assign((const char*)p, sizeof(struct D31ItemStruct));

			TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_D31Item,pCallBack, d)));

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
			printf("mkt(%-9d)\ttra(%-9d)\tord(%-9d)\tque(%-9d)\tdi(%-9d)\tcurtime\n",
				 iMktTime,iTraTime,iOrdTime,iQueTime,iDiTime);
		printf("%-9d\t%-9d\t%-9d\t%-9d\t%-9d\t%d\n",
			iMktCnt,iTraCnt,iOrdCnt,iQueCnt,iDiCnt,nCurTime/1000);

	}
	return NULL;
}
