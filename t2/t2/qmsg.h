/**
	maoyinjie 2009/06/25		
**/
/// RUDP里用来给上层调用者提供消息


#ifndef __F2_COM_QMSG_H__
#define __F2_COM_QMSG_H__

#include <os/inttypes.h>
#include <os/thread_mutex.h>
#include <os/event.h>


/// 消息类型定义
#define		MSG_ACCEPT				0x0001
#define		MSG_DISCONNECT			0x0002
#define		MSG_CONNECT				0x0003
#define		MSG_RECV_DATA			0x0004
#define		MSG_ERROR				0x0005
#define		MSG_STATE				0x0006		// 收到一个纯ACK包, 即表示下层通讯空闲


/// MSG_ERROR时, m_wParam里的第一个字节是指向MsgInfo, m_lParam表示超时类型
#define		ERRORNO_TIMEOUT			0x0101

/// MSG_STATE时, m_wParam里的值, 非指针
#define		STATE_IDLE				0x0201
#define		STATE_RESEND			0x0202
#define		STATE_NONEBLOCK			0x0203		// 阻塞解除


// #include <os/inttypes.h>
// #include <os/thread_mutex.h>

class CQMSGList;
class CMsgPool;


struct MsgInfo
{
	uint32		dwErrorNO;
	uint32		dwConnectID;
	sockaddr	from;

};


// 当m_dwMsg == MSG_ACCEPT时, m_wParam是MsgInfo的结构指针, dwErrorNO表示IsActive(1代表true)
// 当m_dwMsg == MSG_DISCONNECT时, m_wParam是MsgInfo的结构指针, dwErrorNO无意义
// 当m_dwMsg == MSG_RECV_DATA时, m_wParam为CMBufNode的指针, m_lParam表示dwConnectID的指针
// 当m_dwMsg == MSG_ERROR时, m_wParam是一个结构MsgInfo指针
// 当m_dwMsg == MSG_STATE时, m_wParam表示此状态类型, m_lParam是Connect ID
struct QMessage
{
	uint32		m_dwMsg;			// 根据消息类型决定下面两个参数的类型
	void		*m_wParam;			// 消息体
//	uint32		m_lParam;			// 预留
	void*		m_lPCnnt;			// 连接

	QMessage	*m_lpNext;
	CMsgPool	*m_lpOwner;

	QMessage()
	{
		memset(this, 0, sizeof(QMessage));
	}
	inline void FreeBack();
};

class CQMsgListSet;
class CQMsgList
{
private:
	friend class CQMsgListSet;
	CQMsgList();
	~CQMsgList();
	
	int PostMsg(QMessage *lpMsg);
	QMessage *GetMsg(uint32 dwTimeOut);

	void GetCount(uint32 &dwCount, uint32 &dwMaxCount);

private:
	QMessage	*m_lpMsgHead;
	QMessage	*m_lpMsgTail;
	uint32		m_dwCount;
	uint32		m_dwMaxCount;
	//	保护的锁
	FBASE2_NAMESPACE::CThreadMutex	m_Lock;
	FBASE2_NAMESPACE::CEvent		m_Event;
};






  
class CQMsgListSet
{
public:
	CQMsgListSet()
	{
		m_lpMsgList=NULL;
		m_iLists=0;
		m_iCurrentIndex=0;
	}
	~CQMsgListSet()
	{
		delete[] m_lpMsgList;
	}
	bool InitListSet(int iLists)
	{
		m_iCurrentIndex=0;
		m_lpMsgList=FBASE_NEW CQMsgList[iLists];
		if(m_lpMsgList)
		{
			m_iLists=iLists;
		}
		return (m_lpMsgList!=NULL);
	}
	//	maoyinjie 2009/06/25		
//	int PostMsg(QMessage *lpMsg)
	int PostMsg(QMessage *lpMsg, int iList=-1)
	{
		int iIndex;
		if(iList<0 || iList>=m_iLists)
		{
			iIndex=m_iCurrentIndex;
			if(iIndex>=m_iLists)
				iIndex=0;
		}
		else
		{
			iIndex=iList;
		}
		m_lpMsgList[iIndex].PostMsg(lpMsg);
		m_iCurrentIndex++;
		if(m_iCurrentIndex>=m_iLists)
			m_iCurrentIndex=0;
		return 0;
	}
	QMessage *GetMsg(uint32 dwTimeOut, int iListIndex)
	{
		if(iListIndex>=m_iLists)
			return NULL;
		return m_lpMsgList[iListIndex].GetMsg(dwTimeOut);
	}

	void GetCount(uint32 &dwCount, uint32 &dwMaxCount, int iListIndex)
	{
		dwCount=0;
		dwMaxCount=0;
		if(iListIndex>=m_iLists)
			return ;
		m_lpMsgList[iListIndex].GetCount(dwCount, dwMaxCount);
		return;
	}

private:
	CQMsgList*  m_lpMsgList;
	int			m_iLists;
	int			m_iCurrentIndex;
};


#define MSG_SIZE_STEP	10

class CMsgPoolSet;
class CMsgPool
{
	friend class CMsgPoolSet;
private:
	CMsgPool();
	~CMsgPool();
	
	QMessage *Pop();
	
	uint32 GetCount();
	
private:
	void mbufAlloc(void);
	int Push(QMessage *lpNode);
	
private:
	QMessage	*m_lpHead;
	QMessage	*m_lpTail;
	int			m_dwCount;
	
	::FBASE2_NAMESPACE::CThreadMutex	m_Lock;
	
	friend struct QMessage;
	
};


inline void QMessage::FreeBack()
{
	m_lpOwner->Push(this);
}


class CMsgPoolSet
{
public:
	CMsgPoolSet()
	{
		m_iCurrentIndex=0;
	}
	~CMsgPoolSet()
	{
	}
	
	QMessage *Pop()
	{
		QMessage* lpMsg=NULL;
		int iIndex=m_iCurrentIndex;
		if(iIndex>=6)
			iIndex=0;
		lpMsg=m_MsgPool[iIndex].Pop();
		m_iCurrentIndex++;
		if(m_iCurrentIndex>=6)
			m_iCurrentIndex=0;
		return lpMsg;
	}
	
private:
	CMsgPool m_MsgPool[6];
	int		 m_iCurrentIndex;
};


#endif


