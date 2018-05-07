
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>

#include "wwtiny.h"
#include "QTSDataType.h"
#include "QTSStruct.h"
#include "QTSDataFieldDefine.h"

#include "gta_supp.h"

#define MY_TYPE_SSEL2_Quotation		1
#define MY_TYPE_SSEL2_Transaction	2
#define MY_TYPE_SSEL2_Auction		3
#define MY_TYPE_SZSEL2_Quotation	4
#define MY_TYPE_SZSEL2_Transaction	5
#define MY_TYPE_SZSEL2_Order		6

#define MY_TYPE_TDF_MKT			'M'
#define MY_TYPE_TDF_TRA			'T'
#define MY_TYPE_TDF_ORD			'O'
#define MY_TYPE_TDF_QUE			'Q'


#define MY_DATE_CEIL_LONG 1000000000L

int64_t my_yuan2percentFen(const double yuan)
{
	return int64_t((yuan + 0.00005) * 10000);
}
int print_MY_TYPE_SSEL2_Quotation(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	SSEL2_Quotation   *p=(SSEL2_Quotation *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->Symbol)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\t%s\t%s\n",
			"picktime",
			"localtime",
			"packtime",
			"createtime",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->Time;break;
		case 2: *plCurTime=lTmpTime;break;
		case 3:	*plCurTime=p->LocalTimeStamp;break;
		case 4:	*plCurTime=(long)(p->PacketTimeStamp%MY_DATE_CEIL_LONG);break;
		default:*plCurTime=p->Time;break;
	}

	if(*plCurTime<lBgnTime) return 2;


	sprintf(outbuf,"%lld\t%d\t%lld\t%d\t%s\t%d\t%s\
\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\n",
		*(long long *)buf,	//picktime
		p->LocalTimeStamp,	//< ���ݽ���ʱ��HHMMSSMMM
		p->PacketTimeStamp,	//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		p->Time,		//< ��������ʱ��, ���¶���ʱ�䣨���룩;143025001 ��ʾ 14:30:25.001
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)p->Time),
		p->QuotationFlag,				//��һ������
    		(int)my_yuan2percentFen(p->PreClosePrice),         ///< ���ռ�,
    		(int)my_yuan2percentFen(p->OpenPrice),             ///< ���̼�,
    		(int)my_yuan2percentFen(p->HighPrice),             ///< ��߼�,
    		(int)my_yuan2percentFen(p->LowPrice),              ///< ��ͼ�,
    		(int)my_yuan2percentFen(p->LastPrice),             ///< �ּ�,
    		(int)my_yuan2percentFen(p->ClosePrice),            ///< �����̼�,
		p->TradeStatus,    				///< ��ǰƷ�ֽ���״̬,
		p->SecurityPhaseTag,   				///< ��ǰƷ��״̬
		(int)p->TotalNo,                		///< �ɽ�����,
		(int)p->TotalVolume,           			///< �ɽ�����, ��Ʊ����),Ȩ֤����),ծȯ����
    		(int)my_yuan2percentFen(p->TotalAmount),           ///< �ɽ��ܶ�, ��Ԫ��
		(int)p->TotalBuyOrderVolume,    		///< ί����������, ��Ʊ����),Ȩ֤����),ծȯ����
    		(int)my_yuan2percentFen(p->WtAvgBuyPrice),         ///< ��Ȩƽ��ί���, ��Ԫ��
    		(int)my_yuan2percentFen(p->BondWtAvgBuyPrice),     ///< ծȯ��Ȩƽ��ί���, ��Ԫ��
		(int)p->TotalSellOrderVolume,			///< ί����������,
    		(int)my_yuan2percentFen(p->WtAvgSellPrice),        ///< ��Ȩƽ��ί����, ��Ԫ��
    		(int)my_yuan2percentFen(p->BondWtAvgSellPrice),    ///< ծȯ��Ȩƽ��ί����,
    		(int)my_yuan2percentFen(p->IOPV),                  ///< ETF ��ֵ��ֵ,
		p->ETFBuyNo,					///< ETF �깺����,	�ڶ�������
		(int)p->ETFBuyVolume, 			        ///< ETF �깺��,
    		(int)my_yuan2percentFen(p->ETFBuyAmount),          ///< ETF �깺��,
    		p->ETFSellNo,             			///< ETF ��ر���,
    		(int)p->ETFSellVolume,          		///< ETF �����,
    		(int)my_yuan2percentFen(p->ETFSellAmount),         ///< ETF ��ض�,
    		(int)my_yuan2percentFen(p->YTM),                   ///< ծȯ����������,
    		(int)p->TotalWarrantExecVol,    		///< Ȩִ֤�е�������,
    		(int)my_yuan2percentFen(p->WarrantDownLimit),      ///< Ȩ֤��ͣ�۸�, ��Ԫ��
    		(int)my_yuan2percentFen(p->WarrantUpLimit),        ///< Ȩ֤��ͣ�۸�, ��Ԫ��
    		p->WithdrawBuyNo,          			///< ���볷������,
    		(int)p->WithdrawBuyVolume,      		///< ���볷����,
    		(int)my_yuan2percentFen(p->WithdrawBuyAmount),      ///< ���볷����,
    		p->WithdrawSellNo,				///< ������������,
    		(int)p->WithdrawSellVolume,			///< ����������,
    		(int)my_yuan2percentFen(p->WithdrawSellAmount),     ///< ����������,
    		p->TotalBuyNo,             ///< �����ܱ���,
    		p->TotalSellNo,            ///< �����ܱ���,
    		p->MaxBuyDuration,         ///< ����ɽ����ȴ�ʱ��,
    		p->MaxSellDuration,        ///< �����ɽ����ȴ�ʱ��,
    		p->BuyOrderNo,             ///< ��ί�м�λ��,
    		p->SellOrderNo,            ///< ����ί�м�λ��,		����������
    		(int)p->SellLevelNo,	   ///< ���̼�λ����, 10�����飬����ʱ���� ���ĸ�����
    		(int)my_yuan2percentFen(p->SellPrice01),
    		(int)p->SellVolume01,
    		(int)p->TotalSellOrderNo01,
    		(int)my_yuan2percentFen(p->SellPrice02),
    		(int)p->SellVolume02,
    		(int)p->TotalSellOrderNo02,
    		(int)my_yuan2percentFen(p->SellPrice03),
    		(int)p->SellVolume03,
    		(int)p->TotalSellOrderNo03,
    		(int)my_yuan2percentFen(p->SellPrice04),
    		(int)p->SellVolume04,
    		(int)p->TotalSellOrderNo04,
    		(int)my_yuan2percentFen(p->SellPrice05),
    		(int)p->SellVolume05,
    		(int)p->TotalSellOrderNo05,		//���������
    		(int)my_yuan2percentFen(p->SellPrice06),
    		(int)p->SellVolume06,
    		(int)p->TotalSellOrderNo06,
    		(int)my_yuan2percentFen(p->SellPrice07),
    		(int)p->SellVolume07,
    		(int)p->TotalSellOrderNo07,
    		(int)my_yuan2percentFen(p->SellPrice08),
    		(int)p->SellVolume08,
    		(int)p->TotalSellOrderNo08,
    		(int)my_yuan2percentFen(p->SellPrice09),
    		(int)p->SellVolume09,
    		(int)p->TotalSellOrderNo09,
    		(int)my_yuan2percentFen(p->SellPrice10),
    		(int)p->SellVolume10,
    		(int)p->TotalSellOrderNo10,		//����������
    		(int)p->SellLevelQueueNo01,
    		(int)p->SellLevelQueue[0],
    		(int)p->BuyLevelNo,			//���߸�����
    		(int)my_yuan2percentFen(p->BuyPrice01),
    		(int)p->BuyVolume01,
    		(int)p->TotalBuyOrderNo01,
    		(int)my_yuan2percentFen(p->BuyPrice02),
    		(int)p->BuyVolume02,
    		(int)p->TotalBuyOrderNo02,
    		(int)my_yuan2percentFen(p->BuyPrice03),
    		(int)p->BuyVolume03,
    		(int)p->TotalBuyOrderNo03,
    		(int)my_yuan2percentFen(p->BuyPrice04),
    		(int)p->BuyVolume04,
    		(int)p->TotalBuyOrderNo04,
    		(int)my_yuan2percentFen(p->BuyPrice05),
    		(int)p->BuyVolume05,
    		(int)p->TotalBuyOrderNo05,		//�ڰ˸�����
    		(int)my_yuan2percentFen(p->BuyPrice06),
    		(int)p->BuyVolume06,
    		(int)p->TotalBuyOrderNo06,
    		(int)my_yuan2percentFen(p->BuyPrice07),
    		(int)p->BuyVolume07,
    		(int)p->TotalBuyOrderNo07,
    		(int)my_yuan2percentFen(p->BuyPrice08),
    		(int)p->BuyVolume08,
    		(int)p->TotalBuyOrderNo08,
    		(int)my_yuan2percentFen(p->BuyPrice09),
    		(int)p->BuyVolume09,
    		(int)p->TotalBuyOrderNo09,
    		(int)my_yuan2percentFen(p->BuyPrice10),
    		(int)p->BuyVolume10,
    		(int)p->TotalBuyOrderNo10,    		//�ھŸ�����
    		(int)p->BuyLevelQueueNo01,
    		(int)p->BuyLevelQueue[0]
		);

	return 0;
}

