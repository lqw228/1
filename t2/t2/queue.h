/** @file
* ���ζ���ʵ���ļ� 
* @author  dongpf
* @author  �������ӹɷ����޹�˾
* @version 1.0
* @date    20120503
//	maoyinjie

	maoyinjie	2012-11-14	���ӽ�������
	dongpf      2015-03-11  �����������Ƶ�Queue
*/

#if !defined(AFX_QUEUE_H__0FE4EE07_9436_4591_AFD2_77A9F9FF5555__INCLUDED_)
#define AFX_QUEUE_H__0FE4EE07_9436_4591_AFD2_77A9F9FF5555__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <stdio.h>

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
class  CEvent
{
public:
    ///���캯��
    /**
     *@param bInit: �¼��ĳ�ʼ״̬, true֪ͨ״̬; falseδ֪ͨ״̬
     */
	CEvent(bool bInit = false){};
    
    ///�����������ͷ���Դ
	~CEvent(){};
    
    ///�ȴ�һ���¼�
    /**
     *@param dwMillsSecond  �ȴ���ʱ�䣬Ĭ��Ϊ֪�����¼�����
     *@return EVENT_OK ��ʾ�ɹ��ȵ����¼���� EVENT_TIMEOUT ��ʾ��ʱ��EVENT_ABORT��ʾ�¼���ȡ��������ʧ��
     */
	EVENTS Wait(long dwMillsSecond = INFINITE){return EVENT_OK;};
	
    ///�������¼�
    /**
     * @return ���ʾ�ɹ���������ʾʧ��
     */
	int Set(void){return 0;};
	
    ///�����¼�״̬Ϊfalse 
    /**
     * @return ���ʾ�ɹ���������ʾʧ��
    */
	int Reset(void){return 0;};



};

//=================================���ζ���=====================
//class T �ֶ����ͣ���֧���Զ���Ľṹ��ֻ֧��ָ���ϵͳ�Դ������ͣ���class TType ���г������ͣ�class TMutex ��������
template<class T, class TType, class TMutex>
class CCircularQueue  
{
public:
	/*
	* @param TType dwQueSize ��ʼ���г���
	* @param TType dwMaxQueLen  �����г���
	* @param TType dwReallocByStep ÿ�����Ĵ�С
	* @param bool bSeqQue �Ƿ�˳�����(�޿ն�)
	* @param bool bSafe �Ƿ���Ҫ����
	*/
	CCircularQueue(TType dwQueSize = 10, TType dwMaxQueLen = 10000, 
		TType dwReallocByStep = 100, bool bSeqQue = false, bool bSafe = false);
	
	~CCircularQueue();
	
	/*
	* �Ӷ�����ȡ����
	* @param bool bDeleteIfNULL �������ֵ��NULL���Ƿ�ɾ�������Ա������ҿն�����
	* @return ����
	*/
	T Pop(bool bDeleteIfNULL = true);
	
	/*
	* ��ͷ��ʼ��������
	* @param int iSeptCnt Ҫɾ���ĸ���
	* @return �Ƿ�ɹ�
	*/
	int ClearItemsByStep(int iSeptCnt);
	
	/*
	* �ŵ���������
	* @param T item ����
	* @return �Ƿ�ɹ�, 0���ɹ���-1�����ڴ�ʧ�ܣ�-2����˳�����
	*/
	int Push(T item);
	
	/*
	* �ŵ���������
	* @param TType index �ڲ����
	* @param T item ����
	* @param bool bOverWrite �Ƿ񸲸�д
	* @return �Ƿ�ɹ���0���ɹ���-1�����ڴ�ʧ�ܣ�-2������Ϊ�ջ�����˳����У�-3���ظ�д��-4���������д�С
	*/
	int Push(TType index, T item, bool bOverWrite = false);
	
	/*
	* ��ʼ������
	* @param TType QueSize ���г���
	* @return �Ƿ�ɹ�
	*/
	int Resize(TType dwQueSize);
	
	/*
	* ��ǰ���л�ѹ��
	*/
	TType GetCount();
	
	/*
	* ��ǰ���л�ѹ��(����)
	*/
	TType GetSafeCount();

	/*
	* ��ǰ���г���
	*/
	TType GetQueSize();

	/*
	* �Ӷ����л�ȡ����
	* @param TType index �ڲ����
	*/
	T Get(TType index);

	/*
	* �Ӷ����л�ȡ����
	* @param TType index �ڲ����
	*/
	T operator[](TType index);

