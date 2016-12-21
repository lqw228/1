/** @file
* 环形队列实现文件 
* @author  dongpf
* @author  恒生电子股份有限公司
* @version 1.0
* @date    20120503
//	maoyinjie

	maoyinjie	2012-11-14	增加解锁代码
	dongpf      2015-03-11  增加其他类似的Queue
*/

#if !defined(AFX_QUEUE_H__0FE4EE07_9436_4591_AFD2_77A9F9FF5555__INCLUDED_)
#define AFX_QUEUE_H__0FE4EE07_9436_4591_AFD2_77A9F9FF5555__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

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
class  CEvent
{
public:
    ///构造函数
    /**
     *@param bInit: 事件的初始状态, true通知状态; false未通知状态
     */
	CEvent(bool bInit = false){};
    
    ///析构函数，释放资源
	~CEvent(){};
    
    ///等待一个事件
    /**
     *@param dwMillsSecond  等待的时间，默认为知道有事件发生
     *@return EVENT_OK 表示成功等到该事件激活， EVENT_TIMEOUT 表示超时，EVENT_ABORT表示事件被取消，其他失败
     */
	EVENTS Wait(long dwMillsSecond = INFINITE){return EVENT_OK;};
	
    ///激发该事件
    /**
     * @return 零表示成功，其他表示失败
     */
	int Set(void){return 0;};
	
    ///重置事件状态为false 
    /**
     * @return 零表示成功，其他表示失败
    */
	int Reset(void){return 0;};



};

//=================================环形队列=====================
//class T 字段类型（不支持自定义的结构，只支持指针和系统自带的类型），class TType 队列长度类型，class TMutex 加锁类型
template<class T, class TType, class TMutex>
class CCircularQueue  
{
public:
	/*
	* @param TType dwQueSize 初始队列长度
	* @param TType dwMaxQueLen  最大队列长度
	* @param TType dwReallocByStep 每次扩的大小
	* @param bool bSeqQue 是否顺序队列(无空洞)
	* @param bool bSafe 是否需要加锁
	*/
	CCircularQueue(TType dwQueSize = 10, TType dwMaxQueLen = 10000, 
		TType dwReallocByStep = 100, bool bSeqQue = false, bool bSafe = false);
	
	~CCircularQueue();
	
	/*
	* 从队列中取数据
	* @param bool bDeleteIfNULL 如果返回值是NULL，是否删除。用以遍历查找空洞数据
	* @return 数据
	*/
	T Pop(bool bDeleteIfNULL = true);
	
	/*
	* 从头开始批量数据
	* @param int iSeptCnt 要删除的个数
	* @return 是否成功
	*/
	int ClearItemsByStep(int iSeptCnt);
	
	/*
	* 放到队列里面
	* @param T item 数据
	* @return 是否成功, 0：成功，-1：扩内存失败，-2：非顺序队列
	*/
	int Push(T item);
	
	/*
	* 放到队列里面
	* @param TType index 内部编号
	* @param T item 数据
	* @param bool bOverWrite 是否覆盖写
	* @return 是否成功，0：成功，-1：扩内存失败，-2：数据为空或者是顺序队列，-3：重复写，-4：超过队列大小
	*/
	int Push(TType index, T item, bool bOverWrite = false);
	
	/*
	* 初始化队列
	* @param TType QueSize 队列长度
	* @return 是否成功
	*/
	int Resize(TType dwQueSize);
	
	/*
	* 当前队列积压数
	*/
	TType GetCount();
	
	/*
	* 当前队列积压数(加锁)
	*/
	TType GetSafeCount();

	/*
	* 当前队列长度
	*/
	TType GetQueSize();

	/*
	* 从队列中获取数据
	* @param TType index 内部编号
	*/
	T Get(TType index);

	/*
	* 从队列中获取数据
	* @param TType index 内部编号
	*/
	T operator[](TType index);

	/*
	* 摧毁队列
	*/
	void Destroy();

	/*
	* 获取当前队列的内存占用
	*/
	unsigned int GetCurrentMemory();

	//20150407 dongpf 增加复用接口
	/*
	* 重置当前队列
	*/
	void Reset();
	
private:
	T     *  m_Items;     //item的列表
	TType    m_QueSize;   //队列大小
	TType    m_CurrentSize; //当前节点数
	TType    m_first;     //当前可用节点号
	TType    m_end;       //当前空闲节点号(兼职判断是不是顺序队列)
	TMutex * m_mutex;//锁
	TType    m_MaxQueLen; //最大队列值
	TType    m_ReallocByStep; //每次扩的大小
};


