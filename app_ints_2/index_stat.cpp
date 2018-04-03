
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#include "bintree.h"
#include "wwtiny.h"

#include "../Include/QTSStruct.h"
#include "../IncludeTDF/TDFAPIStruct.h"
#include "../GTA2TDF/GTA2TDF.h"


#define MY_DATE_CEIL_LONG 1000000000L
#define MAX_LEVEL_CNT	10

/*
int8b AddNewSec(int8b t,int iMilliSec)
{
}*/

//��֤ȯ��ߵ�ʱ�䣬ת��Ϊ1970�굽��ǰʱ�������
//YYYYMMDDHHMISSNNN
/*timt_t UDateTime2Time(int8b t)
{
	t%10000000000000 - 1970
}
*/
//���ṹ336���ֽ�
struct D31IndexItemStruct
{
	struct D31IndexItemStruct *pNext;
	int	iStockCode;	//��Ʊ����
	int	nActionDay;	//ί������(YYYYMMDD)
	int	nTime;		//ί��ʱ��(HHMMSSmmm)
	int8b	alBuyAmount[MAX_LEVEL_CNT];	//�����ȣ���λ���֣�
	int	aiBuyVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	alBuyOrderNo[MAX_LEVEL_CNT];	//�����������λ���ʣ�
	int8b	alSaleAmount[MAX_LEVEL_CNT];	//�����ȣ���λ���֣�
	int	aiSaleVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	aiSaleOrderNo[MAX_LEVEL_CNT];	//�����������λ���ʣ�
};
int iMaxLevel=MAX_LEVEL_CNT;
int aiAmntLevel[MAX_LEVEL_CNT]={
	0,
	5*10000*100,
	10*10000*100,
	20*10000*100,
	40*10000*100,
	60*10000*100,
	80*10000*100,
	100*10000*100,
	200*10000*100,
	500*10000*100
};
//ÿ���ṹ40���ֽڣ�16+4*6 = 40
struct TinyTransactionStruct
{
	struct TinyTransactionStruct *pNext;
	int	nActionDay;	//�ɽ�����(YYYYMMDD)
	int	nTime;		//�ɽ�ʱ��(HHMMSSmmm)
	int	iStockCode;	//֤ȯ����
	int	iPrice;		//�ɽ��۸�, nPrice==>iPrice,�ɽ��۸񲻻�ܴ���int���á�
	int 	nVolume;	//�ɽ���
	int	nAskOrder;	//����ί�����
	int	nBidOrder;	//��ί�����
	int     nBSFlag;        //��������(��'B', ����'S', ������' ')
};
//ÿ���ṹ40���ֽڣ�16+4*6 = 40
struct TinyOrderStruct
{
	struct TinyOrderStruct *pNext;
	int	nActionDay;	//ί������(YYYYMMDD)
	int	nTime;		//ί��ʱ��(HHMMSSmmm)
	int	iStockCode;	//֤ȯ����
	int	iPrice;		//ί�м۸�, nPrice==>iPrice���ɽ��۸񲻻�ܴ���int���á�
	int 	nVolume;	//�ɽ���
	int 	nOrder;	        //ί�к�
	int     nBroker;	//�����̱��
	int     nBSFlag;  	//ί�д���('B','S','C')
};

/**
10.00�����ɽ���˵��ί������ί�м� <=10.00Ԫ�������ֳɽ���>=10.00Ԫ ������ί�е��Ŵ��� �óɽ����ŵĽ��ף���رոóɽ�����
10.00���򵥳ɽ���˵��ί���򵥵�ί�м�>=10Ԫ�������ֳɽ���<=10.00Ԫ ����ί�е��Ŵ��� �óɽ����ŵĽ��ף���رոóɽ�����
**/
void TDF_TRANSACTION2TinyTransaction(struct TDF_TRANSACTION *pi,struct TinyTransactionStruct *po)
{
	po->pNext=NULL;
	po->nActionDay=	pi->nActionDay+20000000;
	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->iPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nAskOrder=	pi->nAskOrder;
	po->nBidOrder=	pi->nBidOrder;

	po->nBSFlag=	pi->nBSFlag;
}
void TDF_TRANSACTION2TinyOrderS(struct TDF_TRANSACTION *pi,struct TinyOrderStruct *po)
{
//	po->pNext=NULL;
//	po->nActionDay=	pi->nActionDay+20000000;
//	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->iPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nOrder=	pi->nAskOrder;
//	po->nBroker=	0;
//	po->nBSFlag=	(int)'S';
}
void TDF_TRANSACTION2TinyOrderB(struct TDF_TRANSACTION *pi,struct TinyOrderStruct *po)
{
//	po->pNext=NULL;
//	po->nActionDay=	pi->nActionDay+20000000;
//	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->iPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nOrder=	pi->nBidOrder;
//	po->nBroker=	0;
//	po->nBSFlag=	(int)'B';
}
void TDF_ORDER2TinyOrder(struct TDF_ORDER *pi,struct TinyOrderStruct *po)
{
//	po->pNext=NULL;
//	po->nActionDay=	pi->nActionDay+20000000;
//	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->iPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nOrder=	pi->nOrder;
//	po->nBroker=	pi->nBroker;
//	po->nBSFlag=	pi->chFunctionCode;
}

