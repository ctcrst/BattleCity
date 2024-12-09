//-----------------------------------------------------------------------------
// File: socket
// Desc: interface for win socket
// Auth: Lyp
// Date: 2003/11/17
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_WINSOCKET_H_
#define	_GT_WINSOCKET_H_
#pragma once

#include "..\util\sfpclass.h"
#include "socketdef.h"

class CMsgQueue;
//-----------------------------------------------------------------------------
// Server/Client win tcp/ip socket
//-----------------------------------------------------------------------------
class CWinTcp:public Trunk<CWinTcp>
{
public:
	BOOL Init(int nPort, int nMaxClient, BOOL bServer);
	VOID Destroy();

	// 如果是主机且ID=0,就发向所有的客户端
	// 如果是客户端且ID=0，则是发向主机
	VOID Send(BYTE* pMsg, DWORD dwMsgSize, DWORD dwReceiverID=0);

	// 如果是客户端，则一定是收到主机的信息
	BOOL Recv(BYTE** ppMsg, DWORD& dwMsgSize, DWORD& dwSenderID);


	INT  GetClientNum() { return m_Server.nClient; }
	BOOL Connected() { return m_Client.bConnected; }

	CWinTcp();
	~CWinTcp();


private:

	ServerData		m_Server;
	ClientData		m_Client;

	BOOL			m_bServer;


	CMsgQueueTS*	m_pSendMsgQueue;
	CMsgQueueTS*	m_pRecvMsgQueue;

	LANThread		m_Thread;

	
	BOOL InitServer();
	VOID DestroyServer();
	
	BOOL InitClient();
	VOID DestroyClient();
	BOOL SendToAllClient(void* pMsg);


	// thread function
	UINT ThreadAccept(LPVOID);	// 兼做 receive thread
	UINT ThreadSend(LPVOID);
	UINT ThreadConnect(LPVOID);	// 兼做 receive thread
	UINT ThreadBroadcast(LPVOID);	// 广播服务器位置
	UINT ThreadSearchServer(LPVOID);	// 寻找Server
	
};








#endif // _GT_WINSOCKET_H_