template<class T, class TType, class TMutex>
CCircularQueue<T,TType,TMutex>::CCircularQueue(TType dwQueSize, 
								  TType dwMaxQueLen,
								  TType dwReallocByStep,
								  bool bSeqQue,
								  bool bSafe)
{
	m_CurrentSize = 0;
	m_QueSize = 0;
	m_Items = NULL;
	
	if (bSafe)
		m_mutex = new TMutex();
	else
		m_mutex = NULL;

	m_first = 0;
	//-1代表是非顺序队列(带空洞)
	m_end = !bSeqQue ? (TType)-1 : 0;

	m_MaxQueLen = dwMaxQueLen;
	m_ReallocByStep = dwReallocByStep;
	
	//初始化10;
	Resize(dwQueSize);		
	
}

template<class T, class TType, class TMutex>
CCircularQueue<T,TType,TMutex>::~CCircularQueue()
{
	if (m_Items)
	{
		Destroy();
	}
	if (m_mutex)
	{
		delete m_mutex;
		m_mutex = NULL;
	}
}

template<class T, class TType, class TMutex>
int CCircularQueue<T,TType,TMutex>::Resize(TType dwQueSize)
{
	int iRet = 0;
	//默认最小10个
	if (dwQueSize < 10)
	{
		dwQueSize = 10;
	}
	else if (dwQueSize > m_MaxQueLen ) //超过队列最大值
	{
		if (m_QueSize == m_MaxQueLen) //已经扩过了
			return -2;
		else
			//m_QueSize = m_MaxQueLen;
			dwQueSize = m_MaxQueLen;
	}
	else if (m_QueSize == dwQueSize) //不扩内存
	{
		return 0;
	}

	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	
	//如果第一次初始化
	if (m_CurrentSize == 0)
	{
		if (m_Items)
			free(m_Items);
		
		m_Items = (T*)malloc(sizeof(T) * dwQueSize); //申请空间
		if (m_Items == NULL )
		{
			iRet = -1;
		}
		else
		{
			m_QueSize = dwQueSize;
			memset(m_Items, 0, sizeof(T) * dwQueSize);
		}
	}
	else
	{
		T* lpItems = NULL;	
		
		if (m_QueSize < dwQueSize) //扩内存
		{
			lpItems = (T*)realloc(m_Items, sizeof(T) * dwQueSize);	//申请空间	
			if (lpItems == NULL )
			{
				iRet = -1;
			}
			else
			{
				m_Items = lpItems;
				TType stepSize = dwQueSize - m_QueSize;
				if (m_first == 0)
				{
					memset(m_Items + m_QueSize, 0, sizeof(T) *stepSize);
				}
				else if (stepSize >= m_first)  //新增的部分大于尾部大小
				{
					/*  first             step
					----|-----------------|--------
					|*********************        |
					-------------------------------*/
					//只需要移动尾部
					memmove(m_Items + m_QueSize, m_Items, sizeof(T) * m_first ); 
					//清空剩下的数据
					memset(m_Items, 0, sizeof(T) * m_first ); 
					memset(m_Items + m_QueSize + m_first, 0, sizeof(T) *(stepSize - m_first));
				}
				else
				{
					/*       first            step
					----------|---------------|----
					|*************************    |
					------------------------------*/
					//先移动前面可裁剪的尾部
					memmove(m_Items + m_QueSize, m_Items, sizeof(T) * stepSize );
					//再移动剩下的尾部
					memmove(m_Items, m_Items + stepSize, sizeof(T) * (m_first - stepSize));
					//清空剩下的数据
					memset(m_Items + m_first - stepSize, 0, sizeof(T) *stepSize);
				}

				//如果是顺序队列, first头不再移动
				if (m_end != (TType)-1)
				{
					m_end = (m_first + m_CurrentSize) % dwQueSize;
				}
				m_QueSize = dwQueSize;
			}
		}
		else if (m_QueSize > dwQueSize) //缩内存
		{
			lpItems = (T*)malloc(sizeof(T) * dwQueSize);	//申请空间	
			if (lpItems == NULL )
			{
				iRet = -1;
			}
			else
			{
				memset(lpItems, 0, sizeof(T) * dwQueSize);
				if (m_QueSize - m_first < dwQueSize)
				{
					//先拷贝头部
					memcpy(lpItems, m_Items + m_first, sizeof(T) * (m_QueSize - m_first));					
					//再拷贝尾部
					memcpy(lpItems + m_QueSize - m_first, m_Items, sizeof(T) * (dwQueSize - (m_QueSize - m_first)));
				}
				else
				{
					//只拷剩下的
					memcpy(lpItems, m_Items + m_first, sizeof(T) * dwQueSize);
				}	
				
				free(m_Items);
				m_Items = lpItems;

				//如果是顺序队列
				if (m_end != (TType)-1)
				{
					if (m_CurrentSize > dwQueSize) //超过队列长度，截断
					{
						m_end = 0;
						m_CurrentSize = dwQueSize;
						iRet = 1;
					}
					else
					{
						m_end = m_CurrentSize;
					}
				}
				m_first = 0;
				m_QueSize = dwQueSize;
			}
					
		}		
	}
	
	if (m_mutex)
		m_mutex->Release();
	
	return iRet;
}

