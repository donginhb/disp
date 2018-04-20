#ifndef __INDEX_STAT_H__
#define __INDEX_STAT_H__

#define MAX_LEVEL_CNT	10

//���ṹ336���ֽ�
struct D31IndexItemStruct
{
	struct D31IndexItemStruct *pNext;
	int	nActionDay;	//ί������(YYYYMMDD)
	int	nTime;		//ί��ʱ��(HHMMSSmmm)
	int8b	alBidAmount[MAX_LEVEL_CNT];	//�����ȣ���λ���֣�
	int	aiBidVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	aiBidOrderNum[MAX_LEVEL_CNT];	//�����������λ���ʣ�
	int8b	alAskAmount[MAX_LEVEL_CNT];	//������ȣ���λ���֣�
	int	aiAskVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	aiAskOrderNum[MAX_LEVEL_CNT];	//������������λ���ʣ�
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

	struct TinyOrderStruct *pAskOrder;
	struct TinyOrderStruct *pBidOrder;
};
//ÿ���ṹ40���ֽڣ�16+4*6 = 40
struct TinyOrderStruct
{
	struct TinyOrderStruct *pNext;
	struct TinyOrderStruct *pTmpNext;
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
	
	int	nOriOrdPrice;	//ԭʼί�м۸�
	int	nOriOrdVolume;	//ԭʼί����
	long	lOriOrdAmnt;	//ԭʼί�н��
	
};
struct PriceOrderStruct
{
	int nPrice;
	int nOrder;
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
	
	BINTREE *ASK_MAX;	//�Գ���������Ϊ�������ɽ��ۺ����ASK_ORDER�Ĺ�ϵ
	BINTREE *BID_MAX;	//�Գ����뽻��Ϊ�������ɽ��ۺ����BID_ORDER�Ĺ�ϵ
	struct D31IndexItemStruct Zd;		//��ͳ������
	struct D31IndexItemStruct Zb;		//���ͳ������
	BINTREE *M_ORDER;			//�ϲ��������ݣ���ORDER_ID����
//	struct TinyOrderStruct PreS0M;		//֮ǰ�ĺϲ���������
//	struct TinyOrderStruct *pS1M;		//�ϲ����������б�(T0,֮��Ķ�������};
//	struct TinyTransactionStruct *pS0Head;	//���(T0-1,T0]�Ľ�������
//	struct TinyTransactionStruct *pS1Head;  //���(T0,֮��Ľ�������};

	LISTHEAD S0O;	//������е�M_ORDER�еĶ����б�
	LISTHEAD S0T;	//���(T0-1,T0]�Ľ�������
	LISTHEAD S1T;	//���(T0,֮��Ľ�������}
};

#define MY_GET_MILLI_SEC(x)	(x%1000)
#define MY_GET_SEC(x)		((x%100000)/1000)
#define MY_GET_MIM(x)		((x%10000000)/100000)
#define MY_GET_HOUR(x)		(x/10000000)

#endif
