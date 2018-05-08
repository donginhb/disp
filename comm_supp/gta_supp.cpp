#include <cstring>
#include <cstdint>

#include "wwtiny.h"
#include "gta_supp.h"

struct LimitUpDownStruct LIMIT[MAX_STOCK_CODE];

void InitLimitArray()
{
	for(int i=0;i<MAX_STOCK_CODE;i++){
		LIMIT[i].WarrantDownLimit=0.0;
		LIMIT[i].WarrantUpLimit=0.0;
	}
}

char GTA2TDF_MARKET_STATUS(const char *TradeStatus, const char *SecurityPhaseTag) {
	//null ��������

	//0 ��������
	//1 �����¹�
	//2 �������۷���
	//3 �������۷���
	//'A' ���׽�����

	//'B' ����ͣ��
	if (SecurityPhaseTag[0] == 'P') return 'B';
	//'C' ȫ������
	//'D' ��ͣ����

	//'E'(START)����������
	if (!strcmp(TradeStatus, "START")) return 'E';
	//'F'(PRETR)��ǰ����
	if (!strcmp(TradeStatus, "PRETR")) return 'F';
	//'G'(DEL)���ɻָ����׵��۶Ͻ׶Σ��Ͻ�����N��
	if (!strcmp(TradeStatus, "DEL")) return 'G';
	//'H'(HOLIDAY)�ż�

	//'I'(OCALL)���м��Ͼ���
	if (!strcmp(TradeStatus, "OCALL")) return 'I';
	//'J'(ICALL)���м��Ͼ���
	if (!strcmp(TradeStatus, "ICALL")) return 'J';
	//'K'(OPOBB)���ж�����ƽ��ǰ��
	if (!strcmp(TradeStatus, "OPOBB")) return 'K';
	//'L'(IPOBB)���ж�����ƽ��ǰ��
	if (!strcmp(TradeStatus, "IPOBB")) return 'L';
	//'M(OOBB ) ���ж�����ƽ��
	if (!strcmp(TradeStatus, "OOBB")) return 'M';
	//'N'(IOBB)���ж�����ƽ��
	if (!strcmp(TradeStatus, "IOBB")) return 'N';
	//'O'(TRADE)�������
	if (!strcmp(TradeStatus, "TRADE")) return 'O';
	//'P'(BREAK)����
	if (!strcmp(TradeStatus, "BREAK")) return 'P';
	//'Q'(VOLA)�������ж�
	if (!strcmp(TradeStatus, "VOLA")) return 'Q';
	//'R'(BETW)���׼�
	if (!strcmp(TradeStatus, "BETW")) return 'R';
	//'S'(NOTRD)�ǽ��׷���֧��
	if (!strcmp(TradeStatus, "NOTRD")) return 'S';
	//'T'(FCALL)�̶��۸񼯺Ͼ���
	if (!strcmp(TradeStatus, "FCALL")) return 'T';
	//'U'(POSTR)�̺���
	if (!strcmp(TradeStatus, "POSTR")) return 'U';
	//'V'(ENDTR)��������
	if (!strcmp(TradeStatus, "ENDTR")) return 'V';
	//'W'(HALT)��ͣ
	if (!strcmp(TradeStatus, "HALT")) return 'W';
	//'X'(SUSP)ͣ��
	if (!strcmp(TradeStatus, "SUSP")) return 'X';
	//'Y'(ADD)������Ʒ
	if (!strcmp(TradeStatus, "ADD")) return 'Y';
	//'Z'(DEL)��ɾ���Ĳ�Ʒ
	if (!strcmp(TradeStatus, "DEL")) return 'Z';
	//'d' ���Ͼ��۽׶ν������������۽׶ο�ʼ֮ǰ��ʱ�Σ����У�
	if (SecurityPhaseTag[0] == 'D') return 'd';
	//'q' �ɻָ����׵��۶�ʱ��(�Ͻ�����M)
	if (SecurityPhaseTag[0] == 'M') return 'q';

	return 0;
};