template<class T, class TType, class TMutex>
T CCircularQueue<T,TType,TMutex>::Pop(bool bDeleteIfNULL)
{
	T temp = NULL;
	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	
	if (m_CurrentSize > 0)
	{
		temp = m_Items[m_first];
		
		// temp == NULL && !bDeleteIfNULL： 遇到空而且不删除节点
		if (temp != NULL || bDeleteIfNULL)
		{
			//设置为空，防止重复利用
			m_Items[m_first] = NULL;
			m_first = (m_first + 1) % m_QueSize;
		}
		
		if (temp != NULL)
		{
			--m_CurrentSize;
			//已经为空
			if (m_CurrentSize == 0)
			{
				m_first = 0;
				if (m_end != (TType)-1)
				{
					m_end = 0;
				}
			}
		}		
	}
	
	if (m_mutex)
		m_mutex->Release();
	return temp;
}

template<class T, class TType, class TMutex>
int CCircularQueue<T,TType,TMutex>::ClearItemsByStep(int iSeptCnt)
{
	T temp = NULL;
	//加锁
	if (m_mutex)
		m_mutex->Acquire();

	if (m_end != (TType)-1) //顺序队列
	{
		if (m_CurrentSize < iSeptCnt)
		{
			iSeptCnt = m_CurrentSize;
		}

		if (m_first + iSeptCnt < m_QueSize)
		{
			memset(m_Items + m_first, 0, iSeptCnt * sizeof(T) );
			m_first += iSeptCnt;
		}
		else
		{
			//头部分
			memset(m_Items + m_first, 0, (m_QueSize - m_first) * sizeof(T));
			//尾部分
			memset(m_Items, 0, (iSeptCnt - (m_QueSize - m_first)) * sizeof(T));

			m_first = iSeptCnt - (m_QueSize - m_first);
		}
		m_CurrentSize -= iSeptCnt;
	}
	else if (m_CurrentSize > 0) //非顺序队列
	{	
		for (int i = 0; i < iSeptCnt; ++i)
		{
			temp = m_Items[m_first];
			if (temp != NULL) //找到有数据的, 则清空
			{
				//设置为空，防止重复利用
				m_Items[m_first] = NULL;
				--m_CurrentSize;
				
				//已经为空
				if (m_CurrentSize == 0)
				{
					m_first = 0;
					break;
				}
			}		
			m_first = (m_first + 1) % m_QueSize;
		}
	}
	
	if (m_mutex)
		m_mutex->Release();
	
	return 0;
}

template<class T, class TType, class TMutex>
int CCircularQueue<T,TType,TMutex>::Push(T item)
{
	//非顺序队列
	if (m_end == (TType)-1)
	{
		return -2;
	}
	
	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	
	//	maoyinjie
	if(m_CurrentSize>=m_MaxQueLen)
	{
		//	maoyinjie	2012-11-14	增加解锁代码
		if (m_mutex)
			m_mutex->Release();
		return -1;
	}
	
	//超过队列大小
	if (m_CurrentSize >= m_QueSize)
	{
		//自动扩失败
		if (Resize(m_QueSize + m_ReallocByStep) != 0)
		{
			if (m_mutex)
				m_mutex->Release();
			return -1;
		}
		
	}
	//入列
	m_Items[m_end] = item;
	m_end = (m_end + 1) % m_QueSize;
	m_CurrentSize++;
	
	if (m_mutex)
		m_mutex->Release();
	return 0;
}

