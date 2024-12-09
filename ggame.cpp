//-----------------------------------------------------------------------------
// File: gGame.cpp
// Desc: core of game
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "resource.h"

#include "gApp.h"
#include "gGame.h"
#include "gPlane.h"
#include "gSprite.h"
#include "netSession.h"
#include "resmgr.h"


//-----------------------------------------------------------------------------
// construction/destruction
//-----------------------------------------------------------------------------
CGame::CGame(CApp* pApp)
{
	ZeroMemory(this, sizeof(*this));

	m_pApp = pApp;
	ASSERT(m_pApp);

	m_pPlane = new CPlane(this);
	for( int n=0; n<MAX_PLAYER; n++ )
	{
		m_pPlayer[n] = new CPlayer(this);
		m_pPlayer[n]->Create( 28, 28, true );
	}

	for( int n=0; n<NUM_ENEMYS; n++ )
	{
		m_pEnemy[n] = new CEnemy(this);
		m_pEnemy[n]->Create( 28, 28, false );
	}
	
	for( int n=0; n<=NUM_EXPLODES; n++ )
	{
		m_pExplode[n] = new CExplode(this);
		m_pExplode[n]->Create( 28, 28, false );
	}
	
	m_pBonus = new CBonus(this);
	m_pBonus->Create( 30, 28, false );


	m_nRandHolder = 0;
	m_dwTick = 0;
	m_nLevel = 0;
	m_eGameState = GS_SPLASH;
	m_dwLastTime = 0;
	m_nMaxEnemys = 0;
	m_nEnemysLeft = 0;
	m_nEnemys = 0;
	m_dwLockTime = 0;
	m_dwLastEnemyBoreTime = 0;
	for( int n=0; n<MAX_PLAYER; n++ )
		m_bPlayerActive[n] = false;
	m_bEnemyLocked = false;
	m_bShowState = false;


	RegisterVariable();
}



CGame::~CGame()
{
	SAFE_DEL( m_pPlane );

	for( int n=0; n<MAX_PLAYER; n++ )
		SAFE_DEL( m_pPlayer[n] );

	for( int n=0; n<NUM_ENEMYS; n++ )
		SAFE_DEL( m_pEnemy[n] );
	
	for( int n=0; n<=NUM_EXPLODES; n++ )
		SAFE_DEL( m_pExplode[n] );
	
	SAFE_DEL( m_pBonus );

	m_pApp->GetSnapshot()->UnregisterAll();

}



void CGame::RegisterVariable()
{
	CZoneObjSnapshot* pss = m_pApp->GetSnapshot();

	pss->RegisterVar(m_nRandHolder);
	pss->RegisterVar(m_dwTick);
	pss->RegisterVar(m_nLevel);
	pss->RegisterVar(m_eGameState);
	pss->RegisterVar(m_dwLastTime);
	pss->RegisterVar(m_nMaxEnemys);
	pss->RegisterVar(m_nEnemysLeft);
	pss->RegisterVar(m_nEnemys);
	pss->RegisterVar(m_dwLockTime);
	pss->RegisterVar(m_dwLastEnemyBoreTime);
	pss->RegisterVar(m_bPlayerActive);
	pss->RegisterVar(m_bEnemyLocked);
	pss->RegisterVar(m_bShowState);
}


//-----------------------------------------------------------------------------
// 用给定的随机数种子初始化随机数发生器
//-----------------------------------------------------------------------------
VOID CGame::Randomize(DWORD dwSeed)
{
	m_nRandHolder = (int)dwSeed;
}


//-----------------------------------------------------------------------------
// 得到随机数
//-----------------------------------------------------------------------------
INT CGame::Rand()
{
	return (((m_nRandHolder = m_nRandHolder * 214013L + 2531011L) >> 16) & 0x7fff);
}


//-----------------------------------------------------------------------------
// 得到随机数
//-----------------------------------------------------------------------------
INT CGame::Random( int min, int max )
{
	return (min + Rand() % (max - min + 1));
}



