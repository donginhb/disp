#ifndef __INDEX_SUPP_H__
#define __INDEX_SUPP_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <time.h>

#include "bintree.h"
#include "wwtiny.h"

#define MAX_LEVEL_CNT		10
#define MAX_JMP_LEVEL_CNT 	3

//��ʺ���ͳ����Ϣ
struct D31IndexItemStruct
{
//	struct D31IndexItemStruct *pNext;
//	int	nActionDay;	//ί������(YYYYMMDD)
//	int	nTime;		//ί��ʱ��(HHMMSSmmm)
	int8b	alBidAmount[MAX_LEVEL_CNT];	//�����ȣ���λ���֣�
	int	aiBidVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	aiBidOrderNum[MAX_LEVEL_CNT];	//�����������λ���ʣ�
	int8b	alAskAmount[MAX_LEVEL_CNT];	//������ȣ���λ���֣�
	int	aiAskVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	aiAskOrderNum[MAX_LEVEL_CNT];	//������������λ���ʣ�
};

//�Ҳ���չͳ����Ϣ
struct D31IndexExtStruct
{
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
	int8b	alBidAmount[MAX_JMP_LEVEL_CNT];	//�����ȣ���λ���֣�
	int8b	alAskAmount[MAX_JMP_LEVEL_CNT];	//������ȣ���λ���֣�
};

struct TinyTransactionStruct
{
	struct TinyTransactionStruct *pNext;		//��һ����������
	int	nActionDay;	//�ɽ�����(YYYYMMDD)
	int	nTime;		//�ɽ�ʱ��(HHMMSSmmm)
	int	iStockCode;	//֤ȯ����
	int	nPrice;		//�ɽ��۸�(1�֣�100�ɣ���λ��) nPrice==>nPrice,�ɽ��۸񲻻�ܴ���int���á�
	int 	nVolume;	//�ɽ���(����*100=����)
	int	nAskOrder;	//����ί�����
	int	nBidOrder;	//��ί�����
	int     nBSFlag;        //��������(��'B', ����'S', ������' ')
	
	int	nAskOrderSeq;	//����ί�е��ɽ���ţ����ɱ���
	int	nBidOrderSeq;	//��ί�е��ɽ���ţ���ɱ���
//	int	nAskJmpSeq;	//��������
//	int	nBidJmpSeq;	//��������

//	struct TinyTransactionStruct *pAskOrder; //���������ͬ�Ľ����б�
//	struct TinyTransactionStruct *pBidOrder; //���������ͬ�Ľ����б�
};
struct TinyOrderStruct
{
	struct TinyOrderStruct *pNext;
//	struct TinyOrderStruct *pTmpNext;
	int	nActionDay;	//ί������(YYYYMMDD)
	int	nTime;		//ί��ʱ��(HHMMSSmmm)
	int	iStockCode;	//֤ȯ����
	int	nPrice;		//ί�м۸�(1�֣�100�ɣ���λ��) nPrice==>nPrice���ɽ��۸񲻻�ܴ���int���á�
	int 	nVolume;	//�ɽ���(����*100=����)
	int 	nOrder;	        //ί�к�
	int     nBroker;	//�����̱��
	int     nBSFlag;  	//ί�д���('B','S','C')
	
	int	iCloseFlag;	//�����رձ�־��0δ�رգ�1�ѹر�
	int	nOrderSeq;	//�ɽ����к�
	long	lOrderAmnt;	//�ɽ����
	
	int	nLastPrice;	//���¼�
	int	nAskJmpSeq;	//��������
	int	nBidJmpSeq;	//��������

	int	nOriOrdPrice;	//ԭʼί�м۸�
	int	nOriOrdVolume;	//ԭʼί����
	long	lOriOrdAmnt;	//ԭʼί�н��
	
//	struct TinyOrderStruct *pJmpOrder;
	
};
struct TinyQuotationStruct
{
	struct TinyQuotationStruct *pNext;
	int	nActionDay;	//����(YYYYMMDD)
	int	nTime;		//����ʱ��(HHMMSSmmm)
	int	iStockCode;	//֤ȯ����
    	int nPreClose;		//ǰ���̼�
    	int nOpen;		//���̼�
    	int nHigh;		//��߼�
    	int nLow;		//��ͼ�
    	int nMatch;		//���¼�
    	int nAskPrice[10];	//������
    	int nAskVol[10];	//������
    	int nBidPrice[10];	//�����
    	int nBidVol[10];	//������
	int nNumTrades;		//�ɽ�����
    	int8b	iVolume;	//�ɽ�����
    	int8b	iTurnover;	//�ɽ��ܽ��
    	int8b	nTotalBidVol;	//ί����������
    	int8b	nTotalAskVol;	//ί����������
    	int	nWtAvgBidPrice;	//��Ȩƽ��ί��۸�
    	int	nWtAvgAskPrice;	//��Ȩƽ��ί���۸�
};

