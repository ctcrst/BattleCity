//-----------------------------------------------------------------------------
// File: netSession
// Desc: net session
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include "gDefine.h"
#include "gApp.h"
#include "gGame.h"
#include "netSession.h"

//-----------------------------------------------------------------------------
// construction/destruction
//-----------------------------------------------------------------------------
CSession::CSession(CApp* pApp)
{
	ZeroMemory(this, sizeof(*this));
	m_pApp = pApp;

	// 客户端是否已经初始化
	m_bClientInitialized = false;	

	m_pWinTcp = new CWinTcp;
	
	m_pSync = NULL;
	// m_pSync = new CWinTcp;		// TEST

	// 未组织的游戏信息
	m_nUncomposeGameMsg = 0;
	m_nMaxUncomposeGameMsg = BC_MAX_GAME_MSG * 2;

}


CSession::~CSession()
{
	SAFE_DEL(m_pWinTcp);
	SAFE_DEL(m_pSync);
}


//-----------------------------------------------------------------------------
// init/destroy
//-----------------------------------------------------------------------------
BOOL CSession::Init(bool bServer)
{
	m_dwLatency = 3;
	m_bServer = bServer;

	m_bRequireWorldInfoSended = false;
	m_bClientInitialized = false;
	m_bClientPlayerActived = false;
	m_dwWorldInfoSendTick = 0;

	m_dwLastGameTick = ~0;
	m_dwGameTick = 1;
	m_nUncomposeGameMsg = 0;
	ZeroMemory(m_UncomposeArray, sizeof(m_UncomposeArray));

	// 已经组织好的游戏信息
	m_pGameMsg = new CMsgList;

	if( m_pSync )
		m_pSync->Init(BC_SYNC_PORT, 3, FALSE);

	return m_pWinTcp->Init(BC_PORT, BC_MAX_PLAYER, m_bServer);
}

void CSession::Destroy()
{
	SAFE_DEL(m_pGameMsg);
	m_pWinTcp->Destroy();

	if( m_pSync )
		m_pSync->Destroy();
}



//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
bool CSession::Update()
{
	if( ( !m_bServer && !m_pWinTcp->Connected() )
		|| ( m_bServer && m_pWinTcp->GetClientNum()<1 ) )
		return true;

	if( m_bServer && m_dwGameTick == 1 )
	{
		SendLatencyCmd();
	}

	if( !m_bServer && !m_bRequireWorldInfoSended )
	{
		DWORD dwCmd = GT_NET_CMD_REQ_WORLD_INFO;
		m_pWinTcp->Send((BYTE*)&dwCmd, 4);
		m_bRequireWorldInfoSended = true;
	}

	GameMsg TempMsg;
	ZeroMemory(&TempMsg, sizeof(TempMsg));
	BYTE byLocalInput = 0;
	m_pApp->GetDInput()->GetKey(byLocalInput);

	if( m_bServer )
	{
		if( byLocalInput != 0 )
		{
			TempMsg.dwCmd = GT_NET_CMD_PLAYER;
			TempMsg.dwTick = m_dwGameTick;
			TempMsg.byComputer = 1;
			TempMsg.byPlayerInput[0] = byLocalInput;
			TempMsg.byPlayerInput[1] = 0;
			AddUncomposeMsg( (BYTE*)&TempMsg );
		}
		
		if( m_dwLastGameTick != m_dwGameTick )
		{
			ComposeAndSendGameMsg(m_dwGameTick+m_dwLatency);
			m_dwLastGameTick = m_dwGameTick; 
		}
	}
	else
	{
		// 客户端初始化好后才能将操作传送出去
		if( m_bClientInitialized )
		{
			// if( byLocalInput != 0 )
			{
				TempMsg.dwCmd = GT_NET_CMD_PLAYER;
				TempMsg.dwTick = m_dwGameTick;
				TempMsg.byComputer = 2;
				TempMsg.byPlayerInput[0] = 0;
				TempMsg.byPlayerInput[1] = byLocalInput;

				m_pWinTcp->Send( (BYTE*)&TempMsg, sizeof(TempMsg) );
			}
		}
	}


	BYTE* pNetMsg = NULL;
	DWORD dwNetMsgSize = 0;
	DWORD dwSenderID;
	for( int n=0; n<3; n++ )
	{
		pNetMsg = NULL;
		if( FALSE == m_pWinTcp->Recv(&pNetMsg, dwNetMsgSize, dwSenderID) )
			continue;

		switch( *(DWORD*)pNetMsg )
		{
		case GT_NET_CMD_CUT_LINE:	// 断线信息
			SendCutLineMsg();
			TSMemMgr::Free(pNetMsg);
			Sleep(200);
			return false;
			break;
			
		case GT_NET_CMD_TICK:		// 加工好的游戏信息
			m_pGameMsg->AddMsg(pNetMsg, dwNetMsgSize, ((GameMsg*)pNetMsg)->dwTick );
			ASSERT( dwNetMsgSize == sizeof(TempMsg) );
			break;

		case GT_NET_CMD_PLAYER:
			memcpy(&TempMsg, pNetMsg, dwNetMsgSize);
			if( TempMsg.byPlayerInput[0] != 0
				|| TempMsg.byPlayerInput[1] != 0 )
				AddUncomposeMsg( (BYTE*)&TempMsg );
			break;
			
		case GT_NET_CMD_REQ_WORLD_INFO:	// 需要游戏世界信息
			ASSERT(m_bServer);
			SendWorldInfo(dwSenderID);
			break;
			
		case GT_NET_CMD_WORLD_INFO:	// 游戏世界信息
			if( !m_bServer )
				SetWorldInfo(pNetMsg);
			break;
		}

		TSMemMgr::Free(pNetMsg);
	}


	// 运转游戏世界
	GameMsg* pTempGameMsg = NULL;
	if( m_pGameMsg->PeekMsg( (BYTE**)&pTempGameMsg, m_dwGameTick ) )
	{
		if( m_pSync ) //m_pSync->Connected() ) 
		{
			DWORD dwZoneSize = m_pApp->GetSnapshot()->GetAllSize();
			void* pMem = MemMgr::Alloc(dwZoneSize+4+4);
			DWORD dwCmd = GT_NET_CMD_SYNC;
			DWORD dwGameTick = m_dwGameTick;
			
			memcpy(pMem, &dwCmd, 4);
			memcpy((BYTE*)pMem+4, &dwGameTick, 4);
			
			m_pApp->GetSnapshot()->Snapshot((BYTE*)pMem+4+4);
			m_pSync->Send( (BYTE*)pMem, dwZoneSize+4+4 );
			MemMgr::Free(pMem);
		}

		m_pApp->GetGame()->Update(m_dwGameTick++, (BYTE*)&(pTempGameMsg->byPlayerInput[0]));
		if( pTempGameMsg->bActivePlayer )
			m_pApp->GetGame()->ActivatePlayer(1);

		MemMgr::Free(pTempGameMsg);
	}


	// 网络同步弹簧机制
	if( !m_bServer )
	{
		if( GetReadyMsgNum() <= 1 )
			m_pApp->SetSlowdown();

		if( GetReadyMsgNum() > (int)m_dwLatency )
			m_pApp->SetSpeedup();
	}

	return true;

}



