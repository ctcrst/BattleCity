//-----------------------------------------------------------------------------
// File: msgserv
// Desc: 提供线程安全的消息管理
// Auth: Lyp
// Date: 2003/11/30
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "..\gtdefine.h"

#include "..\debug\debug.h"
#include "..\mem\memmgr.h"
#include "msgserv.h"


//-----------------------------------------------------------------------------
// CMsgQueueTS construction / destruction
//-----------------------------------------------------------------------------
CMsgQueueTS::CMsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead)
{
	m_bEvent = bActiveEvent;
	m_bAddSizeAhead = bAddSizeAhead;
	
	m_nMsg = 0;
	m_hEvent = NULL;
	m_pQueueBegin = NULL;
	m_pQueueEnd = NULL;
	
	InitializeCriticalSection(&m_Lock);

	if( m_bEvent )
		m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);	// 自动激活
}

CMsgQueueTS::~CMsgQueueTS()
{
	MsgItem* pMsg = m_pQueueBegin; 
	while( m_pQueueBegin )
	{
		pMsg = m_pQueueBegin->pNext;
		TSMemMgr::Free(m_pQueueBegin->pData);
		TSMemMgr::Free(m_pQueueBegin);
		m_pQueueBegin = pMsg;
	}

	if( m_bEvent )
		CloseHandle(m_hEvent);

	DeleteCriticalSection(&m_Lock);
}


//-----------------------------------------------------------------------------
// 添加消息到队列尾，返回是否添加成功
//-----------------------------------------------------------------------------
BOOL CMsgQueueTS::AddMsg(BYTE* pMsg, DWORD dwSize)
{
	// 预先检查，以免进入critical section
	if( m_nMsg >= GT_MSG_MAX_MSG )
	{
		ASSERT(0);	// too much msg
		return FALSE;
	}

	EnterCriticalSection(&m_Lock);
	ASSERT(m_nMsg < GT_MSG_MAX_MSG);	// too much msg

	DWORD dwOriginSize = dwSize;
	if( m_bAddSizeAhead )
		dwSize += 4;

	// 申请新的单元
	MsgItem* pNewMsg = (MsgItem*)TSMemMgr::Alloc(sizeof(MsgItem));
	if( pNewMsg == NULL )
		FATAL_ERR("new unsuccessfully");

	// 初始化
	pNewMsg->dwDataSize = dwSize;
	pNewMsg->pData = NULL;
	pNewMsg->pNext = NULL;
	
	// 申请内容空间
	pNewMsg->pData = (BYTE*)TSMemMgr::Alloc(dwSize);
	if( pNewMsg->pData == NULL )
		FATAL_ERR("new unsuccessfully");

	// 拷贝内容
	if( m_bAddSizeAhead )
	{
		memcpy(pNewMsg->pData, &dwOriginSize, sizeof(dwOriginSize));
		memcpy(pNewMsg->pData+sizeof(dwOriginSize), pMsg, dwOriginSize);
	}
	else
	{
		memcpy(pNewMsg->pData, pMsg, dwSize);
	}
	
	if( m_pQueueBegin == NULL )	// 队列空
	{
		m_pQueueBegin = pNewMsg;
		m_pQueueEnd = pNewMsg;
	}
	else
	{
		m_pQueueEnd->pNext = pNewMsg;
		m_pQueueEnd = pNewMsg;
	}

	m_nMsg++;

	if( m_bEvent )	// 激活线程
		::SetEvent(m_hEvent);	

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}