void GTA2TDF_SSEL2(const SSEL2_Quotation &src, TDF_MARKET_DATA &m, TDF_ORDER_QUEUE &qB, TDF_ORDER_QUEUE &qS) {
	//char        szWindCode[32];         //600001.SH
	strcpy(m.szWindCode, src.Symbol);
	m.szWindCode[6] = '.';
	m.szWindCode[7] = 'S';
	m.szWindCode[8] = 'H';
	m.szWindCode[9] = '\0';
	//char        szCode[32];             //ԭʼCode
	strcpy(m.szCode, src.Symbol);
	//int         nActionDay;             //ҵ������(��Ȼ��)
	m.nActionDay = int(src.PacketTimeStamp / 1000000000);
	//int         nTradingDay;            //������
	m.nTradingDay = m.nActionDay;
	//int			 nTime;					//ʱ��(HHMMSSmmm)
	m.nTime = src.Time;
	//int			 nStatus;				//״̬
	m.nStatus = GTA2TDF_MARKET_STATUS(src.TradeStatus, src.SecurityPhaseTag);
	//__int64 nPreClose;					//ǰ���̼�
	m.nPreClose = yuan2percentFen(src.PreClosePrice);
	//__int64 nOpen;						//���̼�
	m.nOpen = yuan2percentFen(src.OpenPrice);
	//__int64 nHigh;						//��߼�
	m.nHigh = yuan2percentFen(src.HighPrice);
	//__int64 nLow;						//��ͼ�
	m.nLow = yuan2percentFen(src.LowPrice);
	//__int64 nMatch;						//���¼�
	m.nMatch = yuan2percentFen(src.LastPrice);
	for (unsigned i = 0; i < 10; ++i) {
		//__int64 nAskVol[10];				//������
		m.nAskVol[i] = src.SellLevel[i].Volume;
		m.nAskPrice[i] = yuan2percentFen(src.SellLevel[i].Price);
	}
	for (unsigned i = 0; i < 10; ++i) {
		//__int64 nBidVol[10];				//������
		m.nBidVol[i] = src.BuyLevel[i].Volume;
		m.nBidPrice[i] = yuan2percentFen(src.BuyLevel[i].Price);		
	}

	//int nNumTrades;						//�ɽ�����
	m.nNumTrades = int(src.TotalNo);
	//__int64		 iVolume;				//�ɽ�����
	m.iVolume = src.TotalVolume;
	//__int64		 iTurnover;				//�ɽ��ܽ��
	m.iTurnover = (__int64)(src.TotalAmount);
	//__int64		 nTotalBidVol;			//ί����������
	m.nTotalBidVol = src.TotalBuyOrderVolume;
	//__int64		 nTotalAskVol;			//ί����������
	m.nTotalAskVol = src.TotalSellOrderVolume;
	//__int64 nWeightedAvgBidPrice;		//��Ȩƽ��ί��۸�
	m.nWeightedAvgBidPrice = yuan2percentFen(src.WtAvgBuyPrice);
	//__int64 nWeightedAvgAskPrice;		//��Ȩƽ��ί���۸�
	m.nWeightedAvgAskPrice = yuan2percentFen(src.WtAvgSellPrice);
	//int			 nIOPV;					//IOPV��ֵ��ֵ
	//int			 nYieldToMaturity;		//����������

	//__int64 nHighLimited;				//��ͣ��
	//__int64 nLowLimited;				//��ͣ��
	//g_LimitPriceMgr.getLimitPrices(m.nHighLimited, m.nLowLimited, src.Symbol);
	
	m.nHighLimited=		yuan2percentFen(src.WarrantUpLimit);
	m.nLowLimited=		yuan2percentFen(src.WarrantDownLimit);

	//char		 chPrefix[4];			//֤ȯ��Ϣǰ׺
	//int			 nSyl1;					//��ӯ��1
	//int			 nSyl2;					//��ӯ��2
	//int			 nSD2;					//����2���Ա���һ�ʣ�

	//char    szWindCode[32]; //600001.SH
	strcpy(qB.szWindCode, m.szWindCode);
	strcpy(qS.szWindCode, m.szWindCode);
	//char    szCode[32];     //ԭʼCode
	strcpy(qB.szCode, src.Symbol);
	strcpy(qS.szCode, src.Symbol);
	//int     nActionDay;     //��Ȼ��
	qB.nActionDay = m.nActionDay;
	qS.nActionDay = m.nActionDay;
	//int 	nTime;			//ʱ��(HHMMSSmmm)
	qB.nTime = m.nTime;
	qS.nTime = m.nTime;
	//int     nSide;			//��������('B':Bid 'A':Ask)
	qB.nSide = 'B';
	qS.nSide = 'A';
	//__int64		nPrice;		//ί�м۸�
	qB.nPrice = yuan2percentFen(src.BuyPrice01);
	qS.nPrice = yuan2percentFen(src.SellPrice01);
	//int 	nOrders;		//��������
	qB.nOrders = src.BuyLevelQueueNo01;
	qS.nOrders = src.SellLevelQueueNo01;
	//int 	nABItems;		//��ϸ����
	//int 	nABVolume[200];	//������ϸ
	qB.nABItems=qS.nABItems=0;
	for (unsigned i = 0; i < ORDER_LEVEL_FIFTY; ++i) {
		if (!(qB.nABVolume[i] = src.BuyLevelQueue[i])) {
			qB.nABItems = i;
			break;
		}
	}
	for (unsigned i = 0; i < ORDER_LEVEL_FIFTY; ++i) {
		if (!(qS.nABVolume[i] = src.SellLevelQueue[i])) {
			qS.nABItems = i;
			break;
		}
	}
};

