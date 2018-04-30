
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

#include "index_stat.h"

#define MY_DATE_CEIL_LONG 1000000000L
#define MY_CLOSE_MARKET_TIME 150000000
#define MY_OPEN_MARKET_TIME  93000000


long alAmntLevel[MAX_LEVEL_CNT]={
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
long alJmpLevel[MAX_JMP_LEVEL_CNT]={
	20*10000*100,
	50*10000*100,
	100*10000*100
};
struct IndexStatStruct *INDEX_HEAD=NULL;
struct IndexStatStruct *AISTAT[MAX_STOCK_CODE];

void  (*GTA2TDF_Q2T)(void *p, TDF_TRANSACTION *pt);
void (*GTA_Quotation2TinyQuotation)(void *p, struct TinyQuotationStruct *po);


void TDF_TRANSACTION2TinyTransaction(struct TDF_TRANSACTION *pi,struct TinyTransactionStruct *po)
{
	po->pNext=NULL;
//	po->pAskOrder=po->pBidOrder=NULL;
	po->nActionDay=	pi->nActionDay;
	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->nPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nAskOrder=	pi->nAskOrder;
	po->nBidOrder=	pi->nBidOrder;

	po->nBSFlag=	pi->nBSFlag;

	po->nAskOrderSeq=0;
	po->nBidOrderSeq=0;
	
//	po->nAskJmpSeq=0;
//	po->nBidJmpSeq=0;
}
void TDF_TRANSACTION2TinyOrderS(struct TDF_TRANSACTION *pi,struct TinyOrderStruct *po)
{
	po->pNext=NULL;
	po->nActionDay=	pi->nActionDay;
	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->nPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nOrder=	pi->nAskOrder;
	po->nBroker=	0;
	po->nBSFlag=	pi->nBSFlag;
	po->nOrderSeq=	0;
	po->lOrderAmnt=(long)(pi->nPrice)*pi->nVolume/100;

	po->iCloseFlag=0;
	po->nOriOrdPrice=0;
	po->nOriOrdVolume=0;
	po->lOriOrdAmnt=0;
	
	po->nLastPrice=0;
	po->nAskJmpSeq=0;
	po->nBidJmpSeq=0;
}
void TDF_TRANSACTION2TinyOrderB(struct TDF_TRANSACTION *pi,struct TinyOrderStruct *po)
{
	po->pNext=NULL;
	po->nActionDay=	pi->nActionDay;
	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->nPrice=	(int)pi->nPrice;
	po->nVolume=	pi->nVolume;
	po->nOrder=	pi->nBidOrder;
	po->nBroker=	0;
	po->nBSFlag=	pi->nBSFlag;
	po->nOrderSeq=	0;
	po->lOrderAmnt=(long)(pi->nPrice)*pi->nVolume/100;

	po->iCloseFlag=0;
	po->nOriOrdPrice=0;
	po->nOriOrdVolume=0;
	po->lOriOrdAmnt=0;
	
	po->nLastPrice=0;
	po->nAskJmpSeq=0;
	po->nBidJmpSeq=0;
}
void TDF_ORDER2TinyOrder(struct TDF_ORDER *pi,struct TinyOrderStruct *po)
{
	po->pNext=NULL;
	po->nActionDay=	pi->nActionDay;
	po->nTime=	pi->nTime;
	po->iStockCode=	atoi(pi->szCode);
	po->nPrice=	0;
	po->nVolume=	0;
	po->nOrder=	pi->nOrder;
	po->nBroker=	pi->nBroker;
	po->nBSFlag=	pi->chFunctionCode;
	po->nOrderSeq=	0;
	po->lOrderAmnt= 0;

	po->iCloseFlag=	0;
	po->nOriOrdPrice=(int)pi->nPrice;
	po->nOriOrdVolume=pi->nVolume;
	po->lOriOrdAmnt=(long)(pi->nPrice)*pi->nVolume/100;
	
	po->nLastPrice=0;
	po->nAskJmpSeq=0;
	po->nBidJmpSeq=0;

}

void GTA2TDF_QH2T(void *p, TDF_TRANSACTION *pt)
{
	const SSEL2_Transaction *pi=(const SSEL2_Transaction *)p;

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
}

int data_search_bintree_order_2(void *pValue,void*pData)
{
	return *((int*)pValue)-((struct TinyOrderStruct *)pData)->nOrder;
}
struct IndexStatStruct *NewInitIndexStat(int iStockCode,int nActionDay,int nPreT0,int nT0)
{
	struct IndexStatStruct *p;

	if((p=(struct IndexStatStruct *)malloc(sizeof(struct IndexStatStruct)))==NULL){
		printf("malloc IndexStatStruct error.\n");
		return NULL;
	}
	bzero((void*)p,sizeof(struct IndexStatStruct));

	p->iStockCode=	iStockCode;
	p->nActionDay=	nActionDay;
	p->nPreT0=	nPreT0;
	p->nT0=		nT0;

	return p;
}
//#define MY_TINY_ORDER(x) ((struct TinyOrderStruct *)x)

int data_search_bintree_stock_code_order(void *pValue,void*pData)
{
	return ((struct TinyOrderStruct *)pValue)->nOrder-
		((struct TinyOrderStruct *)pData)->nOrder;
}
void assign_insert_bintree_stock_code_order_e(void **ppData,void *pData)
{
	LISTHEAD *pS0O;
	struct TinyOrderStruct *p=(struct TinyOrderStruct *)pData;

	pS0O=(LISTHEAD*)(p->pNext);

	Append2List(pS0O,(LIST*)p);

	*ppData=pData;
}

//�����ϲ������׺ϲ���������
int AddTinyOrder2Tree(BINTREE **PP,int *piCnt,struct TinyOrderStruct *p)
{
	struct TinyOrderStruct *pTemp;

	if(SearchBin(*PP,p,data_search_bintree_stock_code_order,
		(void**)&pTemp)==NOTFOUND){

		if((pTemp=(struct TinyOrderStruct*)malloc(
			sizeof(struct TinyOrderStruct)))==NULL){
			printf("malloc TinyOrderStruct error.\n");
			return -1;
		}
		memcpy((void*)pTemp,(void*)p,sizeof(struct TinyOrderStruct));

		if(InsertBin(PP,(void *)pTemp,
			data_search_bintree_stock_code_order,
			assign_insert_bintree_stock_code_order_e)<0){
			printf("insert bin malloc error th.\n");
			return -1;
		}
		(*piCnt)++;
	}
	else{

		pTemp->nVolume+=	p->nVolume;
		//���ǵ�ͳ�Ƶı�������ʵ��ͳ�ƶ��ٸ������������ﲻ�ۼ�OrderNo
		//pTemp->nOrderNum+=	p->nOrderNum;
		pTemp->lOrderAmnt+=	p->lOrderAmnt;

	}
	return 0;
}

//�����ϲ������׺ϲ������б���
int AddTinyOrder2List(LISTHEAD *pS1O,struct TinyOrderStruct *p)
{
	struct TinyOrderStruct *pTemp;

	if((pTemp=(struct TinyOrderStruct*)malloc(
		sizeof(struct TinyOrderStruct)))==NULL){
		printf("malloc TinyOrderStruct error.\n");
		return -1;
	}
	memcpy((void*)pTemp,(void*)p,sizeof(struct TinyOrderStruct));

	Append2List(pS1O,(LIST*)pTemp);

	return 0;
}


int AddTransaction2IndexStat(struct TDF_TRANSACTION *pi,int nT0,
	struct IndexStatStruct *pIndexStat)
{
	struct TinyOrderStruct ob,os;

	TDF_TRANSACTION2TinyOrderS(pi,&os);
	TDF_TRANSACTION2TinyOrderB(pi,&ob);

	if(pi->nTime<nT0||
		(nT0==MY_CLOSE_MARKET_TIME&&pi->nTime==MY_CLOSE_MARKET_TIME)){

		//������ͷ��ָ�����
		os.pNext=(struct TinyOrderStruct*)&(pIndexStat->S0O);
		//���������ϲ������׺ϲ���������
		if(AddTinyOrder2Tree(&(pIndexStat->M_ORDER),&pIndexStat->iS0OCnt,&os)<0){
			printf("error add ask tiny order to tree\n");
			return -1;
		}

		//������ͷ��ָ�����
		//ob.pNext=&(pIndexStat->PreS0M);
		ob.pNext=(struct TinyOrderStruct*)&(pIndexStat->S0O);
		//���붩���ϲ������׺ϲ���������
		if(AddTinyOrder2Tree(&(pIndexStat->M_ORDER),&pIndexStat->iS0OCnt,&ob)<0){
			printf("error add bid tiny order to tree \n");
			return -1;
		}
	}
	else{
		//����������ӵ����׺ϲ�����������
		if(AddTinyOrder2List(&(pIndexStat->S1O),&os)<0){
			printf("error add ask tiny order to list\n");
			return -1;
		}
		//���붩����ӵ����׺ϲ�����������
		if(AddTinyOrder2List(&(pIndexStat->S1O),&ob)<0){
			printf("error add bid tiny order to list\n");
			return -1;
		}
	}
	return 0;
}

/**
10.00�����ɽ���˵��ί������ί�м� <=10.00Ԫ�������ֳɽ���>=10.00Ԫ ������ί�е��Ŵ��� �óɽ����ŵĽ��ף���رոóɽ�����
10.00���򵥳ɽ���˵��ί���򵥵�ί�м�>=10Ԫ�������ֳɽ���<=10.00Ԫ ����ί�е��Ŵ��� �óɽ����ŵĽ��ף���رոóɽ�����
	int nAskDownPrice;	//������ͳɽ���,��λ���֣�
	int nBidUpPrice;	//������߳ɽ���,��λ���֣�
	int nMaxAskOrder;	//������󶩵��ţ���Ӧ��������ͼ�
	int nMaxBidOrder;	//������󶩵��ţ���Ӧ��������߼�
**/
int data_search_bintree_price_order(void *pValue,void*pData)
{
	return ((struct PriceOrderStruct *)pValue)->nPrice-
		((struct PriceOrderStruct *)pData)->nPrice;
}
void assign_insert_bintree_price_order(void **ppData,void *pData)
{
	*ppData=pData;
}


//�����ϲ������׺ϲ���������
int AddPriceOrder2Tree(BINTREE **PP,int *piCnt,struct PriceOrderStruct *p)
{
	struct PriceOrderStruct *pTemp;

	if(SearchBin(*PP,p,data_search_bintree_price_order,
		(void**)&pTemp)==NOTFOUND){

		if((pTemp=(struct PriceOrderStruct*)malloc(
			sizeof(struct PriceOrderStruct)))==NULL){
			printf("malloc PriceOrderStruct error.\n");
			return -1;
		}
		memcpy((void*)pTemp,(void*)p,sizeof(struct PriceOrderStruct));

		if(InsertBin(PP,(void *)pTemp,
			data_search_bintree_price_order,
			assign_insert_bintree_price_order)<0){
			printf("insert bin malloc error th.\n");
			return -1;
		}
		(*piCnt)++;
	}
	else{
		if(pTemp->nOrder<p->nOrder){
			pTemp->nOrder=p->nOrder;
			pTemp->nOrderTime=p->nOrderTime;
		}
	}

	return 0;
}
int AdjustIndexStatS0(struct IndexStatStruct *p,struct TinyTransactionStruct *pt)
{
	struct PriceOrderStruct t;

	t.nPrice=pt->nPrice;
	t.nOrderTime=pt->nTime;

	t.nOrder=pt->nAskOrder;


	if(AddPriceOrder2Tree(&(p->ASK_MAX),&p->iAskMaxCnt,&t)<0) return -1;

	t.nOrder=pt->nBidOrder;

	if(pt->iStockCode==601168&&t.nOrder==179900&&t.nPrice==69300){
		printf("hello 4\n");
	}

	if(AddPriceOrder2Tree(&(p->BID_MAX),&p->iBidMaxCnt,&t)<0) return -1;

	return 0;
}
int data_search_bintree_price_order_1(void *pValue,void*pData)
{
	return ((struct TinyOrderStruct *)pValue)->nPrice-
		((struct PriceOrderStruct *)pData)->nPrice;
}
int iErrorFlag=0;
int IsMyLeave(BINTREE *p)
{
	if(p->pLeft==NULL&&p->pRight==NULL) return true;
	return false;
}
int GetTreeHigh(BINTREE *p)
{
	int iLeft,iRight;

	if(p==NULL) return 0;

	iLeft=GetTreeHigh(p->pLeft)+1;
	iRight=GetTreeHigh(p->pRight)+1;

	if(iLeft>iRight) return iLeft;

	return iRight;
}
int check_my_balance(BINTREE *ptHead)
{
	int iLeft,iRight;

	iLeft=GetTreeHigh(ptHead->pLeft);
	iRight=GetTreeHigh(ptHead->pRight);

	if(ptHead->diff!=(iRight-iLeft)){
		iErrorFlag=1;
		printf("logic error 7 left=%d,right=%d diff=%d head=%p.\n",iLeft,iRight,ptHead->diff,(void*)ptHead);
	}

	return 0;
}
int TravelMyBin(BINTREE *ptHead, int ( *pOperation)(BINTREE *))
{
	int j=0;

	if(ptHead==NULL) return 0;

	if(ptHead->pLeft!=NULL)
		j+=TravelMyBin(ptHead->pLeft,pOperation);
	j+=(*pOperation)(ptHead);
	if(ptHead->pRight!=NULL)
		j+=TravelMyBin(ptHead->pRight,pOperation);
	return j;
}
//����S0O,����ԭ��ɾ����������
void DeleteCloseOrder(struct IndexStatStruct *p,int nLastTime)
{
        struct TinyOrderStruct *ptHead,*pTemp,*pTinyOrder;
        struct PriceOrderStruct *pPriceOrder;
        BINTREE *pBin,*BS_MAX;

        LISTHEAD *pS0O=&(p->S0O);

        ptHead=(struct TinyOrderStruct *)pS0O->pHead;

        pS0O->pHead=pS0O->pTail=NULL;

        while(ptHead!=NULL){
        	pTemp=ptHead;
        	ptHead=ptHead->pNext;

        	//���ʱ������ĳ��ʱ���֮�ڵģ���ɾ��
        	if(pTemp->nTime>nLastTime){
			pTemp->pNext=NULL;
			Append2List(pS0O,(LIST*)pTemp);
        		continue;
		}
        	if(SearchBin(p->M_ORDER,pTemp,data_search_bintree_stock_code_order,
			(void**)&pTinyOrder)==NOTFOUND){
			printf("logic error search.\n");
			pTemp->pNext=NULL;
			Append2List(pS0O,(LIST*)pTemp);
			continue;
		}

		if(pTinyOrder->nBSFlag=='B')
			BS_MAX=p->BID_MAX;
		else if(pTinyOrder->nBSFlag=='S')
			BS_MAX=p->ASK_MAX;
		else	BS_MAX=NULL;

		//���δ�ҵ��������Ǳ��۸�δ����
		if(SearchBin(BS_MAX,pTemp,data_search_bintree_price_order_1,
			(void**)&pPriceOrder)==NOTFOUND){
			pTemp->pNext=NULL;
			Append2List(pS0O,(LIST*)pTemp);
			continue;
		}

		//ͬ���۸�ͬ�������и��󶩵��ųɽ����򱾶����ŷ���
		if(pTemp->nOrder>=pPriceOrder->nOrder){
			pTemp->pNext=NULL;
			Append2List(pS0O,(LIST*)pTemp);
			continue;
		}
		//�������ɽ���󶩵���ʱ�仹û���ڣ�����������ڴ���
		if(pPriceOrder->nOrderTime>=nLastTime){
			pTemp->pNext=NULL;
			Append2List(pS0O,(LIST*)pTemp);
			continue;
		}

		if(pTemp->nOrder==16590){
			printf("hello 1.\n");
			if(pTemp->nPrice==1010000&&
				pTemp->nOrder==16590){
				printf("hello 2.\n");
			}
		}

//		TravelMyBin(p->M_ORDER,check_my_balance);

		DeleteBin(&(p->M_ORDER),(void*)pTemp,
			data_search_bintree_stock_code_order,&pBin);

//		TravelMyBin(p->M_ORDER,check_my_balance);

		if(iErrorFlag==1){
			printf("hello world.------------------------------------%d,%d.\n",pTemp->iStockCode,pTemp->nOrder);
			exit(1);
		}

		if(pBin==NULL){
			printf("logic error search.\n");
			pTemp->pNext=NULL;
			Append2List(pS0O,(LIST*)pTemp);
			continue;
		}

		p->iFreeS0OCnt++;
		p->iS0OCnt--;

		free(pBin);
		free(pTemp);
	}
}
void AskOrder2D31(struct TinyOrderStruct *p,struct TinyTransactionStruct *pt,
	struct D31IndexItemStruct *pD31,struct IndexStatStruct *pInd)
{
	long lAmnt=p->nOriOrdVolume?(long)p->nOriOrdVolume*(long)pt->nPrice/100:p->lOrderAmnt;

	for(int i=0;i<MAX_LEVEL_CNT;i++){
		if(alAmntLevel[i]>lAmnt) break;

		pD31->alAskAmount[i]+=	(long)pt->nVolume*pt->nPrice/100;
		pD31->aiAskVolume[i]+=	pt->nVolume;

		//ֻͳ��ί�е��ĵ�һ�ʳɽ�
		if(pt->nAskOrderSeq==1){
			if(pInd->iStockCode==2320&&
				pInd->nT0==93100000&&
				i==5)
				printf("================================%d================\n",p->nOrder);

			pD31->aiAskOrderNum[i]++;
		}
	}
}
void BidOrder2D31(struct TinyOrderStruct *p,struct TinyTransactionStruct *pt,
	struct D31IndexItemStruct *pD31)
{

	long lAmnt=p->nOriOrdVolume?(long)p->nOriOrdVolume*(long)pt->nPrice/100:p->lOrderAmnt;

	for(int i=0;i<MAX_LEVEL_CNT;i++){
		if(alAmntLevel[i]>lAmnt) break;
		pD31->alBidAmount[i]+=	(long)pt->nVolume*pt->nPrice/100;
		pD31->aiBidVolume[i]+=	pt->nVolume;

		//ֻͳ��ί�е��ĵ�һ�ʳɽ�
		if(pt->nBidOrderSeq==1)pD31->aiBidOrderNum[i]++;
	}
}

void AskOrder2D31Zb(struct TinyOrderStruct *p,struct TinyTransactionStruct *pt,
	struct D31IndexItemStruct *pD31)
{
	long lAmnt=p->nOriOrdVolume?(long)p->nOriOrdVolume*(long)pt->nPrice/100:p->lOrderAmnt;

	for(int i=0;i<MAX_LEVEL_CNT;i++){
		if(alAmntLevel[i]>lAmnt) break;
		pD31->alAskAmount[i]+=	(long)pt->nVolume*pt->nPrice/100;
		pD31->aiAskVolume[i]+=	pt->nVolume;

		//ֻͳ��ί�е��ĵ�һ�ʳɽ�
		if(pt->nAskOrderSeq==1)pD31->aiAskOrderNum[i]++;
/*		if(i>=9&&pt->iStockCode==2320){
			printf("--------------------------------------------.\n");
		}
*/
	}
}

void BidOrder2D31Zb(struct TinyOrderStruct *p,struct TinyTransactionStruct *pt,
	struct D31IndexItemStruct *pD31)
{
	long lAmnt=p->nOriOrdVolume?(long)p->nOriOrdVolume*(long)pt->nPrice/100:p->lOrderAmnt;

	for(int i=0;i<MAX_LEVEL_CNT;i++){
		if(alAmntLevel[i]>lAmnt) break;
		pD31->alBidAmount[i]+=	(long)pt->nVolume*pt->nPrice/100;
		pD31->aiBidVolume[i]+=	pt->nVolume;

		//ֻͳ��ί�е��ĵ�һ�ʳɽ�
		if(pt->nBidOrderSeq==1)pD31->aiBidOrderNum[i]++;
	}
}
void AskOrder2D31Ex(struct TinyOrderStruct *p,struct TinyTransactionStruct *pt,
	struct D31IndexExtStruct *pD31)
{
	long lAmnt=p->nOriOrdVolume?(long)p->nOriOrdVolume*(long)pt->nPrice/100:p->lOrderAmnt;

	for(int i=0;i<MAX_JMP_LEVEL_CNT;i++){
		if(alJmpLevel[i]>lAmnt) break;
		pD31->alAskAmount[i]+=	(long)pt->nVolume*pt->nPrice/100;
	}
}
void BidOrder2D31Ex(struct TinyOrderStruct *p,struct TinyTransactionStruct *pt,
	struct D31IndexExtStruct *pD31,struct IndexStatStruct *pInd)
{
	long lAmnt=p->nOriOrdVolume?(long)p->nOriOrdVolume*(long)pt->nPrice/100:p->lOrderAmnt;

	for(int i=0;i<MAX_JMP_LEVEL_CNT;i++){
		if(alJmpLevel[i]>lAmnt) break;
		if(i==0&&pInd->nT0==93800000&&
			pInd->iStockCode==603912){
				printf("=xxxxxxxxxxxxxx========%d================\n",p->nOrder);
		}
		pD31->alBidAmount[i]+=	(long)pt->nVolume*pt->nPrice/100;
	}
}
/***
void InsertJmpOrder(struct TinyOrderStruct **pptHead,struct TinyOrderStruct *p)
{
	p->pJmpOrder= *pptHead;
	*pptHead=p;
}
void InsertAskOrderTransaction(struct TinyTransactionStruct **pptHead,struct TinyTransactionStruct *p)
{
	p->pAskOrder= *pptHead;
	*pptHead=p;
}
void InsertBidOrderTransaction(struct TinyTransactionStruct **pptHead,struct TinyTransactionStruct *p)
{
	p->pBidOrder= *pptHead;
	*pptHead=p;
}
***/
//b)����S0T�б��У�ѭ������M_ORDER���У����� D31IndexItem���ݣ�
int GenD31Stat(struct IndexStatStruct *p)
{
	struct D31IndexItemStruct *pD31Zd=&(p->Zd),*pD31Zb=&(p->Zb);
	struct D31IndexExtStruct *pD31Ex=&p->Ex;
	BINTREE *M_ORDER=p->M_ORDER;

	struct TinyOrderStruct *pAskOrder,*pBidOrder;
	struct TinyTransactionStruct *ptHead,*pTemp;

	struct TinyQuotationStruct *ptPre,*ptCur;

	ptHead=(struct TinyTransactionStruct *)p->S0T.pHead;

	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pNext;

		//�ҵ������������
		if(SearchBin(M_ORDER,(void*)&(pTemp->nAskOrder),
			data_search_bintree_order_2,(void**)&pAskOrder)==NOTFOUND){
			printf("hello 3 code=%d,askord=%d time=%d,flag=%c\n",
				p->iStockCode,pTemp->nAskOrder,pTemp->nTime,(char)(pTemp->nBSFlag));
			printf("hello 2\n");
			return -1;
		}
		//�ҵ��򷽶������
		if(SearchBin(M_ORDER,(void*)&(pTemp->nBidOrder),
			data_search_bintree_order_2,(void**)&pBidOrder)==NOTFOUND){

			printf("hello 3 code=%d,bidord=%d time=%d,flag=%c\n",
				p->iStockCode,pTemp->nBidOrder,pTemp->nTime,(char)(pTemp->nBSFlag));
			return -1;
		}

		//����߼��Ժ�Ҫ�Ƶ������߼���
		pAskOrder->nOrderSeq++;
		pBidOrder->nOrderSeq++;

		pTemp->nAskOrderSeq=pAskOrder->nOrderSeq;
		pTemp->nBidOrderSeq=pBidOrder->nOrderSeq;

		if(pTemp->nAskOrder==1691843&&pTemp->nBidOrder==1732854){
			printf("shenme luoji error.\n");
		}
		if(pTemp->nAskOrder==374174&&pTemp->nBidOrder==371812){
			printf("shenme luoji error =============2.\n");
		}
		//ͳ������Ϣ
		AskOrder2D31(pAskOrder,pTemp,pD31Zd,p);
		BidOrder2D31(pBidOrder,pTemp,pD31Zd);

		if(pD31Zd->aiBidVolume[0]!=pD31Zd->aiAskVolume[0]&&
			p->nT0==94400000&&p->iStockCode==2908){
			printf("shenqi lulu askord=%d,bidord=%d.\n",
				pTemp->nAskOrder,pTemp->nBidOrder);
		}

		//ͳ�������Ϣ
		if(pTemp->nBSFlag=='S')
			AskOrder2D31Zb(pAskOrder,pTemp,pD31Zb);
		else	BidOrder2D31Zb(pBidOrder,pTemp,pD31Zb);

		//�۸��µ� = (trans['�ɽ��۸�'] < trans['�ɽ��۸�'].shift(1)) &
		//(trans['�������'] == trans['�������'].shift(1))
		if(pTemp->nPrice<pAskOrder->nLastPrice){
			pAskOrder->nAskJmpSeq++;
//			pTemp->nAskJmpSeq=pAskOrder->nAskJmpSeq;
		}
		//�۸����� = (trans['�ɽ��۸�'] > trans['�ɽ��۸�'].shift(1)) &
		//(trans['�������'] == trans['�������'].shift(1))
		if(pTemp->nPrice>pBidOrder->nLastPrice&&
			pBidOrder->nLastPrice!=0){
			pBidOrder->nBidJmpSeq++;
//			pTemp->nBidJmpSeq=pBidOrder->nBidJmpSeq;
		}
		
		//�������¼�
		pAskOrder->nLastPrice=pBidOrder->nLastPrice=pTemp->nPrice;
	}

	//����һ��ѭ�����������������ı��£������������������ͳ��
	ptHead=(struct TinyTransactionStruct *)p->S0T.pHead;
	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pNext;

		//�ҵ������������
		if(SearchBin(M_ORDER,(void*)&(pTemp->nAskOrder),
			data_search_bintree_order_2,(void**)&pAskOrder)==NOTFOUND){
			printf("hello 3 code=%d,askord=%d time=%d,flag=%c\n",
				p->iStockCode,pTemp->nAskOrder,pTemp->nTime,(char)(pTemp->nBSFlag));
			printf("hello 2\n");
			return -1;
		}
		//�ҵ��򷽶������
		if(SearchBin(M_ORDER,(void*)&(pTemp->nBidOrder),
			data_search_bintree_order_2,(void**)&pBidOrder)==NOTFOUND){

			printf("hello 3 code=%d,bidord=%d time=%d,flag=%c\n",
				p->iStockCode,pTemp->nBidOrder,pTemp->nTime,(char)(pTemp->nBSFlag));
			return -1;
		}
		
		//d31_new[����i] = trans[(trans['ί�����'] >= i * 10000) &
		//(trans['��������']>0)].groupby(['time_str'])['�ɽ����'].sum().reindex(index = trade_time)/10000
		if(pAskOrder->nAskJmpSeq>0) AskOrder2D31Ex(pAskOrder,pTemp,pD31Ex);
		//d31_new[����i] = trans[(trans['ί����'] >= i * 10000) &
		//(trans['�������']>0)].groupby(['time_str'])['�ɽ����'].sum().reindex(index = trade_time)/10000
		if(pBidOrder->nBidJmpSeq>0) BidOrder2D31Ex(pBidOrder,pTemp,pD31Ex,p);
	}

	//ȡʱ����ڵ����һ��������д���
	/*****
	//�ϰ벿��ָ�꿪ʼ
	int	nTenBidVolume;			//ʮ���������֣�
	int	nTenAskVolume;			//ʮ���������֣�
	int8b	lTenBidAmnt;			//ʮ�����֣�
	int8b	lTenAskAmnt;			//ʮ������֣�
	int	nTotalBidVolume;		//�����������֣�
	int	nTotalAskVolume;		//�����������֣�
	int8b	lTotalBidAmnt;			//�����ܶ�֣�
	int8b	lTotalAskAmnt;			//�����ܶ�֣�
	int	nWtAvgBidPrice;			//��Ȩƽ������ۣ��֣�
	int	nWtAvgAskPrice;			//��Ȩƽ�������ۣ��֣�
	//�°벿��ָ�꿪ʼ
	int	nLastClose;			//�����̼�
	int	nCurPrice;			//���¼�
	int8b	lAvgTotalBidAmnt;		//ƽ�������ܶ����ƽ�����֣�
	int8b	lAvgTotalAskAmnt;		//ƽ�������ܶ����ƽ�����֣�
	*****/

	ptCur=(struct TinyQuotationStruct *)p->S0Q.pHead;
	//ͳ�����������Լ�ÿ������Ľ����ܶ�����ܶ���ۼ�
	while(ptCur!=NULL){
		ptPre=ptCur;
		ptCur=ptCur->pNext;

		p->iQuotationCnt++;
		p->lAddupTotalBidAmnt+=	ptPre->nTotalBidVol*ptPre->nWtAvgBidPrice/100;
		p->lAddupTotalAskAmnt+=	ptPre->nTotalAskVol*ptPre->nWtAvgAskPrice/100;

		if(p->lAddupTotalBidAmnt<0||
			p->lAddupTotalAskAmnt<0){
			printf("design errror.\n");
		}

		if((LIST*)ptPre==p->S0Q.pTail){
//			p->lLastTotalBidAmnt=	ptPre->nTotalBidVol*ptPre->nWtAvgBidPrice/100;
//			p->lLastTotalAskAmnt=	ptPre->nTotalAskVol*ptPre->nWtAvgAskPrice/100;
			break;
		}
		//�������һ���ڵ㶼�ͷ����ڴ�
		free(ptPre);
	}
	ptCur=(struct TinyQuotationStruct *)p->S0Q.pTail;
	if(ptCur!=NULL){
		for(int i=0;i<10;i++){
			pD31Ex->nTenBidVolume+=	ptCur->nBidVol[i];
			pD31Ex->nTenAskVolume+=	ptCur->nAskVol[i];
			pD31Ex->lTenBidAmnt+=	(int8b)ptCur->nBidVol[i]*ptCur->nBidPrice[i]/100;
			pD31Ex->lTenAskAmnt+=	(int8b)ptCur->nAskVol[i]*ptCur->nAskPrice[i]/100;
		}
		pD31Ex->nLastClose=	ptCur->nPreClose;
		pD31Ex->nCurPrice=	ptCur->nMatch;

		pD31Ex->nTotalBidVolume=ptCur->nTotalBidVol;
		pD31Ex->nTotalAskVolume=ptCur->nTotalAskVol;
		pD31Ex->lTotalBidAmnt=	ptCur->nTotalBidVol*ptCur->nWtAvgBidPrice/100;
		pD31Ex->lTotalAskAmnt=	ptCur->nTotalAskVol*ptCur->nWtAvgAskPrice/100;

		pD31Ex->nWtAvgBidPrice=	ptCur->nWtAvgBidPrice;
		pD31Ex->nWtAvgAskPrice=	ptCur->nWtAvgAskPrice;
		
		p->iSamplingCnt++;
		p->lAddupSamplingBidAmnt+=pD31Ex->lTotalBidAmnt;
		p->lAddupSamplingAskAmnt+=pD31Ex->lTotalAskAmnt;

		pD31Ex->lAvgTotalBidAmnt=p->lAddupSamplingBidAmnt/p->iSamplingCnt;
		pD31Ex->lAvgTotalAskAmnt=p->lAddupSamplingAskAmnt/p->iSamplingCnt;

/*
		if(p->iQuotationCnt<0){
			pD31Ex->lAvgTotalBidAmnt=(p->lAddupTotalBidAmnt-p->lLastTotalBidAmnt)/(p->iQuotationCnt-1);
			pD31Ex->lAvgTotalAskAmnt=(p->lAddupTotalAskAmnt-p->lLastTotalAskAmnt)/(p->iQuotationCnt-1);
		}
		else{
			pD31Ex->lAvgTotalBidAmnt=p->lAddupTotalBidAmnt/p->iQuotationCnt;
			pD31Ex->lAvgTotalAskAmnt=p->lAddupTotalAskAmnt/p->iQuotationCnt;
		}
*/
		//�ͷ����һ���ڵ�
		free(ptCur);
	}

	//��LISTHEAD���
	p->S0Q.pHead=p->S0Q.pTail=NULL;	
	return 0;
}

