//-----------------------------------------------------------------------------
// File: socket
// Desc: interface for win socket
// Auth: Lyp
// Date: 2003/11/17
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "..\debug\debug.h"
#include "..\util\util.h"
#include "..\util\SFPClass.h"
#include "..\mem\MemMgr.h"

#include <process.h>	// _beginthreadex
#include "msgserv.h"
#include "socket.h"


//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CWinTcp::CWinTcp():Trunk<CWinTcp>(this)
{
}

CWinTcp::~CWinTcp()
{
}


//-----------------------------------------------------------------------------
// init
//-----------------------------------------------------------------------------
BOOL CWinTcp::Init(int nPort, int nMaxClient, BOOL bServer)
{
	Clear();
	m_bServer = bServer;

	m_Server.Clear();
	m_Client.Clear();
	m_Server.nPort = nPort;
	m_Client.nPort = nPort;

	m_pSendMsgQueue = new CMsgQueueTS(TRUE, TRUE);
	m_pRecvMsgQueue = new CMsgQueueTS(FALSE, FALSE);
	
	m_Thread.Clear();

	if( m_bServer )
	{
		m_Server.nMaxClient = nMaxClient;
		InitServer();
	}

	return InitClient();
}


void CWinTcp::Destroy()
{

	m_Thread.bTerminateAccept = TRUE;
	m_Thread.bTerminateSend = TRUE;
	m_Thread.bTerminateConnect = TRUE;

	if( m_bServer )
		DestroyServer();

	DestroyClient();

	while( m_Thread.nThread > 0 )
		Sleep(50);

	for(int n=0; n<GT_MAX_CLIENT; n++)
		SAFE_CLOSE_HANDLE(m_Thread.hAccept[n]);

	SAFE_CLOSE_HANDLE(m_Thread.hBroadcast);
	SAFE_CLOSE_HANDLE(m_Thread.hSend);
	SAFE_CLOSE_HANDLE(m_Thread.hSend2);
	SAFE_CLOSE_HANDLE(m_Thread.hConnect);
	SAFE_CLOSE_HANDLE(m_Thread.hSearchServer);


	delete m_pSendMsgQueue;
	delete m_pRecvMsgQueue;
}


//-----------------------------------------------------------------------------
// init server
//-----------------------------------------------------------------------------
BOOL CWinTcp::InitServer()
{
	// �趨��ַ
	m_Server.Address.sin_addr.s_addr = htonl(INADDR_ANY);
	m_Server.Address.sin_family = AF_INET;
	m_Server.Address.sin_port = m_Server.nPort;
	int nAddlen = sizeof(m_Server.Address);

	// ���� socket
	m_Server.Sock = socket(AF_INET, SOCK_STREAM, 0);

	// m_Server.sock ʹ�÷�������ʽ
	DWORD dwCmd = 1;
	INT nResult = ioctlsocket( m_Server.Sock, FIONBIO, &dwCmd );
	if( SOCKET_ERROR == nResult )
	{
		FATAL_ERR("set nonblocking mode failed in init server");
		closesocket( m_Server.Sock );
		return FALSE;
	}

	// ʹ����ر�
	linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0;
	setsockopt(m_Server.Sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));


	// ��
	if( bind(m_Server.Sock, (sockaddr*)&m_Server.Address, nAddlen) )
	{
		FATAL_ERR("bind error");
		return FALSE;
	}
	else
	{
		// ��ʼ����
		listen( m_Server.Sock, 5 );

		// �����߳� Accept/Send
		m_Thread.hAccept[m_Thread.nAcceptThreadNum] = 
			(HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadAccept), NULL, 0, NULL);
		
		m_Thread.hSend = 
			(HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadSend), NULL, 0, NULL);
		
		m_Server.bBroadcast = TRUE;
		m_Thread.hBroadcast = 
			(HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadBroadcast), NULL, 0, NULL);
	}

	return TRUE;
}


//-----------------------------------------------------------------------------
// destroy server
//-----------------------------------------------------------------------------
void CWinTcp::DestroyServer()
{
	closesocket( m_Server.Sock );
	m_Server.bBroadcast = FALSE;
	m_Server.nClient = 0;
}