//-----------------------------------------------------------------------------
// 发送延迟命令
//-----------------------------------------------------------------------------
void CSession::SendLatencyCmd()
{
	GameMsg TempMsg;
	ZeroMemory( &TempMsg, sizeof(TempMsg) );

	TempMsg.dwCmd = GT_NET_CMD_TICK;
	TempMsg.dwTick = m_dwGameTick;
	TempMsg.byComputer = 0;

	for( int n=0; n<(int)m_dwLatency; n++ )
	{
		TempMsg.dwTick = m_dwGameTick+n;
		m_pWinTcp->Send( (BYTE*)&TempMsg, sizeof(TempMsg) );
	}

}


//-----------------------------------------------------------------------------
// 发送游戏世界信息
//-----------------------------------------------------------------------------
void CSession::SendWorldInfo(DWORD dwReceiverID)
{
	DWORD dwZoneSize = m_pApp->GetSnapshot()->GetAllSize();
	void* pMem = MemMgr::Alloc(dwZoneSize+4+4);
	DWORD dwCmd = GT_NET_CMD_WORLD_INFO;
	DWORD dwGameTick = m_dwGameTick;
	m_dwWorldInfoSendTick = dwGameTick;
	
	memcpy(pMem, &dwCmd, 4);
	memcpy((BYTE*)pMem+4, &dwGameTick, 4);

	m_pApp->GetSnapshot()->Snapshot((BYTE*)pMem+4+4);
	m_pWinTcp->Send( (BYTE*)pMem, dwZoneSize+4+4, dwReceiverID );
	MemMgr::Free(pMem);

	// 需要将缓存里的游戏命令发送出去
	BYTE* pMsg = NULL;
	DWORD dwSize = 0;
	DWORD dwFirstGameTick = dwGameTick;
	CMsgList list;
	do
	{
		pMsg = NULL;
		dwSize = m_pGameMsg->PeekMsg(&pMsg, dwGameTick);
		if( dwSize )
		{
			list.AddMsg(pMsg, dwSize, dwGameTick);
			m_pWinTcp->Send( pMsg, dwSize, dwReceiverID );
			MemMgr::Free(pMsg);
		}

		dwGameTick++;

	}while( dwSize != 0 );


	do
	{
		pMsg = NULL;
		dwSize = list.PeekMsg(&pMsg, dwFirstGameTick);
		if( dwSize )
		{
			m_pGameMsg->AddMsg(pMsg, dwSize, dwFirstGameTick);
			MemMgr::Free(pMsg);
		}

		dwFirstGameTick++;
	}while( dwSize != 0 );


	m_bClientInitialized = true;
}