int WriteD31Stat(FILE *fp,struct IndexStatStruct *p)
{
	int i;
	struct D31IndexItemStruct *pD31;

	fprintf(fp,"code=%06d,t=%09d,zd,zb\n",p->iStockCode,p->nT0);


	pD31=&(p->Zb);
	for(i=0;i<MAX_LEVEL_CNT;i++)
		fprintf(fp,"zb:i=%-5ld\tbm=%-10ld\tbv=%-6d\tbo=%d\tam=%-10ld\tav=%-6d\tao=%d\n",
			alAmntLevel[i]/1000000,
			pD31->alBidAmount[i],pD31->aiBidVolume[i],pD31->aiBidOrderNum[i],
			pD31->alAskAmount[i],pD31->aiAskVolume[i],pD31->aiAskOrderNum[i]);

	bzero((void*)pD31,sizeof(struct D31IndexItemStruct));

	pD31=&(p->Zd);
	for(i=0;i<MAX_LEVEL_CNT;i++)
		fprintf(fp,"zd:i=%-5ld\tbm=%-10ld\tbv=%-6d\tbo=%d\tam=%-10ld\tav=%-6d\tao=%d\n",
			alAmntLevel[i]/1000000,
			pD31->alBidAmount[i],pD31->aiBidVolume[i],pD31->aiBidOrderNum[i],
			pD31->alAskAmount[i],pD31->aiAskVolume[i],pD31->aiAskOrderNum[i]);

	bzero((void*)pD31,sizeof(struct D31IndexItemStruct));

	return 0;
}
int WriteD31Stat1(FILE *fp,struct IndexStatStruct *p,int iWriteFlag)
{
	int i;
	struct D31IndexItemStruct *pD31;

//	fprintf(fp,"code=%06d,t=%09d,zd,zb\n",p->iStockCode,p->nT0);
//	$sz_code $prefix $tmp_time ${arr_level[i]} $bid_amnt $bid_volume $bid_num $ask_amnt $ask_volume $ask_num

	pD31=&(p->Zb);
	
	if(iWriteFlag==0){
		for(i=0;i<MAX_LEVEL_CNT;i++){
			fprintf(fp,"%-6d,%s,%-4d,%-4d,%-10ld,%-6d,%-6d,%-10ld,%-6d,%-6d\n",
				p->iStockCode,"z",p->nT0/100000,(int)(alAmntLevel[i]/1000000),
				pD31->alBidAmount[i],pD31->aiBidVolume[i],pD31->aiBidOrderNum[i],
				pD31->alAskAmount[i],pD31->aiAskVolume[i],pD31->aiAskOrderNum[i]);
		}
	}
	bzero((void*)pD31,sizeof(struct D31IndexItemStruct));

	pD31=&(p->Zd);
	if(iWriteFlag==0){
		for(i=0;i<MAX_LEVEL_CNT;i++){
			fprintf(fp,"%-6d,%s,%-4d,%-4d,%-10ld,%-6d,%-6d,%-10ld,%-6d,%-6d\n",
				p->iStockCode,"w",p->nT0/100000,(int)(alAmntLevel[i]/1000000),
				pD31->alBidAmount[i],pD31->aiBidVolume[i],pD31->aiBidOrderNum[i],
				pD31->alAskAmount[i],pD31->aiAskVolume[i],pD31->aiAskOrderNum[i]);

		}
	}
	bzero((void*)pD31,sizeof(struct D31IndexItemStruct));

	if(iWriteFlag==1){
		struct D31IndexExtStruct  *pEx=&p->Ex;
		fprintf(fp,"%-6d,%s,%-4d,%-10d,%-10d,%-12ld,%-12ld,%-10d,%-10d,%-12ld,%-12ld,\
%-10d,%-10d,%-10d,%-10d,%-12ld,%-12ld,%-12ld,%-12ld,%-12ld,%-12ld,%-12ld,%-12ld\n",
			p->iStockCode,"e",p->nT0/100000,
			pEx->nTenBidVolume,	//ʮ���������֣�
			pEx->nTenAskVolume,	//ʮ���������֣�
			pEx->lTenBidAmnt,	//ʮ�����֣�
			pEx->lTenAskAmnt,	//ʮ������֣�
			pEx->nTotalBidVolume,	//�����������֣�
			pEx->nTotalAskVolume,	//�����������֣�
			pEx->lTotalBidAmnt,	//�����ܶ�֣�
			pEx->lTotalAskAmnt,	//�����ܶ�֣�
			pEx->nWtAvgBidPrice,	//��Ȩƽ������ۣ��֣�
			pEx->nWtAvgAskPrice,	//��Ȩƽ�������ۣ��֣�
			pEx->nLastClose,	//�����̼�
			pEx->nCurPrice,		//���¼�
			pEx->lAvgTotalBidAmnt,	//ƽ�������ܶ����ƽ�����֣�
			pEx->lAvgTotalAskAmnt,	//ƽ�������ܶ����ƽ�����֣�
			pEx->alBidAmount[0],	//������20w����λ���֣�
			pEx->alAskAmount[0],	//�������20w����λ���֣� 
			pEx->alBidAmount[1],	//������50w����λ���֣�
			pEx->alAskAmount[1],	//�������50w����λ���֣�
			pEx->alBidAmount[2],	//������100w����λ���֣�
			pEx->alAskAmount[2]	//�������100w����λ���֣�
			);
			
	}
	bzero((void*)&p->Ex,sizeof(struct D31IndexExtStruct));
	return 0;
}

