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

int iWriteFlag=0,iDelayMilSec=100,iMultiTimes=1;
char sDispName[1024],sSourcePath[1024];

CallBackBase *pCallBack;

void *MainReplayRunQh(void *);
void *MainReplayRunTh(void *);
void *MainReplayRunAh(void *);

void *MainReplayRunQz(void *);
void *MainReplayRunTz(void *);
void *MainReplayRunOz(void *);


int main(int argc, char *argv[])
{

	string strWork;
	strcpy(sDispName,	"./disp.json");
	strcpy(sSourcePath,	"/stock/work");
	
	strWork=string(sSourcePath);

	for (int c; (c = getopt(argc, argv, "r:w:t:m:s:")) != EOF;){

		switch (c){
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
		case '?':
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-r disp-name ]\n");
			printf("   [-w (1,writegta,2 writetdf,other nowrite) ]\n");
			printf("   [-t (query delay mil sec def=100ms) ]\n");
			printf("   [-m (multi_times) ]\n");
			printf("   [-s (source-path) ]\n");
			exit(1);
			break;
		}
	}

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

	sprintf(sInFileName,"%s/gta_qh.dat",sSourcePath);
	
	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		exit(1);
	}
	
	lItemLen=(sizeof(SSEL2_Quotation))+sizeof(long long);

	
	//ȷ����ʼʱ��
	PUTIL_GetLocalTime(&sStartTime);
	
	//�����ʱ��ȷ��Ϊ9:30��
	lStartTime=	PUTIL_SystemTimeToDateTime(&sStartTime);
	lStartTime=	lStartTime%MY_DATE_CEIL_LONG;
	lDiffTime=	lStartTime-  93000000L;

	while(1){
		
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}
		if(feof(fpIn)) break;
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
			
			if((++iCount)%300==0)
				printf("qh stock rt=%d,ct=%lld cur process count =%d.\n",
					p->Time,lCurTime%MY_DATE_CEIL_LONG,iCount);
		}
	}

	fclose(fpIn);
	
	return NULL;
}