//-----------------------------------------------------------------------------
// game init
//-----------------------------------------------------------------------------
bool CGame::Init()
{
	CResMgr* pMgr = m_pApp->GetResMgr();

	m_pPlane->BitBlt( pMgr->GetBmp(0) );
	m_pPlayer[0]->BitBlt( pMgr->GetBmp(1), pMgr->GetBmp(2), 
		pMgr->GetBmp(14), pMgr->GetBmp(7) );
	m_pPlayer[1]->BitBlt( pMgr->GetBmp(9), pMgr->GetBmp(2), 
		pMgr->GetBmp(14), pMgr->GetBmp(7) );

	for( int i = 0; i < NUM_ENEMYS; i ++ )
	{
		m_pEnemy[i]->BitBlt( pMgr->GetBmp(5), pMgr->GetBmp(2), 
			pMgr->GetBmp(14), pMgr->GetBmp(7) );
	}

	for( int i = 0; i < NUM_EXPLODES;  i ++ )
		m_pExplode[i]->BitBlt( pMgr->GetBmp(3), pMgr->GetBmp(4) );

	m_pBonus->BitBlt( pMgr->GetBmp(6) );

	return true;
}



//-----------------------------------------------------------------------------
// process input
//-----------------------------------------------------------------------------
void CGame::ProcessInput(DWORD dwTick, BYTE* pInput)
{
	CBullet* pBullet = NULL;
	int x = 0, y = 0;
	DIRECTION dir = DIR_UP;
	int surface = 0;
	int i = 0, j = 0, k = 0;
	
	for( k = 0; k < 2; k ++ )
	{
		if( !m_pPlayer[k]->m_bActive )
			continue;

		x = m_pPlayer[k]->m_x;
		y = m_pPlayer[k]->m_y;
		dir = m_pPlayer[k]->m_dir;

		if( !m_pPlayer[k]->m_bBoring && m_eGameState != GS_OVER )
		{
			if( m_pPlayer[k]->ProcessInput(pInput[k]) )
				m_pApp->GetDSound()->Play( EFFECT_FIRE );
		}
		surface = m_pPlane->GetSurface( m_pPlayer[k] );
		if( surface != OBJ_NULL && surface != OBJ_TREE )
		{
			m_pPlayer[k]->m_x = x;
			m_pPlayer[k]->m_y = y;
			m_pPlayer[k]->m_dir = dir;
		}
		if( m_pPlayer[k]->HitTest( m_pPlayer[!k], x, y ) )
		{
			// two players hit together 
			m_pPlayer[k]->m_x = x;
			m_pPlayer[k]->m_y = y;
			m_pPlayer[k]->m_dir = dir;
		}

		// 玩家是否击中敌人
		for( i = 0; i < m_nMaxEnemys; i ++ )
		{
			if( m_pPlayer[k]->HitTest( m_pEnemy[i], x, y ) )	
			{
				m_pPlayer[k]->m_x = x;
				m_pPlayer[k]->m_y = y;
				m_pPlayer[k]->m_dir = dir;
				break;
			}
		}
	
		if( m_pPlayer[k]->HitTest( m_pBonus ) )	// 玩家碰到奖励物品
			EatBonus( m_pPlayer[k] );

		// bullet
		for( j = 0; j < 2; j ++ )
		{
			pBullet = m_pPlayer[k]->m_pBullet[j];
			if( !pBullet->m_bActive )
				continue;

			if( !pBullet->Move() )
			{
				pBullet->m_bActive = false;
				continue;
			}

			switch( m_pPlane->HitSurface( pBullet, m_pPlayer[k]->m_type>=3 ) )
			{
			case OBJ_BRICK:	// 击中砖头
				pBullet->m_bActive = false;
				Explode( pBullet );
				break;
			case OBJ_CONCRETE:	// 击中钢筋水泥
				pBullet->m_bActive = false;
				Explode( pBullet );
				m_pApp->GetDSound()->Play( EFFECT_HIT );
				break;
			case OBJ_HAWK:	// 击中老巢
				pBullet->m_bActive = false;
				Explode( pBullet, true );
				m_pApp->GetDSound()->Play( EFFECT_EXPLODE );
				m_eGameState = GS_OVER;
			}
			

			// 玩家是否相互击中
			if( m_pPlayer[!k]->m_bActive && pBullet->HitTest( m_pPlayer[!k] ) )
			{
				pBullet->m_bActive = false;
				if( !m_pPlayer[!k]->m_bShield )	// 击中后如果没有护甲就锁定
					m_pPlayer[!k]->Lock();
			}
			
			for( i = 0; i < m_nMaxEnemys; i ++ )
			{
				if( !m_pEnemy[i]->m_bShield && !m_pEnemy[i]->m_bBoring &&
					pBullet->HitTest( m_pEnemy[i] ) )
				{
					// our bullet hit the enemy
					pBullet->m_bActive = false;
					Explode( pBullet );					
					if( m_pEnemy[i]->m_bBonus )
					{
						m_pEnemy[i]->m_bBonus = false;
						BoreBonus();
					}

					if( m_pEnemy[i]->m_type == 2 )
					{
						if( --m_pEnemy[i]->m_nLevel < 0 )
							m_pEnemy[i]->m_bActive = false;
					}
					else
						m_pEnemy[i]->m_bActive = false;

					if( !m_pEnemy[i]->m_bActive )
					{
						// the enemy is dead
						Explode( m_pEnemy[i], true );
						m_pApp->GetDSound()->Play( EFFECT_EXPLODE );
						m_nEnemys --;
						m_pPlayer[k]->m_nScore += (m_pEnemy[i]->m_type+1) * 100;
					}
					break;
				}
				else if( pBullet->HitTest( m_pEnemy[i]->m_pBullet[0] ) )
				{
					// our bullet hit the enemy's
					pBullet->m_bActive = false;
					m_pEnemy[i]->m_pBullet[0]->m_bActive = false;
					break;
				}
			}
		}
	}


	// 两个玩家的子弹相互抵消
	if( m_pPlayer[0]->m_bActive && m_pPlayer[1]->m_bActive )
	{
		for( i = 0; i < 2; i ++ )
			for( j = 0; j < 2; j ++ )
				if( m_pPlayer[0]->m_pBullet[i]->HitTest(
					m_pPlayer[1]->m_pBullet[j] ) )
				{
					m_pPlayer[0]->m_pBullet[i]->m_bActive = false;
					m_pPlayer[1]->m_pBullet[j]->m_bActive = false;
				}
	}

	
	// 处理敌人
	for( i = 0; i < m_nMaxEnemys; i ++ )
	{
		if( m_pEnemy[i]->m_bActive && !m_bEnemyLocked && !m_pEnemy[i]->m_bBoring )
		{
			x = m_pEnemy[i]->m_x;
			y = m_pEnemy[i]->m_y;
			dir = m_pEnemy[i]->m_dir;
			
			if( (Rand() % 200) == 0 || !m_pEnemy[i]->Move() )
				m_pEnemy[i]->ChangeDirection();
			
			surface = m_pPlane->GetSurface( m_pEnemy[i] );
			if( surface == OBJ_BRICK )
			{
				if( (Rand() % 100) < 30 )
				{
					m_pEnemy[i]->ChangeDirection();
					surface = m_pPlane->GetSurface( m_pEnemy[i] );
				}
				else
					m_pEnemy[i]->Fire();
			}
			else if( surface == OBJ_CONCRETE || surface == OBJ_RIVER )
			{
				m_pEnemy[i]->ChangeDirection();
				surface = m_pPlane->GetSurface( m_pEnemy[i] );
			}
			if( m_pEnemy[i]->HitTest( m_pPlayer[0], x, y ) ||
				m_pEnemy[i]->HitTest( m_pPlayer[1], x, y ) )
			{
				m_pEnemy[i]->m_x = x;
				m_pEnemy[i]->m_y = y;
				m_pEnemy[i]->m_dir = dir;
				m_pEnemy[i]->Fire();
			}
			else if( surface != OBJ_NULL && surface != OBJ_TREE )
			{
				m_pEnemy[i]->m_x = x;
				m_pEnemy[i]->m_y = y;
				m_pEnemy[i]->m_dir = dir;
			}
			for( int j = 0; j < m_nMaxEnemys; j ++ )
			{
				if( i != j && m_pEnemy[i]->HitTest( m_pEnemy[j], x, y ) )
				{
					// two enemys hit each other
					m_pEnemy[i]->ChangeDirection();
					if( m_pEnemy[i]->HitTest( m_pEnemy[j], x, y ) )
					{
						m_pEnemy[i]->m_x = x;
						m_pEnemy[i]->m_y = y;
						m_pEnemy[i]->m_dir = dir;
					}
					break;
				}
			}
			
			// the enemy can also eat the bonus
			if( m_pEnemy[i]->HitTest( m_pBonus ) )
				EatBonus( m_pEnemy[i] );
						
			if( (Rand() % 100) == 0 )
				m_pEnemy[i]->Fire();
		}

		pBullet = m_pEnemy[i]->m_pBullet[0];
		if( pBullet->m_bActive )
		{
			if( !pBullet->Move() )
				pBullet->m_bActive = false;
	
			surface = m_pPlane->HitSurface( pBullet );
			if( surface == OBJ_BRICK || surface == OBJ_CONCRETE )
			{
				pBullet->m_bActive = false;
				Explode( pBullet );
			}
			else if( surface == OBJ_HAWK )
			{
				pBullet->m_bActive = false;
				Explode( pBullet, true );
				m_pApp->GetDSound()->Play( EFFECT_EXPLODE );
				m_eGameState = GS_OVER;
			}

			for( k = 0; k < 2; k ++ )
			{
				if( pBullet->HitTest( m_pPlayer[k] ) )
				{
					// 玩家被敌人击中
					pBullet->m_bActive = false;
					if( !m_pPlayer[k]->m_bShield )
					{
						Explode( pBullet );
						PlayerBeenHit( m_pPlayer[k] );
						m_pApp->GetDSound()->Play( EFFECT_EXPLODE );
					}
				}
			}
		}
	}

	// 产生敌人
	DWORD thisTime = dwTick;
	if( thisTime - m_dwLastEnemyBoreTime > m_pApp->GetGameFreq()*3 )
	{
		BoreEnemy();
		m_dwLastEnemyBoreTime = thisTime;
	}

	// 敌人的锁定是否失效
	if( m_bEnemyLocked && thisTime - m_dwLockTime > m_pApp->GetGameFreq()*10 )
		m_bEnemyLocked = false;

	// 是否胜利
	if( m_nEnemys <= 0 && m_eGameState == GS_ACTIVE )
	{
		m_eGameState = GS_WIN;
		m_dwLastTime = dwTick;
	}
	
	// 失败
	if( !m_pPlayer[0]->m_bActive && !m_pPlayer[1]->m_bActive )
		m_eGameState = GS_OVER;
}



