#include "qmsg.h"


CQMsgList::CQMsgList()
{
	m_lpMsgHead = NULL;
	m_lpMsgTail = NULL;
	m_dwCount	= 0;
	m_dwMaxCount= 0;

}

CQMsgList::~CQMsgList()
{
	::FBASE2_NAMESPACE::CAutoMutex Lock(&m_Lock);
	QMessage *lpItem = m_lpMsgHead;
	QMessage *lpOldItem = NULL;

	while (lpItem)
	{
		lpOldItem = lpItem->m_lpNext;
		delete lpItem;
		lpItem = lpOldItem;
	}
}

int CQMsgList::PostMsg(QMessage *lpMsg)
{
	if (!lpMsg)
		return -1;

	lpMsg->m_lpNext = NULL;
	
	m_Lock.Acquire();
	
	if (m_lpMsgHead == NULL)					// 空队列
	{
		m_lpMsgTail = lpMsg;
		m_lpMsgHead = lpMsg;
	}
	else										// 加入尾部
	{		
		m_lpMsgTail->m_lpNext = lpMsg;
		m_lpMsgTail = lpMsg;
	}
	m_dwCount++;
	if (m_dwMaxCount < m_dwCount)
		m_dwMaxCount = m_dwCount;

	if (m_dwCount <= 1)
		m_Event.Set();
	
	m_Lock.Release();

	return 0;
}

QMessage *CQMsgList::GetMsg(uint32 dwTimeOut)
{
	QMessage *lpMsg;
	
	m_Lock.Acquire();

	if (!m_lpMsgHead)
	{
		m_Lock.Release();
		//m_Event.Reset();
		m_Event.Wait(dwTimeOut);
		
		m_Lock.Acquire();
	}

	lpMsg = m_lpMsgHead;

	if (m_lpMsgHead != NULL)
	{
		m_lpMsgHead = m_lpMsgHead->m_lpNext;
		if (!m_lpMsgHead)
		{
//			m_Event.Reset();
			m_lpMsgTail = NULL;
		}
		m_dwCount --;
	}
	
	if ( lpMsg )
	{
		lpMsg->m_lpNext = NULL;
	}
	
	m_Lock.Release();
	
	return lpMsg;
}

void CQMsgList::GetCount(uint32 &dwCount, uint32 &dwMaxCount)
{
	::FBASE2_NAMESPACE::CAutoMutex Lock(&m_Lock);
	
	dwCount		= m_dwCount;
	dwMaxCount	= m_dwMaxCount;
}





////////////////////////	CMsgPool的实现		////////////////////////////////////////

CMsgPool::CMsgPool()
{
	m_dwCount	= 0;
	m_lpHead	= NULL;
	m_lpTail	= NULL;

	mbufAlloc();
}

CMsgPool::~CMsgPool()
{
	QMessage *lpItem, *lpNext;
	
	::FBASE2_NAMESPACE::CAutoMutex Lock(&m_Lock);
	lpItem = m_lpHead;
	while (lpItem)
	{
		lpNext = lpItem->m_lpNext;
		delete lpItem;
		lpItem = lpNext;
	}
}

int CMsgPool::Push(QMessage *lpNode)
{
	assert(lpNode);
	if (!lpNode)
		return -1;

	lpNode->m_lpNext = NULL;
	lpNode->m_dwMsg = 0;
//	lpNode->m_lParam = 0;
	lpNode->m_wParam = NULL;
	lpNode->m_lPCnnt=NULL;

	m_Lock.Acquire();
		
	if (m_lpHead == NULL)
	{
		m_lpHead = lpNode;
		m_lpTail = lpNode;
	}
	else
	{
		m_lpTail->m_lpNext = lpNode;
		m_lpTail = lpNode;
	}
	m_dwCount++;

	m_Lock.Release();
	
	return 0;
}

void CMsgPool::mbufAlloc()
{
	
	::FBASE2_NAMESPACE::CAutoMutex Lock(&m_Lock);
	
	QMessage *lpNode;
	for (int i = 0; i < MSG_SIZE_STEP; i++)
	{
		lpNode = FBASE_NEW QMessage;
		if (!lpNode)
			continue;

		lpNode->m_lpOwner = this;
		
		Push(lpNode);
	}
}

uint32 CMsgPool::GetCount()
{
	uint32 dwCount;

	::FBASE2_NAMESPACE::CAutoMutex Lock(&m_Lock);
	
	dwCount = m_dwCount;
	
	return dwCount;
}

QMessage *CMsgPool::Pop()
{
	QMessage *lpNode;
	
	m_Lock.Acquire();
	
	lpNode = m_lpHead;
	if (m_lpHead != NULL)
	{
		m_lpHead = m_lpHead->m_lpNext;
		if (!m_lpHead)
			m_lpTail = NULL;
		m_dwCount--;
		
		if (m_dwCount < 5)
			mbufAlloc();

		lpNode->m_lpNext = NULL;
	}

	if (lpNode)
	{
		lpNode->m_lpNext = NULL;
	}

	m_Lock.Release();
	
	return lpNode;
}



