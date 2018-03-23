#ifndef _CALLBACK_BASE_H
#define _CALLBACK_BASE_H
#include "GTAQTSInterfaceBase.h"
#include "../Common/IoService.h"
#include "../Common/SubData.h"

typedef struct _filename_set_
{
	string gtaqhName;
	string gtaahName;
	string gtathName;
	string gtaqzName;
	string gtatzName;
	string gtaozName;
	
	string tdfmktName;
	string tdfqueName;
	string tdftraName;
	string tdfordName;
	

	FILE *fpGtaQh;
	FILE *fpGtaAh;
	FILE *fpGtaTh;
	FILE *fpGtaQz;
	FILE *fpGtaTz;
	FILE *fpGtaOz;
	
	FILE *fpTdfMkt;
	FILE *fpTdfQue;
	FILE *fpTdfTra;
	FILE *fpTdfOrd;
	

} FileNameSet;

//�û�ͨ�����Ľӿڶ�����Ӧ��Ϣ���ݺ����ݽ�ͨ�����ӿڷ���
//�������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.1.2 �ص��ӿ���IGTAQTSCallbackBase �½�
class CallBackBase :public IGTAQTSCallbackBase
{
public:
	CallBackBase(int iWriteFlag,string &strWork);
	virtual ~CallBackBase();

	void SetIoService(IoService *ios);

public:
	virtual void OnLoginState(RetCode errCode) override;
    /// ����״̬���أ����ӳɹ�/ʧ��
    /// @param  msgType     -- ��Ϣ����
    /// @param  errCode     -- ʧ��ԭ�򣬳ɹ�ʱ����0
    ///                     �������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ5.5�����뺬���б�RetCode �½�
	virtual void OnConnectionState(MsgType msgType, RetCode errCode);

    /// �Ͻ���L2ʵʱ������ն�������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Quotation  -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Quotation(const SSEL2_Quotation& RealSSEL2Quotation);

    /// �Ͻ���L2��ʳɽ���������ʵʱ�ص��ӿ�
    /// @param  RealSSEL2Transaction    -- ʵʱ����
	virtual void OnSubscribe_SSEL2_Transaction(const SSEL2_Transaction& RealSSEL2Transaction);
    
    /// �Ͻ���L2���Ͼ��۶�������ʵʱ�ص��ӿ�
	virtual void OnSubscribe_SSEL2_Auction(const SSEL2_Auction& RealSSEL2Auction);

    /// ���L2ʵʱ���鶩������ʵʱ�ص��ӿ�
    /// @param  RealSZSEL2Quotation -- ʵʱ����
	virtual void OnSubscribe_SZSEL2_Quotation(const SZSEL2_Quotation& RealSZSEL2Quotation);

	/**
	    ��Ҫ����������Ϣ����ʱ����ͨ��ͷ�ļ�GTAQTSInterfaceBase.h�ڵ�IGTAQTSCallbackBase�࣬
	    ������Ӧ��OnSubscribe_xxx�ӿڵ��˴�����������ķ�����ʵ�ָú�������
	    �������̩��ʵʱ����ϵͳV2.X �û��ֲᡷ4.1.2.3 ʵʱ���ݻص�OnSubscribe �½�
	*/
	virtual void OnSubscribe_SZSEL2_Transaction(const SZSEL2_Transaction& RealSZSEL2Transaction) override;
	virtual void OnSubscribe_SZSEL2_Order(const SZSEL2_Order& RealSZSEL2Order) override;
	
	/// ����������
	void Deal_Message_SSEL2_Quotation(SubData *subdata);
	void Deal_Message_SSEL2_Transaction(SubData *subdata);
	void Deal_Message_SSEL2_Auction(SubData *subdata);
	void Deal_Message_SZSEL2_Quotation(SubData *subdata);
	void Deal_Message_SZSEL2_Transaction(SubData *subdata);
	void Deal_Message_SZSEL2_Order(SubData *subdata);


private:
	IoService	*m_ios;
	FileNameSet m_fileSet;
	int	m_iWriteFlag;	
};

void LockWorkThread();
void UnLockWorkThread();
int IsWorkThreadLock();

void InitUserArray(char sDispName[]);
void FlashUserArray(char sDispName[]);

#endif  //_CALLBACK_BASE_H