//-----------------------------------------------------------------------------
// draw world
//-----------------------------------------------------------------------------
void CGame::DrawWorld()
{
	RECT rc = {0,0,0,0};
	rc.right = SCREEN_W;
	rc.bottom = SCREEN_H;

	CDisplay* pDisplay = m_pApp->GetDisplay();
	DWORD dwFillColor = pDisplay->DDColorMatch(RGB(128,128,128));
	pDisplay->DDClear( &rc, dwFillColor );

	rc.left = OFFSETX;
	rc.top = OFFSETY;
	rc.right = rc.left + 416;
	rc.bottom = rc.top + 416;
	pDisplay->DDClear( &rc, 0x0 );
	
	m_pPlane->Render( pDisplay->GetBackBuffer() );

	for( int i = 0; i < m_nMaxEnemys; i ++ )
		m_pEnemy[i]->Draw( pDisplay->GetBackBuffer() );

	m_pPlayer[0]->Draw( pDisplay->GetBackBuffer() );
	m_pPlayer[1]->Draw( pDisplay->GetBackBuffer() );	
	m_pPlane->DrawTree( pDisplay->GetBackBuffer() );

	for( int i = 0; i < NUM_EXPLODES; i ++ )
		m_pExplode[i]->Draw( pDisplay->GetBackBuffer() );

	m_pBonus->Draw( pDisplay->GetBackBuffer() );
	
	rc.left = rc.top = 0;
	rc.right = rc.bottom = 14;

	for( int i = 0; i < m_nEnemysLeft; i ++ )
	{
		int x = 550 + (i % 2) * 15;
		int y = 68 + i / 2 * 15;
		pDisplay->GetBackBuffer()->BLT( x, y, m_pApp->GetResMgr()->GetBmp(8), &rc );
	}

	rc.left = 28;
	rc.right = rc.left + 28;
	pDisplay->GetBackBuffer()->BLT( 550, 300, m_pApp->GetResMgr()->GetBmp(8), &rc );
	pDisplay->GetBackBuffer()->BLT( 20, 20, m_pApp->GetResMgr()->GetBmp(8), &rc );
	BltNumber( 70, 20, m_pPlayer[0]->m_nScore );

	rc.left = 14;
	rc.right = rc.left + 14;
	pDisplay->GetBackBuffer()->BLT( 550, 317, m_pApp->GetResMgr()->GetBmp(8), &rc );
	BltNumber( 570, 317, m_pPlayer[0]->m_nLife );

	if( m_pPlayer[1]->IsActive() )
	{
		rc.left = 56;
		rc.right = rc.left + 28;
		pDisplay->GetBackBuffer()->BLT( 550, 350, m_pApp->GetResMgr()->GetBmp(8), &rc );
		pDisplay->GetBackBuffer()->BLT( 480, 20, m_pApp->GetResMgr()->GetBmp(8), &rc );
		BltNumber( 530, 20, m_pPlayer[1]->m_nScore );

		rc.left = 14;
		rc.right = rc.left + 14;
		pDisplay->GetBackBuffer()->BLT( 550, 367, m_pApp->GetResMgr()->GetBmp(8), &rc );
		BltNumber( 570, 367, m_pPlayer[1]->m_nLife );
	}

	// draw level No.
	pDisplay->GetBackBuffer()->BLT( 550, 400, m_pApp->GetResMgr()->GetBmp(12), NULL );
	BltNumber( 580, 420, m_nLevel );
}