template<class T, class TType, class TMutex>
int CCircularQueue<T,TType,TMutex>::Push(TType index, T item, bool bOverWrite)
{
	int iRet = 0;
	//数据为空或者是顺序队列
	if (item == NULL || m_end != (TType)-1)
	{
		return -2;
	}

	//	maoyinjie
	if(index>=m_MaxQueLen)
		return -1;
	//索引值超过数组大小
	if (index >= m_QueSize)
	{
		TType iSize = (TType)((index - m_QueSize) / m_ReallocByStep + 1) * m_ReallocByStep;
		if (Resize(m_QueSize + iSize) != 0)
			return -1;
	}

	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	
	//超过队列大小，并且不是覆盖写
	if (m_CurrentSize >= m_QueSize && !bOverWrite)
	{
		if (m_mutex)
			m_mutex->Release();
		return -4;
	}
	
	TType iTemp = (index + m_first) % m_QueSize; //获取偏移量
	if (m_Items[iTemp] == NULL)
	{
		m_Items[iTemp] = item;
		m_CurrentSize++;
	}
	else if (bOverWrite) //覆盖写
	{
		m_Items[iTemp] = item;
	}
	else
	{
		iRet = -3;
	}
	
	//解锁
	if (m_mutex)
		m_mutex->Release();
	return iRet;
}

template<class T, class TType, class TMutex>
TType CCircularQueue<T,TType,TMutex>::GetCount()
{
	return m_CurrentSize;
}

template<class T, class TType, class TMutex>
TType CCircularQueue<T,TType,TMutex>::GetSafeCount()
{
	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	TType temp = m_CurrentSize;
	if (m_mutex)
		m_mutex->Release();
	return temp;
}

template<class T, class TType, class TMutex>
TType CCircularQueue<T,TType,TMutex>::GetQueSize()
{
	return m_QueSize;
}

template<class T, class TType, class TMutex>
T CCircularQueue<T,TType,TMutex>::Get(TType index)
{	
	//超过队列大小
	if (index >= m_QueSize)
	{
		return NULL;
	}
	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	TType iTemp = (index + m_first) % m_QueSize; //获取偏移量
	T temp = m_Items[iTemp];	
	//解锁
	if (m_mutex)
		m_mutex->Release();

	return temp;
}

template<class T, class TType, class TMutex>
T CCircularQueue<T,TType,TMutex>::operator[](TType index)
{
	return Get(index);
}

template<class T, class TType, class TMutex>
void CCircularQueue<T,TType,TMutex>::Destroy()
{
	//加锁
	if (m_mutex)
		m_mutex->Acquire();
	if (m_Items)
	{
		free(m_Items);
		m_Items = NULL;
	}
	//解锁
	if (m_mutex)
		m_mutex->Release();
}

template<class T, class TType, class TMutex>
unsigned int CCircularQueue<T,TType,TMutex>::GetCurrentMemory()
{
	return sizeof(CCircularQueue) + m_QueSize * sizeof(T);
}

//20150407 dongpf 增加复用接口
template<class T, class TType, class TMutex>
void CCircularQueue<T,TType,TMutex>::Reset()
{
	//加锁
	if (m_mutex)
		m_mutex->Acquire();

	m_CurrentSize = 0;
	m_first = 0;
	if (m_end != (TType)-1)
	{
		m_end = 0;
	}

	if (m_mutex)
		m_mutex->Release();
}

//dongpf      2015-03-11  增加其他类型的Queue
//#include <os/config_env.h>
//#include <os/thread_mutex.h>
//#include <os/event.h>
//USING_FBASE2_NAMESPACE

//===========================================不加锁队列==================================

template<class T>
class CNoLockQueue  
{
public:
	CNoLockQueue()
	{
		m_ReadEvent = FBASE_NEW CEvent();
		m_WriteEvent = FBASE_NEW CEvent();
		m_Items = NULL;
	}
	
	/*
	* 从队列中取数据
	* @param int iTimeOut 超时时间
	*/
	T * pop(int iTimeOut);

	/*
	* 从队列中取数据
	* @param int iTimeOut 超时时间
	* @param int &iCount 返回获取个数
	*/
	T ** popList(int iTimeOut, int &iCount);