	/*
	* �ݻٶ���
	*/
	void Destroy();

	/*
	* ��ȡ��ǰ���е��ڴ�ռ��
	*/
	unsigned int GetCurrentMemory();

	//20150407 dongpf ���Ӹ��ýӿ�
	/*
	* ���õ�ǰ����
	*/
	void Reset();
	
private:
	T     *  m_Items;     //item���б�
	TType    m_QueSize;   //���д�С
	TType    m_CurrentSize; //��ǰ�ڵ���
	TType    m_first;     //��ǰ���ýڵ��
	TType    m_end;       //��ǰ���нڵ��(��ְ�ж��ǲ���˳�����)
	TMutex * m_mutex;//��
	TType    m_MaxQueLen; //������ֵ
	TType    m_ReallocByStep; //ÿ�����Ĵ�С
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
	//-1�����Ƿ�˳�����(���ն�)
	m_end = !bSeqQue ? (TType)-1 : 0;

	m_MaxQueLen = dwMaxQueLen;
	m_ReallocByStep = dwReallocByStep;
	
	//��ʼ��10;
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
	//Ĭ����С10��
	if (dwQueSize < 10)
	{
		dwQueSize = 10;
	}
	else if (dwQueSize > m_MaxQueLen ) //�����������ֵ
	{
		if (m_QueSize == m_MaxQueLen) //�Ѿ�������
			return -2;
		else
			//m_QueSize = m_MaxQueLen;
			dwQueSize = m_MaxQueLen;
	}
	else if (m_QueSize == dwQueSize) //�����ڴ�
	{
		return 0;
	}

	//����
	if (m_mutex)
		m_mutex->Acquire();
	
