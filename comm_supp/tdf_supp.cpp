#include "tdf_supp.h"


void TDF_MARKET_DATA2MktData(MktData &output, const TDF_MARKET_DATA &src)
{
	output.set_szcode(strtoul(src.szCode,nullptr,10));	//600001.SH
	output.set_ntime(src.nTime);			//ʱ��(HHMMSSmmm)
	output.set_nstatus(src.nStatus);		//״̬
	output.set_npreclose(uint32_t(src.nPreClose));	//ǰ���̼�
	output.set_nopen(uint32_t(src.nOpen));			//���̼�
	output.set_nhigh(uint32_t(src.nHigh));			//��߼�
	output.set_nlow(uint32_t(src.nLow));				//��ͼ�
	output.set_nmatch(uint32_t(src.nMatch));			//���¼�
	for (unsigned char i = 0; i < 10; ++i) {
		output.add_naskprice(uint32_t(src.nAskPrice[i]));//������
		output.add_naskvol(src.nAskVol[i]);	//������
		output.add_nbidprice(uint32_t(src.nBidPrice[i]));//�����
		output.add_nbidvol(src.nBidVol[i]);	//������
	}
	output.set_nnumtrades(src.nNumTrades);	//�ɽ�����
	output.set_ivolume(src.iVolume);		//�ɽ�����
	output.set_iturnover(src.iTurnover);	//�ɽ��ܽ��
	output.set_ntotalbidvol(src.nTotalBidVol);//ί����������
	output.set_ntotalaskvol(src.nTotalAskVol);//ί����������
	output.set_nweightedavgbidprice(uint32_t(src.nWeightedAvgBidPrice));//��Ȩƽ��ί��۸�
	output.set_nweightedavgaskprice(uint32_t(src.nWeightedAvgAskPrice));//��Ȩƽ��ί���۸�
	output.set_niopv(src.nIOPV);		//IOPV��ֵ��ֵ
	output.set_nyieldtomaturity(src.nYieldToMaturity);//����������
	output.set_nhighlimited(uint32_t(src.nHighLimited));//��ͣ��
	output.set_nlowlimited(uint32_t(src.nLowLimited));//��ͣ��
	output.set_nsyl1(src.nSyl1);			//��ӯ��1
	output.set_nsyl2(src.nSyl2);			//��ӯ��2
	output.set_nsd2(src.nSD2);				//����2���Ա���һ�ʣ�
}

void TDF_TRANSACTION2Transaction(Transaction &output, const TDF_TRANSACTION &src)
{
	output.set_szcode(strtoul(src.szCode, nullptr, 10));	//600001.SH
	output.set_ntime(src.nTime);			//�ɽ�ʱ��(HHMMSSmmm)
	output.set_nindex(src.nIndex);			//�ɽ����
	output.set_nprice(uint32_t(src.nPrice));//�ɽ��۸�
	output.set_nvolume(src.nVolume);		//�ɽ�����
	output.set_nturnover(src.nTurnover);	//�ɽ����
	output.set_nbsflag(src.nBSFlag);		//��������(��'B', ����'S', ������' ')
	output.set_chorderkind(src.chOrderKind);//�ɽ����
	output.set_chfunctioncode(src.chFunctionCode);//�ɽ�����
	output.set_naskorder(src.nAskOrder);	//������ί�����
	output.set_nbidorder(src.nBidOrder);
}

void TDF_ORDER2Order(Order &output, const TDF_ORDER&src)
{
	output.set_szcode(strtoul(src.szCode, nullptr, 10));	//600001.SH
	output.set_ntime(src.nTime);			//ʱ��(HHMMSSmmm)
	output.set_norder(src.nOrder);		//ί�к�
	output.set_nprice(uint32_t(src.nPrice));//ί�м۸�
	output.set_nvolume(src.nVolume);		//ί������
	output.set_chorderkind(src.chOrderKind);//ί�����
	output.set_chfunctioncode(src.chFunctionCode);//ί�д���('B','S','C')
	output.set_nbroker(src.nBroker);		//�����̱���
	output.set_chstatus(src.chStatus);		//ί��״̬
	output.set_chflag(src.chFlag);
}

