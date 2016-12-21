/** ��ƽ̨�¼� CEvent �ඨ��
Դ��������: event.h
�������Ȩ: �������ӹɷ����޹�˾ ��Ȩ����
ϵͳ����: ���ü���ƽ̨
ģ������: ��ƽ̨����� 
����˵��: ��ƽ̨�¼���װ��
����ĵ�: 
����:
���: (��ѡ��)
	
�޸ļ�¼
�޸�����:
�޸���Ա:
�޸�˵��:
*******************************************************************/


#ifndef EVENT_INC_
#define EVENT_INC_

#include <os/config_env.h>

BEGIN_FBASE2_NAMESPACE

/**
 * �������߳�
 * int proceduer()
 * {
 *      while(true){
 *          worklist.push(worktask) ///���һ������
 *          if( worklist.size() > 0)
 *              event.Set();  //�����¼������������������߳�
 *      }
 * }
 * �������߳�
 * int consumer()
 * {
 *      while(true)
 *      {
 *          if( worklist.size() == 0)
 *          {
 *             event.Wait(1000); //�ȴ��¼�����������1000ms
 *          }
 *          else
 *          {
 *             ///������������
 *             ....
            }
 *      }
 *  }
 */
 
///�ȴ��¼�ʱ����ֵ
/**
 *EVENT_FAILED ʧ��
 *EVENT_OK ���¼�����
 *EVENT_TIMEOUT �ȴ�ʱ�䳬ʱ
 *EVENT_ABANDONED �¼�����Ƿ�ʹ��
 */
enum EVENTS
{
    EVENT_FAILED = -1, EVENT_OK = 0, EVENT_TIMEOUT, EVENT_ABANDONED
};

///�¼�������
/**
 *���¼������������win32��֧��posix��׼������ƽ̨��,���¼�Ϊ�Զ������¼�
 */
class API_EXPORT CEvent
{
public:
    ///���캯��
    /**
     *@param bInit: �¼��ĳ�ʼ״̬, true֪ͨ״̬; falseδ֪ͨ״̬
     */
    CEvent(bool bInit = false);
    
    ///�����������ͷ���Դ
    ~CEvent();
    
    ///�ȴ�һ���¼�
    /**
     *@param dwMillsSecond  �ȴ���ʱ�䣬Ĭ��Ϊ֪�����¼�����
     *@return EVENT_OK ��ʾ�ɹ��ȵ����¼���� EVENT_TIMEOUT ��ʾ��ʱ��EVENT_ABORT��ʾ�¼���ȡ��������ʧ��
     */
	EVENTS Wait(long dwMillsSecond = INFINITE);
	
    ///�������¼�
    /**
     * @return ���ʾ�ɹ���������ʾʧ��
     */
	int Set(void);
	
    ///�����¼�״̬Ϊfalse 
    /**
     * @return ���ʾ�ɹ���������ʾʧ��
    */
    int Reset(void);
private:
#ifdef _WIN32
    ///�¼�������
    HANDLE  m_hEvent;
#else
	///�������������Ļ�����
    pthread_mutex_t   m_Mutex;
    ///�����������
    pthread_cond_t  m_Cond;
    
    bool			m_bNotify;
#endif

};

END_FBASE2_NAMESPACE

#endif