void GTA2TDF_SSEL2_T(const SSEL2_Transaction &src, TDF_TRANSACTION &t) {
	//char    szCode[32];                 //ԭʼCode
	strcpy(t.szCode, src.Symbol);
	//char    szWindCode[32];             //600001.SH 
	strcpy(t.szWindCode, src.Symbol);
	t.szWindCode[6] = '.';
	t.szWindCode[7] = 'S';
	t.szWindCode[8] = 'H';
	//int     nActionDay;                 //��Ȼ��
	t.nActionDay = int(src.PacketTimeStamp / 1000000000);
	//int 	nTime;		                //�ɽ�ʱ��(HHMMSSmmm)
	t.nTime = src.TradeTime;
	//int 	nIndex;		                //�ɽ����
	t.nIndex = src.RecID;
	//__int64		nPrice;		            //�ɽ��۸�
	t.nPrice = yuan2percentFen(src.TradePrice);
	//int 	nVolume;	                //�ɽ�����
	t.nVolume = src.TradeVolume;
	//__int64		nTurnover;	            //�ɽ����
	t.nTurnover = yuan2percentFen(src.TradeAmount);
	//int     nBSFlag;                    //��������(��'B', ����'S', ������' ')
	switch (src.BuySellFlag) {
		case 'B':
		case 'b':
			t.nBSFlag = 'B';
			break;
		case 'S':
		case 's':
			t.nBSFlag = 'S';
			break;
		default:
			if (src.BuyRecID > src.SellRecID)
				t.nBSFlag = 'B';
			else
				t.nBSFlag = 'S';
	}
	//char    chOrderKind;                //�ɽ����
	t.chOrderKind = 0;
	//char    chFunctionCode;             //�ɽ�����
	t.chFunctionCode = 0;
	//int	    nAskOrder;	                //������ί�����
	t.nAskOrder = int(src.SellRecID);
	//int	    nBidOrder;	                //����ί�����
	t.nBidOrder = int(src.BuyRecID);
}