struct IndexStatStruct *GetIndexStat(int iStockCode,char sFileName[],long lCurPos,
	int nBgnActionDay,int nPreT0,int nT0)
{
	struct IndexStatStruct *pTemp;

	//����Ʊ����ĺϷ���
	if(iStockCode<=0||iStockCode>=MAX_STOCK_CODE){
		printf("error stockcode file=%s,pos=%ld,stockcode=%06d\n",
			sFileName,lCurPos,iStockCode);
		return NULL;
	}

	if((pTemp=AISTAT[iStockCode])==NULL){
		if((AISTAT[iStockCode]=NewInitIndexStat(iStockCode,
			nBgnActionDay,nPreT0,nT0))==NULL){
			printf("error new_init_index_stat file=%s,pos=%ld\n",
				sFileName,lCurPos);
			return NULL;
		}
		pTemp=AISTAT[iStockCode];

			//�����ɽڵ㣬���뵽ȫ�ֽṹ����
		InsertList((LIST**)&INDEX_HEAD,(LIST*)pTemp);
	}
	return pTemp;
}


/**
��������
	TAIL_NO_STAT	�����ļ�ĩβ��δͳ��
	WANT_STAT	����nEndTime0��������Ҫͳ��
	MY_PROC_ERROR	�������
**/
#define MY_WANT_STAT	1
#define MY_TAIL_NO_STAT	2

