#include "wwtiny.h"

#include "dmpfmt.h"

using namespace std;

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
		*(long long *)buf,//picktime
		p->LocalTimeStamp,//< ���ݽ���ʱ��HHMMSSMMM
		p->PacketTimeStamp,//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		p->Time,	//< ��������ʱ��, ���¶���ʱ�䣨���룩;143025001 ��ʾ 14:30:25.001
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)p->Time),
		p->QuotationFlag,			//��һ������
    		(int)yuan2percentFen(p->PreClosePrice),         ///< ���ռ�,
    		(int)yuan2percentFen(p->OpenPrice),             ///< ���̼�,
    		(int)yuan2percentFen(p->HighPrice),             ///< ��߼�,
    		(int)yuan2percentFen(p->LowPrice),              ///< ��ͼ�,
    		(int)yuan2percentFen(p->LastPrice),             ///< �ּ�,
    		(int)yuan2percentFen(p->ClosePrice),            ///< �����̼�,
		p->TradeStatus,    				///< ��ǰƷ�ֽ���״̬,
		p->SecurityPhaseTag,   				///< ��ǰƷ��״̬
		(int)p->TotalNo,                		///< �ɽ�����,
		(int)p->TotalVolume,           			///< �ɽ�����, ��Ʊ����),Ȩ֤����),ծȯ����
    		(int)yuan2percentFen(p->TotalAmount),           ///< �ɽ��ܶ�, ��Ԫ��
		(int)p->TotalBuyOrderVolume,    		///< ί����������, ��Ʊ����),Ȩ֤����),ծȯ����
    		(int)yuan2percentFen(p->WtAvgBuyPrice),         ///< ��Ȩƽ��ί���, ��Ԫ��
    		(int)yuan2percentFen(p->BondWtAvgBuyPrice),     ///< ծȯ��Ȩƽ��ί���, ��Ԫ��
		(int)p->TotalSellOrderVolume,		///< ί����������,
    		(int)yuan2percentFen(p->WtAvgSellPrice),        ///< ��Ȩƽ��ί����, ��Ԫ��
    		(int)yuan2percentFen(p->BondWtAvgSellPrice),    ///< ծȯ��Ȩƽ��ί����,
    		(int)yuan2percentFen(p->IOPV),                  ///< ETF ��ֵ��ֵ,
		p->ETFBuyNo,				///< ETF �깺����,�ڶ�������
		(int)p->ETFBuyVolume, 			        ///< ETF �깺��,
    		(int)yuan2percentFen(p->ETFBuyAmount),          ///< ETF �깺��,
    		p->ETFSellNo,             			///< ETF ��ر���,
    		(int)p->ETFSellVolume,          		///< ETF �����,
    		(int)yuan2percentFen(p->ETFSellAmount),         ///< ETF ��ض�,
    		(int)yuan2percentFen(p->YTM),                   ///< ծȯ����������,
    		(int)p->TotalWarrantExecVol,    		///< Ȩִ֤�е�������,
    		(int)yuan2percentFen(p->WarrantDownLimit),      ///< Ȩ֤��ͣ�۸�, ��Ԫ��
    		(int)yuan2percentFen(p->WarrantUpLimit),        ///< Ȩ֤��ͣ�۸�, ��Ԫ��
    		p->WithdrawBuyNo,          			///< ���볷������,
    		(int)p->WithdrawBuyVolume,      		///< ���볷����,
    		(int)yuan2percentFen(p->WithdrawBuyAmount),      ///< ���볷����,
    		p->WithdrawSellNo,			///< ������������,
    		(int)p->WithdrawSellVolume,		///< ����������,
    		(int)yuan2percentFen(p->WithdrawSellAmount),     ///< ����������,
    		p->TotalBuyNo,             ///< �����ܱ���,
    		p->TotalSellNo,            ///< �����ܱ���,
    		p->MaxBuyDuration,         ///< ����ɽ����ȴ�ʱ��,
    		p->MaxSellDuration,        ///< �����ɽ����ȴ�ʱ��,
    		p->BuyOrderNo,             ///< ��ί�м�λ��,
    		p->SellOrderNo,            ///< ����ί�м�λ��,	����������
    		(int)p->SellLevelNo,   ///< ���̼�λ����, 10�����飬����ʱ���� ���ĸ�����
    		(int)yuan2percentFen(p->SellPrice01),
    		(int)p->SellVolume01,
    		(int)p->TotalSellOrderNo01,
    		(int)yuan2percentFen(p->SellPrice02),
    		(int)p->SellVolume02,
    		(int)p->TotalSellOrderNo02,
    		(int)yuan2percentFen(p->SellPrice03),
    		(int)p->SellVolume03,
    		(int)p->TotalSellOrderNo03,
    		(int)yuan2percentFen(p->SellPrice04),
    		(int)p->SellVolume04,
    		(int)p->TotalSellOrderNo04,
    		(int)yuan2percentFen(p->SellPrice05),
    		(int)p->SellVolume05,
    		(int)p->TotalSellOrderNo05,	//���������
    		(int)yuan2percentFen(p->SellPrice06),
    		(int)p->SellVolume06,
    		(int)p->TotalSellOrderNo06,
    		(int)yuan2percentFen(p->SellPrice07),
    		(int)p->SellVolume07,
    		(int)p->TotalSellOrderNo07,
    		(int)yuan2percentFen(p->SellPrice08),
    		(int)p->SellVolume08,
    		(int)p->TotalSellOrderNo08,
    		(int)yuan2percentFen(p->SellPrice09),
    		(int)p->SellVolume09,
    		(int)p->TotalSellOrderNo09,
    		(int)yuan2percentFen(p->SellPrice10),
    		(int)p->SellVolume10,
    		(int)p->TotalSellOrderNo10,	//����������
    		(int)p->SellLevelQueueNo01,
    		(int)p->SellLevelQueue[0],
    		(int)p->BuyLevelNo,		//���߸�����
    		(int)yuan2percentFen(p->BuyPrice01),
    		(int)p->BuyVolume01,
    		(int)p->TotalBuyOrderNo01,
    		(int)yuan2percentFen(p->BuyPrice02),
    		(int)p->BuyVolume02,
    		(int)p->TotalBuyOrderNo02,
    		(int)yuan2percentFen(p->BuyPrice03),
    		(int)p->BuyVolume03,
    		(int)p->TotalBuyOrderNo03,
    		(int)yuan2percentFen(p->BuyPrice04),
    		(int)p->BuyVolume04,
    		(int)p->TotalBuyOrderNo04,
    		(int)yuan2percentFen(p->BuyPrice05),
    		(int)p->BuyVolume05,
    		(int)p->TotalBuyOrderNo05,	//�ڰ˸�����
    		(int)yuan2percentFen(p->BuyPrice06),
    		(int)p->BuyVolume06,
    		(int)p->TotalBuyOrderNo06,
    		(int)yuan2percentFen(p->BuyPrice07),
    		(int)p->BuyVolume07,
    		(int)p->TotalBuyOrderNo07,
    		(int)yuan2percentFen(p->BuyPrice08),
    		(int)p->BuyVolume08,
    		(int)p->TotalBuyOrderNo08,
    		(int)yuan2percentFen(p->BuyPrice09),
    		(int)p->BuyVolume09,
    		(int)p->TotalBuyOrderNo09,
    		(int)yuan2percentFen(p->BuyPrice10),
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
		*(long long *)buf,//picktime
		p->LocalTimeStamp,//< ���ݽ���ʱ��HHMMSSMMM
		p->PacketTimeStamp,//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		p->TradeTime,	//< �ɽ�ʱ��(����), 14302506 ��ʾ14:30:25.06
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)p->TradeTime),//�ڶ��п�ʼ
		p->QuotationFlag,  			///< 1=�Ϻ��ϻ��������Դ��2=���ڸ�����������Դ
		(int)p->RecID,                  	///< ҵ������, �� 1 ��ʼ���� TradeChannel����
		(int)p->TradeChannel,           	///< �ɽ�ͨ��,
		(int)yuan2percentFen(p->TradePrice),///< �ɽ��۸�,
		(int)p->TradeVolume,            	///< �ɽ�����, ��Ʊ����,Ȩ֤����,ծȯ����
		(int)yuan2percentFen(p->TradeAmount),///< �ɽ����,
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
		*(long long *)buf,//picktime
		p->LocalTimeStamp,//< ���ݽ���ʱ��HHMMSSMMM
		p->PacketTimeStamp,//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		p->Time,	//< ��������ʱ��, 143025001 ��ʾ 14:30:25.001
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)p->Time),
		p->QuotationFlag,
		(int)yuan2percentFen(p->OpenPrice),
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
		*(long long *)buf,//picktime
		p->LocalTimeStamp,//< ���ݽ���ʱ��HHMMSSMMM
		99999999999999999,//< ����ֵ
		(int)(p->Time%MY_DATE_CEIL_LONG),//< ��������ʱ��YYYYMMDDHHMMSSMMM
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)(p->Time%MY_DATE_CEIL_LONG)),
		p->QuotationFlag,			//��һ������
    		(int)yuan2percentFen(p->PreClosePrice),
    		(int)yuan2percentFen(p->OpenPrice),
    		(int)yuan2percentFen(p->LastPrice),
    		(int)yuan2percentFen(p->HighPrice),
    		(int)yuan2percentFen(p->LowPrice),
    		(int)yuan2percentFen(p->PriceUpLimit),
    		(int)yuan2percentFen(p->PriceDownLimit),
    		(int)yuan2percentFen(p->PriceUpdown1),
    		(int)yuan2percentFen(p->PriceUpdown2),
    		(int)p->TotalNo,
    		(int)yuan2percentFen(p->TotalVolume),
    		(int)yuan2percentFen(p->TotalAmount),
    		(int)yuan2percentFen(p->ClosePrice),
    		"0",//p->SecurityPhaseTag,
    		(int)yuan2percentFen(p->PERatio1),
    		(int)yuan2percentFen(p->NAV),
    		(int)yuan2percentFen(p->PERatio2),
    		(int)yuan2percentFen(p->IOPV),
    		(int)yuan2percentFen(p->PremiumRate),
    		(int)yuan2percentFen(p->TotalSellOrderVolume),
    		(int)yuan2percentFen(p->WtAvgSellPrice),//�ڶ�������
    		(int)p->SellLevelNo,   ///< ���̼�λ����, 10�����飬����ʱ���� ����������
    		(int)yuan2percentFen(p->SellPrice01),
    		(int)p->SellVolume01,
    		(int)p->TotalSellOrderNo01,
    		(int)yuan2percentFen(p->SellPrice02),
    		(int)p->SellVolume02,
    		(int)p->TotalSellOrderNo02,
    		(int)yuan2percentFen(p->SellPrice03),
    		(int)p->SellVolume03,
    		(int)p->TotalSellOrderNo03,
    		(int)yuan2percentFen(p->SellPrice04),
    		(int)p->SellVolume04,
    		(int)p->TotalSellOrderNo04,
    		(int)yuan2percentFen(p->SellPrice05),
    		(int)p->SellVolume05,
    		(int)p->TotalSellOrderNo05,	//���ĸ�����
    		(int)yuan2percentFen(p->SellPrice06),
    		(int)p->SellVolume06,
    		(int)p->TotalSellOrderNo06,
    		(int)yuan2percentFen(p->SellPrice07),
    		(int)p->SellVolume07,
    		(int)p->TotalSellOrderNo07,
    		(int)yuan2percentFen(p->SellPrice08),
    		(int)p->SellVolume08,
    		(int)p->TotalSellOrderNo08,
    		(int)yuan2percentFen(p->SellPrice09),
    		(int)p->SellVolume09,
    		(int)p->TotalSellOrderNo09,
    		(int)yuan2percentFen(p->SellPrice10),
    		(int)p->SellVolume10,
    		(int)p->TotalSellOrderNo10,	//���������
    		(int)p->SellLevelQueueNo01,
    		(int)p->SellLevelQueue[0],
    		(int)yuan2percentFen(p->TotalBuyOrderVolume),
    		(int)yuan2percentFen(p->WtAvgBuyPrice),
    		(int)p->BuyLevelNo,		//����������
    		(int)yuan2percentFen(p->BuyPrice01),
    		(int)p->BuyVolume01,
    		(int)p->TotalBuyOrderNo01,
    		(int)yuan2percentFen(p->BuyPrice02),
    		(int)p->BuyVolume02,
    		(int)p->TotalBuyOrderNo02,
    		(int)yuan2percentFen(p->BuyPrice03),
    		(int)p->BuyVolume03,
    		(int)p->TotalBuyOrderNo03,
    		(int)yuan2percentFen(p->BuyPrice04),
    		(int)p->BuyVolume04,
    		(int)p->TotalBuyOrderNo04,
    		(int)yuan2percentFen(p->BuyPrice05),
    		(int)p->BuyVolume05,
    		(int)p->TotalBuyOrderNo05,	//���߸�����
    		(int)yuan2percentFen(p->BuyPrice06),
    		(int)p->BuyVolume06,
    		(int)p->TotalBuyOrderNo06,
    		(int)yuan2percentFen(p->BuyPrice07),
    		(int)p->BuyVolume07,
    		(int)p->TotalBuyOrderNo07,
    		(int)yuan2percentFen(p->BuyPrice08),
    		(int)p->BuyVolume08,
    		(int)p->TotalBuyOrderNo08,
    		(int)yuan2percentFen(p->BuyPrice09),
    		(int)p->BuyVolume09,
    		(int)p->TotalBuyOrderNo09,
    		(int)yuan2percentFen(p->BuyPrice10),
    		(int)p->BuyVolume10,
    		(int)p->TotalBuyOrderNo10,    		//�ڰ˸�����
    		(int)p->BuyLevelQueueNo01,
    		(int)p->BuyLevelQueue[0],
    		(int)yuan2percentFen(p->WtAvgRate),
    		(int)yuan2percentFen(p->WtAvgRateUpdown),
    		(int)yuan2percentFen(p->PreWtAvgRate)
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
		*(long long *)buf,//picktime
		p->LocalTimeStamp,//< ���ݽ���ʱ��HHMMSSMMM
		99999999999999999,//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		(int)(p->TradeTime%MY_DATE_CEIL_LONG),//�ɽ�ʱ��YYYYMMDDHHMMSSMMM
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)(p->TradeTime%MY_DATE_CEIL_LONG)),//�ڶ��п�ʼ
		p->QuotationFlag,  			///< 1=�Ϻ��ϻ��������Դ��2=���ڸ�����������Դ
		(int)p->SetID,                  	///< ֤ȯ������
		(int)p->RecID,           		///< ��Ϣ��¼�� �� 1 ��ʼ������ͬһƵ������
		(int)p->BuyOrderID,                  	///< ��ί������
		(int)p->SellOrderID,           		///< ����ί������
		p->SymbolSource,		///< ֤ȯ����Դ
		(int)yuan2percentFen(p->TradePrice),///< �ɽ��۸�,
		(int)yuan2percentFen(p->TradeVolume),///< �ɽ����,
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
		*(long long *)buf,//picktime
		p->LocalTimeStamp,//< ���ݽ���ʱ��HHMMSSMMM
		99999999999999999,//< ���ݰ�ͷʱ��YYYYMMDDHHMMSSMMM
		(int)(p->Time%MY_DATE_CEIL_LONG),//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->Symbol,
		iDiffnTime((int)lTmpTime,(int)(p->Time%MY_DATE_CEIL_LONG)),//�ڶ��п�ʼ
		p->QuotationFlag,  			///< 1=�Ϻ��ϻ��������Դ��2=���ڸ�����������Դ
		(int)p->SetID,                  	///< ֤ȯ������
		(int)p->RecID,           		///< ��Ϣ��¼�� �� 1 ��ʼ������ͬһƵ������
		p->SymbolSource,		///< ֤ȯ����Դ
		(int)yuan2percentFen(p->OrderPrice),///< ί�м۸�,4λС��
		(int)yuan2percentFen(p->OrderVolume),///< ί������,2λС��
		p->OrderCode,			///< ��������1=�� 2=�� G=���� F=����
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
		*(long long *)buf,//picktime
		(int)p->nTime,	//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//��һ�м���
		(int)p->nStatus,//״̬
		(int)p->nPreClose,//ǰ����
		(int)p->nOpen,	//���̼�
		(int)p->nHigh,	//��߼�
		(int)p->nLow,	//��ͼ�
		(int)p->nMatch,	//���¼�	//�ڶ��п�ʼ
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
		(int)p->nAskVol[9],//�����п�ʼ
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
		(int)p->nBidVol[9],//�����п�ʼ
		(int)p->nNumTrades,		//�ɽ�����
		(int)p->iVolume,		//�ɽ�����
		(int)p->iTurnover,		//�ɽ��ܽ��
		(int)p->nTotalBidVol,		//ί����������
		(int)p->nTotalAskVol,		//ί����������
		(int)p->nWeightedAvgBidPrice,	//��Ȩƽ��ί��۸�
		(int)p->nWeightedAvgAskPrice,	//��Ȩƽ��ί���۸�
		(int)p->nIOPV,			//IOPV��ֵ��ֵ
		(int)p->nYieldToMaturity,	//����������
		(int)p->nHighLimited,		//��ͣ��
		(int)p->nLowLimited,		//��ͣ��
		p->chPrefix,			//֤ȯ��Ϣǰ׺
		(int)p->nSyl1,			//��ӯ��1
		(int)p->nSyl2,			//��ӯ��2
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
		*(long long *)buf,//picktime
		(int)p->nTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//�ڶ��п�ʼ
		(int)p->nIndex,            //�ɽ����
		(int)p->nPrice,            //�ɽ��۸�
		(int)p->nVolume,        //�ɽ�����
		(int)p->nTurnover,            //�ɽ����
		(char)p->nBSFlag,                    //��������(��'B', ����'S', ������' ')
		p->chOrderKind,                //�ɽ����
		p->chFunctionCode,             //�ɽ�����
		(int)p->nAskOrder,                //������ί�����
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
		*(long long *)buf,//picktime
		(int)p->nTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//�ڶ��п�ʼ
		(int)p->nOrder,        	//ί�к�
		(int)p->nPrice,         	//ί�м۸�
		(int)p->nVolume,	//ί������
		p->chOrderKind,                //ί�����
		p->chFunctionCode,             //ί�д���('B','S','C')
		(int)p->nBroker,	//�����̱���(PT�г���Ч)
		p->chStatus,		//ί��״̬(PT�г���Ч)
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
		*(long long *)buf,//picktime
		(int)p->nTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		p->szCode,
		iDiffnTime((int)lTmpTime,(int)p->nTime),//�ڶ��п�ʼ
		(char)p->nSide,		//��������('B':Bid 'A':Ask)
		(int)p->nPrice,         	//ί�м۸�
		(int)p->nOrders,	//��������
		(int)p->nABItems,	//��ϸ����
		(int)p->nABVolume[0]		//������ϸ
		);

	return 0;
}