int print_MY_TYPE_SSEL2_Transaction(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	SSEL2_Transaction   *p=(SSEL2_Transaction *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->Symbol)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\t%s\t%s\n",
			"picktime",
			"localtime",
			"packtime",
			"tradetime",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->TradeTime;break;
		case 2: *plCurTime=lTmpTime;break;
		case 3:	*plCurTime=p->LocalTimeStamp;break;
		case 4:	*plCurTime=(long)(p->PacketTimeStamp%MY_DATE_CEIL_LONG); break;
		default:*plCurTime=p->TradeTime;break;
	}

	if(*plCurTime<lBgnTime) return 2;

	sprintf(outbuf,"%lld\t%d\t%lld\t%d\t%s\t%d\
\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%c\n",
		*(long long *)buf,	//picktime
		p->LocalTimeStamp,	//< ���ݽ���ʱ��HHMMSSMMM
		p->PacketTimeStamp,	//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		p->TradeTime,		//< �ɽ�ʱ��(����), 14302506 ��ʾ14:30:25.06
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)p->TradeTime),//�ڶ��п�ʼ
		p->QuotationFlag,  			///< 1=�Ϻ��ϻ��������Դ��2=���ڸ�����������Դ
		(int)p->RecID,                  	///< ҵ������, �� 1 ��ʼ���� TradeChannel����
		(int)p->TradeChannel,           	///< �ɽ�ͨ��,
		(int)my_yuan2percentFen(p->TradePrice),	///< �ɽ��۸�,
		(int)p->TradeVolume,            	///< �ɽ�����, ��Ʊ����,Ȩ֤����,ծȯ����
		(int)my_yuan2percentFen(p->TradeAmount),///< �ɽ����,
		(int)p->BuyRecID,               	///< �򷽶�����,
		(int)p->SellRecID,              	///< ����������,
		p->BuySellFlag            		///< �����̱�־, B �C ����,������,S �C ����,������,N �C δ֪
		);

	return 0;
}
int print_MY_TYPE_SSEL2_Auction(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	SSEL2_Auction   *p=(SSEL2_Auction *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->Symbol)==NULL) return 1;