int MountTrsData2IndexStatArray(char sFileName[],int nBgnActionDay,
	int nPreT0,int nT0,int nEndTime0,long lItemLen,long *plCurPos)
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

		if(t.nActionDay!=nBgnActionDay){
			printf("error date file=%s,pos=%ld,actionday=%d,calcdate=%d\n",
				sFileName,lCurPos,t.nActionDay,nBgnActionDay);
			return -1;
		}

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
		if(t.nTime<nPreT0) continue;

		//ʵ�����,ȡ��Ϊ nPreT0<=x<nT0 �����ȥȡ����
		//���,t.nTimeΪ����ʱ��Ҳ�� 15:00:00�������
		if(t.nTime<nT0||(t.nTime==MY_CLOSE_MARKET_TIME&&nT0==MY_CLOSE_MARKET_TIME))
			pSXT=&(pIndexStat->S0T);
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

	fclose(fp);
	*plCurPos=lCurPos;

	return iRet;
}
/*
����ֵ��
	0 �����ļ�ĩβ�������nEndTime0,�Ż�
	-1	�������
*/
int MountQutation2IndexStatArray(char sFileName[],int nBgnActionDay,
	int nPreT0,int nT0,int nEndTime0,long lItemLen,long *plCurPos)
{
	int iRet;
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
	while(1){
		if(fread((void*)sBuffer,lItemLen,1,fp)!=1){
			iRet=MY_TAIL_NO_STAT;
			break;
		}

		lCurPos+=lItemLen;

		GTA_Quotation2TinyQuotation(p,&q);

		if((pIndexStat=GetIndexStat(q.iStockCode,sFileName,
			lCurPos,nBgnActionDay,nPreT0,nT0))==NULL) return -1;


		//��������nEndTime0������ֹͣ
		if(q.nTime>=nEndTime0) break;

		//��lCurPos,���� nT0, ���ڴ��� nPreT0�Ĳ��֣����뵽
		//S0T������nT0�����ݼӵ�pS1Head,��������������nEndTime0������ֹͣ;
		if(q.nTime<=nPreT0) continue;

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

		//��������nEndTime0������ֹͣ
		if(q.nTime>nEndTime0){
			iRet=MY_WANT_STAT;
			break;
		}

	}

	fclose(fp);
	*plCurPos=lCurPos;

	return iRet;
}