//-----------------------------------------------------------------------------
// init client
//-----------------------------------------------------------------------------
BOOL CWinTcp::InitClient()
{
	//����socket
	m_Client.Sock = socket(AF_INET, SOCK_STREAM, 0);	

	// ʹ�÷�������ʽ
	DWORD dwCmd = 1;
	INT nResult = ioctlsocket( m_Client.Sock, FIONBIO, &dwCmd );
	if( SOCKET_ERROR == nResult )
	{
		FATAL_ERR("set nonblocking mode failed in init client");
		closesocket( m_Client.Sock );
		return FALSE;
	}

	// ʹ����ر�
	linger lin;
	lin.l_onoff = 1;
	lin.l_linger = 0;
	setsockopt(m_Client.Sock, SOL_SOCKET, SO_LINGER, (char*)&lin, sizeof(lin));

	// �����߳� connect/send
	m_Thread.hConnect = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadConnect), NULL, 0, NULL);
	m_Thread.hSend2 = (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadSend), NULL, 0, NULL);
	
	// �����������߳�
	m_Client.bSerchServer = TRUE;
	m_Thread.hSearchServer = 
		(HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadSearchServer), NULL, 0, NULL);
			
	return TRUE;
}

//-----------------------------------------------------------------------------
// destroy client
//-----------------------------------------------------------------------------
void CWinTcp::DestroyClient()
{
	closesocket( m_Client.Sock );
	m_Client.bSerchServer = FALSE;
	m_Client.bConnected = FALSE;
}



//-----------------------------------------------------------------------------
// ����������Ϣ
//-----------------------------------------------------------------------------
VOID CWinTcp::Send(BYTE* pMsg, DWORD dwMsgSize, DWORD dwClientID)
{
	// �ڷ�����Ϣǰ�����dwClientID
	BYTE* pNew = (BYTE*)TSMemMgr::Alloc(dwMsgSize+4);
	*(DWORD*)pNew = dwClientID;
	memcpy((BYTE*)pNew+4, pMsg, dwMsgSize);

	// ��������Ϣ��ӵ�������Ϣ����
	m_pSendMsgQueue->AddMsg(pNew, dwMsgSize+4);

	TSMemMgr::Free(pNew);
}



//-----------------------------------------------------------------------------
// ����������Ϣ,����FALSE˵��û����Ϣ
//-----------------------------------------------------------------------------
BOOL CWinTcp::Recv(BYTE** ppMsg, DWORD& dwMsgSize, DWORD& dwSenderID)
{
	ASSERT(*ppMsg == NULL);

	BYTE* pRecv = NULL;
	// �ӽ��ܻ��������Ϣ,������ش�СΪ��,˵��û����Ϣ
	dwMsgSize = m_pRecvMsgQueue->PeekMsg(&pRecv);
	if( dwMsgSize == 0 )
		return FALSE;

	dwSenderID = *(DWORD*)pRecv;
	dwMsgSize-=4;
	*ppMsg = (BYTE*)TSMemMgr::Alloc(dwMsgSize);
	memcpy( *ppMsg, pRecv+4, dwMsgSize );

	TSMemMgr::Free(pRecv);
	return TRUE;

}



