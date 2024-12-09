//-----------------------------------------------------------------------------
// File: netSession
// Desc: net session
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_NETSESSION_H_
#define _BC_NETSESSION_H_
#pragma once


class CMsgList;
class CWinTcp;
class CApp;


#include "gDefine.h"
#define BC_MAX_GAME_MSG		128

#pragma pack(push)
#pragma pack(1)
//-----------------------------------------------------------------------------
// game msg
//-----------------------------------------------------------------------------
struct GameMsg
{
	DWORD	dwCmd;
	DWORD	dwTick;
	BYTE	byPlayerInput[MAX_PLAYER];
	BYTE	byComputer;
	bool	bValid;
	bool	bActivePlayer;
};
#pragma pack(pop)


#define GT_NET_CMD_TICK				0xffff1234	// ��Ϸ����
#define GT_NET_CMD_PLAYER			0x00000001	// �������
#define GT_NET_CMD_REQ_WORLD_INFO	0xffff0001	// ���������������ʼ����Ϣ
#define GT_NET_CMD_WORLD_INFO		0xffff0002	// ���������������ʼ����Ϣ
#define GT_NET_CMD_CUT_LINE			0xffff0004	// ����
#define GT_NET_CMD_SYNC				0xffff0005	// ͬ����Ϣ
#define GT_NET_CMD_SYNC_END			0xffff0006	// ��������ͬ����Ϣ
//-----------------------------------------------------------------------------
// �Ự�ӿ�
//-----------------------------------------------------------------------------
class CSession
{
public:
	
	BOOL Init(bool bServer);
	void Destroy();

	bool Update();

	void SendCutLineMsg();
	int	 GetReadyMsgNum();

	CSession(CApp* pApp);
	~CSession();

private:

	DWORD		m_dwLatency;
	bool		m_bServer;

	CApp*		m_pApp;
	CWinTcp*	m_pWinTcp;
	CWinTcp*	m_pSync;
	bool		m_bClientInitialized;	// �ͻ����Ƿ��Ѿ���ʼ��
	bool		m_bRequireWorldInfoSended;
	bool		m_bClientPlayerActived;
	DWORD		m_dwWorldInfoSendTick;
	
	DWORD		m_dwLastGameTick;
	DWORD		m_dwGameTick;
	
	// ��֯�õ���Ϸ��Ϣ
	CMsgList*	m_pGameMsg;

	// δ��֯����Ϸ��Ϣ
	int			m_nMaxUncomposeGameMsg;
	int			m_nUncomposeGameMsg;
	GameMsg		m_UncomposeArray[BC_MAX_GAME_MSG*2];	// ������

	void		SendLatencyCmd();

	void		SendWorldInfo(DWORD dwReceiverID);
	void		SetWorldInfo(BYTE* pNetMsg);

	void		ComposeAndSendGameMsg(DWORD dwGameTick);
	bool		AddUncomposeMsg(BYTE* pMsg);
	void		DelUncomposeMsg(int nIndex);

};



#endif // _BC_NETSESSION_H_