void MoveS1T2S0T(struct IndexStatStruct *p,int nPreT0,int nT0)
{
	LISTHEAD *pS0T=&(p->S0T),*pS1T=&(p->S1T);
	struct TinyTransactionStruct *ptHead,*pTemp;

	Destroy2List(pS0T);

	ptHead=(struct TinyTransactionStruct *)pS1T->pHead;

	pS1T->pHead=pS1T->pTail=NULL;

	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pNext;

		if(pTemp->nTime<nT0||
			(pTemp->nTime==MY_CLOSE_MARKET_TIME&&nT0==MY_CLOSE_MARKET_TIME))
			Append2List(pS0T,(LIST*)pTemp);
		else	Append2List(pS1T,(LIST*)pTemp);
	}
	p->nT0=		nT0;
	p->nPreT0=	nPreT0;

}
void MoveS1Q2S0Q(struct IndexStatStruct *p,int nPreT0,int nT0)
{
	LISTHEAD *pS0Q=&(p->S0Q),*pS1Q=&(p->S1Q);
	struct TinyQuotationStruct *ptHead,*pTemp;

	ptHead=(struct TinyQuotationStruct *)pS1Q->pHead;

	pS1Q->pHead=pS1Q->pTail=NULL;

	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pNext;

		if(pTemp->nTime<=nT0)
			Append2List(pS0Q,(LIST*)pTemp);
		else	Append2List(pS1Q,(LIST*)pTemp);
	}
}

