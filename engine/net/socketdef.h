//-----------------------------------------------------------------------------
// File: SocketDefine
// Desc: interface for win socket
// Auth: Lyp
// Date: 2003/11/30
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_WINSOCKET_DEFINE_H_
#define	_GT_WINSOCKET_DEFINE_H_
#pragma once


#define GT_MAX_CLIENT			8
#define GT_MAX_SERVER			8
#define GT_LAN_CUT_LAN			0xff
#define GT_LAN_BLOCK_TIME		500

#define NET_ID					"BattleCityLan"
#define NET_ID_LEN				16

// 服务器信息数据
struct ServerData	
{
	DWORD		dwID;
	INT			nPort;
	sockaddr_in	Address;
	SOCKET		Sock;
	BOOL		bBroadcast;
	
	SOCKET		SockClient[GT_MAX_CLIENT];
	DWORD		dwClientID[GT_MAX_CLIENT];
	INT			nMaxClient;
	INT			nClient;

	INT GetFreeClientSock()
	{
		for( int n=0; n<GT_MAX_CLIENT; n++ )
			if( SockClient[n] == NULL || SockClient[n] == INVALID_SOCKET)
				return n;
		return -1;
	}

	VOID Clear() { ZeroMemory(this, sizeof(*this)); }
	ServerData() { ZeroMemory(this, sizeof(*this)); }
};


// 客户端数据信息
struct ClientData
{
	DWORD		dwID;
	INT			nPort;
	sockaddr_in Address;
	SOCKET		Sock;
	sockaddr_in	AddressAllServer[GT_MAX_SERVER];
	BOOL		bConnected;
	BOOL		bSerchServer;

	INT GetServerNum()
	{
		for( int n=0; n<GT_MAX_SERVER; n++ )
			if( AddressAllServer[n].sin_addr.s_addr == 0 )
				return n;
		return GT_MAX_SERVER;
	}

	VOID Clear() { ZeroMemory(this, sizeof(*this)); }
	ClientData() { ZeroMemory(this, sizeof(*this)); }
};


// LAN联网游戏所需要的线程数据
struct LANThread
{
	INT			nThread;
	HANDLE		hAccept[GT_MAX_CLIENT];
	HANDLE		hSend;
	HANDLE		hSend2;
	HANDLE		hConnect;
	HANDLE		hBroadcast;
	HANDLE		hSearchServer;

	INT			nAcceptThreadNum;
	BOOL		bAcceptActive;
	BOOL		bTerminateAccept;

	BOOL		bSendActive;
	BOOL		bTerminateSend;

	BOOL		bConnectActive;
	BOOL		bTerminateConnect;

	VOID Clear() { ZeroMemory(this, sizeof(*this)); }
	LANThread()	 { ZeroMemory(this, sizeof(*this)); }
};


#endif // _GT_WINSOCKET_DEFINE_H_