	/*
	* 放到队列里面
	* @param T * item 数据
	* @param int iTimeOut 超时时间
	*/
	int push(T * item, int iTimeOut);

	/*
	* 初始化队列
	* @param int QueSize 队列长度，不能低于10
	*/
	void init(int QueSize = 10);

	/*
	* 当前队列积压数
	*/
	int getSize();

protected:

	inline T * mf_Pop();
private:
	T   ** m_Items;     //item的列表
	int    m_QueSize;   //队列大小
	int    m_first;     //当前可用节点号
	int    m_end;       //当前空闲节点号
	CEvent * m_ReadEvent;   //可读事件
	CEvent * m_WriteEvent;  //可写事件
};

template<class T>
T * CNoLockQueue<T>::pop(int iTimeOut)
{
	T * temp = NULL;
	temp = mf_Pop();	
	//队列为空
	if (temp == NULL && iTimeOut > 0)
	{
		//等待一段时间
		m_ReadEvent->Wait(iTimeOut);
		temp = mf_Pop();
	}
	return temp;
}

template<class T>
T ** CNoLockQueue<T>::popList(int iTimeOut, int &iCount)
{
	static T *sTlist[5];
	iCount = 0;
	for (; iCount < 5; ++iCount)
	{
		//队列还可读
		if (m_end != m_first )
		{
			sTlist[iCount] = m_Items[m_first];
			m_first = (m_first + 1) % m_QueSize;
		}
		else
		{
			m_WriteEvent->Set(); //设置可写事件
			break;
		}
	}
	if (iCount == 0)
	{
		m_ReadEvent->Wait(iTimeOut);
	}	
	
	return sTlist;
	 
}

template<class T>
inline T * CNoLockQueue<T>::mf_Pop()
{
	T * temp = NULL;
	if (m_end != m_first )
	{
		temp = m_Items[m_first];
		m_first = (m_first + 1) % m_QueSize;
		if (m_first == m_end)
		{
			m_WriteEvent->Set(); //设置可写事件
		}
	}
	return temp;
}

template<class T>
int CNoLockQueue<T>::push(T * item, int iTimeOut)
{	
	if ( (m_end - m_first + m_QueSize) % m_QueSize == m_QueSize-1 ) //队列满
	{
		if (iTimeOut == 0)
		{
			return -1;
		}
		int iResult = m_WriteEvent->Wait(iTimeOut); //等待可写事件
		if (iResult != ::FBASE2_NAMESPACE::EVENT_OK)
		{
			return -1;
		}
	}
	m_Items[m_end] = item;
	m_end = (m_end + 1) % m_QueSize;
	if (m_end - m_first == 1 || m_first - m_end == m_QueSize-1) //刚放入队列中
	{
		m_ReadEvent->Set(); //激活可读事件
	}
	return 0;
}

template<class T>
void CNoLockQueue<T>::init(int QueSize)
{
	//默认最小10个
	if (QueSize < 10)
	{
		QueSize = 10;
	}
	m_QueSize = QueSize;
	if (m_Items)
	{
		free(m_Items);
	}
	m_Items = (T**)malloc(sizeof(T*) * QueSize);
	m_first = 0;
	m_end = 0;
	m_WriteEvent->Reset();
	m_ReadEvent->Reset();
}

template<class T>
int CNoLockQueue<T>::getSize()
{
	return (m_end - m_first + m_QueSize) % m_QueSize;
}

//===========================================安全队列==================================

template<class T>
class CSafeQue  
{
public:
	CSafeQue()
	{
		m_event = FBASE_NEW CEvent();
		m_CurrentSize = 0;
		m_Items = NULL;
	}
	
	/*
	* 从队列中取数据
	* @param int iTimeOut 超时时间
	*/
	T Pop(int iTimeOut);

	/*
	* 放到队列里面
	* @param T * item 数据
	*/
	int push(T item);

	/*
	* 初始化队列
	* @param int QueSize 队列长度，不能低于10
	*/
	void init(int QueSize = 10);

	/*
	* 当前队列积压数
	*/
	int getSize();
protected:

	inline T mf_Pop();
private:
	T    * m_Items;     //item的列表
	int    m_QueSize;   //队列大小
	int    m_CurrentSize; //当前节点数
	int    m_first;     //当前可用节点号
	int    m_end;       //当前空闲节点号
	CEvent * m_event;   //事件
	//CThreadMutex m_mutex;
};

