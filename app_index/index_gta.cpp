#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#include "bintree.h"
#include "wwtiny.h"

#include "QTSStruct.h"
#include "index_supp.h"
#include "gta_supp.h"


void  (*GTA2TDF_Q2T)(void *p, TDF_TRANSACTION *pt);
void (*GTA_Quotation2TinyQuotation)(void *p, struct TinyQuotationStruct *po);

void GTA2TDF_QH2T(void *p, TDF_TRANSACTION *pt)
{
	SSEL2_Transaction *pi=(SSEL2_Transaction *)p;

	//char    szCode[32];                 //ԭʼCode
	strcpy(pt->szCode, pi->Symbol);
	//char    szWindCode[32];             //600001.SH
	strcpy(pt->szWindCode, pi->Symbol);
	pt->szWindCode[6] = '.';
	pt->szWindCode[7] = 'S';
	pt->szWindCode[8] = 'H';
	//int     nActionDay;                 //��Ȼ��
	pt->nActionDay = int(pi->PacketTimeStamp / 1000000000);
	//int 	nTime;		                //�ɽ�ʱ��(HHMMSSmmm)
	pt->nTime = pi->TradeTime;
	//int 	nIndex;		                //�ɽ����
	pt->nIndex = pi->RecID;
	//__int64		nPrice;		            //�ɽ��۸�
	pt->nPrice = yuan2percentFen(pi->TradePrice);
	//int 	nVolume;	                //�ɽ�����
	pt->nVolume = pi->TradeVolume;
	//__int64		nTurnover;	            //�ɽ����
	pt->nTurnover = yuan2percentFen(pi->TradeAmount);
	//int     nBSFlag;                    //��������(��'B', ����'S', ������' ')
	switch (pi->BuySellFlag) {
		case 'B':
		case 'b':
			pt->nBSFlag = 'B';
			break;
		case 'S':
		case 's':
			pt->nBSFlag = 'S';
			break;
		default:
			if (pi->BuyRecID > pi->SellRecID)
				pt->nBSFlag = 'B';
			else
				pt->nBSFlag = 'S';
	}
	//char    chOrderKind;                //�ɽ����
	pt->chOrderKind = 0;
	//char    chFunctionCode;             //�ɽ�����
	pt->chFunctionCode = 0;
	//int	    nAskOrder;	                //������ί�����
	pt->nAskOrder = int(pi->SellRecID);
	//int	    nBidOrder;	                //����ί�����
	pt->nBidOrder = int(pi->BuyRecID);
}
void  GTA2TDF_QZ2T(void *p, TDF_TRANSACTION *pt)
{
	SZSEL2_Transaction *pi=(SZSEL2_Transaction *)p;

	//char    szWindCode[32];             //600001.SH
	strcpy(pt->szWindCode, pi->Symbol);
	pt->szWindCode[6] = '.';
	pt->szWindCode[7] = 'S';
	pt->szWindCode[8] = 'Z';
	pt->szWindCode[9] = '\0';
	//char    szCode[32];                 //ԭʼCode
	strcpy(pt->szCode, pi->Symbol);
	//int     nActionDay;                 //��Ȼ��
	pt->nActionDay = int(pi->TradeTime / 1000000000);
	//int 	nTime;		                //�ɽ�ʱ��(HHMMSSmmm)
	pt->nTime = pi->TradeTime % 1000000000;
	//int 	nIndex;		                //�ɽ����
	pt->nIndex = int(pi->RecID);
	//__int64		nPrice;		            //�ɽ��۸�
	pt->nPrice = yuan2percentFen(pi->TradePrice);
	//int 	nVolume;	                //�ɽ�����
	pt->nVolume = int(pi->TradeVolume + 0.5);
	//__int64		nTurnover;	            //�ɽ����
	pt->nTurnover = pt->nPrice * pt->nVolume;
	//char    chOrderKind;                //�ɽ����
	pt->chOrderKind = '0';
	//char    chFunctionCode;             //�ɽ�����
	switch (pi->TradeType) {
		case '4':
			pt->chFunctionCode = 'C';
			pt->nBSFlag = ' ';
			break;
		default:
			pt->chFunctionCode = '0';
			//int     nBSFlag;                    //��������(��'B', ����'S', ������' ')
			if (pi->BuyOrderID > pi->SellOrderID)
				pt->nBSFlag = 'B';
			else
				pt->nBSFlag = 'S';
	}
	//int	    nAskOrder;	                //������ί�����
	pt->nAskOrder = int(pi->SellOrderID);
	//int	    nBidOrder;	                //����ί�����
	pt->nBidOrder = int(pi->BuyOrderID);
}
void SSEL2_Quotation2TinyQuotation(void *p, struct TinyQuotationStruct *po)
{

	SSEL2_Quotation *pi=(SSEL2_Quotation*)p;

	po->iStockCode=	atoi(pi->Symbol);
	po->nActionDay = int(pi->PacketTimeStamp / 1000000000);
	po->nTime = 	pi->Time;
	po->nPreClose = (int)yuan2percentFen(pi->PreClosePrice);
	po->nOpen = 	(int)yuan2percentFen(pi->OpenPrice);
	po->nHigh = 	(int)yuan2percentFen(pi->HighPrice);
	po->nLow = 	(int)yuan2percentFen(pi->LowPrice);
	po->nMatch = 	(int)yuan2percentFen(pi->LastPrice);
	for (unsigned i = 0; i < 10; ++i) {
		po->nAskVol[i] = (int)pi->SellLevel[i].Volume;
		po->nAskPrice[i] = (int)yuan2percentFen(pi->SellLevel[i].Price);
	}
	for (unsigned i = 0; i < 10; ++i) {
		po->nBidVol[i] = (int)pi->BuyLevel[i].Volume;
		po->nBidPrice[i] = (int)yuan2percentFen(pi->BuyLevel[i].Price);
	}

	po->nNumTrades = int(pi->TotalNo);
	po->iVolume = 		(int8b)pi->TotalVolume;
	po->iTurnover = 	(int8b)(pi->TotalAmount);
	po->nTotalBidVol = 	(int8b)pi->TotalBuyOrderVolume;
	po->nTotalAskVol = 	(int8b)pi->TotalSellOrderVolume;
	po->nWtAvgBidPrice = 	(int)yuan2percentFen(pi->WtAvgBuyPrice);
	po->nWtAvgAskPrice = 	(int)yuan2percentFen(pi->WtAvgSellPrice);
	po->iSamplingFlag=0;
}
void SZSEL2_Quotation2TinyQuotation(void *p, struct TinyQuotationStruct *po)
{

	SZSEL2_Quotation *pi=(SZSEL2_Quotation*)p;

	po->iStockCode=	atoi(pi->Symbol);

	po->nActionDay = int(pi->Time / 1000000000);
	po->nTime = pi->Time % 1000000000;

	po->nPreClose = (int)yuan2percentFen(pi->PreClosePrice);
	po->nOpen = 	(int)yuan2percentFen(pi->OpenPrice);
	po->nHigh = 	(int)yuan2percentFen(pi->HighPrice);
	po->nLow = 	(int)yuan2percentFen(pi->LowPrice);
	po->nMatch = 	(int)yuan2percentFen(pi->LastPrice);

	for (unsigned i = 0; i < LEVEL_TEN; ++i) {
		po->nAskVol[i] = 	(int)(pi->SellLevel[i].Volume);
		po->nAskPrice[i] = 	(int)yuan2percentFen(pi->SellLevel[i].Price);
	}
	for (unsigned i = 0; i < LEVEL_TEN; ++i) {
		po->nBidVol[i] = 	(int)(pi->BuyLevel[i].Volume);
		po->nBidPrice[i] = 	(int)yuan2percentFen(pi->BuyLevel[i].Price);
	}

	po->nNumTrades =int(pi->TotalNo);
	po->iVolume = 	(int8b)(pi->TotalVolume + 0.5);
	po->iTurnover = (int8b)(pi->TotalAmount);
	po->nTotalBidVol = (int8b)(pi->TotalBuyOrderVolume + 0.5);
	po->nTotalAskVol = (int8b)(pi->TotalSellOrderVolume + 0.5);
	po->nWtAvgBidPrice =(int)yuan2percentFen(pi->WtAvgBuyPrice);
	po->nWtAvgAskPrice =(int)yuan2percentFen(pi->WtAvgSellPrice);
	po->iSamplingFlag=0;
}