//-----------------------------------------------------------------------------
// thread accept and receive
//-----------------------------------------------------------------------------
UINT CWinTcp::ThreadAccept(LPVOID)
{
	m_Thread.nThread++;

	DWORD dwCmd = 1;
	INT nIndex = m_Server.GetFreeClientSock();
	INT nAddlen = sizeof(m_Server.Address);
	INT nResult = 0;
	DWORD dwMsgSize = 0;

	m_Server.SockClient[nIndex] = INVALID_SOCKET;

	if( nIndex < 0 )	// no more client space in server
		goto __thread_accept_end;

	fd_set fdread;

	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_LAN_BLOCK_TIME;
	
	while( !m_Thread.bTerminateAccept )
	{
		// "select" will change fdread, so this must put in the loop
		FD_ZERO(&fdread);
		FD_SET(m_Server.Sock, &fdread);

		if( select(0, &fdread, NULL, NULL, &block_time) == 1 )
		{
			m_Server.SockClient[nIndex] 
				= accept( m_Server.Sock, (sockaddr*)&(m_Server.Address), &nAddlen);

			if( m_Server.SockClient[nIndex] != INVALID_SOCKET )
				break;
		}
	}
	
	if( m_Thread.bTerminateAccept )
		goto __thread_accept_end;
	
	// ��¼�û�ID;
	m_Server.dwClientID[nIndex] = Util::GetFreeID();

	
	// ʹ�÷�������ʽ
	dwCmd = 1;
	nResult = ioctlsocket( m_Server.SockClient[nIndex], FIONBIO, &dwCmd );
	if( SOCKET_ERROR == nResult )
	{
		FATAL_ERR("set nonblocking mode failed in thread accept");
		goto __thread_accept_end;
	}

	m_Server.nClient++;
	
	if( m_Server.nClient < m_Server.nMaxClient )	// ������һ�߳� Accept
		m_Thread.hAccept[++m_Thread.nAcceptThreadNum] 
			= (HANDLE)_beginthreadex(NULL, 0, (THREADPROC)FP1(&CWinTcp::ThreadAccept), NULL, 0, NULL);
	
	while( !m_Thread.bTerminateAccept )
	{
		//ѭ����������

		FD_ZERO(&fdread);
		FD_SET(m_Server.SockClient[nIndex], &fdread);

		INT nSock = select(0, &fdread, NULL, NULL, &block_time);

		if( nSock == 1 )
		{
			// ��ȡ�����ݵĴ�С
			nResult = recv(m_Server.SockClient[nIndex], (char*)&dwMsgSize, 4, 0);

			// һ��û����ȫ����4Byte,ѭ����������
			if( nResult > 0 && nResult < 4 )
			{
				DWORD dwTotal = (DWORD)nResult;
				char* pTemp1 = ((char*)&dwMsgSize) + dwTotal;
				while( dwTotal < 4 )	
				{
					FD_ZERO(&fdread);
					FD_SET(m_Server.SockClient[nIndex], &fdread);
					if( select(0, &fdread, NULL, NULL, &block_time) == 1 )
					{
						nResult = recv(m_Server.SockClient[nIndex], pTemp1, 4-dwTotal, 0);
						if( nResult == SOCKET_ERROR )
							break;
						
						if( m_Thread.bTerminateAccept )
							goto __thread_accept_end;
						
						dwTotal += (DWORD)nResult;
						pTemp1 = ((char*)&dwMsgSize) + dwTotal;
					}
				} 
			}

			if( nResult != SOCKET_ERROR )
			{
				char* pTemp = (char*)TSMemMgr::Alloc(dwMsgSize);
				nResult = recv(m_Server.SockClient[nIndex], pTemp, dwMsgSize, 0);
				
				// // ����Ϣ��ǰ��Ľ����߱�Ϊ������
				*(DWORD*)pTemp = m_Server.dwClientID[nIndex];	
			
				if( nResult == (INT)dwMsgSize )
					m_pRecvMsgQueue->AddMsg((BYTE*)pTemp, dwMsgSize);
				else
				{
					// һ��û����ȫ����,ѭ����������
					DWORD dwTotal = (DWORD)nResult;
					char* pTemp1 = pTemp + dwTotal;
					while( dwTotal < dwMsgSize )	
					{
						FD_ZERO(&fdread);
						FD_SET(m_Server.SockClient[nIndex], &fdread);
						if( select(0, &fdread, NULL, NULL, &block_time) == 1 )
						{
							nResult = recv(m_Server.SockClient[nIndex], pTemp1, dwMsgSize-dwTotal, 0);
							if( nResult == SOCKET_ERROR )
							{
								FATAL_ERR("got bad data in thread connect");
							}
							
							dwTotal += (DWORD)nResult;
							pTemp1 = pTemp + dwTotal;
						}
					} 

					// ����Ϣ��ǰ��Ľ����߱�Ϊ������
					*(DWORD*)pTemp = m_Server.dwClientID[nIndex];
					m_pRecvMsgQueue->AddMsg((BYTE*)pTemp, dwMsgSize);
				}

				TSMemMgr::Free(pTemp);
			}
			else
			{
				if( m_Thread.bTerminateAccept )
					goto __thread_accept_end;

				if( WSAGetLastError() == WSAECONNRESET )
					goto __thread_accept_end;

				// ����������
				FATAL_ERR("got bad data in thread accept");
				goto __thread_accept_end;
			}
		}
	}
		

__thread_accept_end:

	closesocket( m_Server.SockClient[nIndex] );
	m_Thread.nThread--;
	return 0;
}