void MktData2TDF_MARKET_DATA(MktData &md, TDF_MARKET_DATA *po)
{
	bzero((void*)po,sizeof(TDF_MARKET_DATA));

	sprintf(po->szCode,"%06d",md.szcode());
	po->nTime=	md.ntime();
	po->nStatus=	md.nstatus();
	po->nPreClose=	md.npreclose();
	po->nOpen=	md.nopen();
	po->nHigh=	md.nhigh();
	po->nLow=	md.nlow();
	po->nMatch=	md.nmatch();

	for(int i=0;i<10;i++){
		po->nAskPrice[i]=	md.naskprice(i);
		po->nAskVol[i]=		md.naskvol(i);;
		po->nBidPrice[i]=	md.nbidprice(i);
		po->nBidVol[i]=		md.nbidvol(i);
	}

	po->nNumTrades=			md.nnumtrades();
	po->iVolume=                	md.ivolume();
	po->iTurnover=                  md.iturnover();
	po->nTotalBidVol=               md.ntotalbidvol();
	po->nTotalAskVol=               md.ntotalaskvol();
	po->nWeightedAvgBidPrice=       md.nweightedavgbidprice();
	po->nWeightedAvgAskPrice=       md.nweightedavgaskprice();
	po->nIOPV=                      md.niopv();
	po->nYieldToMaturity=           md.nyieldtomaturity();
	po->nHighLimited=               md.nhighlimited();
	po->nLowLimited=                md.nlowlimited();
	po->nSyl1=                      md.nsyl1();
	po->nSyl2=                      md.nsyl2();
	po->nSD2=                       md.nsd2();
}