//-----------------------------------------------------------------------------
// blt number
//-----------------------------------------------------------------------------
void CGame::BltNumber( int x, int y, int n )
{
	char szNum[64];
	RECT rc = {0,0,0,0};
	rc.top = 0;
	rc.bottom = 14;

	wsprintf( szNum, "%d", n );
	int len = lstrlen( szNum );

	CDisplay* pDisplay = m_pApp->GetDisplay();
	CResMgr* pResMgr = m_pApp->GetResMgr();

	for( int i = 0; i < len; i ++ )
	{
		rc.left = (szNum[i] - '0') * 14;
		rc.right = rc.left + 14;
		pDisplay->GetBackBuffer()->BLT( x, y, pResMgr->GetBmp(13), &rc );
		x += 15;
	}
}








//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
void CGame::Update( DWORD dwTick, BYTE* pInput )
{
	int i = 0;
	m_dwTick = dwTick;

	if( m_eGameState == GS_OVER || m_eGameState == GS_ACTIVE ||
		m_eGameState == GS_WIN )
	{
		ProcessInput( dwTick, pInput );
	}


	m_pPlane->Update( dwTick );

	for( i = 0; i < m_nMaxEnemys; i ++ )
		m_pEnemy[i]->Update();
	m_pPlayer[0]->Update();
	m_pPlayer[1]->Update();	

	for( i = 0; i < NUM_EXPLODES; i ++ )
		m_pExplode[i]->Update();
	m_pBonus->Update();

	if( m_eGameState == GS_WIN )
	{
		if( dwTick - m_dwLastTime > 3*m_pApp->GetGameFreq() ||
			dwTick < m_dwLastTime )
		{
			m_eGameState = GS_ACTIVE;
			m_nLevel ++;
			InitLevel();
		}
	}

}