int iMaxDelaySec=20;
char sCalcDate[15],sCalcBgn[15],sSourcePath[1024],sWorkRoot[1024];
BINTREE *TRS_ORD_ROOT=NULL,*ORD_ROOT=NULL,*D31_ROOT=NULL;

int data_search_bintree_stock_code_order(void *pValue,void*pData)
{
	struct TinyOrderStruct *pSource=(struct TinyOrderStruct *)pValue;
	struct TinyOrderStruct *pTarget=(struct TinyOrderStruct *)pData;

/*���ݱȽϲ���*/
	int res=0;

	if((res=pSource->iStockCode-pTarget->iStockCode)!=0) return res;

	return pSource->nOrder-pTarget->nOrder;
}
void assign_insert_bintree_stock_code_order_e(void **ppData,void *pData)
{
	*ppData=pData;
}

int AddThData2TrsOrdTree(FILE *fpIn,int nBgnActionDay,int nBgnTime,BINTREE **PP)
{
	int nCurActionDay,nCurTime,iCount=0;
	long lItemLen=(sizeof(SSEL2_Transaction))+sizeof(long long);
	char sBuffer[2048];
	long long *pll=(long long*)sBuffer;
	SSEL2_Transaction *p=(SSEL2_Transaction*)(sBuffer+sizeof(long long));
	TDF_TRANSACTION t;
	struct TinyOrderStruct ob,os,*ptCur,*pTemp;

	while(1){
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("mount process th all count=%d.\n",iCount);
			printf("end of file break.\n");
			break;
		}
		
		nCurActionDay=(int)((*pll)/MY_DATE_CEIL_LONG);
		nCurTime=(int)((*pll)%MY_DATE_CEIL_LONG)/1000;
		
		//����һ������ݣ���ֱ���˳�
		if(nCurActionDay!=nBgnActionDay) break;
			
		if(nCurTime<nBgnTime) continue;
		
		
		if((++iCount)%500000==0)
			printf("mount process th count=%d.\n",iCount);

		GTA2TDF_SSEL2_T(p[0],t);
		
		TDF_TRANSACTION2TinyOrderS(&t,&os);
		TDF_TRANSACTION2TinyOrderB(&t,&ob);
		
		if(SearchBin(*PP,&os,data_search_bintree_stock_code_order,
			(void**)&ptCur)==NOTFOUND){

			if((pTemp=(struct TinyOrderStruct*)malloc(
				sizeof(struct TinyOrderStruct)))==NULL){

				printf("malloc TinyOrderStruct error th.\n");
				return -1;
			}
			memcpy((void*)pTemp,(void*)&os,sizeof(struct TinyOrderStruct));

			if(InsertBin(PP,(void *)pTemp,
				data_search_bintree_stock_code_order,
				assign_insert_bintree_stock_code_order_e)<0){
				printf("insert bin malloc error th.\n");
				return -1;
			}
		}
		else
			ptCur->nVolume+=os.nVolume;
			
			
		if(SearchBin(*PP,&ob,data_search_bintree_stock_code_order,
			(void**)&ptCur)==NOTFOUND){

			if((pTemp=(struct TinyOrderStruct*)malloc(
				sizeof(struct TinyOrderStruct)))==NULL){

				printf("malloc TinyOrderStruct error th1.\n");
				return -1;
			}
			memcpy((void*)pTemp,(void*)&ob,sizeof(struct TinyOrderStruct));

			if(InsertBin(PP,(void *)pTemp,
				data_search_bintree_stock_code_order,
				assign_insert_bintree_stock_code_order_e)<0){
				printf("insert bin malloc error th1.\n");
				return -1;
			}
		}
		else
			ptCur->nVolume+=os.nVolume;
		
		
	}
	return 0;
}