int convert_MY_TYPE_TDF_MKT(char *pi,long lRecLen,char *po)
{
	string msg;

	MktData         md;

	msg.assign(pi, lRecLen);

	md.ParseFromString(msg);

	MktData2TDF_MARKET_DATA(md,(TDF_MARKET_DATA*)po);

	return 0;
}

void Transaction2TDF_TRANSACTION(Transaction &tr, TDF_TRANSACTION *po)
{
	bzero((void*)po,sizeof(TDF_TRANSACTION));

	sprintf(po->szCode,"%06d",tr.szcode());

	po->nTime=		tr.ntime();
	po->nIndex=		tr.nindex();
	po->nPrice=		tr.nprice();
	po->nVolume=		tr.nvolume();
	po->nTurnover=		tr.nturnover();
	po->nBSFlag=		tr.nbsflag();
	po->chOrderKind=	tr.chorderkind();
	po->chFunctionCode=	tr.chfunctioncode();
	po->nAskOrder=		tr.naskorder();
	po->nBidOrder=		tr.nbidorder();
}

int convert_MY_TYPE_TDF_TRA(char *pi,long lRecLen,char *po)
{
	string msg;

	Transaction         tr;

	msg.assign(pi, lRecLen);

	tr.ParseFromString(msg);

	Transaction2TDF_TRANSACTION(tr,(TDF_TRANSACTION*)po);

	return 0;
}
void Order2TDF_ORDER(Order &od, TDF_ORDER *po)
{

	bzero((void*)po,sizeof(TDF_ORDER));

	sprintf(po->szCode,"%06d",od.szcode());

	po->nTime=		od.ntime();

	po->nOrder=		od.norder();
	po->nPrice=		od.nprice();
	po->nVolume=		od.nvolume();


	po->chOrderKind=	od.chorderkind();
	po->chFunctionCode=	od.chfunctioncode();
	po->nBroker=		od.nbroker();
	po->chStatus=		od.chstatus();
	po->chFlag=		od.chflag();
}

