#include "CallBackBase.h"
#include <stdio.h>
#include "../Common/public.h"

CallBackBase::CallBackBase(void)
{
    m_ios = 0;
}


CallBackBase::~CallBackBase(void)
{
}

void CallBackBase::SetIoService(IoService *ios)
{
    m_ios = ios;
}

void CallBackBase::OnLoginState(RetCode errCode) {
//	BOOST_LOG_SEV(g_lg, info) << __FUNCTION__ << ' ' << errCode;
}

/// ����״̬���أ����ӳɹ�/ʧ��
/// @param  msgType     -- ��Ϣ����
/// @param  errCode     -- ʧ��ԭ�򣬳ɹ�ʱ����0 
///                     �������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ5.5�����뺬���б�RetCode �½� 
void CallBackBase::OnConnectionState(MsgType msgType, RetCode errCode)
{
    printf("MsgType:0x%x ConnectionState:%d\n", msgType, errCode);
//	BOOST_LOG_SEV(g_lg, info) << __FUNCTION__ << ' ' << msgType << ' ' << errCode;
}

void CallBackBase::OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation)
{
    /**
    ���棺�����ڻص������ӿ���ִ�к�ʱ�������磺�������㣬д�ļ��ȣ������������ݵĽ��ա�
    ���鴦��ʽ���ѽ��յ������ݷ������У����ɹ����̴߳�����յ����������ݣ�
    �ɲ��ձ��ص������Ĵ�����
    */

    if ( 0 == m_ios )
    {
        printf("IoService object is null");
        return ;
    }

    UTIL_Time stTime;
    PUTIL_GetLocalTime(&stTime);

    //���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
    SubData *subdata = new SubData;
    subdata->msgType = Msg_SSEL2_Quotation;
    subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
    subdata->data.assign((const char*)&RealSSEL2Quotation, sizeof(RealSSEL2Quotation));

    TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message, this, subdata)));
    m_ios->Post(task);
}

/// ���L1ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL1Quotation -- ʵʱ����
void CallBackBase::OnSubscribe_SZSEL1_Quotation(const SZSEL1_Quotation& RealSZSEL1Quotation)
{
    /**
    ���棺�����ڻص������ӿ���ִ�к�ʱ�������磺�������㣬д�ļ��ȣ������������ݵĽ��ա�
    ���鴦��ʽ���ѽ��յ������ݷ������У����ɹ����̴߳�����յ����������ݣ�
    �ɲ��ձ��ص������Ĵ�����
    */

    if ( 0 == m_ios )
    {
        printf("IoService object is null");
        return ;
    }

    UTIL_Time stTime;
    PUTIL_GetLocalTime(&stTime);

    //���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
    SubData *subdata = new SubData;
    subdata->msgType = Msg_SZSEL1_Quotation;
    subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
    subdata->data.assign((const char*)&RealSZSEL1Quotation, sizeof(RealSZSEL1Quotation));

    TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message, this, subdata)));
    m_ios->Post(task);
}

/// �Ͻ���L2��ʳɽ���������ʵʱ�ص��ӿ�
/// @param  RealSSEL2Transaction    -- ʵʱ����
void CallBackBase::OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction)
{
    /**
    ���棺�����ڻص������ӿ���ִ�к�ʱ�������磺�������㣬д�ļ��ȣ������������ݵĽ��ա�
    ���鴦��ʽ���ѽ��յ������ݷ������У����ɹ����̴߳�����յ����������ݣ�
    �ɲ��ձ��ص������Ĵ�����
    */

    if ( 0 == m_ios )
    {
        printf("IoService object is null");
        return ;
    }

    UTIL_Time stTime;
    PUTIL_GetLocalTime(&stTime);

    //���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
    SubData *subdata = new SubData;
    subdata->msgType = Msg_SSEL2_Transaction;
    subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
    subdata->data.assign((const char*)&RealSSEL2Transaction, sizeof(RealSSEL2Transaction));

    TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message, this, subdata)));
    m_ios->Post(task);
}

/// ���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
/// @param  RealSZSEL2Quotation -- ʵʱ����
void CallBackBase::OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation)
{
    /**
    ���棺�����ڻص������ӿ���ִ�к�ʱ�������磺�������㣬д�ļ��ȣ������������ݵĽ��ա�
    ���鴦��ʽ���ѽ��յ������ݷ������У����ɹ����̴߳�����յ����������ݣ�
    �ɲ��ձ��ص������Ĵ�����
    */

    if ( 0 == m_ios )
    {
        printf("IoService object is null");
        return ;
    }

    UTIL_Time stTime;
    PUTIL_GetLocalTime(&stTime);

    //���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
    SubData *subdata = new SubData;
    subdata->msgType = Msg_SZSEL2_Quotation;
    subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
    subdata->data.assign((const char*)&RealSZSEL2Quotation, sizeof(RealSZSEL2Quotation));

    TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message, this, subdata)));
    m_ios->Post(task);
}

