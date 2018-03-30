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
#include <boost/property_tree/json_parser.hpp>
#include "../Common/IoService.h"
#include "../Common/TinyThread.h"

#include "../VecStockCode/VectorStockCodeSH.h"
#include "../VecStockCode/VectorStockCodeSZ.h"

#include "../GTA2TDF/GTA2TDF.h"


#include "../Common/public.h"

int iDebugFlag=0,iWriteFlag=0,iDelayMilSec=100,iMultiTimes=1;
char sCfgJsonName[1024],sDispName[1024],sPrivilegeName[1024],sWorkRoot[1024];

string strCodesSH,strCodesSZ;
CallBackBase *pCallBack;
IGTAQTSApiBase*pGtaApiBase;

void *MainQryRunSz(void *);
void *MainQryRunSh(void *);

int main(int argc, char *argv[])
{

	strcpy(sCfgJsonName,	"./gta_ints.json");
	strcpy(sDispName,	"./disp.json");
	strcpy(sPrivilegeName,	"./user_privilege.json");
	strcpy(sWorkRoot,	"/stock/work");

	for (int c; (c = getopt(argc, argv, "d:c:r:u:w:t:m:h:")) != EOF;){

		switch (c){
		case 'd':
			iDebugFlag = atoi(optarg);
			break;
		case 'c':
			strcpy(sCfgJsonName, optarg);
			break;
		case 'r':
			strcpy(sDispName, optarg);
			break;
		case 'u':
			strcpy(sPrivilegeName, optarg);
			break;
		case 'o':
			strcpy(sWorkRoot, optarg);
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
		case '?':
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-c cfg-name ]\n");
			printf("   [-r disp-name ]\n");
			printf("   [-u privilege-name ]\n");
			printf("   [-o work-root-name ]\n");
			printf("   [-d DebugFlag ]\n");
			printf("   [-w (1,writegta,2 writetdf,other nowrite) ]\n");
			printf("   [-t (query delay mil sec def=100ms) ]\n");
			printf("   [-m (multi_times) ]\n");
			exit(1);
			break;
		}
	}

	//��ʼ��ˢ�����飬�Լ�ˢ���ļ�����ȫ�ֱ���
	InitUserArray(sDispName,&R);

	//ˢ��һ�²���������Ҫ��disp����������������������
	RefreshUserArray(sDispName,&R);

	uint16_t port;
	string host,id,passwd,strWork;
	boost::property_tree::ptree tRoot,t;

	strWork=	string(sWorkRoot);
	try{
		boost::property_tree::read_json(sCfgJsonName,tRoot);
		t 	= tRoot.get_child("gta_server");
		id 	= tRoot.get<string>("id");
		passwd 	= tRoot.get<string>("passwd");
	}
	catch (...) {
		printf("�ļ� %s �����ڻ��ʽ�Ƿ�.\n",sCfgJsonName);
		exit(1);
	}

	IoService	ios;
	//������Ϣ�ص���
	CallBackBase CallbackBase(iWriteFlag,strWork);
	CallbackBase.SetIoService(&ios);

	//�����������ݷ���
	ios.Start();

	//��������API����
	//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.2.1.1 ����ʵ��CreateInstance �½�
	IGTAQTSApiBase* pApiBase = IGTAQTSApiBase::CreateInstance(CallbackBase);
	// ע��FENS��ַ,�����ע��������ݸ߿��÷��棬�����ۿۡ�
	for (auto it = t.begin(); it != t.end(); ++it) {
		try {
			host = it->second.get<string>("host");
			port = it->second.get<uint16_t>("port");

			pApiBase->RegisterService(host.c_str(), port);
		}
		catch (...) {
			break;
		}
	}
	//���ó�ʱʱ��
	pApiBase->SetTimeout(30);

	do{
		//ͨ���û������������������½
		//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.2.1.5 �û���֤Login �½�
		RetCode  ret = pApiBase->LoginX(id.c_str(), passwd.c_str(), "NetType=0");
		if (Ret_Success != ret){
			printf("Login error:%d\n", ret);
			break;
		}

		//****************************** ��ȡ֤ȯ�����б�Ȩ���б�****************************************
		CDataBuffer<StockSymbol> StockList1;

		// ��ȡ�Ͻ�������������б�����SSE��ʾ�Ͻ�����SZSE��ʾ�����CFFEX��ʾ�н���
		//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.2.1.11 ��ȡ�����б�GetStockList �½�
		ret = pApiBase->GetStockList((char*)"sse,szse", StockList1);
		if (Ret_Success != ret){
			printf("GetStockList(sse,szse) error:%d\n", ret);
			break;
		}

		StockSymbol* pStock = StockList1;
		const int sz = StockList1.Size();
		VectorStockCodeSH vSH;
		VectorStockCodeSZ vSZ;

		for (int i = 0; i < sz; ++i) {
			if (!(vSH.push(pStock[i].Symbol))) {
				vSZ.push(pStock[i].Symbol);
			}
		}
		printf("\n");

		CDataBuffer<MsgType> DataTypeList;
		// ��ȡȨ���б�
		//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.1.1.7 ��ȡȨ���б�GetDataTypeList �½�
		ret = pApiBase->GetDataTypeList(DataTypeList);
		if (Ret_Success != ret){
			printf("GetDataTypeList(sse) error:%d\n", ret);
			break;
		}

		MsgType* pMsg = DataTypeList;
		int Count = DataTypeList.Size();
		printf("MsgType Count = %d, List:", Count);
		for (int i = 0; i < Count; ++i)
		{
			printf("Ox%08x, ", pMsg[i]);
		}
		printf("\n");

		// �����붩�����ʵʱ��������
		//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.1.1.8 ����ʵʱ����Subscribe �½�
		vSH.strForSub(strCodesSH);
		vSZ.strForSub(strCodesSZ);

		//����һ���̲߳�ѯ���ݣ��������ݼӵ������߳���
		pCallBack=	&CallbackBase;
		pGtaApiBase=	pApiBase;

		pthread_t pthd,pthd1;
		pthread_attr_t attr,attr1;

		//���������г���ѯ�߳�
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_attr_setstacksize(&attr, 1024*512);
		pthread_create(&pthd, NULL, MainQryRunSz, NULL);

		//�����Ϻ��г���ѯ�߳�
		pthread_attr_init(&attr1);
		pthread_attr_setdetachstate(&attr1, PTHREAD_CREATE_DETACHED);
		pthread_attr_setstacksize(&attr1, 1024*512);
		pthread_create(&pthd1, NULL, MainQryRunSh, NULL);
	}
	while (false);

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

	// ������ʹ��APIʱ����Ҫ���ô˽ӿ��ͷ��ڲ���Դ������������ڴ�й©������Ԥ֪����
	//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.2.1.2 �ͷ�ʵ��ReleaseInstance �½�
	IGTAQTSApiBase::ReleaseInstance(pApiBase);

	return 0;
}

