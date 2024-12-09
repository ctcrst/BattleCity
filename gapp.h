//-----------------------------------------------------------------------------
// File: gApp
// Desc: main entrance of Battle City Lan
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_APP_H_
#define _BC_APP_H_
#pragma once

class CDisplay;
class CDirectInput;
class CDirectSound;
class CResMgr;
class CZoneObjSnapshot;
class CSession;
class CGame;
class CTitle;

//-----------------------------------------------------------------------------
// class capp
//-----------------------------------------------------------------------------
class CApp:public Trunk<CApp>
{
public:

	CDisplay*			GetDisplay()	{ return m_pDisplay; }
	CDirectInput*		GetDInput()		{ return m_pDirectInput; }
	CDirectSound*		GetDSound()		{ return m_pDirectSound; }
	CResMgr*			GetResMgr()		{ return m_pResMgr; }
	CGame*				GetGame()		{ return m_pGame; }
	CSession*			GetSession()	{ return m_pSession; }
	CZoneObjSnapshot*	GetSnapshot()	{ return m_pSnapshot; }

	bool			Initialize( HINSTANCE hInst );
	int				MainLoop();

	void			SetSpeedup() { m_bSpeedup = true; }
	void			SetSlowdown() { m_bSlowdown = true; }

	DWORD			GetGameFreq() { return m_dwGameFreq; }
	bool			IsServer()	{ return m_bServer; }

	CApp();
	~CApp();

private:
	HINSTANCE			m_hInst;
	HWND				m_hWnd;
	bool				m_bActive;
	bool				m_bFullScreen;
	bool				m_bServer;
	bool				m_bGameStarted;

	LARGE_INTEGER		m_qWinFreq;		// �߾��ȼ�ʱ��Ƶ��
	LARGE_INTEGER		m_qWinTick;	// �߾��ȼ�ʱ��Tick
	LARGE_INTEGER		m_qLastWinTick;	// �߾��ȼ�ʱ��Tick
	LARGE_INTEGER		m_qWinTickPerGameTick;
	DWORD				m_dwGameFreq;	// ��Ϸ��ѭ��Ƶ��

	bool				m_bSpeedup;		// �Ƿ������Ҫ�������ѭ��
	bool				m_bSlowdown;	// ����

	CDisplay*			m_pDisplay;
	CDirectInput*		m_pDirectInput;
	CDirectSound*		m_pDirectSound;
	CResMgr*			m_pResMgr;
	CZoneObjSnapshot*	m_pSnapshot;

	CSession*			m_pSession;
	CGame*				m_pGame;
	CTitle*				m_pTitle;

	bool				InitWindow();
	bool				InitSupport();
	void				DestroySupport();

	void				BackToSplash();	// exit current game

	LRESULT				MsgProc( HWND, UINT, WPARAM, LPARAM );
};



#endif // _BC_APP_H_