/**
��������
	TAIL_NO_STAT	�����ļ�ĩβ��δͳ��
	WANT_STAT	����nEndTime0��������Ҫͳ��
	-1	�������
**/


int MountTrsData2IndexStatArray(char sFileName[],int nBgnActionDay,
	int nPreT0,int nT0,int nEndTime0,time_t tBeginTime,
	long lItemLen,char sCodeStr[],long *plCurPos)
{
	FILE *fp;
	int iRet;
	char sBuffer[2048];
//	long long *pll=(long long*)sBuffer;
	void *p=(void*)(sBuffer+sizeof(long long));
	TDF_TRANSACTION t;
	struct TinyTransactionStruct tt,*pt;

	LISTHEAD *pSXT;

	long lCurPos=*plCurPos;

	struct IndexStatStruct *pIndexStat;

	if((fp=fopen(sFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sFileName);
		return -1;
	}

	if(fseek(fp,lCurPos,SEEK_SET)<0){
		printf("error fseek file=%s,pos=%ld.\n",sFileName,lCurPos);
		return -1;
	}
	while(1){
		if(fread((void*)sBuffer,lItemLen,1,fp)!=1){
			iRet=MY_TAIL_NO_STAT;
			break;
		}

		lCurPos+=lItemLen;


		//����ʽת��ΪTRANSACTION
		GTA2TDF_Q2T(p,&t);

		if(CodeInCodeStr(t.szCode,sCodeStr)==false) continue;

		if(t.nActionDay!=nBgnActionDay) continue;
/*		{
			printf("error date file=%s,pos=%ld,actionday=%d,calcdate=%d\n",
				sFileName,lCurPos,t.nActionDay,nBgnActionDay);
			return -1;
		}
*/
		if((pIndexStat=GetIndexStat(atoi(t.szCode),sFileName,
			lCurPos,nBgnActionDay,nPreT0,nT0))==NULL) return -1;

		//���˵�������Ϣ
		if(t.chFunctionCode=='C') continue;

/*		if(iStockCode==2320){
			printf("hello my stock trans.\n");
		}
*/
		//��Transaction��������Order���ݣ���ӵ�Merge�ṹ��
		if(AddTransaction2IndexStat(&t,nT0,pIndexStat)<0){
			printf("error add transaction to index stat file=%s,pos=%ld\n",
			sFileName,lCurPos);
			return -1;
		}

		TDF_TRANSACTION2TinyTransaction(&t,&tt);
/*
		if(tt.nTime==93459180&&tt.nAskOrder==255798){
			printf("find U little cat.\n");
		}
*/
                //�����۸�����ĳɽ�ORDER֮��Ĺ�ϵ
/*               if(AdjustIndexStatS0(pIndexStat,&tt)<0){
			printf("error add index stat s0 file=%s,pos=%ld\n",
				sFileName,lCurPos);
			return -1;
                }
*/
		//��lCurPos,���� nT0, ���ڴ��� nPreT0�Ĳ��֣����뵽
		//S0T������nT0�����ݼӵ�pS1Head,��������������nEndTime0������ֹͣ;
//		if(t.nTime<nPreT0) continue;

		//ʵ�����,ȡ��Ϊ nPreT0<=x<nT0 �����ȥȡ����
		//���,t.nTimeΪ����ʱ��Ҳ�� 15:00:00�������
		if(MY_BELONG_TRANSACTION_T0(t.nTime,nT0)){
//		if(t.nTime<nT0||(t.nTime==MY_CLOSE_MARKET_TIME&&nT0==MY_CLOSE_MARKET_TIME)){
/***
			��дһ������4���ֶ�
			int	nAskOrderSeq;	//����ί�е��ɽ���ţ����ɱ���
			int	nBidOrderSeq;	//��ί�е��ɽ���ţ���ɱ���
			struct TinyOrderStruct *pAskOrder; //��������ָ��
			struct TinyOrderStruct *pBidOrder; //���򶩵�ָ��

***/
			if(InitTinyTransactionField(pIndexStat,&tt)<0) return -1;
			pSXT=&(pIndexStat->S0T);
		}
		else
			pSXT=&(pIndexStat->S1T);

		if((pt=(struct TinyTransactionStruct*)malloc(
			sizeof(struct TinyTransactionStruct)))==NULL){
			printf("error malloc TinyTransactionStruct  file=%s,pos=%ld\n",
				sFileName,lCurPos);
			return -1;
		}
		memcpy((void*)pt,(void*)&tt,sizeof(struct TinyTransactionStruct));
		

		Append2List(pSXT,(LIST*)pt);

		//��������nEndTime0������ֹͣ
		if(t.nTime>nEndTime0){
			iRet=MY_WANT_STAT;
			break;
		}

	}

	if(nEndTime0>=MY_CLOSE_MARKET_TIME&&
		iRet==MY_TAIL_NO_STAT){
		//�����ǰʱ���ѳ����ݽ�ֹʱ��20���ˣ��򴥷�ȥͳ��
		if(tBeginTime>(time_t)(nEndTime0+20000)) iRet=MY_WANT_STAT;
	}

	fclose(fp);
	*plCurPos=lCurPos;

	return iRet;
}
/*
����ֵ��
	0 �����ļ�ĩβ�������nEndTime0,�Ż�
	-1	�������
*/
int MountQuotation2IndexStatArray(char sFileName[],int nBgnActionDay,
	int nPreT0,int nT0,int nEndTime0,long lItemLen,char sCodeStr[],long *plCurPos)
{
	int iRet,nExceedTime=0,iDeltaPre=3000,nPreDel;
	FILE *fp;
	long lCurPos=*plCurPos;
	char sBuffer[2048];
//	long long *pll=(long long*)sBuffer;
	void *p=(SZSEL2_Order*)(sBuffer+sizeof(long long));
	struct TinyQuotationStruct q,*pt;
	struct IndexStatStruct *pIndexStat;

	LISTHEAD *pSXQ;

	if((fp=fopen(sFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sFileName);
		return -1;
	}

	if(fseek(fp,lCurPos,SEEK_SET)<0){
		printf("error fseek file=%s,pos=%ld.\n",sFileName,lCurPos);
		return -1;
	}
	
	nPreDel=nPreT0-iDeltaPre;
	
	while(1){
		if(fread((void*)sBuffer,lItemLen,1,fp)!=1){
			iRet=MY_TAIL_NO_STAT;
			break;
		}

		lCurPos+=lItemLen;

		GTA_Quotation2TinyQuotation(p,&q);
	
		if(IntCodeInCodeStr(q.iStockCode,sCodeStr)==false) continue;

		if((pIndexStat=GetIndexStat(q.iStockCode,sFileName,
			lCurPos,nBgnActionDay,nPreT0,nT0))==NULL) return -1;

		//��lCurPos,���� nT0, ���ڴ��� nPreT0�Ĳ��֣����뵽
		//S0T������nT0�����ݼӵ�pS1Head,��������������nEndTime0������ֹͣ;
		//������һ��X�����ֵ�����ܻ���Ϊ����û�м�ʹ����������
		if(q.nTime<=nPreDel) continue;

		//ʵ�����,{������������}ȡ��Ϊ nPreT0<x<=nT0 �����ȥȡ����
		//����ʱ�򣬲�ȡ9:30����������飬��ȡǰһ�����飬һ����9��25�ֵ�
		if(q.nTime<=nT0&&q.nTime!=MY_OPEN_MARKET_TIME)
			pSXQ=&(pIndexStat->S0Q);
		else	pSXQ=&(pIndexStat->S1Q);

		if((pt=(struct TinyQuotationStruct*)malloc(
			sizeof(struct TinyQuotationStruct)))==NULL){
			printf("error malloc TinyQuotationStruct  file=%s,pos=%ld\n",
				sFileName,lCurPos);
			return -1;
		}
		memcpy((void*)pt,(void*)&q,sizeof(struct TinyQuotationStruct));

		Append2List(pSXQ,(LIST*)pt);

//		��������nEndTime0������ֹͣ
//		if(q.nTime>=nEndTime0) break;

		//�������ڵ���nEndTime0�ļ�¼��һ�γ����������ʱ���
		if(q.nTime>=nEndTime0){
			if(nExceedTime==0) nExceedTime=q.nTime;
		}
		
		//����Ѿ��ҵ����ʱ���ˣ��ٷ��ּ�¼���ݵ�ʱ������ˣ��˳�
		if(nExceedTime>0){
			if(q.nTime>nExceedTime){
				iRet=MY_WANT_STAT;
				break;
			}
		}

	}

	fclose(fp);
	*plCurPos=lCurPos;

	return iRet;
}

/*
����ֵ��
	0 �����ļ�ĩβ�������nEndTime0,�Ż�
	-1	�������
*/
int MountOrdData2IndexStatArray(char sFileName[],int nBgnActionDay,
	int nPreT0,int nT0,int nEndTime0,long lItemLen,char sCodeStr[],long *plCurPos)
{       
	int iRet;
	FILE *fp;
	long lCurPos=*plCurPos;
	char sBuffer[2048];
//	long long *pll=(long long*)sBuffer;
	SZSEL2_Order *p=(SZSEL2_Order*)(sBuffer+sizeof(long long));
	TDF_ORDER od;
	struct TinyOrderStruct o,*pOrder,*pTemp;
	struct IndexStatStruct *pIndexStat;

	if((fp=fopen(sFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sFileName);
		return -1;
	}

	if(fseek(fp,lCurPos,SEEK_SET)<0){
		printf("error fseek file=%s,pos=%ld.\n",sFileName,lCurPos);
		return -1;
	}
	while(1){
		if(fread((void*)sBuffer,lItemLen,1,fp)!=1){
			iRet=MY_TAIL_NO_STAT;
			break;
		}
		lCurPos+=lItemLen;

		GTA2TDF_SZSEL2_O(p[0],od);
		
		if(CodeInCodeStr(od.szCode,sCodeStr)==false) continue;

		if((pIndexStat=GetIndexStat(atoi(od.szCode),sFileName,
			lCurPos,nBgnActionDay,nPreT0,nT0))==NULL) return -1;

//		if(iStockCode==2320){
//			printf("hello my stock order.\n");
//		}
		if(SearchBin(pIndexStat->M_ORDER,(void*)&od.nOrder,
			data_search_bintree_order_2,(void**)&pOrder)==FOUND){

			pOrder->iCloseFlag=	1;
			pOrder->nOriOrdPrice=	od.nPrice;
			pOrder->nOriOrdVolume=	od.nVolume;
			pOrder->lOriOrdAmnt=	(long)od.nPrice*od.nVolume/100;

			//��������nEndTime0������ֹͣ
			if(od.nTime>=nEndTime0){
				iRet=MY_WANT_STAT;
				break;    
			}
			continue;
		}

		TDF_ORDER2TinyOrder(&od,&o);

		//������ͷ��ָ�����
		//o.pNext=&(pIndexStat->PreS0M);
		o.pNext=(struct TinyOrderStruct*)&(pIndexStat->S0O);

		if((pTemp=(struct TinyOrderStruct*)malloc(
			sizeof(struct TinyOrderStruct)))==NULL){
			printf("malloc TinyOrderStruct error.\n");
			return -1;
		}
		memcpy((void*)pTemp,(void*)&o,sizeof(struct TinyOrderStruct));

		if(InsertBin(&(pIndexStat->M_ORDER),(void *)pTemp,
			data_search_bintree_stock_code_order,
			assign_insert_bintree_stock_code_order_e)<0){
			printf("insert bin malloc error th.\n");
			return -1;
		}
		//��������nEndTime0������ֹͣ
		if(od.nTime>=nEndTime0){
			iRet=MY_WANT_STAT;
			break;
		}
	}

	fclose(fp);
	*plCurPos=lCurPos;

	return iRet;
}

int IsBusyTime(int iTime)
{
	if(iTime<93500000||(iTime>125900000&&iTime<130200000)||
		iTime>145900000) return true;
	return false;
}
int IsStopTime(int iTime)
{
	if((iTime>113500000&&iTime<125900000)) return true;
	return false;
}
void PrintUsage(char *argv[])
{
	printf("Usage: %s \n", argv[0]);
	printf("   [-d calc-date (yyyymmdd =8Bytes def=curdate) ]\n");
	printf("   [-b begin-time (HH24MISS =6Bytes def=curtime) ]\n");
	printf("   [-m max-delay-sec (def=20) ]\n");
	printf("   [-s source-path (def=/stock/work) ]\n");
	printf("   [-o work-root-name (def=/stock/work) ]\n");
	printf("   [-e delay str 'b,i:b:i' ]\n");
	printf("   [-t idlewaitmilli \n");
	printf("   [-w writeflag (1,zbzd 2,ext 3,bin info) \n");
	printf("   [-l codelist (e.g \"000001,603912,002415\") ]\n");
	printf("   [-L etflist (e.g \"510050,510180,510300,510500\") ]\n");
	printf("   [-E etfpath (e.g ../conf/etf) ]\n");
}
int main(int argc, char *argv[])
{
	FILE *fpD31;
	int iIdleWaitMilli=10,iWriteFlag=1;//,iContinueFlag=false;
	int iShBusyDelay=12000,iShDelay=3000,iSzBusyDelay=12000,iSzDelay=2000;
	int nBgnActionDay,nBgnTime,nPreT0,nT0,nEndTime0,nCurTime,nPreTime=0;
	int iThRes,iTzRes,iQhRes,iQzRes,iOzRes,iCnt=0,iTotalCnt=0;

	char sCurTime[15],sCalcDate[15],sCalcBgn[15],sMSec[4];
	char sDelayStr[256],sCodeStr[512],sD31Name[512],sSourcePath[512],sWorkRoot[512];
	char sGtaQhName[512],sGtaQzName[512],sGtaThName[512],sGtaTzName[512],sGtaOzName[512],sEtfList[512],sEtfPath[512];

	time_t tBeginTime,tEndTime;
	//��ǰ�ļ�����λ��
	long lThCurPos=0,lTzCurPos=0,lOzCurPos=0,lQhCurPos=0,lQzCurPos=0;

	GetHostTime(sCurTime);

	//���ò���ѡ���Ĭ��ֵ
	strncpy(sCalcDate,sCurTime,8);sCalcDate[8]=0;
	strcpy(sCalcBgn,sCurTime+8);
	strcpy(sSourcePath,	"/stock/work");
	strcpy(sWorkRoot,	"/stock/work");
	strcpy(sCodeStr,"");
	strcpy(sDelayStr,"");
	strcpy(sEtfList,"510050,510180,510300,510500");
	strcpy(sEtfPath,"../conf/etf");

	//��ȡ�����в���
	for (int c; (c = getopt(argc, argv, "d:b:m:s:o:e:t:w:l:L:E:?:")) != EOF;){

		switch (c){
		case 'd':strncpy(sCalcDate, optarg,8);sCalcDate[8]=0;	break;
		case 'b':strncpy(sCalcBgn, optarg,6);sCalcBgn[6]=0;	break;
		case 's':strcpy(sSourcePath, optarg);	break;
		case 'o':strcpy(sWorkRoot, optarg);	break;
		case 'e':strcpy(sDelayStr,optarg);	break;
		case 't':iIdleWaitMilli=atoi(optarg);	break;
		case 'w':iWriteFlag=atoi(optarg);
			if(iWriteFlag!=1&&iWriteFlag!=2&&iWriteFlag!=3)iWriteFlag=1;	break;
		case 'l':strcpy(sCodeStr, optarg);	break;
		case 'L':strcpy(sEtfList, optarg);	break;
		case 'E':strcpy(sEtfPath, optarg);	break;
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
	//����delay���������Ϻ������ڡ�ʱ�Ӳ���
	if(strlen(sDelayStr)!=0){
		char *p0,*p1,*p2;

		if((p0=strchr(sDelayStr,':'))==NULL){
			printf("-e =%s ��ʽ�Ƿ�,��ȷ��ʽ�� '12000:1000,6000:900'.\n",sDelayStr);
			return -1;
		}
		p0++;
		if((p1=strchr(p0,','))==NULL){
			printf("-e =%s ��ʽ�Ƿ�,��ȷ��ʽ�� '12000:1000,6000:900'.\n",sDelayStr);
			return -1;
		}
		p1++;
		if((p2=strchr(p1,':'))==NULL){
			printf("-e =%s ��ʽ�Ƿ�,��ȷ��ʽ�� '12000:1000,6000:900'.\n",sDelayStr);
			return -1;
		}
		p2++;
		iShBusyDelay=	atoi(sDelayStr);
		iShDelay=	atoi(p0);
		iSzBusyDelay=	atoi(p1);
		iSzDelay=	atoi(p2);
	}

	//����ETF�������ݵ��ڴ���
	if(InitIndexEtfList(sEtfList,sEtfPath)<0) return -1;

	//���������ļ���
	sprintf(sGtaThName,"%s/gta_th_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaTzName,"%s/gta_tz_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaOzName,"%s/gta_oz_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaQhName,"%s/gta_qh_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaQzName,"%s/gta_qz_%s.dat",sSourcePath,sCalcDate);

	if(iWriteFlag==3)
		sprintf(sD31Name,"%s/d31_g%d_%s.dat",sWorkRoot,iWriteFlag,sCalcDate);
	else	sprintf(sD31Name,"%s/d31_g%d_%s.txt",sWorkRoot,iWriteFlag,sCalcDate);

        if((fpD31=fopen(sD31Name,"ab+"))==NULL){
                printf("error open file %s to write.\n",sD31Name);
                return -1;
        }

	//9���֮ǰ�����������ÿ�ʼʱ��Ϊ��ʮ��
	if(strcmp(sCalcBgn,"093000")<0)	strcpy(sCalcBgn,"093000");

	//�õ������
	nBgnActionDay=	atoi(sCalcDate);
	nBgnTime=	atoi(sCalcBgn);
	nT0=		nBgnTime*1000;

	if(nBgnTime==93000)
		nPreT0=iAddMilliSec(nT0,-1000*1800);
	else	nPreT0=iAddMilliSec(nT0,-1000*60);

	while(1){

		tBeginTime=tGetHostTime();

		//Ϊ�˱��ⶩ�����ݳٵ�,������ǰȡ20�룬���������һ�ʣ��ֹ�����ʱ��
		if(IsBusyTime(nT0)){
			nEndTime0=iAddMilliSec(nT0,iSzBusyDelay+20000);
			if(nT0==MY_CLOSE_MARKET_TIME)	nEndTime0=MY_CLOSE_MARKET_TIME+1000000;
//			if(nT0==MY_OPEN_MARKET_TIME)	nEndTime0=iAddMilliSec(MY_PRE_OPEN_TIME,iSzBusyDelay);
		}
		else	nEndTime0=iAddMilliSec(nT0,iSzDelay+20000);

			
		//�������ڶ�������
		iOzRes=MountOrdData2IndexStatArray(sGtaOzName,nBgnActionDay,nPreT0,nT0, 
			nEndTime0,sizeof(long long)+sizeof(SZSEL2_Order),sCodeStr,&lOzCurPos);
		if(iOzRes<0) return -1;

		if(IsBusyTime(nT0)){
			nEndTime0=iAddMilliSec(nT0,iSzBusyDelay);
//			if(nT0==MY_OPEN_MARKET_TIME)	nEndTime0=iAddMilliSec(MY_PRE_OPEN_TIME,iSzBusyDelay);
		}
		else	nEndTime0=iAddMilliSec(nT0,iSzDelay);

			
		//�������ڽ�������
		GTA2TDF_Q2T=GTA2TDF_QZ2T;
		iTzRes=MountTrsData2IndexStatArray(sGtaTzName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,tBeginTime,sizeof(long long)+sizeof(SZSEL2_Transaction),sCodeStr,&lTzCurPos);
		if(iTzRes<0) return -1;
                          
		//����������������
		GTA_Quotation2TinyQuotation=SZSEL2_Quotation2TinyQuotation;
		iQzRes=MountQuotation2IndexStatArray(sGtaQzName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SZSEL2_Quotation),sCodeStr,&lQzCurPos);
		if(iQzRes<0) return -1;

		
		if(IsBusyTime(nT0)){
			nEndTime0=iAddMilliSec(nT0,iShBusyDelay);
//			if(nT0==MY_OPEN_MARKET_TIME)	nEndTime0=iAddMilliSec(MY_PRE_OPEN_TIME,iShBusyDelay);
		}
		else	nEndTime0=iAddMilliSec(nT0,iShDelay);


		//�����Ϻ���������
		GTA2TDF_Q2T=GTA2TDF_QH2T;
		iThRes=MountTrsData2IndexStatArray(sGtaThName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,tBeginTime,sizeof(long long)+sizeof(SSEL2_Transaction),sCodeStr,&lThCurPos);
		if(iThRes<0) return -1;

		//�����Ϻ���������
		GTA_Quotation2TinyQuotation=SSEL2_Quotation2TinyQuotation;
		iQhRes=MountQuotation2IndexStatArray(sGtaQhName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SSEL2_Quotation),sCodeStr,&lQhCurPos);
		if(iQhRes<0) return -1;

		//��������ɽ�ʱ�䶼δ�ﵽͳ�Ƶ㣬
		//�������ݵ�����û����ɨ������һ�£�
		
		nCurTime=nGetHostCurTime();
		
		if(iTzRes!=MY_WANT_STAT&&iThRes!=MY_WANT_STAT){

						
			//���ʱ��������ʱ�䣬����
			if(nT0==130001000&&(
				nCurTime>=113000000&&nCurTime<125700000)){
				//����������
				sleep(180);
				continue;
			}
			//ֻ��Ҫ����ʵ�̻������ط�ʱ�򣬻طų���������������У�
			if(nCurTime>=113000000&&nCurTime<125700000){
				//����������
				sleep(180);
				continue;
			}
			//�ŵ��ǰ���Ͼ������ʱ��
			if(nCurTime<92455000||
				(nCurTime>92600000&&nCurTime<92955000)){
				//����5����
				sleep(5);
				continue;

			}
			//�������Ͼ���չʾ���ʱ��
			if(nCurTime<92500000||nCurTime>92600000){
				usleep(iIdleWaitMilli*1000);
				continue;
			}
			//�������ʱ����ϴ�ʱ�䲻��һ�룬�����ɨ��
			if((nCurTime-nPreTime)<1000){
				usleep(iIdleWaitMilli*1000);
				continue;
			}
			//����Ѿ������һ��ҵ���ˣ��ȴ���ʶ��λ
			if(nT0==MY_CLOSE_MARKET_TIME){
				usleep(iIdleWaitMilli*1000);
				continue;
			}
			nPreTime=nCurTime;
		}
		
		//��һ��ѭ����ͳ��ETF�Ľڵ�ʱ�����Ϊ��ǰʱ��
		SetIndexEtfDayTime(nBgnActionDay,nPreT0,nT0);
		
		//��һ��ѭ����D31������ͳ�Ƴ���
		if(GenD31StatAll()<0) return -1;

		//��D31�����������ļ�
		if((iCnt=WriteD31StatAll(fpD31,sCodeStr,iWriteFlag))<0) return -1;

		//��һ�������¼��ͳ��
		iTotalCnt+=iCnt;

		if(nCurTime>92600000){
			
			nPreT0=nT0;

			//��������ˣ����˳���
			if(nT0>=150000000) break;

			//��������ʱ��ֱ������������У�������13��00��01��
			if(nT0>=113000000&&nT0<125959000)
				nT0=130001000;
			else	//����ʱ��1��ɨ��һ��
				nT0=iAddMilliSec(nT0,1000);

			//������Ԥ���ص�T0֮ǰ�����ݣ��ŵ�ͳ�ƻ�����
			if(AddPreT0Data2Ready(nPreT0,nT0)<0) return -1;
		}

		tEndTime=tGetHostTime();

		GetHostTimeX(sCurTime,sMSec);
		printf("%s.%s tp=%d,tc=%d.cost=%ld count=%d\n",
			sCurTime,sMSec,nPreT0,nT0,tEndTime-tBeginTime,iTotalCnt);

	}

	fclose(fpD31);

	GetHostTimeX(sCurTime,sMSec);
	
	printf("%s.%s tp=%d,tc=%d INDEX STAT COMPLETE allcount=%d\n",
		sCurTime,sMSec,nPreT0,nT0,iTotalCnt);

	return 0;
}