//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
void CGame::Render()
{
	static DWORD lastTick = timeGetTime();
	static int frameCount;
	static float fps;
	static int y = SCREEN_H - 160;

	switch( m_eGameState )
	{
	case GS_SPLASH:
		y = SCREEN_H - 160;
		break;
	
	case GS_OVER:
		DrawWorld();
		m_pApp->GetDisplay()->GetBackBuffer()->
			BLT( 200, y, m_pApp->GetResMgr()->GetBmp(11), NULL );

		if( y > 200 )
			y -= 2;
		break;

	case GS_ACTIVE:
	case GS_WIN:
		DrawWorld();
		break;
	}

	frameCount ++;
	DWORD thisTick = timeGetTime();
	if( thisTick - lastTick > 1000 )
	{
		fps = (float)frameCount / (thisTick - lastTick) * 1000;
		frameCount = 0;
		lastTick = thisTick;
	}

	char buf[255];
	if( m_bShowState )
	{
		// Show FPS
		sprintf_s( buf, "FPS: %.2f   GameTick: %lu    Buffer: %lu    Rand: %lu", fps, 
			m_dwTick, m_pApp->GetSession()->GetReadyMsgNum(), GetRandHolder() );

		m_pApp->GetDisplay()->OutputText( 10, 10, buf );
	}
	
	if( false == m_pApp->GetDisplay()->FlipScreen() )
	{
		m_pApp->GetDisplay()->RestoreSurfaces();
		m_pApp->GetResMgr()->RestoreRes();
	}
}



