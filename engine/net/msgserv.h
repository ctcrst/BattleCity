//-----------------------------------------------------------------------------
// File: msgserv
// Desc: �ṩ�̰߳�ȫ����Ϣ����
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
// CMsgQueueTS: �ṩ�̰߳�ȫ���Ƚ��ȳ���Ϣ���й���
// ��ȡ��Ϣ��ʱ��ʹ���ڲ�������ڴ�
// ����贫��һ����ָ��ĵ�ַ�����Ҹ����ͷ��ڴ�
// ������Ϣ����ʱ�����Լ���һ��Event���ʺ�������߳��շ�
// ��������Ϣǰ�������Ϣ���ȣ��ʺ����紫��
// �ڴ����ʹ��CMemMgr
//-----------------------------------------------------------------------------
class CMsgQueueTS
{
public:

	// bActiveEvent: ������Ϣ����ʱ���Ƿ񼤻�һ���ڽ�event
	// bAddSizeAhead: �Ƿ�����Ϣͷ��ǰ������Ϣ���ȣ���ҪΪ����ͨ��׼��
	CMsgQueueTS(BOOL bActiveEvent, BOOL bAddSizeAhead);

	// �����Ϣ������β�������Ƿ���ӳɹ�
	BOOL	AddMsg(BYTE* pMsg, DWORD dwSize);

	// �Ӷ���ͷȡ��Ϣ��������Ϣ���ȣ�����贫����ָ��ĵ�ַ���������ͷ��ڴ�
	DWORD	PeekMsg(BYTE** ppMsg);

	// ȡ���¼����������м�����Ϣʱ�����¼�������
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
// CMsgList: �ṩ�̰߳�ȫ����Ϣ�б��������ID��ȡ��Ϣ�����������ظ�ID�����
// ��ȡ��Ϣ��ʱ��ʹ���ڲ�������ڴ�
// ����贫��һ����ָ��ĵ�ַ�����Ҹ����ͷ��ڴ�
// �ڴ����ʹ��CMemMgr
//-----------------------------------------------------------------------------
class CMsgList
{
public:

	// �����Ϣ���б������Ƿ���ӳɹ�
	BOOL	AddMsg(VOID* pMsg, DWORD dwSize, DWORD dwID);

	// ����IDȡ��Ϣ��������Ϣ���ȣ�����贫����ָ��ĵ�ַ���������ͷ��ڴ�
	DWORD	PeekMsg(BYTE** ppMsg, DWORD dwID);

	// �����б��е���Ϣ����
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