//	TDF_MARKET_DATA m;

//	GTA2TDF_SSEL2_AM(p[0],m);

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\t%s\n",
			"picktime",
			"localtime",
			"packtime",
			"createtime",
			"stockcode",
			"difftime",
			"quotation_flag",
			"open_price",
			"auction_volume",
			"leave_volume",
			"side");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->Time;break;
		case 2: *plCurTime=lTmpTime;break;
		case 3:	*plCurTime=p->LocalTimeStamp;break;
		case 4:	*plCurTime=(long)(p->PacketTimeStamp%MY_DATE_CEIL_LONG);break;
		default:*plCurTime=p->Time;break;
	}

	if(*plCurTime<lBgnTime) return 2;

	sprintf(outbuf,"%lld\t%d\t%lld\t%d\t%s\t%d\t%s\t%d\t%d\t%d\t%c\n",
		*(long long *)buf,	//picktime
		p->LocalTimeStamp,	//< ���ݽ���ʱ��HHMMSSMMM
		p->PacketTimeStamp,	//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		p->Time,		//< ��������ʱ��, 143025001 ��ʾ 14:30:25.001
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)p->Time),
		p->QuotationFlag,
		(int)my_yuan2percentFen(p->OpenPrice),
		(int)p->AuctionVolume,
		(int)p->LeaveVolume,
		p->Side);

	return 0;
}
int print_MY_TYPE_SZSEL2_Quotation(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	SZSEL2_Quotation   *p=(SZSEL2_Quotation *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->Symbol)==NULL) return 1;

	*plCurTime=p->Time%MY_DATE_CEIL_LONG;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\t%s\t%s\n",
			"picktime",
			"localtime",
			"packtime",
			"createtime",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->Time%MY_DATE_CEIL_LONG;break;
		case 2: *plCurTime=lTmpTime;break;
		case 3:	*plCurTime=p->LocalTimeStamp;break;
		default:*plCurTime=p->Time;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	sprintf(outbuf,"%lld\t%d\t%ld\t%d\t%s\t%d\t%s\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\n",
		*(long long *)buf,	//picktime
		p->LocalTimeStamp,	//< ���ݽ���ʱ��HHMMSSMMM
		99999999999999999,	//< ����ֵ
		(int)(p->Time%MY_DATE_CEIL_LONG),//< ��������ʱ��YYYYMMDDHHMMSSMMM
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)(p->Time%MY_DATE_CEIL_LONG)),
		p->QuotationFlag,				//��һ������
    		(int)my_yuan2percentFen(p->PreClosePrice),
    		(int)my_yuan2percentFen(p->OpenPrice),
    		(int)my_yuan2percentFen(p->LastPrice),
    		(int)my_yuan2percentFen(p->HighPrice),
    		(int)my_yuan2percentFen(p->LowPrice),
    		(int)my_yuan2percentFen(p->PriceUpLimit),
    		(int)my_yuan2percentFen(p->PriceDownLimit),
    		(int)my_yuan2percentFen(p->PriceUpdown1),
    		(int)my_yuan2percentFen(p->PriceUpdown2),
    		(int)p->TotalNo,
    		(int)my_yuan2percentFen(p->TotalVolume),
    		(int)my_yuan2percentFen(p->TotalAmount),
    		(int)my_yuan2percentFen(p->ClosePrice),
    		"0",//p->SecurityPhaseTag,
    		(int)my_yuan2percentFen(p->PERatio1),
    		(int)my_yuan2percentFen(p->NAV),
    		(int)my_yuan2percentFen(p->PERatio2),
    		(int)my_yuan2percentFen(p->IOPV),
    		(int)my_yuan2percentFen(p->PremiumRate),
    		(int)my_yuan2percentFen(p->TotalSellOrderVolume),
    		(int)my_yuan2percentFen(p->WtAvgSellPrice),	//�ڶ�������
    		(int)p->SellLevelNo,	   ///< ���̼�λ����, 10�����飬����ʱ���� ����������
    		(int)my_yuan2percentFen(p->SellPrice01),
    		(int)p->SellVolume01,
    		(int)p->TotalSellOrderNo01,
    		(int)my_yuan2percentFen(p->SellPrice02),
    		(int)p->SellVolume02,
    		(int)p->TotalSellOrderNo02,
    		(int)my_yuan2percentFen(p->SellPrice03),
    		(int)p->SellVolume03,
    		(int)p->TotalSellOrderNo03,
    		(int)my_yuan2percentFen(p->SellPrice04),
    		(int)p->SellVolume04,
    		(int)p->TotalSellOrderNo04,
    		(int)my_yuan2percentFen(p->SellPrice05),
    		(int)p->SellVolume05,
    		(int)p->TotalSellOrderNo05,		//���ĸ�����
    		(int)my_yuan2percentFen(p->SellPrice06),
    		(int)p->SellVolume06,
    		(int)p->TotalSellOrderNo06,
    		(int)my_yuan2percentFen(p->SellPrice07),
    		(int)p->SellVolume07,
    		(int)p->TotalSellOrderNo07,
    		(int)my_yuan2percentFen(p->SellPrice08),
    		(int)p->SellVolume08,
    		(int)p->TotalSellOrderNo08,
    		(int)my_yuan2percentFen(p->SellPrice09),
    		(int)p->SellVolume09,
    		(int)p->TotalSellOrderNo09,
    		(int)my_yuan2percentFen(p->SellPrice10),
    		(int)p->SellVolume10,
    		(int)p->TotalSellOrderNo10,		//���������
    		(int)p->SellLevelQueueNo01,
    		(int)p->SellLevelQueue[0],
    		(int)my_yuan2percentFen(p->TotalBuyOrderVolume),
    		(int)my_yuan2percentFen(p->WtAvgBuyPrice),
    		(int)p->BuyLevelNo,			//����������
    		(int)my_yuan2percentFen(p->BuyPrice01),
    		(int)p->BuyVolume01,
    		(int)p->TotalBuyOrderNo01,
    		(int)my_yuan2percentFen(p->BuyPrice02),
    		(int)p->BuyVolume02,
    		(int)p->TotalBuyOrderNo02,
    		(int)my_yuan2percentFen(p->BuyPrice03),
    		(int)p->BuyVolume03,
    		(int)p->TotalBuyOrderNo03,
    		(int)my_yuan2percentFen(p->BuyPrice04),
    		(int)p->BuyVolume04,
    		(int)p->TotalBuyOrderNo04,
    		(int)my_yuan2percentFen(p->BuyPrice05),
    		(int)p->BuyVolume05,
    		(int)p->TotalBuyOrderNo05,		//���߸�����
    		(int)my_yuan2percentFen(p->BuyPrice06),
    		(int)p->BuyVolume06,
    		(int)p->TotalBuyOrderNo06,
    		(int)my_yuan2percentFen(p->BuyPrice07),
    		(int)p->BuyVolume07,
    		(int)p->TotalBuyOrderNo07,
    		(int)my_yuan2percentFen(p->BuyPrice08),
    		(int)p->BuyVolume08,
    		(int)p->TotalBuyOrderNo08,
    		(int)my_yuan2percentFen(p->BuyPrice09),
    		(int)p->BuyVolume09,
    		(int)p->TotalBuyOrderNo09,
    		(int)my_yuan2percentFen(p->BuyPrice10),
    		(int)p->BuyVolume10,
    		(int)p->TotalBuyOrderNo10,    		//�ڰ˸�����
    		(int)p->BuyLevelQueueNo01,
    		(int)p->BuyLevelQueue[0],
    		(int)my_yuan2percentFen(p->WtAvgRate),
    		(int)my_yuan2percentFen(p->WtAvgRateUpdown),
    		(int)my_yuan2percentFen(p->PreWtAvgRate)
		);

	return 0;
}
int print_MY_TYPE_SZSEL2_Transaction(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	SZSEL2_Transaction   *p=(SZSEL2_Transaction *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->Symbol)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\t%s\t%s\n",
			"picktime",
			"localtime",
			"packtime",
			"tradetime",
			"stockcode",
			"difftime"
			);
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->TradeTime%MY_DATE_CEIL_LONG;break;
		case 2: *plCurTime=lTmpTime;break;
		case 3:	*plCurTime=p->LocalTimeStamp;break;
		default:*plCurTime=p->TradeTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	sprintf(outbuf,"%lld\t%d\t%ld\t%d\t%s\t%d\
\t%s\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%c\n",
		*(long long *)buf,	//picktime
		p->LocalTimeStamp,	//< ���ݽ���ʱ��HHMMSSMMM
		99999999999999999,	//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		(int)(p->TradeTime%MY_DATE_CEIL_LONG),//�ɽ�ʱ��YYYYMMDDHHMMSSMMM
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)(p->TradeTime%MY_DATE_CEIL_LONG)),//�ڶ��п�ʼ
		p->QuotationFlag,  			///< 1=�Ϻ��ϻ��������Դ��2=���ڸ�����������Դ
		(int)p->SetID,                  	///< ֤ȯ������
		(int)p->RecID,           		///< ��Ϣ��¼�� �� 1 ��ʼ������ͬһƵ������
		(int)p->BuyOrderID,                  	///< ��ί������
		(int)p->SellOrderID,           		///< ����ί������
		p->SymbolSource,			///< ֤ȯ����Դ
		(int)my_yuan2percentFen(p->TradePrice),	///< �ɽ��۸�,
		(int)my_yuan2percentFen(p->TradeVolume),///< �ɽ����,
		p->TradeType            		///< �ɽ����4=�������������Զ�����ִ�б��棻F=�ɽ����ɽ�ִ�б���
		);

	return 0;
}