//-----------------------------------------------------------------------------
// 从队列头取消息，返回消息长度，外边需传进空指针的地址，并负责释放内存
//-----------------------------------------------------------------------------
DWORD CMsgQueueTS::PeekMsg(BYTE** ppMsg)
{
	if( m_nMsg <= 0 )	// 预先检查，以免进入critical section
		return 0;	// no msg

	ASSERT(*ppMsg == NULL);	// 需要给一个空指针的地址
	DWORD dwSize = 0;
	
	EnterCriticalSection(&m_Lock);

	if( m_nMsg <= 0 )	// 内部再行检查
	{
		LeaveCriticalSection(&m_Lock);
		return 0;	// no msg
	}

	// 取出消息
	*ppMsg = m_pQueueBegin->pData;
	dwSize = m_pQueueBegin->dwDataSize;
	
	MsgItem* m_pTempMsg = m_pQueueBegin;
	m_pQueueBegin = m_pQueueBegin->pNext;

	TSMemMgr::Free(m_pTempMsg);
	m_nMsg--;

	LeaveCriticalSection(&m_Lock);
	return dwSize;
}




//-----------------------------------------------------------------------------
// CMsgList construction / destruction
//-----------------------------------------------------------------------------
CMsgList::CMsgList()
{
	m_nMsg = 0;
	m_pListBegin = NULL;
	m_pListEnd = NULL;
}

CMsgList::~CMsgList()
{
	MsgItem* pMsg = m_pListBegin; 
	while( m_pListBegin )
	{
		pMsg = m_pListBegin->pNext;
		MemMgr::Free(m_pListBegin->pData);
		MemMgr::Free(m_pListBegin);
		m_pListBegin = pMsg;
	}
}


//-----------------------------------------------------------------------------
// 添加消息到列表，返回是否添加成功
//-----------------------------------------------------------------------------
BOOL CMsgList::AddMsg(VOID* pMsg, DWORD dwSize, DWORD dwID)
{
	if( m_nMsg >= GT_MSG_MAX_MSG )
	{
		ASSERT(0);	// too much msg
		return FALSE;
	}

	// 申请新的单元
	MsgItem* pNewMsg = (MsgItem*)MemMgr::Alloc(sizeof(MsgItem));
	if( pNewMsg == NULL )
		FATAL_ERR("new unsuccessfully");

	// 初始化
	pNewMsg->dwID = dwID;
	pNewMsg->dwDataSize = dwSize;
	pNewMsg->pData = NULL;
	pNewMsg->pNext = NULL;
	
	// 申请内容空间
	pNewMsg->pData = (BYTE*)MemMgr::Alloc(dwSize);
	if( pNewMsg->pData == NULL )
		FATAL_ERR("new unsuccessfully");

	// 拷贝内容
	memcpy(pNewMsg->pData, pMsg, dwSize);
	
	if( m_pListBegin == NULL )	// 队列空
	{
		m_pListBegin = pNewMsg;
		m_pListEnd = pNewMsg;
	}
	else
	{
		m_pListEnd->pNext = pNewMsg;
		m_pListEnd = pNewMsg;
	}

	m_nMsg++;
	return TRUE;
}



//-----------------------------------------------------------------------------
// 根据ID取消息，返回消息长度，外边需传进空指针的地址，并负责释放内存
//-----------------------------------------------------------------------------
DWORD CMsgList::PeekMsg(BYTE** ppMsg, DWORD dwID)
{
	if( m_nMsg <= 0 )
		return 0;

	ASSERT(*ppMsg == NULL);	// 需要给一个空指针的地址
	DWORD dwSize = 0;

	// 循环搜索
	MsgItem* pMsg = m_pListBegin;
	MsgItem* pMsgPrev = NULL;
	while( pMsg )
	{
		if( pMsg->dwID == dwID )
			break;

		pMsgPrev = pMsg;
		pMsg = pMsg->pNext;
	}

	// 没有找到
	if( NULL == pMsg )
		return 0;


	// 取出消息
	*ppMsg = pMsg->pData;
	dwSize = pMsg->dwDataSize;
	
	if( pMsgPrev )
		pMsgPrev->pNext = pMsg->pNext;
	

	if( pMsg == m_pListBegin )
		m_pListBegin = pMsg->pNext;

	if( pMsg == m_pListEnd )
		m_pListEnd = pMsgPrev;

	MemMgr::Free(pMsg);
	m_nMsg--;

	return dwSize;
}