int MoveS1O2M_ORDER(struct IndexStatStruct *p,int nT0)
{
	LISTHEAD *pS1O=&(p->S1O);
	struct TinyOrderStruct *ptHead,*pTemp,*pTmp;

	ptHead=(struct TinyOrderStruct *)pS1O->pHead;

	pS1O->pHead=pS1O->pTail=NULL;

	while(ptHead!=NULL){
		pTemp=ptHead;
		ptHead=ptHead->pNext;

		//���������һ��ʱ�εģ�������ŵ�ԭ������
		if(pTemp->nTime>=nT0){
			Append2List(pS1O,(LIST*)pTemp);
			continue;
		}
		//�������������������ţ���ֱ�Ӻϲ��룬���ͷ��ڴ�
		if(SearchBin(p->M_ORDER,(void*)pTemp,
			data_search_bintree_stock_code_order,
			(void**)&pTmp)==FOUND){
			pTmp->nVolume+=		pTemp->nVolume;
			//���ǵ�ͳ�Ƶı�������ʵ��ͳ�ƶ��ٸ������������ﲻ�ۼ�OrderNo
			//pTemp->nOrderNum+=	p->nOrderNum;
			pTmp->lOrderAmnt+=	pTemp->lOrderAmnt;
			free(pTemp);
			continue;
		}

		//������ͷ��ָ�����
		pTemp->pNext=(struct TinyOrderStruct*)&(p->S0O);
		if(InsertBin(&p->M_ORDER,(void *)pTemp,
			data_search_bintree_stock_code_order,
			assign_insert_bintree_stock_code_order_e)<0){
			printf("insert bin malloc error th.\n");
			return -1;
		}
	}
	return 0;
}