int AddTzData2TrsOrdTree(FILE *fpIn,int nBgnActionDay,int nBgnTime,BINTREE **PP)
{
	int nCurActionDay,nCurTime,iCount=0;
	long lItemLen=(sizeof(SZSEL2_Transaction))+sizeof(long long);
	char sBuffer[2048];
	long long *pll=(long long*)sBuffer;
	SZSEL2_Transaction *p=(SZSEL2_Transaction*)(sBuffer+sizeof(long long));
	TDF_TRANSACTION t;
	struct TinyOrderStruct ob,os,*ptCur,*pTemp;

	while(1){
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("mount process tz all count=%d.\n",iCount);
			printf("end of file break.\n");
			break;
		}
		
		nCurActionDay=(int)((*pll)/MY_DATE_CEIL_LONG);
		nCurTime=(int)((*pll)%MY_DATE_CEIL_LONG)/1000;
		
		//����һ������ݣ���ֱ���˳�
		if(nCurActionDay!=nBgnActionDay) break;
			
		if(nCurTime<nBgnTime) continue;
		
		if((++iCount)%500000==0)
			printf("mount process tz count=%d.\n",iCount);

		GTA2TDF_SZSEL2_T(p[0],t);
		
		TDF_TRANSACTION2TinyOrderS(&t,&os);
		TDF_TRANSACTION2TinyOrderB(&t,&ob);
		
		if(SearchBin(*PP,&os,data_search_bintree_stock_code_order,
			(void**)&ptCur)==NOTFOUND){

			if((pTemp=(struct TinyOrderStruct*)malloc(
				sizeof(struct TinyOrderStruct)))==NULL){

				printf("malloc TinyOrderStruct error tz.\n");
				return -1;
			}
			memcpy((void*)pTemp,(void*)&os,sizeof(struct TinyOrderStruct));

			if(InsertBin(PP,(void *)pTemp,
				data_search_bintree_stock_code_order,
				assign_insert_bintree_stock_code_order_e)<0){
				printf("insert bin malloc error tz.\n");
				return -1;
			}
		}
		else
			ptCur->nVolume+=os.nVolume;
			
			
		if(SearchBin(*PP,&ob,data_search_bintree_stock_code_order,
			(void**)&ptCur)==NOTFOUND){

			if((pTemp=(struct TinyOrderStruct*)malloc(
				sizeof(struct TinyOrderStruct)))==NULL){

				printf("malloc TinyOrderStruct error tz1.\n");
				return -1;
			}
			memcpy((void*)pTemp,(void*)&ob,sizeof(struct TinyOrderStruct));

			if(InsertBin(PP,(void *)pTemp,
				data_search_bintree_stock_code_order,
				assign_insert_bintree_stock_code_order_e)<0){
				printf("insert bin malloc error tz1.\n");
				return -1;
			}
		}
		else
			ptCur->nVolume+=os.nVolume;
		
		
	}
	return 0;
}