struct PriceOrderStruct
{
	int nPrice;
	int nOrder;
	int nOrderTime;
};

struct IndexStatStruct
{
	struct IndexStatStruct *pNext;	//��һ֧��Ʊ������
	int iStockCode;
	int nActionDay;
	int nPreT0;
	int nT0;
	int iS0Cnt;		//S0����Ĵ�С
	int iPreS0Cnt;		//��S0�����У����ֵ�С��nPreT0�ļ�¼��
	
	int iAskMaxCnt;		//ASK_MAX���Ľڵ���
	int iBidMaxCnt;		//BID_MAX���Ľڵ���
	int iS0OCnt;		//M_ORDER���Ľڵ���
	int iFreeS0OCnt;	//�ͷŵ���S0D����
	
	int	iQuotationCnt;		//�������
	int8b	lAddupTotalBidAmnt;	//ÿ����������������ۼ�
	int8b	lAddupTotalAskAmnt;	//ÿ����������������ۼ�
//	int8b	lLastTotalBidAmnt;	//���һ�������������
//	int8b	lLastTotalAskAmnt;	//���һ�������������

	int	iSamplingCnt;		//�����������
	int8b	lAddupSamplingBidAmnt;	//ÿ�ʲ�����������������ۼ�
	int8b	lAddupSamplingAskAmnt;	//ÿ�ʲ�����������������ۼ�

	
	BINTREE *ASK_MAX;		//�Գ���������Ϊ�������ɽ��ۺ����ASK_ORDER�Ĺ�ϵ
	BINTREE *BID_MAX;		//�Գ����뽻��Ϊ�������ɽ��ۺ����BID_ORDER�Ĺ�ϵ
	struct D31IndexItemStruct Zd;	//��ͳ������
	struct D31IndexItemStruct Zb;	//���ͳ������
	struct D31IndexExtStruct  Ex;	//��չ��Ϣ
	BINTREE *M_ORDER;		//�ϲ��������ݣ���ORDER_ID����
	LISTHEAD S0O;	//���(~,T0]���е�M_ORDER�еĶ����б�
	LISTHEAD S1O;	//���[T0,~)֮���ORDER����}
	LISTHEAD S0T;	//���[T0-1,T0),{��T0==15:00:00 ʱ��Ϊ [14:59,15:00]}�Ľ�������
	LISTHEAD S1T;	//���[T0,֮��Ľ�������}
	LISTHEAD S0Q;	//���[T0-1,T0),{��T0==15:00:00 ʱ��Ϊ [14:59,15:00]}����������
	LISTHEAD S1Q;	//���[T0,֮�����������}
};

#include "../app_ints/d31_item.h"

#include "TDFAPIStruct.h"

#define MY_DATE_CEIL_LONG 1000000000L
#define MY_CLOSE_MARKET_TIME 150000000
#define MY_OPEN_MARKET_TIME  93000000


void TDF_TRANSACTION2TinyTransaction(struct TDF_TRANSACTION *pi,struct TinyTransactionStruct *po);
void TDF_TRANSACTION2TinyOrderS(struct TDF_TRANSACTION *pi,struct TinyOrderStruct *po);
void TDF_TRANSACTION2TinyOrderB(struct TDF_TRANSACTION *pi,struct TinyOrderStruct *po);
void TDF_ORDER2TinyOrder(struct TDF_ORDER *pi,struct TinyOrderStruct *po);

int AddTransaction2IndexStat(struct TDF_TRANSACTION *pi,int nT0,
	struct IndexStatStruct *pIndexStat);

struct IndexStatStruct *GetIndexStat(int iStockCode,char sFileName[],long lCurPos,
	int nBgnActionDay,int nPreT0,int nT0);
int data_search_bintree_order_2(void *pValue,void*pData);
int data_search_bintree_stock_code_order(void *pValue,void*pData);
void assign_insert_bintree_stock_code_order_e(void **ppData,void *pData);
int GenD31StatAll();
int WriteD31StatAll(FILE *fpD31,char sCodeStr[],int iWriteFlag);
void MoveS1X2S0XAll(int nPreT0,int nT0);
int MoveS1O2M_ORDERAll(int nT0);

#ifndef MAX_STOCK_CODE
#define MAX_STOCK_CODE	1000000
#endif

/***
	TAIL_NO_STAT	�����ļ�ĩβ��δͳ��
	WANT_STAT	����nEndTime0��������Ҫͳ��
***/
#define MY_WANT_STAT	1
#define MY_TAIL_NO_STAT	2


#endif