/// �����Ͻ���L2ʵʱ������ն�������
void CallBackBase::Deal_Message(SubData *subdata)
{
    static int count = 0;// ��ֹ���̫������

    //����ָ�룬���Զ��ͷ�ָ��ָ��Ķ����ͷ��ڴ�ռ�
    std::unique_ptr<SubData> subdataptr(subdata);

    switch ( subdataptr->msgType )
    {
    case Msg_SSEL2_Quotation:
        {
            SSEL2_Quotation *RealSSEL2Quotation = (SSEL2_Quotation*)&subdata->data[0];
		m_gta2csvMgr.writeCsv(*RealSSEL2Quotation);
#if 0
            if( count < 10 )// ��ֹ���̫������
            {
                count++;
                printf("Subscribe Data: PacketTimeStamp = %lld, Symbol = %s, OpenPrice = %f, HighPrice = %f, LowPrice = %f, LastPrice = %f, ClosePrice = %f\n",
                    RealSSEL2Quotation->PacketTimeStamp, RealSSEL2Quotation->Symbol, RealSSEL2Quotation->OpenPrice, RealSSEL2Quotation->HighPrice,
                    RealSSEL2Quotation->LowPrice, RealSSEL2Quotation->LastPrice, RealSSEL2Quotation->ClosePrice);
            }
#endif
        }
        break;
    case Msg_SSEL2_Transaction:
        {
            SSEL2_Transaction *RealSSEL2Transaction = (SSEL2_Transaction*)&subdata->data[0];
			m_gta2csvMgr.writeCsv(*RealSSEL2Transaction);
#if 0
            if( count < 10 )// ��ֹ���̫������
            {
                count++;
                printf("Subscribe Data: PacketTimeStamp = %lld, Symbol = %s, TradeTime = %d, TradePrice = %f, TradeVolume = %u, TradeAmount = %f\n",
                    RealSSEL2Transaction->PacketTimeStamp, RealSSEL2Transaction->Symbol, RealSSEL2Transaction->TradeTime, RealSSEL2Transaction->TradePrice,
                    RealSSEL2Transaction->TradeVolume, RealSSEL2Transaction->TradeAmount);
            }
#endif
        }
        break;
    case Msg_SZSEL1_Quotation:
        {
            SZSEL1_Quotation *RealSZSEL1Quotation = (SZSEL1_Quotation*)&subdata->data[0];

            if( count < 10 )// ��ֹ���̫������
            {
                count++;
                printf("Subscribe Data: Time = %lld, Symbol = %s, OpenPrice = %f, HighPrice = %f, LowPrice = %f, LastPrice = %f\n",
                    RealSZSEL1Quotation->Time, RealSZSEL1Quotation->Symbol, RealSZSEL1Quotation->OpenPrice, RealSZSEL1Quotation->HighPrice,
                    RealSZSEL1Quotation->LowPrice, RealSZSEL1Quotation->LastPrice);
            }
        }
        break;
    case Msg_SZSEL2_Quotation:
        {
            SZSEL2_Quotation *RealSZSEL2Quotation = (SZSEL2_Quotation*)&subdata->data[0];
			m_gta2csvMgr.writeCsv(*RealSZSEL2Quotation);
#if 0
            if( count < 10 )// ��ֹ���̫������
            {
                count++;
                printf("Subscribe Data: LocalTimeStamp = %d, Symbol = %s, OpenPrice = %f, HighPrice = %f, LowPrice = %f, LastPrice = %f\n",
                    RealSSEL2Quotation->LocalTimeStamp, RealSSEL2Quotation->Symbol, RealSSEL2Quotation->OpenPrice, RealSSEL2Quotation->HighPrice,
                    RealSSEL2Quotation->LowPrice, RealSSEL2Quotation->LastPrice);
            }
#endif
        }
        break;
	case Msg_SZSEL2_Transaction:
	{
		SZSEL2_Transaction *RealSZSEL2Transaction = (SZSEL2_Transaction*)&subdata->data[0];
		m_gta2csvMgr.writeCsv(*RealSZSEL2Transaction);
	}
	break;
	case Msg_SZSEL2_Order:
	{
		SZSEL2_Order *RealSZSEL2Order = (SZSEL2_Order*)&subdata->data[0];
		m_gta2csvMgr.writeCsv(*RealSZSEL2Order);
	}
	break;
    default:
        printf("unknown message type:0x%x\n", subdataptr->msgType);
        break;
    }
}

/*virtual*/void CallBackBase::OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order)/*override*/ {
	if (0 == m_ios) {
		printf("IoService object is null");
		return;
	}

	UTIL_Time stTime;
	PUTIL_GetLocalTime(&stTime);

	//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
	SubData *subdata = new SubData;
	subdata->msgType = Msg_SZSEL2_Transaction;
	subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
	subdata->data.assign((const char*)&RealSZSEL2Order, sizeof(RealSZSEL2Order));

	TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message, this, subdata)));
	m_ios->Post(task);
}

void CallBackBase::OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction) {
	if (0 == m_ios)	{
		printf("IoService object is null");
		return;
	}

	UTIL_Time stTime;
	PUTIL_GetLocalTime(&stTime);

	//���յ����ݺ��ȷ��뱾�ض��У��ȴ����ݴ���ӿڴ���
	SubData *subdata = new SubData;
	subdata->msgType = Msg_SZSEL2_Transaction;
	subdata->cur_time = PUTIL_SystemTimeToDateTime(&stTime);
	subdata->data.assign((const char*)&RealSZSEL2Transaction, sizeof(RealSZSEL2Transaction));

	TaskPtr task(new Task(std::bind(&CallBackBase::Deal_Message, this, subdata)));
	m_ios->Post(task);
}