	//�����һ�γ�ʼ��
	if (m_CurrentSize == 0)
	{
		if (m_Items)
			free(m_Items);
		
		m_Items = (T*)malloc(sizeof(T) * dwQueSize); //����ռ�
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
		
		if (m_QueSize < dwQueSize) //���ڴ�
		{
			lpItems = (T*)realloc(m_Items, sizeof(T) * dwQueSize);	//����ռ�	
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
				else if (stepSize >= m_first)  //�����Ĳ��ִ���β����С
				{
					/*  first             step
					----|-----------------|--------
					|*********************        |
					-------------------------------*/
					//ֻ��Ҫ�ƶ�β��
					memmove(m_Items + m_QueSize, m_Items, sizeof(T) * m_first ); 
					//���ʣ�µ�����
					memset(m_Items, 0, sizeof(T) * m_first ); 
					memset(m_Items + m_QueSize + m_first, 0, sizeof(T) *(stepSize - m_first));
				}
				else
				{
					/*       first            step
					----------|---------------|----
					|*************************    |
					------------------------------*/
					//���ƶ�ǰ��ɲü���β��
					memmove(m_Items + m_QueSize, m_Items, sizeof(T) * stepSize );
					//���ƶ�ʣ�µ�β��
					memmove(m_Items, m_Items + stepSize, sizeof(T) * (m_first - stepSize));
					//���ʣ�µ�����
					memset(m_Items + m_first - stepSize, 0, sizeof(T) *stepSize);
				}

				//�����˳�����, firstͷ�����ƶ�
				if (m_end != (TType)-1)
				{
					m_end = (m_first + m_CurrentSize) % dwQueSize;
				}
				m_QueSize = dwQueSize;
			}
		}
		else if (m_QueSize > dwQueSize) //���ڴ�
		{
			lpItems = (T*)malloc(sizeof(T) * dwQueSize);	//����ռ�	
			if (lpItems == NULL )
			{
				iRet = -1;
			}
			else
			{
				memset(lpItems, 0, sizeof(T) * dwQueSize);
				if (m_QueSize - m_first < dwQueSize)
				{
					//�ȿ���ͷ��
					memcpy(lpItems, m_Items + m_first, sizeof(T) * (m_QueSize - m_first));					
					//�ٿ���β��
					memcpy(lpItems + m_QueSize - m_first, m_Items, sizeof(T) * (dwQueSize - (m_QueSize - m_first)));
				}
				else
				{
					//ֻ��ʣ�µ�
					memcpy(lpItems, m_Items + m_first, sizeof(T) * dwQueSize);
				}	
				
				free(m_Items);
				m_Items = lpItems;

				//�����˳�����
				if (m_end != (TType)-1)
				{
					if (m_CurrentSize > dwQueSize) //�������г��ȣ��ض�
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
	//����
	if (m_mutex)
		m_mutex->Acquire();
	
	if (m_CurrentSize > 0)
	{
		temp = m_Items[m_first];
		
		// temp == NULL && !bDeleteIfNULL�� �����ն��Ҳ�ɾ���ڵ�
		if (temp != NULL || bDeleteIfNULL)
		{
			//����Ϊ�գ���ֹ�ظ�����
			m_Items[m_first] = NULL;
			m_first = (m_first + 1) % m_QueSize;
		}
		
		if (temp != NULL)
		{
			--m_CurrentSize;
			//�Ѿ�Ϊ��
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
	//����
	if (m_mutex)
		m_mutex->Acquire();

	if (m_end != (TType)-1) //˳�����
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
			//ͷ����
			memset(m_Items + m_first, 0, (m_QueSize - m_first) * sizeof(T));
			//β����
			memset(m_Items, 0, (iSeptCnt - (m_QueSize - m_first)) * sizeof(T));

			m_first = iSeptCnt - (m_QueSize - m_first);
		}
		m_CurrentSize -= iSeptCnt;
	}
	else if (m_CurrentSize > 0) //��˳�����
	{	
		for (int i = 0; i < iSeptCnt; ++i)
		{
			temp = m_Items[m_first];
			if (temp != NULL) //�ҵ������ݵ�, �����
			{
				//����Ϊ�գ���ֹ�ظ�����
				m_Items[m_first] = NULL;
				--m_CurrentSize;
				
				//�Ѿ�Ϊ��
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
	//��˳�����
	if (m_end == (TType)-1)
	{
		return -2;
	}
	
	//����
	if (m_mutex)
		m_mutex->Acquire();
	
	//	maoyinjie
	if(m_CurrentSize>=m_MaxQueLen)
	{
		//	maoyinjie	2012-11-14	���ӽ�������
		if (m_mutex)
			m_mutex->Release();
		return -1;
	}
	
	//�������д�С
	if (m_CurrentSize >= m_QueSize)
	{
		//�Զ���ʧ��
		if (Resize(m_QueSize + m_ReallocByStep) != 0)
		{
			if (m_mutex)
				m_mutex->Release();
			return -1;
		}
		
	}
	//����
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
	//����Ϊ�ջ�����˳�����
	if (item == NULL || m_end != (TType)-1)
	{
		return -2;
	}

	//	maoyinjie
	if(index>=m_MaxQueLen)
		return -1;
	//����ֵ���������С
	if (index >= m_QueSize)
	{
		TType iSize = (TType)((index - m_QueSize) / m_ReallocByStep + 1) * m_ReallocByStep;
		if (Resize(m_QueSize + iSize) != 0)
			return -1;
	}

	//����
	if (m_mutex)
		m_mutex->Acquire();
	
	//�������д�С�����Ҳ��Ǹ���д
	if (m_CurrentSize >= m_QueSize && !bOverWrite)
	{
		if (m_mutex)
			m_mutex->Release();
		return -4;
	}
	
	TType iTemp = (index + m_first) % m_QueSize; //��ȡƫ����
	if (m_Items[iTemp] == NULL)
	{
		m_Items[iTemp] = item;
		m_CurrentSize++;
	}
	else if (bOverWrite) //����д
	{
		m_Items[iTemp] = item;
	}
	else
	{
		iRet = -3;
	}
	
	//����
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
	//����
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
	//�������д�С
	if (index >= m_QueSize)
	{
		return NULL;
	}
	//����
	if (m_mutex)
		m_mutex->Acquire();
	TType iTemp = (index + m_first) % m_QueSize; //��ȡƫ����
	T temp = m_Items[iTemp];	
	//����
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
	//����
	if (m_mutex)
		m_mutex->Acquire();
	if (m_Items)
	{
		free(m_Items);
		m_Items = NULL;
	}
	//����
	if (m_mutex)
		m_mutex->Release();
}

template<class T, class TType, class TMutex>
unsigned int CCircularQueue<T,TType,TMutex>::GetCurrentMemory()
{
	return sizeof(CCircularQueue) + m_QueSize * sizeof(T);
}

//20150407 dongpf ���Ӹ��ýӿ�
template<class T, class TType, class TMutex>
void CCircularQueue<T,TType,TMutex>::Reset()
{
	//����
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

//dongpf      2015-03-11  �����������͵�Queue
//#include <os/config_env.h>
//#include <os/thread_mutex.h>
//#include <os/event.h>
//USING_FBASE2_NAMESPACE

//===========================================����������==================================

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
	* �Ӷ�����ȡ����
	* @param int iTimeOut ��ʱʱ��
	*/
	T * pop(int iTimeOut);

	/*
	* �Ӷ�����ȡ����
	* @param int iTimeOut ��ʱʱ��
	* @param int &iCount ���ػ�ȡ����
	*/
	T ** popList(int iTimeOut, int &iCount);

	/*
	* �ŵ���������
	* @param T * item ����
	* @param int iTimeOut ��ʱʱ��
	*/
	int push(T * item, int iTimeOut);

	/*
	* ��ʼ������
	* @param int QueSize ���г��ȣ����ܵ���10
	*/
	void init(int QueSize = 10);

	/*
	* ��ǰ���л�ѹ��
	*/
	int getSize();

protected:

	inline T * mf_Pop();
private:
	T   ** m_Items;     //item���б�
	int    m_QueSize;   //���д�С
	int    m_first;     //��ǰ���ýڵ��
	int    m_end;       //��ǰ���нڵ��
	CEvent * m_ReadEvent;   //�ɶ��¼�
	CEvent * m_WriteEvent;  //��д�¼�
};

template<class T>
T * CNoLockQueue<T>::pop(int iTimeOut)
{
	T * temp = NULL;
	temp = mf_Pop();	
	//����Ϊ��
	if (temp == NULL && iTimeOut > 0)
	{
		//�ȴ�һ��ʱ��
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
		//���л��ɶ�
		if (m_end != m_first )
		{
			sTlist[iCount] = m_Items[m_first];
			m_first = (m_first + 1) % m_QueSize;
		}
		else
		{
			m_WriteEvent->Set(); //���ÿ�д�¼�
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
			m_WriteEvent->Set(); //���ÿ�д�¼�
		}
	}
	return temp;
}

template<class T>
int CNoLockQueue<T>::push(T * item, int iTimeOut)
{	
	if ( (m_end - m_first + m_QueSize) % m_QueSize == m_QueSize-1 ) //������
	{
		if (iTimeOut == 0)
		{
			return -1;
		}
		int iResult = m_WriteEvent->Wait(iTimeOut); //�ȴ���д�¼�
		if (iResult != ::FBASE2_NAMESPACE::EVENT_OK)
		{
			return -1;
		}
	}
	m_Items[m_end] = item;
	m_end = (m_end + 1) % m_QueSize;
	if (m_end - m_first == 1 || m_first - m_end == m_QueSize-1) //�շ��������
	{
		m_ReadEvent->Set(); //����ɶ��¼�
	}
	return 0;
}

template<class T>
void CNoLockQueue<T>::init(int QueSize)
{
	//Ĭ����С10��
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

//===========================================��ȫ����==================================

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
	* �Ӷ�����ȡ����
	* @param int iTimeOut ��ʱʱ��
	*/
	T Pop(int iTimeOut);

	/*
	* �ŵ���������
	* @param T * item ����
	*/
	int push(T item);

	/*
	* ��ʼ������
	* @param int QueSize ���г��ȣ����ܵ���10
	*/
	void init(int QueSize = 10);

	/*
	* ��ǰ���л�ѹ��
	*/
	int getSize();
protected:

	inline T mf_Pop();
private:
	T    * m_Items;     //item���б�
	int    m_QueSize;   //���д�С
	int    m_CurrentSize; //��ǰ�ڵ���
	int    m_first;     //��ǰ���ýڵ��
	int    m_end;       //��ǰ���нڵ��
	CEvent * m_event;   //�¼�
	//CThreadMutex m_mutex;
};

template<class T>
T CSafeQue<T>::Pop(int iTimeOut)
{
	T temp = NULL;
	//m_mutex.Acquire();
	temp = mf_Pop();
	//m_mutex.Release();
	
	//����Ϊ��
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
		//�Ѿ�Ϊ��
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
		//���Ѷ���
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
	//Ĭ����С10��
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

//===========================================���ж���==================================

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
	* �Ӷ�����ȡ����
	*/
	T * get();

	/*
	* �ŵ���������
	* @param T * item ����
	*/
	int free(T * item);

	/*
	* ��ʼ������
	* @param int QueSize ���г��ȣ����ܵ���10
	*/
	int init(int QueSize = 10);

	/*
	* ��ǰ���п�����
	*/
	int getSize()
	{
		return m_iBackCount;
	}

	void destroy();
private:
	T   ** m_Items;     //item���б�
	int    m_QueSize;   //���д�С
	int    m_iBackCount;//���ø���
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