//-----------------------------------------------------------------------------
// thread send
//-----------------------------------------------------------------------------
UINT CWinTcp::ThreadSend(LPVOID)
{
	m_Thread.nThread++;
	BYTE* pTemp = NULL;
	DWORD dwMsgSize = 0;
	DWORD dwReceiverID = 0;

	while( !m_Thread.bTerminateSend )
	{
		for(;;)
		{
			dwMsgSize = m_pSendMsgQueue->PeekMsg(&pTemp);
			if( dwMsgSize == 0 )
				break;	// ����Ϊֹ
			
			// ��Ϣ��ǰ4Byte��ʾ���ݳ���
			// ������4ByteΪ��Ҫ������ID
			dwReceiverID = *((DWORD*)pTemp+1);
			if( m_bServer )
			{
				for( INT n=0; n<GT_MAX_CLIENT; n++ )
				{
					if( (m_Server.SockClient[n] != NULL)
						&&( dwReceiverID == 0 || m_Server.dwClientID[n] == dwReceiverID ) )
						send(m_Server.SockClient[n], (char*)pTemp, dwMsgSize, 0);
				}
			}
			else
			{
				send(m_Client.Sock, (char*)pTemp, dwMsgSize, 0);
			}

			TSMemMgr::Free(pTemp);
			pTemp = NULL;
		}

		WaitForSingleObject(m_pSendMsgQueue->GetEvent(), 500);
	}

	m_Thread.nThread--;
	return 0;
}


