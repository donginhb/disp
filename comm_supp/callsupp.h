#ifndef __CALLSUPP_H__
#define __CALLSUPP_H__
#define MAX_USER_NAME_LEN 64
#define MAX_CLIENT_CNT 1024
#ifndef MAX_STOCK_CODE
#define MAX_STOCK_CODE	1000000
#endif


#include <iostream>
#include <string>

struct UserStruct
{
	struct UserStruct *pNext;
	char sUserName[MAX_USER_NAME_LEN];
	int iParam;	//��������
	int iMqId;
	int iMqPos;
	int iStockCode;
	int iSubscribed;
	struct UserStruct *pFreeNext;
};


struct DispRuleStruct
{
	struct UserStruct *PMALL;
	struct UserStruct *PTALL;
	struct UserStruct *PQALL;
	struct UserStruct *POALL;
	struct UserStruct *PDALL;
	struct UserStruct *AMUSER[MAX_STOCK_CODE];
	struct UserStruct *ATUSER[MAX_STOCK_CODE];
	struct UserStruct *AQUSER[MAX_STOCK_CODE];
	struct UserStruct *AOUSER[MAX_STOCK_CODE];
	struct UserStruct *ADUSER[MAX_STOCK_CODE];
};

extern struct LimitUpDownStruct LIMIT[MAX_STOCK_CODE];

void LockWorkThread();
void UnLockWorkThread();
int IsWorkThreadLock();

void InitUserArray(char sDispName[],struct DispRuleStruct *p);
void RefreshUserArray(char sDispName[],struct DispRuleStruct *p);

int WatchFileCloseWriteAndLock(char sFileName[]);

void SendMsg2Cli(int iStockCode,char cType,string& str);

bool ValidShStockCode(char sStockCode[]);
bool ValidSzStockCode(char sStockCode[]);
bool ValidStockCode(char szWinCode[]);

//#include "QTSStruct.h"

extern struct DispRuleStruct R;
extern char sRefreshDispName[1024];

#define MY_AM_MARKET_BEGIN_TIME	90000000
#define MY_AM_MARKET_OPEN_TIME	93000000
#define MY_AM_MARKET_CLOSE_TIME	113000000
#define MY_PM_MARKET_OPEN_TIME	130000000
#define MY_PM_MARKET_CLOSE_TIME	150000000
#define MY_PM_MARKET_STOP_TIME	153000000
#define MY_DAY_END_TIME		240000000

//����, ָ���طſ�ʼʱ��λ��, ������걾�λط���Ҫ�೤ʱ�䣬��λ�����룩
int GetReplayCostMSec(int nBgnTime);
//���������󣬵�ǰ��ʣ����ٺ������ʹ��
int GetReplayDayLeftMSec(int nStartTime);
//����������Ԥ�Ƴ�����ʲôʱ������ط�
int GetReplayEndTime(int nStartTime,int iCostMSec);
//���ݼ���ó��ģ���ǰ��Ӧ�Ļط�λ�ã����룺MY_PM_MARKET_STOP_TIME�ĺ���������
//�����ReplayTime����ֵΪ
int GetReplayTimeByLeftMSec(int iEndLeftMSec);
//���������е�ǰʱ�䣬ӳ�䵽�ط�λ����ȥ
int nGetReplayTimeByCur(int nCurTime,int nStartTime,int nEndTime,
	int iCostMSec,int iDayLeftMSec);

long lFileSize(char sFileName[]);//��ȡ�ļ���Ϊfilename���ļ���С��

extern unsigned int    nD31TradeTime;

#endif  //__CALLSUPP_H__