//-----------------------------------------------------------------------------
// 根据得到的信息,设置游戏世界
//-----------------------------------------------------------------------------
void CSession::SetWorldInfo(BYTE* pNetMsg)
{
	GameMsg TempMsg;
	ZeroMemory(&TempMsg, sizeof(TempMsg));
	DWORD dwRandHolder = 0;

	memcpy(&m_dwGameTick, pNetMsg+4, 4);
	m_pApp->GetSnapshot()->LoadFrom((BYTE*)pNetMsg+4+4);

	m_bClientInitialized = true;
}




//-----------------------------------------------------------------------------
// 发送断线信息
//-----------------------------------------------------------------------------
void CSession::SendCutLineMsg()
{
	GameMsg TempMsg;
	ZeroMemory(&TempMsg, sizeof(TempMsg));
	TempMsg.dwCmd = GT_NET_CMD_CUT_LINE;

	m_pWinTcp->Send( (BYTE*)&TempMsg, sizeof(TempMsg) );
}




//-----------------------------------------------------------------------------
// 组织消息放入组织队列
//-----------------------------------------------------------------------------
void CSession::ComposeAndSendGameMsg(DWORD dwGameTick)
{
	GameMsg TempMsg;
	ZeroMemory( &TempMsg, sizeof(TempMsg) );

	TempMsg.dwCmd = GT_NET_CMD_TICK;
	TempMsg.dwTick = dwGameTick;
	TempMsg.byComputer = 0;

	DWORD dwMinTick1 = ~0;
	DWORD dwMinTick2 = ~0;
	int	nMinIndex1 = ~0;
	int	nMinIndex2 = ~0;

	for( int n=0; n<m_nMaxUncomposeGameMsg; n++ )
	{
		if( !m_UncomposeArray[n].bValid )
			continue;

		if( m_UncomposeArray[n].byComputer == 1 )
		{
			if( m_UncomposeArray[n].dwTick < dwMinTick1 )
			{
				TempMsg.byPlayerInput[0] = m_UncomposeArray[n].byPlayerInput[0];
				dwMinTick1 = m_UncomposeArray[n].dwTick;
				nMinIndex1 = n;
			}
		}
		else
		{
			if( m_UncomposeArray[n].dwTick < dwMinTick2 )
			{
				TempMsg.byPlayerInput[1] = m_UncomposeArray[n].byPlayerInput[1];
				dwMinTick2 = m_UncomposeArray[n].dwTick;
				nMinIndex2 = n;
			}
		}
	}

	if( nMinIndex1 != ~0 )
		DelUncomposeMsg(nMinIndex1);

	if( nMinIndex2 != ~0 )
		DelUncomposeMsg(nMinIndex2);

	if( !m_bClientPlayerActived && m_pWinTcp->GetClientNum() > 1 )
	{
		if( TempMsg.dwTick == m_dwWorldInfoSendTick + 10 )
		{
			TempMsg.bActivePlayer = true;
			m_bClientPlayerActived = true;
		}
	}
					
	m_pWinTcp->Send( (BYTE*)&TempMsg, sizeof(TempMsg) );

}



//-----------------------------------------------------------------------------
// 将消息放入未组织队列
//-----------------------------------------------------------------------------
bool CSession::AddUncomposeMsg(BYTE* pMsg)
{
	if( m_nUncomposeGameMsg >= m_nMaxUncomposeGameMsg )
	{
		// too much msg
		FATAL_ERR("too much msg in uncompose array");
		return false;
	}
	
	for( int n=0; n<m_nMaxUncomposeGameMsg; n++ )
	{
		if( false == m_UncomposeArray[n].bValid )
		{
			memcpy(&m_UncomposeArray[n], pMsg, sizeof(GameMsg));
			m_UncomposeArray[n].bValid = true;
			m_nUncomposeGameMsg++;
			return true;
		}
	}
	
	FATAL_ERR("too much msg in uncompose array");
	return false;
}




//-----------------------------------------------------------------------------
// 删除未组织队列的消息
//-----------------------------------------------------------------------------
void CSession::DelUncomposeMsg(int nIndex)
{
	if( nIndex >= m_nMaxUncomposeGameMsg 
		|| m_nUncomposeGameMsg <= 0 )
	{
		return;	// no this msg 
	}

	m_UncomposeArray[nIndex].bValid = false;
	m_nUncomposeGameMsg--;

	return;
}



int CSession::GetReadyMsgNum()
{
	return m_pGameMsg->GetMsgNumInList();
}