//-----------------------------------------------------------------------------
// thread connect and receive
//-----------------------------------------------------------------------------
UINT CWinTcp::ThreadConnect(LPVOID)
{
	m_Thread.nThread++;

	while( m_Client.GetServerNum() <= 0 )
		Sleep(500);	// �ȴ���Ѱ�����Ľ��
		
	memcpy(&m_Client.Address.sin_addr.s_addr, 
		&m_Client.AddressAllServer[0].sin_addr.s_addr, 
		sizeof(m_Client.Address.sin_addr.s_addr) );
	
	m_Client.Address.sin_family = AF_INET;
	m_Client.Address.sin_port = m_Client.nPort;
	
	INT nResult = 0;
	DWORD dwMsgSize = 0;


	fd_set fdwrite;
	timeval block_time;	// ��������ʱ��
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_LAN_BLOCK_TIME;
	
	// ���ӵ�������

	// For connection-oriented, nonblocking sockets, it is often not possible 
	// to complete the connection immediately. In such a case, "connect"
	// returns the error WSAEWOULDBLOCK. However, the operation proceeds.
	nResult = connect(m_Client.Sock, (sockaddr*)&(m_Client.Address),
		sizeof(m_Client.Address));
	
	if( nResult == SOCKET_ERROR )
	{	
		if( WSAGetLastError() == WSAEWOULDBLOCK )
		{
			INT nTest = 0;
			while( nTest < 16 )// retry 16 times
			{
				FD_ZERO(&fdwrite);
				FD_SET(m_Client.Sock, &fdwrite);

				if( 1 == select(0, NULL, &fdwrite, NULL, &block_time) )
					goto __connected;

				nTest++;
				
				if( m_Thread.bTerminateConnect )
					goto __thread_connect_end;
			}
		}
			
		FATAL_ERR("connect failed, check your firewall");
		goto __thread_connect_end;
	}
	
__connected:
		
	if( m_Thread.bTerminateConnect )
		goto __thread_connect_end;

	m_Client.bConnected = TRUE;

	fd_set fdread;

	// ѭ���������
	while( !m_Thread.bTerminateConnect )
	{
		FD_ZERO(&fdread);
		FD_SET(m_Client.Sock, &fdread);

		if( select(0, &fdread, NULL, NULL, &block_time) == 1 )
		{
			// ��ȡ�����ݵĴ�С
			nResult = recv(m_Client.Sock, (char*)&dwMsgSize, 4, 0);

			// һ��û����ȫ����4Byte,ѭ����������
			if( nResult > 0 && nResult < 4 )
			{
				DWORD dwTotal = (DWORD)nResult;
				char* pTemp1 = ((char*)&dwMsgSize) + dwTotal;
				while( dwTotal < 4 )	
				{
					FD_ZERO(&fdread);
					FD_SET(m_Client.Sock, &fdread);
					if( select(0, &fdread, NULL, NULL, &block_time) == 1 )
					{
						nResult = recv(m_Client.Sock, pTemp1, 4-dwTotal, 0);
						if( nResult == SOCKET_ERROR )
							break;
						
						if( m_Thread.bTerminateConnect )
							goto __thread_connect_end;
						
						dwTotal += (DWORD)nResult;
						pTemp1 = ((char*)&dwMsgSize) + dwTotal;
					}
				} 
			}

			if( nResult != SOCKET_ERROR )
			{
				char* pTemp = (char*)TSMemMgr::Alloc(dwMsgSize);
				nResult = recv(m_Client.Sock, pTemp, dwMsgSize, 0);
				*(DWORD*)pTemp = 0;	// ����Ϣ��ǰ��Ľ����߱�Ϊ������
				
				if( nResult == (INT)dwMsgSize )
					m_pRecvMsgQueue->AddMsg((BYTE*)pTemp, dwMsgSize);
				else
				{
					// һ��û����ȫ����,ѭ����������
					DWORD dwTotal = (DWORD)nResult;
					char* pTemp1 = pTemp + dwTotal;
					while( dwTotal < dwMsgSize )	
					{
						FD_ZERO(&fdread);
						FD_SET(m_Client.Sock, &fdread);
						if( select(0, &fdread, NULL, NULL, &block_time) == 1 )
						{
							nResult = recv(m_Client.Sock, pTemp1, dwMsgSize-dwTotal, 0);
							if( nResult == SOCKET_ERROR )
								FATAL_ERR("got bad data in thread connect");
							
							dwTotal += (DWORD)nResult;
							pTemp1 = pTemp + dwTotal;
						}
					} 

					*(DWORD*)pTemp = 0;	// ����Ϣ��ǰ��Ľ����߱�Ϊ������
					m_pRecvMsgQueue->AddMsg((BYTE*)pTemp, dwMsgSize);
				}

				TSMemMgr::Free(pTemp);
			}
			else
			{
				if( m_Thread.bTerminateConnect )
					goto __thread_connect_end;

				if( WSAGetLastError() == WSAECONNRESET )
					goto __thread_connect_end;

				FATAL_ERR("got bad data in thread connect");
				goto __thread_connect_end;
			}
		}
	}

__thread_connect_end:

	m_Thread.nThread--;
	return 0;
}



//-----------------------------------------------------------------------------
// �㲥������λ��
//-----------------------------------------------------------------------------
UINT CWinTcp::ThreadBroadcast(LPVOID)
{
	m_Thread.nThread++;

	SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
	if( sock == INVALID_SOCKET )
	{
		FATAL_ERR("couldn't create socket in thread broadcast");
		m_Thread.nThread--;
		return 0;
	}

	BOOL opt = TRUE;
	int nResult = 0;
	char msg[NET_ID_LEN];
	int nSent = 0;

	sockaddr_in	add;
	add.sin_family = PF_INET;
	add.sin_addr.s_addr = htonl( INADDR_ANY );
	add.sin_port = htons( m_Server.nPort );

	sockaddr_in dest;	// ����Ŀ���ַ

	if( bind( sock, (struct sockaddr *)&add, sizeof(add)) != 0 )
	{
		if( WSAGetLastError() == WSAEADDRINUSE )	// ���ܱ����Ѿ�������һ���ͻ���
		{
			FATAL_ERR("bind error in thread broadcast");
			FATAL_ERR("if you connect to yourself, please create server first");
			goto __broadcast_end;
		}

		FATAL_ERR("bind error in thread broadcast");
		goto __broadcast_end;
	}

	opt = TRUE;
	nResult = setsockopt(sock, SOL_SOCKET, SO_BROADCAST, (char*)&opt, sizeof(BOOL));
	if( SOCKET_ERROR == nResult )
	{
		FATAL_ERR("setsockopt failed");
		goto __broadcast_end;
	}

	
	dest.sin_family = PF_INET;
	dest.sin_addr.s_addr = htonl( INADDR_BROADCAST );
	dest.sin_port = htons( m_Server.nPort );

_broadcast:

	wsprintf(msg, NET_ID);
	nSent = sendto( sock, msg, NET_ID_LEN, 0, (sockaddr *)&dest, sizeof(sockaddr_in) );
	
	if( nSent != NET_ID_LEN )
	{
		// �������ǲ��ھ�������
		// FATAL_ERR("send udp broadcast message failed");
		goto __broadcast_end;
	}

	Sleep(1000);	// broadcast per second
	
	if( m_Server.bBroadcast && m_Server.GetFreeClientSock() < m_Server.nMaxClient )
		goto _broadcast;
	

__broadcast_end:

	m_Server.bBroadcast = FALSE;
	closesocket( sock );

	m_Thread.nThread--;
	return 0;
}



