#include <iostream>  
#include <time.h>  
#include <boost/asio.hpp>  
#include <boost/bind.hpp>    
#include <boost/function.hpp>  
#include <boost/date_time/posix_time/posix_time.hpp>    
  
using namespace std;  
  
//��ʱ��������  
class TimerController   
{  
public:  
  
    /** 
     * ��ʱ���������캯�� 
     * @param ios �첽I/O���� 
     * @param callbackFunc ��ʱ����ص����� 
     * @param uiWaitSec ��ʱ������ȴ�ʱ�䣬��λ���� 
     */  
    explicit TimerController(boost::asio::io_service &ios, boost::function<void()> callbackFunc, unsigned int uiWaitSec) :  
            m_timer(ios, boost::posix_time::seconds(uiWaitSec))
    {  
        m_timeoutHandle = callbackFunc;  
        m_uiWaitSec = uiWaitSec;  
        m_timer.async_wait(boost::bind(&TimerController::onTime, this, boost::asio::placeholders::error));  
    }  
  
    /** 
     * �������� 
     */  
    ~TimerController()   
    {  
        m_timer.cancel();  
    }  
  
    /** 
     * ��ʱ����Ӧ���� 
     * @param error_code ��ʱ���쳣������Ϣ 
     */  
    void onTime(const boost::system::error_code& err)   
    {  
    	if(err)
        {
    		cout<<"canle:"<<errno<<endl;
            return;
        }
        m_timeoutHandle();  
        //m_timer.expires_at(m_timer.expires_at() + boost::posix_time::seconds(m_uiWaitSec));  
        m_timer.expires_from_now(boost::posix_time::seconds(m_uiWaitSec));
        m_timer.async_wait(boost::bind(&TimerController::onTime, this, boost::asio::placeholders::error));  
    }
    void resetTimer(unsigned int uiWaitSec)
    {
    	m_timer.cancel();
    	m_uiWaitSec = uiWaitSec;
			m_timer.expires_from_now(boost::posix_time::seconds(m_uiWaitSec));
			m_timer.async_wait(boost::bind(&TimerController::onTime, this, boost::asio::placeholders::error));   	
    }
  
private:  
    unsigned int                    m_uiWaitSec;                //��ʱ�����ȴ�ʱ��  
    boost::asio::deadline_timer     m_timer;                    //asio��ʱ��  
    boost::function<void()>         m_timeoutHandle;            //��ʱ����ص�����  
};  