int AddOzData2OrdTree(FILE *fpIn,int nBgnActionDay,int nBgnTime,BINTREE **PP)
{
	int nCurActionDay,nCurTime,iCount=0;
	long lItemLen=(sizeof(SZSEL2_Order))+sizeof(long long);
	char sBuffer[2048];
	long long *pll=(long long*)sBuffer;
	SZSEL2_Order *p=(SZSEL2_Order*)(sBuffer+sizeof(long long));
	TDF_ORDER od;
	struct TinyOrderStruct o,*ptCur,*pTemp;

	while(1){
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("mount process oz all count=%d.\n",iCount);
			printf("end of file break.\n");
			break;
		}
		
		nCurActionDay=(int)((*pll)/MY_DATE_CEIL_LONG);
		nCurTime=(int)((*pll)%MY_DATE_CEIL_LONG)/1000;
		
		//����һ������ݣ���ֱ���˳�
		if(nCurActionDay!=nBgnActionDay) break;
			
		if(nCurTime<nBgnTime) continue;
		
		if((++iCount)%500000==0)
			printf("mount process oz count=%d.\n",iCount);

		GTA2TDF_SZSEL2_O(p[0],od);
		
		TDF_ORDER2TinyOrder(&od,&o);
		
		if(SearchBin(*PP,&o,data_search_bintree_stock_code_order,
			(void**)&ptCur)==NOTFOUND){

			if((pTemp=(struct TinyOrderStruct*)malloc(
				sizeof(struct TinyOrderStruct)))==NULL){

				printf("malloc TinyOrderStruct error oz.\n");
				return -1;
			}
			memcpy((void*)pTemp,(void*)&o,sizeof(struct TinyOrderStruct));

			if(InsertBin(PP,(void *)pTemp,
				data_search_bintree_stock_code_order,
				assign_insert_bintree_stock_code_order_e)<0){
				printf("insert bin malloc error oz.\n");
				return -1;
			}
		}
		else
			ptCur->nVolume+=o.nVolume;
		
		
	}
	return 0;
}

int main(int argc, char *argv[])
{
	FILE *fpThIn,*fpTzIn,*fpOzIn;
	char sCurTime[16];
	char sGtaThName[1024],sGtaTzName[1024],sGtaOzName[1024];
	
	int nBgnActionDay,nBgnTime;

	GetHostTime(sCurTime);

	//���ò���ѡ���Ĭ��ֵ
	strncpy(sCalcDate,sCurTime,8);sCalcDate[8]=0;
	strcpy(sCalcBgn,sCurTime+8);
	strcpy(sSourcePath,	"/stock/work");
	iMaxDelaySec=20;
	strcpy(sWorkRoot,	"/stock/work");


	//��ȡ�����в���
	for (int c; (c = getopt(argc, argv, "d:b:m:s:o:?:")) != EOF;){

		switch (c){
		case 'd':
			strncpy(sCalcDate, optarg,8);sCalcDate[8]=0;
			break;
		case 'b':
			strncpy(sCalcBgn, optarg,6);sCalcBgn[6]=0;
			break;
		case 'm':
			iMaxDelaySec=atoi(optarg);
			break;
		case 's':
			strcpy(sSourcePath, optarg);
			break;
		case 'o':
			strcpy(sWorkRoot, optarg);
			break;
		case '?':
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-d calc-date (yyyymmdd =8Bytes def=curdate) ]\n");
			printf("   [-b begin-time (HH24MISS =6Bytes def=curtime) ]\n");
			printf("   [-m max-delay-sec (def=20) ]\n");
			printf("   [-s source-path (def=/stock/work) ]\n");
			printf("   [-o work-root-name (def=/stock/work) ]\n");
			exit(1);
			break;
		}
	}

	//���������ļ���
	sprintf(sGtaThName,"%s/gta_th_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaTzName,"%s/gta_tz_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaOzName,"%s/gta_oz_%s.dat",sSourcePath,sCalcDate);

	if((fpThIn=fopen(sGtaThName,"r"))==NULL){
		printf("error open file %s to read.\n",sGtaThName);
		return -1;
	}
	if((fpTzIn=fopen(sGtaTzName,"r"))==NULL){
		printf("error open file %s to read.\n",sGtaTzName);
		return -1;
	}
	if((fpOzIn=fopen(sGtaOzName,"r"))==NULL){
		printf("error open file %s to read.\n",sGtaOzName);
		return -1;
	}
	//�õ������
	nBgnActionDay=	atoi(sCalcDate);
	nBgnTime=	atoi(sCalcBgn);
	
/*
	һ�ѽ��������ݼ��ص��ڴ��� �е�ɲ���.
*/
	if(AddThData2TrsOrdTree(fpThIn,nBgnActionDay,nBgnTime,&TRS_ORD_ROOT)<0) return -1;
	if(AddTzData2TrsOrdTree(fpTzIn,nBgnActionDay,nBgnTime,&TRS_ORD_ROOT)<0) return -1;
	if(AddOzData2OrdTree(fpOzIn,nBgnActionDay,nBgnTime,&ORD_ROOT)<0) return -1;
	

sleep(300);
	printf("hello world.\n");

	fclose(fpThIn);
	fclose(fpTzIn);
	fclose(fpOzIn);

	return 0;
}