int convert_MY_TYPE_TDF_ORD(char *pi,long lRecLen,char *po)
{
	string msg;

	Order         od;

	msg.assign(pi, lRecLen);

	od.ParseFromString(msg);

	Order2TDF_ORDER(od,(TDF_ORDER*)po);

	return 0;
}
void Order_queue2TDF_ORDER_QUEUE(Order_queue &oq, TDF_ORDER_QUEUE *po)
{
	bzero((void*)po,sizeof(TDF_ORDER_QUEUE));

	sprintf(po->szCode,"%06d",oq.szcode());

	po->nTime=		oq.ntime();
	po->nSide=		oq.nside()==1?'B':'A';
//	po->nSide=		(po->nSide==1?'B':'A');

	po->nPrice=		oq.nprice();
	po->nOrders=		oq.norders();
	po->nABItems=		oq.nabitems();

	for (int i = 0; i < po->nABItems; i++)
		po->nABVolume[i]=oq.nabvolume(i);
}
int convert_MY_TYPE_TDF_QUE(char *pi,long lRecLen,char *po)
{
	string msg;

	Order_queue         oq;

	msg.assign(pi, lRecLen);

	oq.ParseFromString(msg);

	Order_queue2TDF_ORDER_QUEUE(oq,(TDF_ORDER_QUEUE*)po);

	return 0;
}
int aiMyAmntLevel[10]={0,5,10,20,40,60,80,100,200,500};

