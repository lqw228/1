/** 跨平台事件 CEvent 类定义
源程序名称: event.h
软件著作权: 恒生电子股份有限公司 版权所有
系统名称: 公用技术平台
模块名称: 跨平台适配层 
功能说明: 跨平台事件封装类
相关文档: 
作者:
审核: (可选的)
	
修改记录
修改日期:
修改人员:
修改说明:
*******************************************************************/


#ifndef EVENT_INC_
#define EVENT_INC_

#include <os/config_env.h>

BEGIN_FBASE2_NAMESPACE

/**
 * 生产者线程
 * int proceduer()
 * {
 *      while(true){
 *          worklist.push(worktask) ///添加一个任务
 *          if( worklist.size() > 0)
 *              event.Set();  //产生事件，唤醒所有消费者线程
 *      }
 * }
 * 消费者线程
 * int consumer()
 * {
 *      while(true)
 *      {
 *          if( worklist.size() == 0)
 *          {
 *             event.Wait(1000); //等待事件产生，最多等1000ms
 *          }
 *          else
 *          {
 *             ///以下是任务处理
 *             ....
            }
 *      }
 *  }
 */
 
///等待事件时返回值
/**
 *EVENT_FAILED 失败
 *EVENT_OK 有事件发生
 *EVENT_TIMEOUT 等待时间超时
 *EVENT_ABANDONED 事件对象非法使用
 */
enum EVENTS
{
    EVENT_FAILED = -1, EVENT_OK = 0, EVENT_TIMEOUT, EVENT_ABANDONED
};

///事件对象定义
/**
 *该事件对象可以用于win32和支持posix标准的其它平台上,该事件为自动重置事件
 */
class API_EXPORT CEvent
{
public:
    ///构造函数
    /**
     *@param bInit: 事件的初始状态, true通知状态; false未通知状态
     */
    CEvent(bool bInit = false);
    
    ///析构函数，释放资源
    ~CEvent();
    
    ///等待一个事件
    /**
     *@param dwMillsSecond  等待的时间，默认为知道有事件发生
     *@return EVENT_OK 表示成功等到该事件激活， EVENT_TIMEOUT 表示超时，EVENT_ABORT表示事件被取消，其他失败
     */
	EVENTS Wait(long dwMillsSecond = INFINITE);
	
    ///激发该事件
    /**
     * @return 零表示成功，其他表示失败
     */
	int Set(void);
	
    ///重置事件状态为false 
    /**
     * @return 零表示成功，其他表示失败
    */
    int Reset(void);
private:
#ifdef _WIN32
    ///事件对象句柄
    HANDLE  m_hEvent;
#else
	///保护条件变量的互斥子
    pthread_mutex_t   m_Mutex;
    ///条件变量句柄
    pthread_cond_t  m_Cond;
    
    bool			m_bNotify;
#endif

};

END_FBASE2_NAMESPACE

#endif