int print_MY_TYPE_SZSEL2_Order(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	SZSEL2_Order   *p=(SZSEL2_Order *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->Symbol)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\t%s\t%s\n",
			"picktime",
			"localtime",
			"packtime",
			"ordertime",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->Time%MY_DATE_CEIL_LONG;break;
		case 2: *plCurTime=lTmpTime;break;
		case 3:	*plCurTime=p->LocalTimeStamp;break;
		default:*plCurTime=p->Time;break;
	}
	if(*plCurTime<lBgnTime) return 2;


	sprintf(outbuf,"%lld\t%d\t%ld\t%d\t%s\t%d\
\t%s\t%d\t%d\t%s\t%d\t%d\t%c\t%c\n",
		*(long long *)buf,	//picktime
		p->LocalTimeStamp,	//< ���ݽ���ʱ��HHMMSSMMM
		99999999999999999,	//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		(int)(p->Time%MY_DATE_CEIL_LONG),//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)(p->Time%MY_DATE_CEIL_LONG)),//�ڶ��п�ʼ
		p->QuotationFlag,  			///< 1=�Ϻ��ϻ��������Դ��2=���ڸ�����������Դ
		(int)p->SetID,                  	///< ֤ȯ������
		(int)p->RecID,           		///< ��Ϣ��¼�� �� 1 ��ʼ������ͬһƵ������
		p->SymbolSource,			///< ֤ȯ����Դ
		(int)my_yuan2percentFen(p->OrderPrice),	///< ί�м۸�,4λС��
		(int)my_yuan2percentFen(p->OrderVolume),///< ί������,2λС��
		p->OrderCode,				///< ��������1=�� 2=�� G=���� F=����
		p->OrderType				///< �������1=�м� 2=�޼� U=��������
		);

	return 0;
}