int print_MY_TYPE_D31_STD(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	int l=0,i;
	char sCode[7];
	struct D31ItemStruct   *p=(D31ItemStruct *)(buf+sizeof(long long));

	sprintf(sCode,"%06d",p->nStockCode);

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,sCode)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\n",
			"picktime",
			"tradetime",
			"stockcode",
			"difftime");
	}

	char sTime[15];
	sFormatTime((time_t)p->nTradeTime,sTime);

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);
	long lTime=atoi(sTime+8)*1000;

	switch(iTimeFlag){
		case 1: *plCurTime=lTime;break;
		case 2: *plCurTime=lTmpTime;break;
		default:*plCurTime=lTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	for(i=0;i<10;i++){
		l+=sprintf(outbuf+l,"%lld\t%d\t%s\t%d\
\t%s\t%4d\t%-10ld\t%-6d\t%-6d\t%-10ld\t%-6d\t%-6d\n",
			*(long long *)buf,//picktime
			(int)lTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
			sCode,
			iDiffnTime((int)lTmpTime,(int)lTime),
			"z",aiMyAmntLevel[i],
			(long)(p->afZbBidAmount[i]*1000000),
			(int)(p->afZbBidVolume[i]*100),
			p->anZbBidOrderNum[i],
			(long)(p->afZbAskAmount[i]*1000000),
			(int)(p->afZbAskVolume[i]*100),
			p->anZbAskOrderNum[i]);

		l+=sprintf(outbuf+l,"%lld\t%d\t%s\t%d\
\t%s\t%4d\t%-10ld\t%-6d\t%-6d\t%-10ld\t%-6d\t%-6d\n",
			*(long long *)buf,//picktime
			(int)lTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
			sCode,
			iDiffnTime((int)lTmpTime,(int)lTime),
			"w",aiMyAmntLevel[i],
			(long)(p->afZdBidAmount[i]*1000000),
			(int)(p->afZdBidVolume[i]*100),
			p->anZdBidOrderNum[i],
			(long)(p->afZdAskAmount[i]*1000000),
			(int)(p->afZdAskVolume[i]*100),
			p->anZdAskOrderNum[i]);

	}

	return 0;
}

