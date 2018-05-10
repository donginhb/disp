#pragma once
#include <string>
#include <vector>

#include <chrono>
#include <iostream>
#include <thread>
#include <sstream>

using namespace std;

#pragma  pack(push)
#pragma pack(1)

struct IpDomain
{
	string ip;
	bool fromDomain = false; //ԭ�������ļ����Ƿ�Ϊ����
};

extern unsigned int g_iPort;
extern string g_strPassword;
extern string g_strUsername;
extern string password;
extern int st_interface;
extern IpDomain g_strServer;

struct TDF_MARKET_DATA2
{
	char szWindCode[32]; //600001.SH
	char szCode[32]; //ԭʼCode
	int nActionDay; //ҵ������(��Ȼ��)
	int nTradingDay; //������
	int nTime; //ʱ��(HHMMSSmmm)
	int nStatus; //״̬
	int64_t nPreClose; //ǰ���̼�
	int64_t nOpen; //���̼�
	int64_t nHigh; //��߼�
	int64_t nLow; //��ͼ�
	int64_t nMatch; //���¼�
	int64_t nAskPrice[10]; //������
	int64_t nAskVol[10]; //������
	int64_t nBidPrice[10]; //�����
	int64_t nBidVol[10]; //������
	int nNumTrades; //�ɽ�����
	int64_t iVolume; //�ɽ�����
	int64_t iTurnover; //�ɽ��ܽ��
	int64_t nTotalBidVol; //ί����������
	int64_t nTotalAskVol; //ί����������
	int64_t nWeightedAvgBidPrice; //��Ȩƽ��ί��۸�
	int64_t nWeightedAvgAskPrice; //��Ȩƽ��ί���۸�
	int nIOPV; //IOPV��ֵ��ֵ
	int nYieldToMaturity; //����������
	int64_t nHighLimited; //��ͣ��
	int64_t nLowLimited; //��ͣ��
	char chPrefix[4]; //֤ȯ��Ϣǰ׺
	int nSyl1; //��ӯ��1
	int nSyl2; //��ӯ��2
	int nSD2; //����2���Ա���һ�ʣ�
};

struct TDF_TRANSACTION2
{
	char szWindCode[32]; //600001.SH
	char szCode[32]; //ԭʼCode
	int32_t nActionDay; //��Ȼ��
	int32_t nTime; //�ɽ�ʱ��(HHMMSSmmm)
	int32_t nIndex; //�ɽ����
	int64_t nPrice; //�ɽ��۸�
	int nVolume; //�ɽ�����
	int64_t nTurnover; //�ɽ����
	int nBSFlag; //��������(��'B', ����'S', ������' ')
	char chOrderKind; //�ɽ����
	char chFunctionCode; //�ɽ�����
	int nAskOrder; //������ί�����
	int nBidOrder; //����ί�����
};

struct TDF_ORDER_QUEUE2
{
	char szWindCode[32]; //600001.SH
	char szCode[32]; //ԭʼCode
	int nActionDay; //��Ȼ��
	int nTime; //ʱ��(HHMMSSmmm)
	int nSide; //��������('B':Bid 'A':Ask)
	int64_t nPrice; //ί�м۸�
	int nOrders; //��������
	int nABItems; //��ϸ����
	int nABVolume[200]; //������ϸ
	//int getABItems();
};

struct TDF_ORDER2
{
	char szWindCode[32]; //600001.SH
	char szCode[32]; //ԭʼCode
	int nActionDay; //ί������(YYMMDD)
	int nTime; //ί��ʱ��(HHMMSSmmm)
	int nOrder; //ί�к�
	int64_t nPrice; //ί�м۸�
	int nVolume; //ί������
	char chOrderKind; //ί�����
	char chFunctionCode; //ί�д���('B','S','C')
	int nBroker; //�����̱���
	char chStatus; //ί��״̬
	char chFlag; //��־
};

#ifndef SYMBOL_LEN
#define SYMBOL_LEN 40
#endif
/// �Ͻ���L2ָ��
struct SSEL2_Index2
{
	int Time; ///< ��������ʱ��(����), 143025000 ��ʾ 14:30:25000
	int TradeTime; ///< �ɽ�ʱ��(����),
	char Symbol[SYMBOL_LEN]; ///< ֤ȯ����,
	double TotalAmount; ///< �ɽ��ܶ�,
	double HighPrice; ///< ��߼�,
	double LowPrice; ///< ��ͼ�,
	double LastPrice; ///< �ּ�,
	unsigned long long TotalVolume; ///< �ɽ�����, ��
};

/// ���L2ָ������
struct SZSEL2_Index2
{
	long long Time; ///< ��������ʱ��YYYYMMDDHHMMSSMMM
	char Symbol[SYMBOL_LEN]; ///< ֤ȯ����
	double HighPrice; ///< ���ָ��,6λС��
	double LowPrice; ///< ���ָ��,6λС��
	double LastPrice; ///< ����ָ��,6λС��
	double TotalAmount; ///< ���������Ӧָ���ĳɽ����,4λС��
	unsigned long long TotalNo; ///< ���������Ӧָ���ĳɽ�����
	double TotalVolume; ///< ���������Ӧָ���Ľ���������2λС��
};

enum MarketInterfaceError
{
	M_SUCCESS = (int16_t) 0,
	ERR_CONNECT = (int16_t) 1, //�������Ӵ���
	ERR_LOGIN = (int16_t) 2, //��¼ʧ��
};

#pragma pack(pop)


typedef void (*RecvHandleM)(const TDF_MARKET_DATA2 *msg);
typedef void (*RecvHandleT)(const TDF_TRANSACTION2 *msg);
typedef void (*RecvHandleQ)(const TDF_ORDER_QUEUE2 *msg);
typedef void (*RecvHandleO)(const TDF_ORDER2 *msg);
//typedef void( *RecvHandleIdxH)(const SSEL2_Index2 *msg);
//typedef void( *RecvHandleIdxZ)(const SZSEL2_Index2 *msg);
typedef void (*RecvHandleE)(int16_t errCode, const char *errmsg);

//����ֵ��true:�ɹ� false:ʧ��
//bool initIdxH(RecvHandleIdxH handleIH);
//bool initIdxZ(RecvHandleIdxZ handleIZ);
//errmsg��׼����256�ֽڿռ�
int16_t m_connect(char *errmsg);
int16_t login(char *errmsg);
//��¼�ɹ����ٵ��á���ȡ�ɶ��ĵ�֤ȯ���롣codes������׼����8000*4byte�Ŀռ䡣����ֵ��֤ȯ��������
int32_t getCodes(int32_t *codes);
//һ�����ô˺�������ֻ�ᷢ�����֤ȯ��������顣����δ���ô˺�������ȫ���뷢�͡�addReduce:1����,0����
void addReduceCode(int8_t addReduce, int32_t *codes, int32_t size);
void subscribe(bool marketdata, bool transaction, bool orderqueue, bool order);
//void subscribeIH(int8_t subOrCancel);//1���Ļ�ָ��0ȡ������
//void subscribeIZ(int8_t subOrCancel);//1������ָ��0ȡ������
void replay(bool marketdata, bool transaction, bool orderqueue, bool order, uint32_t timeBegin, uint32_t timeEnd, uint32_t date);
//m_connect, login, RecvHandleE��������ʱ����ô˺�����֮���û����ٴ�m_connect��login
//���Ƽ���RecvHandleE�Ķ�ջ�������˺���
void marketInterfaceClose();