int print_MY_TYPE_TDF_MKT(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	struct TDF_MARKET_DATA   *p=(struct TDF_MARKET_DATA *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->szCode)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\n",
			"picktime",
			"time",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->nTime;break;
		case 2: *plCurTime=lTmpTime;break;
		default:*plCurTime=p->nTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;


	sprintf(outbuf,"%lld\t%d\t%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\
\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%s\t%d\t%d\t%d\n",
		*(long long *)buf,	//picktime
		(int)p->nTime,		//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//��һ�м���
		(int)p->nStatus,	//״̬
		(int)p->nPreClose,	//ǰ����
		(int)p->nOpen,		//���̼�
		(int)p->nHigh,		//��߼�
		(int)p->nLow,		//��ͼ�
		(int)p->nMatch,		//���¼�	//�ڶ��п�ʼ
		(int)p->nAskPrice[0],
		(int)p->nAskPrice[1],
		(int)p->nAskPrice[2],
		(int)p->nAskPrice[3],
		(int)p->nAskPrice[4],
		(int)p->nAskPrice[5],
		(int)p->nAskPrice[6],
		(int)p->nAskPrice[7],
		(int)p->nAskPrice[8],
		(int)p->nAskPrice[9],
		(int)p->nAskVol[0],
		(int)p->nAskVol[1],
		(int)p->nAskVol[2],
		(int)p->nAskVol[3],
		(int)p->nAskVol[4],
		(int)p->nAskVol[5],
		(int)p->nAskVol[6],
		(int)p->nAskVol[7],
		(int)p->nAskVol[8],
		(int)p->nAskVol[9],	//�����п�ʼ
		(int)p->nBidPrice[0],
		(int)p->nBidPrice[1],
		(int)p->nBidPrice[2],
		(int)p->nBidPrice[3],
		(int)p->nBidPrice[4],
		(int)p->nBidPrice[5],
		(int)p->nBidPrice[6],
		(int)p->nBidPrice[7],
		(int)p->nBidPrice[8],
		(int)p->nBidPrice[9],
		(int)p->nBidVol[0],
		(int)p->nBidVol[1],
		(int)p->nBidVol[2],
		(int)p->nBidVol[3],
		(int)p->nBidVol[4],
		(int)p->nBidVol[5],
		(int)p->nBidVol[6],
		(int)p->nBidVol[7],
		(int)p->nBidVol[8],
		(int)p->nBidVol[9],	//�����п�ʼ
		(int)p->nNumTrades,			//�ɽ�����
		(int)p->iVolume,			//�ɽ�����
		(int)p->iTurnover,			//�ɽ��ܽ��
		(int)p->nTotalBidVol,			//ί����������
		(int)p->nTotalAskVol,			//ί����������
		(int)p->nWeightedAvgBidPrice,		//��Ȩƽ��ί��۸�
		(int)p->nWeightedAvgAskPrice,		//��Ȩƽ��ί���۸�
		(int)p->nIOPV,				//IOPV��ֵ��ֵ
		(int)p->nYieldToMaturity,		//����������
		(int)p->nHighLimited,			//��ͣ��
		(int)p->nLowLimited,			//��ͣ��
		p->chPrefix,				//֤ȯ��Ϣǰ׺
		(int)p->nSyl1,				//��ӯ��1
		(int)p->nSyl2,				//��ӯ��2
		(int)p->nSD2				//����2���Ա���һ�ʣ�
		);

	return 0;
}