int print_MY_TYPE_D31_EXT(char *buf,char sCodeList[],int iTimeFlag,
	long lBgnTime,long *plCurTime,char *outbuf)
{
	char sCode[7];
	struct D31ItemStruct   *p=(struct D31ItemStruct *)(buf+sizeof(long long));

	sprintf(sCode,"%06d",p->nStockCode);

	//��������б�ǿ��ң������벻�ڴ����б�֮�ڣ��򲻴�ӡ
	if(sCodeList[0]&&
		strstr(sCodeList,sCode)==NULL) return 1;

	static int iFirstFlag=1;
	if(iFirstFlag){
		iFirstFlag=0;
		printf("%s\t%s\t%s\t%s\n",
			"picktime",
			"tradetime",
			"stockcode",
			"difftime");
	}

	char sTime[15];
	sFormatTime((time_t)p->nTradeTime,sTime);

	long lTmpTime=(long)((*(long long *)buf)%MY_DATE_CEIL_LONG);
	long lTime=atoi(sTime+8)*1000;

	switch(iTimeFlag){
		case 1: *plCurTime=lTime;break;
		case 2: *plCurTime=lTmpTime;break;
		default:*plCurTime=lTime;break;
	}
	if(*plCurTime<lBgnTime) return 2;

	sprintf(outbuf,"%lld\t%d\t%s\t%d\t\
%-10d\t%-10d\t%-12ld\t%-12ld\t%-10d\t%-10d\t%-12ld\t%-12ld\t\
%-10d\t%-10d\t%-10d\t%-10d\t%-12ld\t%-12ld\t%-12ld\t%-12ld\t%-12ld\t%-12ld\t%-12ld\t%-12ld\n",
		*(long long *)buf,//picktime
		(int)lTime,//ί��ʱ��YYYYMMDDHHMMSSMMM
		sCode,
		iDiffnTime((int)lTmpTime,(int)lTime),
		(int)(p->fTenBidVolume*100),		//ʮ���������֣�
		(int)(p->fTenAskVolume*100),		//ʮ���������֣�
		(long)(p->fTenBidAmnt*1000000),		//ʮ�����֣�
		(long)(p->fTenAskAmnt*1000000),		//ʮ������֣�
		(int)(p->fTotalBidVolume*100),		//�����������֣�
		(int)(p->fTotalAskVolume*100),		//�����������֣�
		(long)(p->fTotalBidAmnt*1000000),	//�����ܶ�֣�
		(long)(p->fTotalAskAmnt*1000000),	//�����ܶ�֣�
		(int)(p->fWtAvgBidPrice*10000),		//��Ȩƽ������ۣ��֣�
		(int)(p->fWtAvgAskPrice*10000),		//��Ȩƽ�������ۣ��֣�
		(int)(p->fLastClose*10000),		//�����̼�
		(int)(p->fCurPrice*10000),		//���¼�
		(long)(p->fAvgTotalBidAmnt*1000000),	//ƽ�������ܶ����ƽ�����֣�
		(long)(p->fAvgTotalAskAmnt*1000000),	//ƽ�������ܶ����ƽ�����֣�
		(long)(p->fBidAmount20*1000000),	//������20w����λ���֣�
		(long)(p->fAskAmount20*1000000),	//�������20w����λ���֣�
		(long)(p->fBidAmount50*1000000),	//������50w����λ���֣�
		(long)(p->fAskAmount50*1000000),	//�������50w����λ���֣�
		(long)(p->fBidAmount100*1000000),	//������100w����λ���֣�
		(long)(p->fAskAmount100*1000000)	//�������100w����λ���֣�
	);

	return 0;
}

