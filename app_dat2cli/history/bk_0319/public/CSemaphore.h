/*VER: 1*/
#ifndef __CSEMAPHORE_HEADER_INCLULDED__
#define __CSEMAPHORE_HEADER_INCLULDED__

/**********************************************************************************************\
* �ź�����
 *
 *
 \**********************************************************************************************/

class CSemaphore
{
public:
	bool getSemEx(const char * sem_name,
	/*��Windows�Ͽ������ַ�����ʾ����unix�����������֣���ʾkey_t*/
	int init_count = 1,
	/*��ʼ��Դ������Ĭ��Ϊ 1 */
	int max_count = 64);
	/*���Է���Ľ���������Ĭ��Ϊ64*/

	bool getSemEx(int iKey,
	/*��Windows�Ͽ������ַ�����ʾ����unix�����������֣���ʾkey_t*/
	int init_count = 1,
	/*��ʼ��Դ������Ĭ��Ϊ 1 */
	int max_count = 64);
	/*���Է���Ľ���������Ĭ��Ϊ64*/

	bool getSem(const char * sem_name,
	/*��Windows�Ͽ������ַ�����ʾ����unix�����������֣���ʾkey_t*/
	int init_count = 1,
	/*��ʼ��Դ������Ĭ��Ϊ 1 */
	int max_count = 64);
	/*���Է���Ľ���������Ĭ��Ϊ64*/
	bool P(bool bWait = true); /*P��������ʾ������Դ, bWait�Ƿ�Ҫͬ���ȴ�*/
	bool V(); /*v��������ʾ�ͷ���Դ*/
	bool V_W(); /*v��������ʾ�ͷ���Դ*/
	bool P_W(bool bWait = true); /*P��������ʾ������Դ, bWait�Ƿ�Ҫͬ���ȴ�*/
	bool close(); /*�ر��ź���*/
public:
	CSemaphore(); /*���캯�����������semaphore�����룬��ҪgetSem��ʹ��*/
	virtual ~CSemaphore(); /*�رվ��*/
private:
	void * _Handle; /*�ź�����������*/
	char m_sSemName[19];
};
//ֻ�����ڵ��߳��������ͬһ�����ظ�Lock�����̻߳�������
class CSemaphoreEx
{
public:
	CSemaphoreEx();
	virtual ~CSemaphoreEx();
	//�������źż�
	bool getSemExMulit(const char* sem_name, int iSemNum, int max_count = 64);
	bool P_Mulit_R(int iPos, bool bWait = true);
	bool V_Mulit_R(int iPos);

	bool P_Mulit_W(int iPos, bool bWait = true);
	bool V_Mulit_W(int iPos);

	bool close(); /*�ر��ź���*/
private:
	bool P_Mulit(int iPos, int iCnt, bool bWait = true);
	bool V_Mulit(int iPos, int iCnt);

private:
	void * _Handle; /*�ź�����������*/
	char m_sSemName[19];
	int m_iMaxCnt;
};

#endif