int print_MY_TYPE_TDF_TRA(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	struct TDF_TRANSACTION   *p=(TDF_TRANSACTION *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->szCode)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\n",
			"picktime",
			"tradetime",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->nTime;break;
		case 2: *plCurTime=lTmpTime;break;
		default:*plCurTime=p->nTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	if(p->chOrderKind==0) 	p->chOrderKind='0';
	else			p->chOrderKind='1';
	if(p->chFunctionCode==0) p->chFunctionCode='0';

	sprintf(outbuf,"%lld\t%d\t%s\t%d\
\t%d\t%d\t%d\t%d\t%c\t%c\t%c\t%d\t%d\n",
		*(long long *)buf,	//picktime
		(int)p->nTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//�ڶ��п�ʼ
		(int)p->nIndex,            //�ɽ����
		(int)p->nPrice,            //�ɽ��۸�
		(int)p->nVolume,	        //�ɽ�����
		(int)p->nTurnover,	            //�ɽ����
		(char)p->nBSFlag,                    //��������(��'B', ����'S', ������' ')
		p->chOrderKind,                //�ɽ����
		p->chFunctionCode,             //�ɽ�����
		(int)p->nAskOrder,	                //������ί�����
		(int)p->nBidOrder	                //����ί�����
		);

	return 0;
}