void GTA2TDF_SZSEL2(const SZSEL2_Quotation &src, TDF_MARKET_DATA &m, TDF_ORDER_QUEUE &qB, TDF_ORDER_QUEUE &qS) {
	//char        szWindCode[32];         //600001.SH
	strcpy(m.szWindCode, src.Symbol);
	m.szWindCode[6] = '.';
	m.szWindCode[7] = 'S';
	m.szWindCode[8] = 'Z';
	m.szWindCode[9] = '\0';
	//char        szCode[32];             //ԭʼCode
	strcpy(m.szCode, src.Symbol);
	//int         nActionDay;             //ҵ������(��Ȼ��)
	m.nActionDay = int(src.Time / 1000000000);
	//int         nTradingDay;            //������
	m.nTradingDay = m.nActionDay;
	//int			 nTime;					//ʱ��(HHMMSSmmm)
	m.nTime = src.Time % 1000000000;
	//int			 nStatus;				//״̬		
	//< �� 1 λ��0=����״̬,1=ȫ��ͣ��"
	if (src.SecurityPhaseTag[1] == '1') {
		m.nStatus = 'B';
	}
	else {
		//�� 0 λ��
		switch (src.SecurityPhaseTag[0]) {
			//T = ����
		case 'T':
			m.nStatus = 'O';
			break;
			//S = ����������ǰ��
		case 'S':
			m.nStatus = 'E';
			break;
			//O = ���̼��Ͼ���
		case 'O':
			m.nStatus = 'I';
			break;
			//B = ����
		case 'B':
			m.nStatus = 'P';
			break;
			//C = ���̼��Ͼ���

			//E = �ѱ���
		case 'E':
			m.nStatus = 'V';
			break;
			//H = ��ʱͣ��
		case 'H':
			m.nStatus = 'W';
			break;
			//A = �̺���
		case 'A':
			m.nStatus = 'U';
			break;
			//V = �������ж�
		case 'V':
			m.nStatus = 'Q';
			break;
		}
	}
	//__int64 nPreClose;					//ǰ���̼�
	m.nPreClose = yuan2percentFen(src.PreClosePrice);
	//__int64 nOpen;						//���̼�
	m.nOpen = yuan2percentFen(src.OpenPrice);
	//__int64 nHigh;						//��߼�
	m.nHigh = yuan2percentFen(src.HighPrice);
	//__int64 nLow;						//��ͼ�
	m.nLow = yuan2percentFen(src.LowPrice);
	//__int64 nMatch;						//���¼�
	m.nMatch = yuan2percentFen(src.LastPrice);

	for (unsigned i = 0; i < LEVEL_TEN; ++i) {
		//��ȫ���ƻ�ȡ����		
		m.nAskVol[i] = (__int64)(src.SellLevel[i].Volume);
		m.nAskPrice[i] = yuan2percentFen(src.SellLevel[i].Price);
	}
	for (unsigned i = 0; i < LEVEL_TEN; ++i) {
		//��ȫ���ƻ�ȡ����__int64 nBidVol[10];			//������
		m.nBidVol[i] = (__int64)(src.BuyLevel[i].Volume);
		m.nBidPrice[i] = yuan2percentFen(src.BuyLevel[i].Price);
	}

	//int nNumTrades;						//�ɽ�����
	m.nNumTrades = int(src.TotalNo);
	//__int64		 iVolume;				//�ɽ�����
	m.iVolume = (__int64)(src.TotalVolume + 0.5);
	//__int64		 iTurnover;				//�ɽ��ܽ��
	m.iTurnover = (__int64)(src.TotalAmount);
	//__int64		 nTotalBidVol;			//ί����������
	m.nTotalBidVol = (__int64)(src.TotalBuyOrderVolume + 0.5);
	//__int64		 nTotalAskVol;			//ί����������
	m.nTotalAskVol = (__int64)(src.TotalSellOrderVolume + 0.5);
	//__int64 nWeightedAvgBidPrice;		//��Ȩƽ��ί��۸�
	m.nWeightedAvgBidPrice = yuan2percentFen(src.WtAvgBuyPrice);
	//__int64 nWeightedAvgAskPrice;		//��Ȩƽ��ί���۸�
	m.nWeightedAvgAskPrice = yuan2percentFen(src.WtAvgSellPrice);
	//int			 nIOPV;					//IOPV��ֵ��ֵ
	//int			 nYieldToMaturity;		//����������
	//__int64 nHighLimited;				//��ͣ��
	m.nHighLimited = yuan2percentFen(src.PriceUpLimit);
	//__int64 nLowLimited;				//��ͣ��
	m.nLowLimited = yuan2percentFen(src.PriceDownLimit);
	//char		 chPrefix[4];			//֤ȯ��Ϣǰ׺

	//int			 nSyl1;					//��ӯ��1
	m.nSyl1 = int((src.PERatio1 + 0.005) * 100);
	//int			 nSyl2;					//��ӯ��2
	m.nSyl2 = int((src.PERatio2 + 0.005) * 100);
	//int			 nSD2;					//����2���Ա���һ�ʣ�
	m.nSD2 = int((src.PriceUpdown2 + 0.005) * 100);

	//char    szWindCode[32];             //600001.SH
	strcpy(qB.szWindCode, m.szWindCode);
	strcpy(qS.szWindCode, m.szWindCode);
	//char    szCode[32];                 //ԭʼCode
	strcpy(qB.szCode, src.Symbol);
	strcpy(qS.szCode, src.Symbol);
	//int     nActionDay;                 //��Ȼ��
	qB.nActionDay = m.nActionDay;
	qS.nActionDay = m.nActionDay;
	//int 	nTime;		                //�ɽ�ʱ��(HHMMSSmmm)
	qB.nTime = m.nTime;
	qS.nTime = m.nTime;
	//int     nSide;			//��������('B':Bid 'A':Ask)
	qB.nSide = 'B';
	qS.nSide = 'A';
	//__int64		nPrice;		//ί�м۸�
	qB.nPrice = yuan2percentFen(src.BuyPrice01);
	qS.nPrice = yuan2percentFen(src.SellPrice01);
	//int 	nOrders;		//��������
	qB.nOrders = src.BuyLevelQueueNo01;
	qS.nOrders = src.SellLevelQueueNo01;
	//int 	nABItems;		//��ϸ����
	//int 	nABVolume[200];	//������ϸ
	qB.nABItems=qS.nABItems=0;
	for (unsigned i = 0; i < ORDER_LEVEL_FIFTY; ++i) {
		if (!(qB.nABVolume[i] = int(src.BuyLevelQueue[i]))) {
			qB.nABItems = i;
			break;
		}
	}
	for (unsigned i = 0; i < ORDER_LEVEL_FIFTY; ++i) {
		if (!(qS.nABVolume[i] = int(src.SellLevelQueue[i]))) {
			qS.nABItems = i;
			break;
		}
	}
}

