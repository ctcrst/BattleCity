//-----------------------------------------------------------------------------
// File: gGame.cpp
// Desc: core of game
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_GAME_H_
#define _BC_GAME_H_
#pragma once

#include "gDefine.h"

class CApp;
class CPlayer;
class CSprite;
class CEnemy;
class CPlane;
class CExplode;
class CBonus;

#pragma pack(push)
#pragma pack(1)
//-----------------------------------------------------------------------------
// core of game
//-----------------------------------------------------------------------------
class CGame  
{
public:
	CGame(CApp* pApp);
	~CGame();

	bool		Init();
	bool		ResetGame();
	void		ProcessInput(DWORD dwTick, BYTE* pInput);
	void		Update(DWORD dwTick, BYTE* pInput);
	void		Render();

	CApp*		GetApp() { return m_pApp; }
	DWORD		GetTick() { return m_dwTick; }

	void		SetShowState(bool b) { m_bShowState = b; }
	bool		IsShowState() { return m_bShowState; }
	GAMESTATE	GetState() { return m_eGameState; }
	
	void		ActivatePlayer(int n);

	void		Randomize(DWORD dwSeed);
	DWORD		GetRandHolder() { return m_nRandHolder; }
	int			Rand();
	int			Random(int min, int max);


private:

	void	SetState(GAMESTATE s) { m_eGameState = s; }	// this must be private

	bool	InitLevel();
	void	ResetPlayer( CPlayer* pPlayer );

	void	Explode( CSprite* pSprite, bool bExplode=false );
	void	PlayerBeenHit( CPlayer* pPlayer );
	bool	BoreBonus();
	bool	BoreEnemy();
	void	EatBonus( CPlayer* pPlayer );
	void	EatBonus( CEnemy* pEnemy );	

	void	DrawWorld();
	void	BltNumber( int x, int y, int n );

	// 游戏世界的恢复
	DWORD	GetSaveSize();
	void	Save(BYTE* pDest);
	void	Load(BYTE* pSource);

private:

	CApp*		m_pApp;

	// 游戏世界的构成
	CPlane*		m_pPlane;
	CPlayer*	m_pPlayer[MAX_PLAYER];
	CEnemy*		m_pEnemy[NUM_ENEMYS];
	CExplode*	m_pExplode[NUM_EXPLODES];
	CBonus*		m_pBonus;

	int			m_nRandHolder;
	DWORD		m_dwTick;
	int			m_nLevel;
	GAMESTATE	m_eGameState;
	DWORD		m_dwLastTime;
	int			m_nMaxEnemys;
	int			m_nEnemysLeft;
	int			m_nEnemys;
	DWORD		m_dwLockTime;
	DWORD		m_dwLastEnemyBoreTime;
	bool		m_bPlayerActive[MAX_PLAYER];
	bool		m_bEnemyLocked;
	bool		m_bShowState;

	void		RegisterVariable();

};

#pragma pack(pop)


#endif // _BC_GAME_H_