//-----------------------------------------------------------------------------
// 爆炸
//-----------------------------------------------------------------------------
void CGame::Explode( CSprite* pSprite, bool bExplode )
{
	for( int i = 0; i < NUM_EXPLODES; i ++ )
		if( !m_pExplode[i]->m_bActive )
		{
			int x = pSprite->m_x + pSprite->m_width/2;
			int y = pSprite->m_y + pSprite->m_height/2;
			m_pExplode[i]->m_bActive = true;
			m_pExplode[i]->m_x = x - m_pExplode[i]->m_width/2;
			m_pExplode[i]->m_y = y - m_pExplode[i]->m_height/2;
			m_pExplode[i]->m_time = m_dwTick;
			m_pExplode[i]->m_bExplode = bExplode;
			break;
		}
}




//-----------------------------------------------------------------------------
// 捡到奖励物品
//-----------------------------------------------------------------------------
void CGame::EatBonus( CPlayer* pPlayer )
{
	m_pApp->GetDSound()->Play( EFFECT_PICK );

	int i = 0;
	switch( m_pBonus->m_type )
	{
	case BONUS_LIFE:
		pPlayer->m_nLife ++;
		m_pApp->GetDSound()->Play( EFFECT_LIFE );
		break;
	case BONUS_CLOCK:
		m_bEnemyLocked = true;
		m_dwLockTime = m_dwTick;
		break;
	case BONUS_SHOVEL:
		m_pPlane->Protect();
		break;
	case BONUS_BOMB:
		for( i = 0; i < m_nMaxEnemys; i ++ )
		{
			if( m_pEnemy[i]->m_bActive &&
				!m_pEnemy[i]->m_bBoring )
			{
				Explode( m_pEnemy[i], true );
				m_pEnemy[i]->m_bActive = false;
				m_nEnemys --;
			}
		}
		m_pApp->GetDSound()->Play( EFFECT_EXPLODE );
		break;
	case BONUS_STAR:
		if( ++pPlayer->m_type > 3 )
			pPlayer->m_type = 3;
		break;
	case BONUS_HELMET:
		pPlayer->Shield( 10 * GetApp()->GetGameFreq() );
		break;
	}
	m_pBonus->m_bActive = false;
}