/*
����ֵ��
	0 �����ļ�ĩβ�������nEndTime0,�Ż�
	-1	�������
*/
int MountOrdData2IndexStatArray(char sFileName[],int nBgnActionDay,
	int nPreT0,int nT0,int nEndTime0,long lItemLen,long *plCurPos)
{
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
		if(fread((void*)sBuffer,lItemLen,1,fp)!=1)
			break;

		lCurPos+=lItemLen;

		GTA2TDF_SZSEL2_O(p[0],od);

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
			if(od.nTime>=nEndTime0) break;
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
		if(od.nTime>=nEndTime0) break;
	}

	fclose(fp);
	*plCurPos=lCurPos;

	return 0;
}

int GenD31StatAll()
{
	struct IndexStatStruct *pIndexStat=INDEX_HEAD;

	while(pIndexStat!=NULL){
		if(GenD31Stat(pIndexStat)<0){
			printf("gen d31 state error.\n");
			return -1;
		}
		pIndexStat=pIndexStat->pNext;
	}
	return 0;
}

int WriteD31StatAll(FILE *fpD31,char sCodeStr[],int iWriteFlag)
{
	char sTempCode[8];
	struct IndexStatStruct *pIndexStat=INDEX_HEAD;

	while(pIndexStat!=NULL){

		sprintf(sTempCode,"%06d",pIndexStat->iStockCode);
		//������е�Ʊ������ָ��Ʊ�����õ��б���
		if(strlen(sCodeStr)==0||
			strstr(sCodeStr,sTempCode)!=NULL){

			if(WriteD31Stat1(fpD31,pIndexStat,iWriteFlag)<0){
				printf("write d31 error.\n");
				return -1;
			}
		}

		pIndexStat=pIndexStat->pNext;
	}
	return 0;
}
void MoveS1X2S0XAll(int nPreT0,int nT0)
{
	struct IndexStatStruct *pIndexStat=INDEX_HEAD;

	while(pIndexStat!=NULL){
		MoveS1T2S0T(pIndexStat,nPreT0,nT0);
		MoveS1Q2S0Q(pIndexStat,nPreT0,nT0);
		pIndexStat=pIndexStat->pNext;
	}
}

