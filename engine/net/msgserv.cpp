//-----------------------------------------------------------------------------
// File: msgserv
// Desc: �ṩ�̰߳�ȫ����Ϣ����
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
		m_hEvent = ::CreateEvent(NULL, FALSE, TRUE, NULL);	// �Զ�����
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
// �����Ϣ������β�������Ƿ���ӳɹ�
//-----------------------------------------------------------------------------
BOOL CMsgQueueTS::AddMsg(BYTE* pMsg, DWORD dwSize)
{
	// Ԥ�ȼ�飬�������critical section
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

	// �����µĵ�Ԫ
	MsgItem* pNewMsg = (MsgItem*)TSMemMgr::Alloc(sizeof(MsgItem));
	if( pNewMsg == NULL )
		FATAL_ERR("new unsuccessfully");

	// ��ʼ��
	pNewMsg->dwDataSize = dwSize;
	pNewMsg->pData = NULL;
	pNewMsg->pNext = NULL;
	
	// �������ݿռ�
	pNewMsg->pData = (BYTE*)TSMemMgr::Alloc(dwSize);
	if( pNewMsg->pData == NULL )
		FATAL_ERR("new unsuccessfully");

	// ��������
	if( m_bAddSizeAhead )
	{
		memcpy(pNewMsg->pData, &dwOriginSize, sizeof(dwOriginSize));
		memcpy(pNewMsg->pData+sizeof(dwOriginSize), pMsg, dwOriginSize);
	}
	else
	{
		memcpy(pNewMsg->pData, pMsg, dwSize);
	}
	
	if( m_pQueueBegin == NULL )	// ���п�
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

	if( m_bEvent )	// �����߳�
		::SetEvent(m_hEvent);	

	LeaveCriticalSection(&m_Lock);
	return TRUE;
}



//-----------------------------------------------------------------------------
// �Ӷ���ͷȡ��Ϣ��������Ϣ���ȣ�����贫����ָ��ĵ�ַ���������ͷ��ڴ�
//-----------------------------------------------------------------------------
DWORD CMsgQueueTS::PeekMsg(BYTE** ppMsg)
{
	if( m_nMsg <= 0 )	// Ԥ�ȼ�飬�������critical section
		return 0;	// no msg

	ASSERT(*ppMsg == NULL);	// ��Ҫ��һ����ָ��ĵ�ַ
	DWORD dwSize = 0;
	
	EnterCriticalSection(&m_Lock);

	if( m_nMsg <= 0 )	// �ڲ����м��
	{
		LeaveCriticalSection(&m_Lock);
		return 0;	// no msg
	}

	// ȡ����Ϣ
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
// �����Ϣ���б������Ƿ���ӳɹ�
//-----------------------------------------------------------------------------
BOOL CMsgList::AddMsg(VOID* pMsg, DWORD dwSize, DWORD dwID)
{
	if( m_nMsg >= GT_MSG_MAX_MSG )
	{
		ASSERT(0);	// too much msg
		return FALSE;
	}

	// �����µĵ�Ԫ
	MsgItem* pNewMsg = (MsgItem*)MemMgr::Alloc(sizeof(MsgItem));
	if( pNewMsg == NULL )
		FATAL_ERR("new unsuccessfully");

	// ��ʼ��
	pNewMsg->dwID = dwID;
	pNewMsg->dwDataSize = dwSize;
	pNewMsg->pData = NULL;
	pNewMsg->pNext = NULL;
	
	// �������ݿռ�
	pNewMsg->pData = (BYTE*)MemMgr::Alloc(dwSize);
	if( pNewMsg->pData == NULL )
		FATAL_ERR("new unsuccessfully");

	// ��������
	memcpy(pNewMsg->pData, pMsg, dwSize);
	
	if( m_pListBegin == NULL )	// ���п�
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
// ����IDȡ��Ϣ��������Ϣ���ȣ�����贫����ָ��ĵ�ַ���������ͷ��ڴ�
//-----------------------------------------------------------------------------
DWORD CMsgList::PeekMsg(BYTE** ppMsg, DWORD dwID)
{
	if( m_nMsg <= 0 )
		return 0;

	ASSERT(*ppMsg == NULL);	// ��Ҫ��һ����ָ��ĵ�ַ
	DWORD dwSize = 0;

	// ѭ������
	MsgItem* pMsg = m_pListBegin;
	MsgItem* pMsgPrev = NULL;
	while( pMsg )
	{
		if( pMsg->dwID == dwID )
			break;

		pMsgPrev = pMsg;
		pMsg = pMsg->pNext;
	}

	// û���ҵ�
	if( NULL == pMsg )
		return 0;


	// ȡ����Ϣ
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