//-----------------------------------------------------------------------------
// Ѱ��Server
//-----------------------------------------------------------------------------
UINT CWinTcp::ThreadSearchServer(LPVOID)
{
	m_Thread.nThread++;

	INT nResult = 0;
	int nIndex = GT_INVALID;
	INT nLen = 0;

	SOCKET sock = socket(PF_INET, SOCK_DGRAM, 0);
	if( sock == INVALID_SOCKET )
	{
		FATAL_ERR("create socket failed in thread search server");
		m_Thread.nThread--;
		return 0;
	}

	// sock ʹ�÷�������ʽ
	DWORD dwCmd = 1;
	nResult = ioctlsocket( sock, FIONBIO, &dwCmd );
	if( SOCKET_ERROR == nResult )
	{
		FATAL_ERR("set nonblocking mode failed in thread search server");
		goto __search_server_end;
	}

	sockaddr_in	source;
	sockaddr_in	add;
	add.sin_family = PF_INET;
	add.sin_addr.s_addr = htonl( INADDR_ANY );
	add.sin_port = htons( m_Client.nPort );

	if( SOCKET_ERROR == bind(sock, (struct sockaddr *)&add, sizeof(add)) )
	{
		if( WSAGetLastError() == WSAEADDRINUSE )	// �����������ڱ���
		{
			// �������������һ�������Լ��Ŀͻ���
			char szHostName[256];
			nResult = gethostname( szHostName, sizeof(szHostName) );
			if( nResult != 0 )
			{
				FATAL_ERR("couldn't gethostname in thread connect");
				goto __search_server_end;
			}
			else
			{
				hostent* pHost = gethostbyname( szHostName );
				memcpy(&source.sin_addr.s_addr, pHost->h_addr_list[0],
				sizeof(source.sin_addr.s_addr));
				goto _found;
			}
		}

		FATAL_ERR("bind error in thread search server");
		goto __search_server_end;
	}

	
	CHAR msg[NET_ID_LEN];
	nLen = sizeof(sockaddr_in);

	// ��������ʱ��
	fd_set fdread;

	timeval block_time;
	block_time.tv_sec = 0;
	block_time.tv_usec = GT_LAN_BLOCK_TIME;
	
	for(;;)
	{
		// "select" will change fdread, so this must put in the loop
		FD_ZERO(&fdread);
		FD_SET(sock, &fdread);
		nResult = select(0, &fdread, NULL, NULL, &block_time);

		if( nResult == 1 )
		{
			nResult = recvfrom(sock, msg, NET_ID_LEN, 0, (sockaddr *)&source, &nLen);
			if( 0 == strcmp(msg, NET_ID) )
				goto _found;
		}
		
		if( SOCKET_ERROR == nResult )
		{
			FATAL_ERR("recvfrom failed in thread search server");
			m_Client.bSerchServer = FALSE;	// quit
		}

		if( !m_Client.bSerchServer )
			goto __search_server_end;
	}


_found:	

	// �ҵ�������
	m_Client.bSerchServer = FALSE;
	nIndex = m_Client.GetServerNum();
	memcpy(&m_Client.AddressAllServer[nIndex], &source, sizeof(source));

__search_server_end:
	
	closesocket( sock );
	m_Thread.nThread--;
	return 0;
}