int MoveS1O2M_ORDERAll(int nT0)
{
	struct IndexStatStruct *pIndexStat=INDEX_HEAD;

	while(pIndexStat!=NULL){
		if(MoveS1O2M_ORDER(pIndexStat,nT0)<0) return -1;
		pIndexStat=pIndexStat->pNext;
	}
	return 0;
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

int main(int argc, char *argv[])
{
	FILE *fpD31;
	int iMaxWaitSec=20,iIdleWaiteMilli=10,iWriteFlag=0;
	int iShBusyDelay=12000,iShDelay=3000,iSzBusyDelay=12000,iSzDelay=2000;
	int nBgnActionDay,nBgnTime,nPreT0,nT0,nEndTime0,iThRes,iTzRes,iQhRes,iQzRes;

	char sCurTime[15],sCalcDate[15],sCalcBgn[15];
	char sDelayStr[256],sCodeStr[512],sD31Name[512],sSourcePath[512],sWorkRoot[512];
	char sGtaQhName[512],sGtaQzName[512],sGtaThName[512],sGtaTzName[512],sGtaOzName[512];

	time_t tBeginTime,tEndTime,tDelTime,nLastStatTime;
	//��ǰ�ļ�����λ��
	long lThCurPos=0,lTzCurPos=0,lOzCurPos=0,lQhCurPos=0,lQzCurPos=0;

	GetHostTime(sCurTime);

	//���ò���ѡ���Ĭ��ֵ
	strncpy(sCalcDate,sCurTime,8);sCalcDate[8]=0;
	strcpy(sCalcBgn,sCurTime+8);
	strcpy(sSourcePath,	"/stock/work");
	iMaxWaitSec=20;
	strcpy(sWorkRoot,	"/stock/work");
	strcpy(sCodeStr,"");
	strcpy(sDelayStr,"");

	//��ȡ�����в���
	for (int c; (c = getopt(argc, argv, "d:b:m:s:o:e:t:w:l:?:")) != EOF;){

		switch (c){
		case 'd':strncpy(sCalcDate, optarg,8);sCalcDate[8]=0;	break;
		case 'b':strncpy(sCalcBgn, optarg,6);sCalcBgn[6]=0;	break;
		case 'm':iMaxWaitSec=atoi(optarg);	break;
		case 's':strcpy(sSourcePath, optarg);	break;
		case 'o':strcpy(sWorkRoot, optarg);	break;
		case 'e':strcpy(sDelayStr,optarg);	break;
		case 't':iIdleWaiteMilli=atoi(optarg);	break;
		case 'w':iWriteFlag=atoi(optarg);
			if(iWriteFlag!=1)iWriteFlag=0;	break;
		case 'l':strcpy(sCodeStr, optarg);	break;
		case '?':
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-d calc-date (yyyymmdd =8Bytes def=curdate) ]\n");
			printf("   [-b begin-time (HH24MISS =6Bytes def=curtime) ]\n");
			printf("   [-m max-delay-sec (def=20) ]\n");
			printf("   [-s source-path (def=/stock/work) ]\n");
			printf("   [-o work-root-name (def=/stock/work) ]\n");
			printf("   [-e delay str 'b,i:b:i' ]\n");
			printf("   [-t idlewaitmilli \n");
			printf("   [-w writeflag (0,zbzd 1,ext info) \n");
			printf("   [-l codelist (e.g \"000001,603912,002415\") ]\n");
			exit(1);
			break;
		}
	}

	//����delay���������Ϻ������ڡ�ʱ�Ӳ���
	if(strlen(sDelayStr)!=0){
		char *p0,*p1,*p2;

		if((p0=strchr(sDelayStr,','))==NULL){
			printf("-e =%s ��ʽ�Ƿ�,��ȷ��ʽ�� '12000,1000:6000,900'.\n",sDelayStr);
			return -1;
		}
		p0++;
		if((p1=strchr(p0,':'))==NULL){
			printf("-e =%s ��ʽ�Ƿ�,��ȷ��ʽ�� '12000,1000:6000,900'.\n",sDelayStr);
			return -1;
		}
		p1++;
		if((p2=strchr(p1,','))==NULL){
			printf("-e =%s ��ʽ�Ƿ�,��ȷ��ʽ�� '12000,1000:6000,900'.\n",sDelayStr);
			return -1;
		}
		p2++;
		iShBusyDelay=	atoi(sDelayStr);
		iShDelay=	atoi(p0);
		iSzBusyDelay=	atoi(p1);
		iSzDelay=	atoi(p2);
	}

	//���������ļ���
	sprintf(sGtaThName,"%s/gta_th_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaTzName,"%s/gta_tz_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaOzName,"%s/gta_oz_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaQhName,"%s/gta_qh_%s.dat",sSourcePath,sCalcDate);
	sprintf(sGtaQzName,"%s/gta_qz_%s.dat",sSourcePath,sCalcDate);
	sprintf(sD31Name,	"%s/d31_%s.txt",sWorkRoot,sCalcDate);

        if((fpD31=fopen(sD31Name,"w"))==NULL){
                printf("error open file %s to write.\n",sD31Name);
                return -1;
        }

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
			if(nT0==MY_CLOSE_MARKET_TIME) nEndTime0=MY_CLOSE_MARKET_TIME+1000000;
		}
		else	nEndTime0=iAddMilliSec(nT0,iSzDelay+20000);

		//�������ڶ�������
		if(MountOrdData2IndexStatArray(sGtaOzName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SZSEL2_Order),&lOzCurPos)<0) return -1;

		if(IsBusyTime(nT0))
			nEndTime0=iAddMilliSec(nT0,iSzBusyDelay);
		else	nEndTime0=iAddMilliSec(nT0,iSzDelay);

		//�������ڽ�������
		GTA2TDF_Q2T=GTA2TDF_QZ2T;
		iTzRes=MountTrsData2IndexStatArray(sGtaTzName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SZSEL2_Transaction),&lTzCurPos);
		if(iTzRes<0) return -1;

		//����������������
		GTA_Quotation2TinyQuotation=SZSEL2_Quotation2TinyQuotation;
		iQzRes=MountQutation2IndexStatArray(sGtaQzName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SZSEL2_Quotation),&lQzCurPos);

		if(IsBusyTime(nT0))
			nEndTime0=iAddMilliSec(nT0,iShBusyDelay);
		else	nEndTime0=iAddMilliSec(nT0,iShDelay);

		//�����Ϻ���������
		GTA2TDF_Q2T=GTA2TDF_QH2T;
		iThRes=MountTrsData2IndexStatArray(sGtaThName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SSEL2_Transaction),&lThCurPos);
		if(iThRes<0) return -1;


		//�����Ϻ���������
		GTA_Quotation2TinyQuotation=SSEL2_Quotation2TinyQuotation;
		iQhRes=MountQutation2IndexStatArray(sGtaQhName,nBgnActionDay,nPreT0,nT0,
			nEndTime0,sizeof(long long)+sizeof(SSEL2_Quotation),&lQhCurPos);

		//�����һ���ļ�δ����ͳ�Ƶ㣬��ȴ��ض�����
		if(iThRes==MY_TAIL_NO_STAT||
			iTzRes==MY_TAIL_NO_STAT||
			iQhRes==MY_TAIL_NO_STAT||
			iQzRes==MY_TAIL_NO_STAT){

			int nCurTime=nGetHostCurTime();
			//���û�е���������̵��ӳٷ�Χ�������ߺ��������
			if(iAddMilliSec(nLastStatTime,iMaxWaitSec*1000)<nCurTime){
				usleep(iIdleWaiteMilli*1000);
				continue;
			}
			//д�澯��Ϣ������ͳ��
			printf("cur_time=%d,delay=%d,statit,th=%d,thp=%ld,tz=%d,tzp=%ld.\n",
				nCurTime,iMaxWaitSec,iThRes,lThCurPos,iTzRes,lTzCurPos);
		}

		nLastStatTime=nGetHostCurTime();

		//��һ��ѭ����D31������ͳ�Ƴ���
		if(GenD31StatAll()<0) return -1;

		//��D31�����������ļ�
		if(WriteD31StatAll(fpD31,sCodeStr,iWriteFlag)<0) return -1;

//		printf("hello world tp=%d,tc=%d.\n",nPreT0,nT0);

		nPreT0=nT0;

		//��������ˣ����˳���
		if(nT0>=150000000) break;

		//��������ʱ��ֱ�������������
		if(nT0>=113000000&&nT0<125959000)
			nT0=130100000;
		else	//����ʱ��1����ɨ��һ��
			nT0=iAddMilliSec(nT0,1000*60);

		//����һ��ʱ��˵�ORDER�ϲ�����ǰ���������У�
		//�������ʱ�䳬��nT0�򽫶���������ԭ������
		if(MoveS1O2M_ORDERAll(nT0)<0) return -1;

		//����һ��ʱ��ε�Transaction�ϲ�����ǰ������
		//���Transactionʱ�����nT0�򣬼���������ԭ������
		//����һ��ʱ��ε�Quotation�ϲ�����ǰ������
		MoveS1X2S0XAll(nPreT0,nT0);

		//���϶��رյ�ORDER����ڴ�
		//����Ϊ����������,ֻ�з�æʱ������ٴ���

		tDelTime=tGetHostTime();

		int iAskMaxCnt=0,iCnt=0;	//ASK_MAX���Ľڵ���
		int iBidMaxCnt=0;		//BID_MAX���Ľڵ���
		int iS0OCnt=0;			//M_ORDER���Ľڵ���
		int iFreeS0OCnt=0;		//�ͷŵ���S0O����
/*
		if(!IsBusyTime(nT0)){
			nLastTime=iAddMilliSec(nT0,-1000*60*5);
			pIndexStat=INDEX_HEAD;
			while(pIndexStat!=NULL){

				if(IsStopTime(nT0))
					DeleteCloseOrder(pIndexStat,nLastTime);

				iAskMaxCnt+=	pIndexStat->iAskMaxCnt;
				iBidMaxCnt+=	pIndexStat->iBidMaxCnt;
				iS0OCnt+=	pIndexStat->iS0OCnt;
				iFreeS0OCnt+=	pIndexStat->iFreeS0OCnt;
				iCnt++;

				pIndexStat=pIndexStat->pNext;
			}
		}
*/
		tEndTime=tGetHostTime();

		printf("hello world tp=%d,tc=%d.cost=%ld,%ld\t%d\t%d\t%d\t%d\t%d\n",
			nPreT0,nT0,tEndTime-tBeginTime,tEndTime-tDelTime,
			iCnt,iAskMaxCnt,iBidMaxCnt,iS0OCnt,iFreeS0OCnt);

	}

	fclose(fpD31);

	printf("hello world.\n");
	return 0;
}