int print_MY_TYPE_TDF_ORD(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	struct TDF_ORDER   *p=(TDF_ORDER *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->szCode)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\n",
			"picktime",
			"ordertime",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->nTime;break;
		case 2: *plCurTime=lTmpTime;break;
		default:*plCurTime=p->nTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	if(p->chOrderKind==0) p->chOrderKind='0';
	if(p->chFunctionCode==0) p->chFunctionCode='0';
	if(p->chStatus==0) 	p->chStatus='0';
	if(p->chFlag==0)	p->chFlag='0';

	sprintf(outbuf,"%lld\t%d\t%s\t%d\
\t%d\t%d\t%d\t%c\t%c\t%d\t%c\t%c\n",
		*(long long *)buf,	//picktime
		(int)p->nTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//�ڶ��п�ʼ
		(int)p->nOrder,	        	//ί�к�
		(int)p->nPrice,         	//ί�м۸�
		(int)p->nVolume,		//ί������
		p->chOrderKind,                //ί�����
		p->chFunctionCode,             //ί�д���('B','S','C')
		(int)p->nBroker,		//�����̱���(PT�г���Ч)
		p->chStatus,			//ί��״̬(PT�г���Ч)
		p->chFlag			//��־(PT�г���Ч)
		);

	return 0;
}

int print_MY_TYPE_TDF_QUE(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	struct TDF_ORDER_QUEUE   *p=(TDF_ORDER_QUEUE *)(buf+sizeof(long long));

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,p->szCode)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\n",
			"picktime",
			"time",
			"stockcode",
			"difftime");
	}

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);

	switch(iTimeFlag){
		case 1: *plCurTime=p->nTime;break;
		case 2: *plCurTime=lTmpTime;break;
		default:*plCurTime=p->nTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	sprintf(outbuf,"%lld\t%d\t%s\t%d\
\t%c\t%d\t%d\t%d\t%d\n",
		*(long long *)buf,	//picktime
		(int)p->nTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//�ڶ��п�ʼ
		(char)p->nSide,			//��������('B':Bid 'A':Ask)
		(int)p->nPrice,         	//ί�м۸�
		(int)p->nOrders,		//��������
		(int)p->nABItems,		//��ϸ����
		(int)p->nABVolume[0]		//������ϸ
		);

	return 0;
}


int (*print_MY_TYPE)(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf);

