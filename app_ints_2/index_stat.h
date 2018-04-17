#ifndef __INDEX_STAT_H__
#define __INDEX_STAT_H__

#define MAX_LEVEL_CNT	10

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
	int8b	alSaleAmount[MAX_LEVEL_CNT];	//������ȣ���λ���֣�
	int	aiSaleVolume[MAX_LEVEL_CNT];	//����������λ���֣�
	int	aiSaleOrderNo[MAX_LEVEL_CNT];	//������������λ���ʣ�
};

//ÿ���ṹ40���ֽڣ�16+4*6 = 40
struct TinyTransactionStruct
{
	struct TinyTransactionStruct *pNext;		//��һ����������
	int	nActionDay;	//�ɽ�����(YYYYMMDD)
	int	nTime;		//�ɽ�ʱ��(HHMMSSmmm)
	int	iStockCode;	//֤ȯ����
	int	nPrice;		//�ɽ��۸�, nPrice==>nPrice,�ɽ��۸񲻻�ܴ���int���á�
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
	int	nPrice;		//ί�м۸�, nPrice==>nPrice���ɽ��۸񲻻�ܴ���int���á�
	int 	nVolume;	//�ɽ���
	int 	nOrder;	        //ί�к�
	int     nBroker;	//�����̱��
	int     nBSFlag;  	//ί�д���('B','S','C')
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
/*
	int nAskDownPrice;	//������ͳɽ���,��λ���֣�
	int nBidUpPrice;	//������߳ɽ���,��λ���֣�
	int nAskMaxOrder;	//������󶩵��ţ���Ӧ��������ͼ�
	int nBidMaxOrder;	//������󶩵��ţ���Ӧ��������߼�
*/	
	BINTREE *ASK_MAX;	//�Գ���������Ϊ�������ɽ��ۺ����ASK_ORDER�Ĺ�ϵ
	BINTREE *BID_MAX;	//�Գ����뽻��Ϊ�������ɽ��ۺ����BID_ORDER�Ĺ�ϵ
	struct D31IndexItemStruct I;
	BINTREE *M_ORDER;			//�ϲ��������ݣ���ORDER_ID����
	BINTREE *R_ORDER;			//��ʵ�������ݣ���ORDER_ID����
	struct TinyOrderStruct PreS0M;		//֮ǰ�ĺϲ���������
	struct TinyOrderStruct PreS0R;		//֮ǰ��ʵ�ʶ�������
	struct TinyOrderStruct *pS0M;		//�ϲ����������б�
	struct TinyOrderStruct *pS0R;		//ʵ�ʶ��������б�
	struct TinyTransactionStruct *pS0Head;	//���(T0-1,T0]�Ľ�������
	struct TinyTransactionStruct *pS1Head;  //���(T0,֮��Ľ�������};
};

#define MY_GET_MILLI_SEC(x)	(x%1000)
#define MY_GET_SEC(x)		((x%100000)/1000)
#define MY_GET_MIM(x)		((x%10000000)/100000)
#define MY_GET_HOUR(x)		(x/10000000)

#endif
