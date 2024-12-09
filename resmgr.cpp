//-----------------------------------------------------------------------------
// File: resMgr
// Desc: resource manager
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "ResMgr.h"

#include "resource.h"


CResMgr::CResMgr()
{
	ZeroMemory(this, sizeof(*this));
}


CResMgr::~CResMgr()
{
	UnloadRes();
}

bool CResMgr::Init(CDisplay* pDisplay)
{
	m_pDisplay = pDisplay;
	return TRUE;
}


bool CResMgr::LoadRes()
{
	this->UnloadRes();

	// Loading bitmaps...
	
	m_pBmpList[0] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_TILE) );
	m_pDisplay->DDSetColorKey( m_pBmpList[0], 0 );

	m_pBmpList[1] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_PLAYER1) );
	m_pDisplay->DDSetColorKey( m_pBmpList[1], 0 );

	m_pBmpList[2] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_BULLET) );
	m_pDisplay->DDSetColorKey( m_pBmpList[2], 0 );
	
	m_pBmpList[3] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_EXPLODE1) );
	m_pDisplay->DDSetColorKey( m_pBmpList[3], 0 );

	m_pBmpList[4] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_EXPLODE2) );
	m_pDisplay->DDSetColorKey( m_pBmpList[4], 0 );

	m_pBmpList[5] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_ENEMY) );
	m_pDisplay->DDSetColorKey( m_pBmpList[5], 0 );

	m_pBmpList[6] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_BONUS) );
	m_pDisplay->DDSetColorKey( m_pBmpList[6], 0 );

	m_pBmpList[7] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_BORE) );
	m_pDisplay->DDSetColorKey( m_pBmpList[7], 0 );
	
	m_pBmpList[8] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_MISC) );
	m_pDisplay->DDSetColorKey( m_pBmpList[8], RGB(255,255,255) );

	m_pBmpList[9] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_PLAYER2) );
	m_pDisplay->DDSetColorKey( m_pBmpList[9], 0 );

	m_pBmpList[10] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_SPLASH) );
	m_pDisplay->DDSetColorKey( m_pBmpList[10], 0 );

	m_pBmpList[11] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_GAMEOVER) );
	m_pDisplay->DDSetColorKey( m_pBmpList[11], 0 );

	m_pBmpList[12] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_FLAG) );
	m_pDisplay->DDSetColorKey( m_pBmpList[12], RGB(255,255,255) );

	m_pBmpList[13] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_NUM) );
	m_pDisplay->DDSetColorKey( m_pBmpList[13], RGB(255,255,255) );

	m_pBmpList[14] = m_pDisplay->DDLoadBitmap( MAKEINTRESOURCE(IDB_SHIELD) );
	m_pDisplay->DDSetColorKey( m_pBmpList[14], 0 );

	return TRUE;
}



bool CResMgr::RestoreRes()
{
	for( int i = 0; i < NUM_BITMAPS; i ++ )
		m_pBmpList[i]->Restore();

	m_pDisplay->DDReLoadBitmap( m_pBmpList[0], MAKEINTRESOURCE(IDB_TILE) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[1], MAKEINTRESOURCE(IDB_PLAYER1) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[2], MAKEINTRESOURCE(IDB_BULLET) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[3], MAKEINTRESOURCE(IDB_EXPLODE1) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[4], MAKEINTRESOURCE(IDB_EXPLODE2) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[5], MAKEINTRESOURCE(IDB_ENEMY) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[6], MAKEINTRESOURCE(IDB_BONUS) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[7], MAKEINTRESOURCE(IDB_BORE) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[8], MAKEINTRESOURCE(IDB_MISC) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[9], MAKEINTRESOURCE(IDB_PLAYER2) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[10], MAKEINTRESOURCE(IDB_SPLASH) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[11], MAKEINTRESOURCE(IDB_GAMEOVER) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[12], MAKEINTRESOURCE(IDB_FLAG) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[13], MAKEINTRESOURCE(IDB_NUM) );
	m_pDisplay->DDReLoadBitmap( m_pBmpList[14], MAKEINTRESOURCE(IDB_SHIELD) );

	return TRUE;
}


bool CResMgr::UnloadRes()
{
	for( int i = 0; i < NUM_BITMAPS; i ++ )
		SAFE_RELEASE( m_pBmpList[i] );

	return TRUE;
}