void D31Item2D31_ITEM(D31Item &di, struct D31ItemStruct *po)
{
	int i;

	bzero((void*)po,sizeof(struct D31ItemStruct));

	po->nStockCode=		di.nstockcode();
	po->nTradeTime=		di.ntradetime();

        for(i=0;i<10;i++) po->afZbBidAmount[i]=    di.afzbbidamount(i);
	for(i=0;i<10;i++) po->afZbBidVolume[i]=    di.afzbbidvolume(i);
	for(i=0;i<10;i++) po->anZbBidOrderNum[i]=  di.anzbbidordernum(i);
	for(i=0;i<10;i++) po->afZbAskAmount[i]=    di.afzbaskamount(i);
	for(i=0;i<10;i++) po->afZbAskVolume[i]=    di.afzbaskvolume(i);
	for(i=0;i<10;i++) po->anZbAskOrderNum[i]=  di.anzbaskordernum(i);
	for(i=0;i<10;i++) po->afZdBidAmount[i]=    di.afzdbidamount(i);
	for(i=0;i<10;i++) po->afZdBidVolume[i]=    di.afzdbidvolume(i);
	for(i=0;i<10;i++) po->anZdBidOrderNum[i]=  di.anzdbidordernum(i);
	for(i=0;i<10;i++) po->afZdAskAmount[i]=    di.afzdaskamount(i);
	for(i=0;i<10;i++) po->afZdAskVolume[i]=    di.afzdaskvolume(i);
	for(i=0;i<10;i++) po->anZdAskOrderNum[i]=  di.anzdaskordernum(i);

        po->fTenBidVolume=	di.ftenbidvolume();
	po->fTenAskVolume=       di.ftenaskvolume();
	po->fTenBidAmnt=         di.ftenbidamnt();
	po->fTenAskAmnt=         di.ftenaskamnt();
	po->fTotalBidVolume=     di.ftotalbidvolume();
	po->fTotalAskVolume=     di.ftotalaskvolume();
	po->fTotalBidAmnt=       di.ftotalbidamnt();
	po->fTotalAskAmnt=       di.ftotalaskamnt();
	po->fWtAvgBidPrice=      di.fwtavgbidprice();
	po->fWtAvgAskPrice=      di.fwtavgaskprice();
	po->fLastClose=          di.flastclose();
	po->fCurPrice=           di.fcurprice();
	po->fAvgTotalBidAmnt=    di.favgtotalbidamnt();
	po->fAvgTotalAskAmnt=    di.favgtotalaskamnt();
	po->fBidAmount20=        di.fbidamount20();
	po->fAskAmount20=        di.faskamount20();
	po->fBidAmount50=        di.fbidamount50();
	po->fAskAmount50=        di.faskamount50();
	po->fBidAmount100=       di.fbidamount100();
	po->fAskAmount100=       di.faskamount100();
}


int convert_MY_TYPE_D31_ITEM(char *pi,long lRecLen,char *po)
{
	string msg;

	D31Item         di;

	msg.assign(pi, lRecLen);

	di.ParseFromString(msg);

	D31Item2D31_ITEM(di,(struct D31ItemStruct*)po);

	return 0;
}