//-----------------------------------------------------------------------------
// 玩家被打中
//-----------------------------------------------------------------------------
void CGame::PlayerBeenHit( CPlayer* pPlayer )
{
	if( !pPlayer->m_bActive )
		return;

	Explode( pPlayer, true );
	if( --pPlayer->m_nLife <= 0 )
	{
		pPlayer->m_bActive = false;
		return;
	}

	ResetPlayer( pPlayer );
	pPlayer->m_type = 0;
}


//-----------------------------------------------------------------------------
// 敌人吃到奖励物品
//-----------------------------------------------------------------------------
void CGame::EatBonus( CEnemy* pEnemy )
{
	switch( m_pBonus->m_type )
	{
	case BONUS_LIFE:
		m_nEnemys += 5;
		m_nEnemysLeft += 5;
		break;

	case BONUS_CLOCK:
		m_pPlayer[0]->Lock();
		m_pPlayer[1]->Lock();
		break;

	case BONUS_SHOVEL:
		m_pPlane->Bare();
		break;

	case BONUS_BOMB:
		PlayerBeenHit( m_pPlayer[0] );
		PlayerBeenHit( m_pPlayer[1] );
		m_pApp->GetDSound()->Play( EFFECT_EXPLODE );
		break;

	case BONUS_STAR:
		pEnemy->m_type = 2;
		pEnemy->m_nLevel = 2;
		break;

	case BONUS_HELMET:
		pEnemy->Shield( 10 * GetApp()->GetGameFreq() );
		break;
	}
	m_pBonus->m_bActive = false;
}



//-----------------------------------------------------------------------------
// 产生奖励物品
//-----------------------------------------------------------------------------
bool CGame::BoreBonus()
{
	m_pBonus->m_bActive = true;
	int temp = Rand() % 100;
	if( temp < 10 )
		m_pBonus->m_type = BONUS_LIFE;
	else if( temp < 30 )
		m_pBonus->m_type = BONUS_CLOCK;
	else if( temp < 50 )
		m_pBonus->m_type = BONUS_SHOVEL;
	else if( temp < 65 )
		m_pBonus->m_type = BONUS_BOMB;
	else if( temp < 85 )
		m_pBonus->m_type = BONUS_STAR;
	else
		m_pBonus->m_type = BONUS_HELMET;

	m_pBonus->m_x = Random( 0, 416 - m_pBonus->m_width );
	m_pBonus->m_y = Random( 0, 416 - m_pBonus->m_height );
	m_pBonus->m_dwLastTime = m_pBonus->m_dwFlickerTime = m_dwTick;

	return true;
}