void  GTA2TDF_SZSEL2_T(const SZSEL2_Transaction &src, TDF_TRANSACTION &t) {
	//char    szWindCode[32];             //600001.SH
	strcpy(t.szWindCode, src.Symbol);
	t.szWindCode[6] = '.';
	t.szWindCode[7] = 'S';
	t.szWindCode[8] = 'Z';
	t.szWindCode[9] = '\0';
	//char    szCode[32];                 //ԭʼCode
	strcpy(t.szCode, src.Symbol);
	//int     nActionDay;                 //��Ȼ��
	t.nActionDay = int(src.TradeTime / 1000000000);
	//int 	nTime;		                //�ɽ�ʱ��(HHMMSSmmm)
	t.nTime = src.TradeTime % 1000000000;
	//int 	nIndex;		                //�ɽ����
	t.nIndex = int(src.RecID);
	//__int64		nPrice;		            //�ɽ��۸�
	t.nPrice = yuan2percentFen(src.TradePrice);
	//int 	nVolume;	                //�ɽ�����
	t.nVolume = int(src.TradeVolume + 0.5);
	//__int64		nTurnover;	            //�ɽ����
	t.nTurnover = t.nPrice * t.nVolume;
	//char    chOrderKind;                //�ɽ����
	t.chOrderKind = '0';
	//char    chFunctionCode;             //�ɽ�����
	switch (src.TradeType) {
		case '4':
			t.chFunctionCode = 'C';
			t.nBSFlag = ' ';
			break;
		default:
			t.chFunctionCode = '0';
			//int     nBSFlag;                    //��������(��'B', ����'S', ������' ')
			if (src.BuyOrderID > src.SellOrderID)
				t.nBSFlag = 'B';
			else
				t.nBSFlag = 'S';
	}
	//int	    nAskOrder;	                //������ί�����
	t.nAskOrder = int(src.SellOrderID);
	//int	    nBidOrder;	                //����ί�����
	t.nBidOrder = int(src.BuyOrderID);
}