template<class T>
T CSafeQue<T>::Pop(int iTimeOut)
{
	T temp = NULL;
	//m_mutex.Acquire();
	temp = mf_Pop();
	//m_mutex.Release();
	
	//队列为空
	if (temp == NULL)
	{
		int iResult = m_event->Wait(iTimeOut);
		if (iResult == ::FBASE2_NAMESPACE::EVENT_OK)
		{
			//m_mutex.Acquire();
			temp = mf_Pop();
			//m_mutex.Release();
		}
	}
	return temp;
}

template<class T>
inline T CSafeQue<T>::mf_Pop()
{
	T temp = NULL;
	if (m_CurrentSize > 0)
	{
		temp = m_Items[m_first];
		m_first = (m_first + 1) % m_QueSize;
		//已经为空
		--m_CurrentSize;
		if (m_CurrentSize == 0)
		{
			m_first = m_end = 0;
		}
	}
	return temp;
}

template<class T>
int CSafeQue<T>::push(T item)
{
	if (item == NULL)
	{
		//唤醒队列
		m_event->Set();
		return 0;
	}
	//CAutoMutex AMutex(&m_mutex);
	if (m_CurrentSize >= m_QueSize)
	{
		return -1;
	}
	m_Items[m_end] = item;
	m_end = (m_end + 1) % m_QueSize;
	m_CurrentSize++;
	if (m_CurrentSize == 1)
	{
		m_event->Set();
	}
	return 0;
}

template<class T>
void CSafeQue<T>::init(int QueSize)
{
	//默认最小10个
	if (QueSize < 10)
	{
		QueSize = 10;
	}
	
	if (m_Items)
	{
		free(m_Items);
	}
	m_Items = (T*)malloc(sizeof(T) * QueSize);

	m_QueSize = 0;
	if (m_Items != NULL)
	{
		m_QueSize = QueSize;
	}
	m_first = 0;
	m_end = 0;
	m_CurrentSize = 0;
	m_event->Reset();
}

template<class T>
int CSafeQue<T>::getSize()
{
	return m_CurrentSize;
}

//===========================================空闲队列==================================

template<class T>
class CFreeQueue  
{
public:
	CFreeQueue()
	{
		m_Items = NULL;
		m_iBackCount = 0;
		m_QueSize = 0;
	}

	~CFreeQueue()
	{
		destroy();
	}
	
	/*
	* 从队列中取空闲
	*/
	T * get();

	/*
	* 放到队列里面
	* @param T * item 数据
	*/
	int free(T * item);

	/*
	* 初始化队列
	* @param int QueSize 队列长度，不能低于10
	*/
	int init(int QueSize = 10);

	/*
	* 当前队列空闲数
	*/
	int getSize()
	{
		return m_iBackCount;
	}

	void destroy();
private:
	T   ** m_Items;     //item的列表
	int    m_QueSize;   //队列大小
	int    m_iBackCount;//可用个数
	//CThreadMutex m_mutex;
};

template<class T>
int CFreeQueue<T>::init(int QueSize)
{
	if (QueSize < 10)
	{
		QueSize = 10;
	}
	m_Items = FBASE_NEW T *[QueSize];
	if (m_Items == NULL)
	{
		return -1;
	}
	for (m_iBackCount = 0; m_iBackCount < QueSize; ++m_iBackCount)
	{
		m_Items[m_iBackCount] = FBASE_NEW T;
	}
	m_QueSize = QueSize;
	return 0;
}

template<class T>
T * CFreeQueue<T>::get()
{
	{
		CAutoMutex mutex(&m_mutex);
		if (m_iBackCount > 0)
		{
			return m_Items[--m_iBackCount];
		}
	}
	return FBASE_NEW T;	
}

template<class T>
int CFreeQueue<T>::free(T * item)
{
	m_mutex.Acquire();
	if (m_iBackCount < m_QueSize)
	{
		m_Items[m_iBackCount++] = item;
		m_mutex.Release();
		return 0;
	}
	m_mutex.Release();
	delete item;
	return 0;
}

template<class T>
void CFreeQueue<T>::destroy()
{
	for (int i = 0; i < m_iBackCount; ++i)
	{
		delete m_Items[i];
	}
	delete []m_Items;
}


#endif // !defined(AFX_QUEUE_H__0FE4EE07_9436_4591_AFD2_77A9F9FF5555__INCLUDED_)