//-----------------------------------------------------------------------------
// 随机产生敌人
//-----------------------------------------------------------------------------
bool CGame::BoreEnemy()
{
	if( m_nEnemysLeft <= 0 )
		return false;
	
	int which = Random( 0, 4 );
	int x = 0, y = 0;

	for( int i = 0; i < m_nMaxEnemys; i ++ )
		if( !m_pEnemy[i]->m_bActive )
		{
			switch( which )
			{
			case 0:
				x = 2; y = 2;
				break;
			case 1:
				x = 194; y = 2;
				break;
			case 2:
				x = 386; y = 2;
				break;
			case 3:
				x = 2; y = 160;
				break;
			case 4:
				x = 386; y = 160;
				break;
			}

			int temp = Rand() % 100;

			if( temp < 50 )
			{
				m_pEnemy[i]->m_type = 0;
				m_pEnemy[i]->m_speed = 1;
			}
			else if( temp < 80 )
			{
				m_pEnemy[i]->m_type = 1;
				m_pEnemy[i]->m_speed = 2;
			}
			else
			{
				m_pEnemy[i]->m_type = 2;
				m_pEnemy[i]->m_speed = 1;
			}
			m_pEnemy[i]->m_x = x;
			m_pEnemy[i]->m_y = y;
			m_pEnemy[i]->m_dir = DIR_DOWN;
			if( m_nEnemysLeft == 4 ||
				m_nEnemysLeft == 11 ||
				m_nEnemysLeft == 18 )
			{
				m_pEnemy[i]->m_bBonus = true;
				m_pEnemy[i]->m_dwRedTime = 0;
			}
			else
				m_pEnemy[i]->m_bBonus = false;

			if( m_pEnemy[i]->m_type == 2 )
				m_pEnemy[i]->m_nLevel = 2;

			m_pEnemy[i]->Reborn();

			m_nEnemysLeft --;
			if( ++which > 2 )
				which = 0;
			return true;
		}

	return false;
}




//-----------------------------------------------------------------------------
// 设置玩家初始位置
//-----------------------------------------------------------------------------
void CGame::ResetPlayer( CPlayer* pPlayer )
{
	if( pPlayer == m_pPlayer[0] )
	{
		pPlayer->m_x = PLAYER1_STARTX;
		pPlayer->m_y = PLAYER1_STARTY;
	}
	else
	{
		pPlayer->m_x = PLAYER2_STARTX;
		pPlayer->m_y = PLAYER2_STARTY;
	}
	pPlayer->Reborn();
}



//-----------------------------------------------------------------------------
// 初始游戏每关
//-----------------------------------------------------------------------------
bool CGame::InitLevel()
{
	// 初始化地图
	if( !m_pPlane->Create( m_nLevel ) )
	{
		m_nLevel = 1;
		if( !m_pPlane->Create( m_nLevel ) )
			return false;
	}

	// 初始化所有玩家
	for( int n=0; n<MAX_PLAYER; n++ )
	{
		if( m_bPlayerActive[n] && m_pPlayer[n]->m_nLife > 0 )
		{
			m_pPlayer[n]->m_bActive = true;
			ResetPlayer( m_pPlayer[n] );
		}
		else 
			m_pPlayer[n]->m_bActive = false;

		m_pPlayer[n]->m_pBullet[0]->m_bActive = false;
		m_pPlayer[n]->m_pBullet[1]->m_bActive = false;
	}
		

	for( int i = 0; i < NUM_ENEMYS; i ++ )
	{
		m_pEnemy[i]->m_bActive = false;
		m_pEnemy[i]->m_pBullet[0]->m_bActive = false;
	}

	for( int i = 0; i < NUM_EXPLODES; i ++ )
		m_pExplode[i]->m_bActive = false;

	m_pBonus->m_bActive = false;
	m_bEnemyLocked = false;
	m_nEnemysLeft = m_nEnemys = 20;

	return true;
}



//-----------------------------------------------------------------------------
// 初始游戏
//-----------------------------------------------------------------------------
bool CGame::ResetGame()
{
	for( int n=0; n<MAX_PLAYER; n++ )
	{
		m_pPlayer[n]->m_nLife = 3;
		m_pPlayer[n]->m_type = 0;
		m_pPlayer[n]->m_nScore = 0;
		m_bPlayerActive[n] = false;
	}

	m_bPlayerActive[0] = true;	// 至少有一个玩家
	m_bPlayerActive[1] = false;	// 至少有一个玩家
	m_nMaxEnemys = 4;
	m_eGameState = GS_ACTIVE;
	m_nLevel = 1;
	return InitLevel();
}


void CGame::ActivatePlayer(int n)
{
	m_bPlayerActive[n] = true;
	m_pPlayer[n]->m_bActive = true;
	ResetPlayer( m_pPlayer[n] );
}




