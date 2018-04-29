#ifndef __INDEX_STAT_H__
#define __INDEX_STAT_H__

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

//ÿ���ṹ40���ֽڣ�16+4*6 = 40
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
	int	nAskJmpSeq;	//��������
	int	nBidJmpSeq;	//��������

	struct TinyOrderStruct *pAskOrder;
	struct TinyOrderStruct *pBidOrder;
};
//ÿ���ṹ40���ֽڣ�16+4*6 = 40
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

#endif