int main(int argc, char *argv[])
{
	int iType=1,ret,i=0,iSkipCnt=0,iFilterCnt=0,iMaxCount=100;
	int iExceedCnt=0,iDelayCnt=0,iDelaySec=10,iTimeFlag=1;
	long lBgnTime=0,lEndTime=999999999999,lCurTime=0,lItemLen=0;

	char sInFileName[1024],sCodeStr[1024],sBuffer[10240],sOutBuf[10240];

	FILE *fpIn;

	for (int c; (c = getopt(argc, argv, "i:s:c:b:e:l:t:d:f:")) != EOF;){

		switch (c){
		case 'i':strcpy(sInFileName, optarg);	break;
		case 's':iSkipCnt=atoi(optarg);		break;
		case 'c':iMaxCount=atoi(optarg);	break;
		case 'b':lBgnTime=atol(optarg);		break;
		case 'e':lEndTime=atol(optarg);		break;
		case 'l':strcpy(sCodeStr, optarg);	break;
		case 't':
			iType=atoi(optarg);
			if(iType<=0||iType>6) iType=1;
			//����TDF����������
			switch (optarg[0]){
				case 'm':case 'M':iType='M';break;
				case 't':case 'T':iType='T';break;
				case 'o':case 'O':iType='O';break;
				case 'q':case 'Q':iType='Q';break;
			}
			break;
		case 'd':iDelaySec=atoi(optarg);	break;
		case 'f':iTimeFlag=atoi(optarg);	break;
		default:
			printf("Usage: %s \n", argv[0]);
			printf("   [-i infile-name ]\n");
			printf("   [-s skipcnt (def=0)]\n");
			printf("   [-c count (def=100,-1=all) ]\n");
			printf("   [-b begin-time (def=0) ]\n");
			printf("   [-e end-time (def=99999999999) ]\n");
			printf("   [-t type (1-qh,2-th,3-ah,4-qz,5-tz,6-oz,mtoq|MTOQ=>mkt,trs,ord,que) ]\n");
			printf("   [-d delay sec (-e,-l multi-code effect) ]\n");
			printf("   [-l codelist (e.g \"000001,603912,002415\") ]\n");
			printf("   [-f time_flag (def=1,1-createtime,2-picktime,3-localtime,4-packtime) ]\n");
			exit(1);
			break;
		}
	}


	if((fpIn=fopen(sInFileName,"r"))==NULL){
		printf("error open file %s to read.\n",sInFileName);
		return -1;
	}

	switch (iType){
		case MY_TYPE_SSEL2_Quotation	:
		lItemLen=(sizeof(SSEL2_Quotation));
		print_MY_TYPE=print_MY_TYPE_SSEL2_Quotation;
		break;
		case MY_TYPE_SSEL2_Transaction:
		lItemLen=(sizeof(SSEL2_Transaction));
		print_MY_TYPE=print_MY_TYPE_SSEL2_Transaction;
		break;
		case MY_TYPE_SSEL2_Auction:
		lItemLen=(sizeof(SSEL2_Auction));
		print_MY_TYPE=print_MY_TYPE_SSEL2_Auction;
		break;
		case MY_TYPE_SZSEL2_Quotation:
		lItemLen=(sizeof(SZSEL2_Quotation));
		print_MY_TYPE=print_MY_TYPE_SZSEL2_Quotation;
		break;
		case MY_TYPE_SZSEL2_Transaction:
		lItemLen=(sizeof(SZSEL2_Transaction));
		print_MY_TYPE=print_MY_TYPE_SZSEL2_Transaction;
		break;
		case MY_TYPE_SZSEL2_Order:
		lItemLen=(sizeof(SZSEL2_Order));
		print_MY_TYPE=print_MY_TYPE_SZSEL2_Order;
		break;
		case MY_TYPE_TDF_MKT:
		lItemLen=(sizeof(TDF_MARKET_DATA));
		print_MY_TYPE=print_MY_TYPE_TDF_MKT;
		break;
		case MY_TYPE_TDF_TRA:
		lItemLen=(sizeof(TDF_TRANSACTION));
		print_MY_TYPE=print_MY_TYPE_TDF_TRA;
		break;
		case MY_TYPE_TDF_ORD:
		lItemLen=(sizeof(TDF_ORDER));
		print_MY_TYPE=print_MY_TYPE_TDF_ORD;
		break;
		case MY_TYPE_TDF_QUE:
		lItemLen=(sizeof(TDF_ORDER_QUEUE));
		print_MY_TYPE=print_MY_TYPE_TDF_QUE;
		break;
		default:
		lItemLen=(sizeof(SSEL2_Quotation));
		print_MY_TYPE=print_MY_TYPE_SSEL2_Quotation;
		break;
	}
	lItemLen+=sizeof(long long);

	if(fseek(fpIn,lItemLen*iSkipCnt,SEEK_SET)<0){
		perror("seek file");
		return -1;
	}

	while(1){
		//size_t fread ( void *buffer, size_t size, size_t count, FILE *stream) ;
		if(fread((void*)sBuffer,lItemLen,1,fpIn)!=1){
			printf("end of file break.\n");
			break;
		}

		if((ret=print_MY_TYPE(sBuffer,sCodeStr,iTimeFlag,lBgnTime,&lCurTime,sOutBuf))!=0){
			if(ret==1)	iFilterCnt++;
			else		iSkipCnt++;
			continue;
		}
		//���������ѡ�Ĺ�Ʊ��ʱ������������ָ��ʱ��
		if(lCurTime>lEndTime){

			//���ֻѡһֻ��Ʊ����ֱ��break
			if(strlen(sCodeStr)==6) break;

			//���ѡ��֧��Ʊ����ȫѡ������������10��,
			//Ϊ�˱�����Ϊһֻ��Ʊ��ʱ��������������Ʊ��endtime֮ǰ������ûȡ��
			if(lCurTime>(lEndTime+iDelaySec*1000L)) break;

			iExceedCnt++;
			continue;
		}
		printf("%s",sOutBuf);

		//������ֹ�Ʊ��ʱ�䵽���ˣ�ͳ��һ�³������ʱ��֮��Ļ��ж�����ѡ����Ϣ
		if(iExceedCnt>0) iDelayCnt++;

		if((++i)>=iMaxCount) break;

	}

	printf("total skip cnt = %d filter cnt =%d exceed cnt =%d delay cnt =%d output cnt =%d.\n",
		iSkipCnt,iFilterCnt,iExceedCnt,iDelayCnt,i);

	fclose(fpIn);

	return 0;
}