void *MainQryRunSz(void *)
{

	int ret,iCount=0;
	// ���ʵʱ������ղ�ѯ����������ͨ��Snap_Quotation�ṹ����
	CDataBuffer<SZSEL2_Quotation> szSnap_Quotation;

	while(1){

		ret = pGtaApiBase->QuerySnap_SZSEL2_Quotation((char*)(strCodesSZ.c_str()), szSnap_Quotation);
		if (Ret_Success != ret){
			printf("Login error:%d\n", ret);
			break;
		}

		if(iDebugFlag){
			SZSEL2_Quotation* pSse;
			if ((pSse = szSnap_Quotation) != NULL){
				printf("QuerySnap_Base(sz):Count = %d, LocalTimeStamp = %d, Symbol = %s, OpenPrice = %f, TotalAmount = %f\n",
					szSnap_Quotation.Size(), pSse->LocalTimeStamp,
					pSse->Symbol, pSse->OpenPrice, pSse->TotalAmount);
			}
		}

		// ��ȡȫ������
		for (int i = 0; i < szSnap_Quotation.Size(); ++i){

			SZSEL2_Quotation& RealSZSEL2Quotation = szSnap_Quotation[i];

			//����multi-times������
			for(int j=0;j<iMultiTimes;j++){

				UTIL_Time stTime;
				PUTIL_GetLocalTime(&stTime);

				//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
				SubData *subdata = new SubData;
				subdata->msgType = Msg_SZSEL2_Quotation;
				subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
				subdata->data.assign((const char*)&RealSZSEL2Quotation, sizeof(SZSEL2_Quotation));

				TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SZSEL2_Quotation,pCallBack, subdata)));

				pCallBack->m_ios->Post(task);

				if((++iCount)%30000==0)
					printf("sz stock cur process count =%d----------------------------.\n",iCount);
			}

		}

		//����ָ����ʱ��
		usleep(iDelayMilSec*1000);
	}
	return NULL;
}

void *MainQryRunSh(void *)
{

	int ret,iCount=0;
	// ���ʵʱ������ղ�ѯ����������ͨ��Snap_Quotation�ṹ����
	CDataBuffer<SSEL2_Quotation> sSnap_Quotation;

	while(1){

		ret = pGtaApiBase->QuerySnap_SSEL2_Quotation((char*)(strCodesSH.c_str()), sSnap_Quotation);
		if (Ret_Success != ret){
			printf("Login error:%d\n", ret);
			break;
		}

		if(iDebugFlag){
			SSEL2_Quotation* pSse;
			if ((pSse = sSnap_Quotation) != NULL){
				printf("QuerySnap_Base(sh):Count = %d, LocalTimeStamp = %d, Symbol = %s, OpenPrice = %f, TotalAmount = %f\n",
					sSnap_Quotation.Size(), pSse->LocalTimeStamp,
					pSse->Symbol, pSse->OpenPrice, pSse->TotalAmount);
			}
		}

		// ��ȡȫ������
		for (int i = 0; i < sSnap_Quotation.Size(); ++i){

			SSEL2_Quotation& RealSSEL2Quotation = sSnap_Quotation[i];

			//����multi-times������
			for(int j=0;j<iMultiTimes;j++){

				UTIL_Time stTime;
				PUTIL_GetLocalTime(&stTime);

				//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
				SubData *subdata = new SubData;
				subdata->msgType = Msg_SSEL2_Quotation;
				subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
				subdata->data.assign((const char*)&RealSSEL2Quotation, sizeof(SSEL2_Quotation));

				TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message_SSEL2_Quotation,pCallBack, subdata)));

				pCallBack->m_ios->Post(task);

				if((++iCount)%30000==0)
					printf("sh stock cur process count =%d----------------------------.\n",iCount);
			}

		}

		//����ָ����ʱ��
		usleep(iDelayMilSec*1000);
	}
	return NULL;
}