void TDF_ORDER_QUEUE2Order_queue(Order_queue &output, const TDF_ORDER_QUEUE&src)
{
	output.set_szcode(strtoul(src.szCode, nullptr, 10));	//600001.SH
	output.set_ntime(src.nTime);			//ʱ��(HHMMSSmmm)
	output.set_nside(src.nSide=='B'||src.nSide=='b');//��������('B':Bid 'A':Ask)
	output.set_nprice(uint32_t(src.nPrice));//ί�м۸�
	output.set_norders(src.nOrders);		//��������
	output.set_nabitems(src.nABItems);		//��ϸ����
	for (int i = 0; i < src.nABItems; ++i) {
		output.add_nabvolume(src.nABVolume[i]);
	}
}

void D31_ITEM2D31Item(D31Item &o, const struct D31ItemStruct &d)
{
	int i;

        o.set_nstockcode(d.nStockCode);
        o.set_ntradetime(d.nTradeTime);
        
        for(i=0;i<10;i++) o.add_afzbbidamount(d.afZbBidAmount[i]);
	for(i=0;i<10;i++) o.add_afzbbidvolume(d.afZbBidVolume[i]);
	for(i=0;i<10;i++) o.add_anzbbidordernum(d.anZbBidOrderNum[i]);
	for(i=0;i<10;i++) o.add_afzbaskamount(d.afZbAskAmount[i]);
	for(i=0;i<10;i++) o.add_afzbaskvolume(d.afZbAskVolume[i]);
	for(i=0;i<10;i++) o.add_anzbaskordernum(d.anZbAskOrderNum[i]);
	for(i=0;i<10;i++) o.add_afzdbidamount(d.afZdBidAmount[i]);
	for(i=0;i<10;i++) o.add_afzdbidvolume(d.afZdBidVolume[i]);
	for(i=0;i<10;i++) o.add_anzdbidordernum(d.anZdBidOrderNum[i]);
	for(i=0;i<10;i++) o.add_afzdaskamount(d.afZdAskAmount[i]);
	for(i=0;i<10;i++) o.add_afzdaskvolume(d.afZdAskVolume[i]);
	for(i=0;i<10;i++) o.add_anzdaskordernum(d.anZdAskOrderNum[i]);

        o.set_ftenbidvolume(	    d.fTenBidVolume);
	o.set_ftenaskvolume(        d.fTenAskVolume);
	o.set_ftenbidamnt(          d.fTenBidAmnt);
	o.set_ftenaskamnt(          d.fTenAskAmnt);
	o.set_ftotalbidvolume(      d.fTotalBidVolume);
	o.set_ftotalaskvolume(      d.fTotalAskVolume);
	o.set_ftotalbidamnt(        d.fTotalBidAmnt);
	o.set_ftotalaskamnt(        d.fTotalAskAmnt);
	o.set_fwtavgbidprice(       d.fWtAvgBidPrice);
	o.set_fwtavgaskprice(       d.fWtAvgAskPrice);
	o.set_flastclose(           d.fLastClose);
	o.set_fcurprice(            d.fCurPrice);
	o.set_favgtotalbidamnt(     d.fAvgTotalBidAmnt);
	o.set_favgtotalaskamnt(     d.fAvgTotalAskAmnt);
	o.set_fbidamount20(         d.fBidAmount20);
	o.set_faskamount20(         d.fAskAmount20);
	o.set_fbidamount50(         d.fBidAmount50);
	o.set_faskamount50(         d.fAskAmount50);
	o.set_fbidamount100(        d.fBidAmount100);
	o.set_faskamount100(        d.fAskAmount100);
	
	for(i=0;i<27;i++) o.add_afreserve(d.afReserve[i]);
}
