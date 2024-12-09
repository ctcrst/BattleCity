//-----------------------------------------------------------------------------
// File: msgserv
// Desc: 提供线程安全的消息服务
// Auth: Lyp
// Date: 2003/11/30
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_MSG_SERVICE_H_
#define	_GT_MSG_SERVICE_H_
#pragma once

#define GT_MSG_MAX_MSG		4096


//-----------------------------------------------------------------------------
// CMsgQueueTS: 提供线程安全的先进先出信息队列管理
// 在取消息的时候使用内部分配的内存
// 外边需传进一个空指针的地址，并且负责释放内存
// 当有消息加入时，可以激活一个Event，适合网络多线程收发
// 可以在信息前面加入信息长度，适合网络传输
// 内存分配使用CMemMgr
//-----------------------------------------------------------------------------
class CMsgQueueTS
{
public:

	// bActiveEvent: 当有消息加入时，是否激活一个内建event
	// bAddSizeAhead: 是否在信息头部前加入信息长度，主要为网络通信准备
	CMsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead);

	// 添加消息到队列尾，返回是否添加成功
	BOOL	AddMsg(BYTE* pMsg, DWORD dwSize);

	// 从队列头取消息，返回消息长度，外边需传进空指针的地址，并负责释放内存
	DWORD	PeekMsg(BYTE** ppMsg);

	// 取得事件，当队列中加入消息时，此事件被激活
	HANDLE	GetEvent() { return m_hEvent; }

	~CMsgQueueTS();

private:
	struct MsgItem
	{
		DWORD			dwDataSize;
		BYTE*			pData;
		MsgItem*		pNext;
	};

	CRITICAL_SECTION	m_Lock;
	HANDLE				m_hEvent;
	INT					m_nMsg;
	BOOL				m_bEvent;
	BOOL				m_bAddSizeAhead;	

	MsgItem*			m_pQueueBegin;
	MsgItem*			m_pQueueEnd;
};





//-----------------------------------------------------------------------------
// CMsgList: 提供线程安全的信息列表管理，根据ID存取信息，不考虑有重复ID的情况
// 在取消息的时候使用内部分配的内存
// 外边需传进一个空指针的地址，并且负责释放内存
// 内存分配使用CMemMgr
//-----------------------------------------------------------------------------
class CMsgList
{
public:

	// 添加消息到列表，返回是否添加成功
	BOOL	AddMsg(VOID* pMsg, DWORD dwSize, DWORD dwID);

	// 根据ID取消息，返回消息长度，外边需传进空指针的地址，并负责释放内存
	DWORD	PeekMsg(BYTE** ppMsg, DWORD dwID);

	// 返回列表中的消息个数
	INT		GetMsgNumInList() { return m_nMsg; }
	
	CMsgList();
	~CMsgList();

private:
	struct MsgItem
	{
		DWORD			dwID;
		DWORD			dwDataSize;
		BYTE*			pData;
		MsgItem*		pNext;
	};

	INT					m_nMsg;
	MsgItem*			m_pListBegin;
	MsgItem*			m_pListEnd;
};



#endif // _GT_MSG_SERVICE_H_