void GTA2TDF_SZSEL2_O(const SZSEL2_Order &src, TDF_ORDER &o) {
	//char    szWindCode[32]; //600001.SH 
	strcpy(o.szWindCode, src.Symbol);
	o.szWindCode[6] = '.';
	o.szWindCode[7] = 'S';
	o.szWindCode[8] = 'Z';
	o.szWindCode[9] = '\0';
	//char    szCode[32];     //ԭʼCode
	strcpy(o.szCode, src.Symbol);
	//int 	nActionDay;	    //ί������(YYMMDD)
	o.nActionDay = int(src.Time / 100000000);
	//int 	nTime;			//ί��ʱ��(HHMMSSmmm)
	o.nTime = src.Time % 1000000000;
	//int 	nOrder;	        //ί�к�
	o.nOrder = int(src.RecID);
	//__int64		nPrice;		//ί�м۸�
	o.nPrice = yuan2percentFen(src.OrderPrice);
	//int 	nVolume;		//ί������
	o.nVolume = int(src.OrderVolume);
	//char    chOrderKind;	//ί�����
	o.chOrderKind = src.OrderType;
	switch (src.OrderType) {
	case '2':
		o.chOrderKind = '0';
		break;
	default:
		o.chOrderKind = src.OrderType;
	}
	//char    chFunctionCode;	//ί�д���('B','S','C')
	switch (src.OrderCode) {
	case '1':
		o.chFunctionCode = 'B';
		break;
	case '2':
		o.chFunctionCode = 'S';
		break;
	default:
		o.chFunctionCode = src.OrderCode;
	}
	//int     nBroker;		//�����̱���
	//char    chStatus;		//ί��״̬
	//char	chFlag;			//��־
}

void  GTA2TDF_SSEL2_AM(const SSEL2_Auction &src, TDF_MARKET_DATA &m) {
	
	bzero((void*)&m,sizeof(TDF_MARKET_DATA));

	//char        szWindCode[32];         //600001.SH
	strcpy(m.szWindCode, src.Symbol);
	m.szWindCode[6] = '.';
	m.szWindCode[7] = 'S';
	m.szWindCode[8] = 'H';
	m.szWindCode[9] = '\0';
	//char        szCode[32];             //ԭʼCode
	strcpy(m.szCode, src.Symbol);
	//int         nActionDay;             //ҵ������(��Ȼ��)
	m.nActionDay = int(src.PacketTimeStamp / 1000000000);
	//int         nTradingDay;            //������
	m.nTradingDay = m.nActionDay;
	//int			 nTime;					//ʱ��(HHMMSSmmm)
	m.nTime = src.Time % 1000000000;
	//int			 nStatus;				//״̬
	//__int64 nPreClose;					//ǰ���̼�
	//__int64 nOpen;						//���̼�
	//__int64 nHigh;						//��߼�
	//__int64 nLow;						//��ͼ�
	//__int64 nMatch;						//���¼�

	//__int64 nAskVol[10];				//������
	m.nBidVol[0] = m.nAskVol[0] = src.AuctionVolume;
	//__int64 nAskPrice[10];				//������
	m.nBidPrice[0] = m.nAskPrice[0] = yuan2percentFen(src.OpenPrice);
	switch (src.Side) {
	case '1':
		m.nBidVol[1] = src.LeaveVolume;
		break;
	case '2':
		m.nAskVol[1] = src.LeaveVolume;
		break;
	}
}
