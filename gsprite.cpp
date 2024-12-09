//-----------------------------------------------------------------------------
// File: gSprite
// Desc: sprite controller
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "gApp.h"
#include "gGame.h"
#include "gSprite.h"


#define	MAX_X		416
#define MAX_Y		416
#define TILE_W		32
#define TILE_H		32
#define BULLET_W	8
#define BULLET_H	8
#define OFFSETX		100
#define OFFSETY		48


//-----------------------------------------------------------------------------
// CSprite Construction/Destruction
//-----------------------------------------------------------------------------
CSprite::CSprite(CGame* pGame)
{
	m_pGame = pGame;

	m_x = 0;
	m_y = 0;
	m_width = 0;
	m_height = 0;
	m_bActive = false;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_x);
	pss->RegisterVar(m_y);
	pss->RegisterVar(m_width);
	pss->RegisterVar(m_height);
	pss->RegisterVar(m_bActive);
}


void CSprite::Create( int w, int h, bool active )
{
	m_width = w;
	m_height = h; 
	m_bActive = active;
}



bool CSprite::HitTest( CSprite* pSprite )
{
	if( !m_bActive || !pSprite->m_bActive )
		return false;
	
	RECT rc1={0,0,0,0}, rc2={0,0,0,0}, temp={0,0,0,0};
	
	rc1.left = m_x;
	rc1.right = rc1.left + m_width;
	rc1.top = m_y;
	rc1.bottom = rc1.top + m_height;
	
	rc2.left = pSprite->m_x;
	rc2.right = rc2.left + pSprite->m_width;
	rc2.top = pSprite->m_y;
	rc2.bottom = rc2.top + pSprite->m_height;

	return (Util::RectIntersection( temp, rc1, rc2 ) == TRUE);
}


void CSprite::Draw( LPDIRECTDRAWSURFACE pdds )
{
	if( !m_bActive )
		return;

	m_pGame->GetApp()->GetDisplay()->DrawPic(m_x + OFFSETX, m_y + OFFSETY, m_bmp);
}


//-----------------------------------------------------------------------------
// CBonus Construction/Destruction
//-----------------------------------------------------------------------------
CBonus::CBonus(CGame* pGame):CSprite(pGame) 
{
	m_type = BONUS_LIFE;
	m_dwLastTime = 0;
	m_dwFlickerTime = 0;
	m_bShow = false;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_type);
	pss->RegisterVar(m_dwLastTime);
	pss->RegisterVar(m_dwFlickerTime);
	pss->RegisterVar(m_bShow);
}


void CBonus::Update()
{
	if( !m_bActive )
		return;

	if( m_pGame->GetTick() - m_dwFlickerTime > m_pGame->GetApp()->GetGameFreq() / 5 )
	{
		m_dwFlickerTime = m_pGame->GetTick();
		m_bShow = !m_bShow;
	}	

	if( m_pGame->GetTick() - m_dwLastTime > 10 * m_pGame->GetApp()->GetGameFreq() )
		m_bActive = false;
}


void CBonus::Draw( LPDIRECTDRAWSURFACE pdds )
{
	if( !m_bActive || !m_bShow )
		return;
	
	RECT rc;
	rc.left = (int)m_type * m_width;
	rc.right = rc.left + m_width;
	rc.top = 0;
	rc.bottom = m_height;

	pdds->BLT( m_x + OFFSETX, m_y + OFFSETY, m_bmp, &rc );
}


//-----------------------------------------------------------------------------
// CExplode Construction/Destruction
//-----------------------------------------------------------------------------
CExplode::CExplode( CGame* pGame ):CSprite(pGame) 
{
	m_time = 0;
	m_bExplode = false;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_time);
	pss->RegisterVar(m_bExplode);
}


void CExplode::BitBlt( LPDIRECTDRAWSURFACE bmp1, LPDIRECTDRAWSURFACE bmp2 )
{
	m_bmp = bmp1;
	m_explode = bmp2;
}


void CExplode::Update()
{
	if( !m_bActive )
		return;

	DWORD time = m_pGame->GetTick() - m_time;

	if( m_bExplode )
	{
		if( time > m_pGame->GetApp()->GetGameFreq() / 5 )
			m_bActive = false;
	}
	else
	{
		if( time > m_pGame->GetApp()->GetGameFreq() / 14 )
			m_bActive = false;
	}
}


void CExplode::Draw( LPDIRECTDRAWSURFACE pdds )
{
	if( !m_bActive )
		return;

	DWORD time = m_pGame->GetTick() - m_time;
	int x = m_x - 20 + OFFSETX;
	int y = m_y - 20 + OFFSETY;

	if( m_bExplode )
	{
		if( time < m_pGame->GetApp()->GetGameFreq()/20 
			|| time > m_pGame->GetApp()->GetGameFreq() / 7 )
			CSprite::Draw( pdds );
		else
			pdds->BLT( x, y, m_explode, NULL );
	}
	else
	{
		CSprite::Draw( pdds );
	}
}


//-----------------------------------------------------------------------------
// CBore Construction/Destruction
//-----------------------------------------------------------------------------
CBore::CBore(CGame* pGame):CSprite(pGame) 
{
	m_frame = 0;
	m_time = 0;
	m_bAdvance = false;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_frame);
	pss->RegisterVar(m_time);
	pss->RegisterVar(m_bAdvance);
}


void CBore::Update()
{
	if( !m_bActive )
		return;

	if( m_pGame->GetTick() - m_time > m_pGame->GetApp()->GetGameFreq() / 10 )
	{
		m_time = m_pGame->GetTick();
		if( m_bAdvance )
		{
			if( ++m_frame > 3 )
			{
				m_frame = 2;
				m_bAdvance = false;
			}
		}
		else
		{
			if( --m_frame < 0 )
			{
				m_frame = 1;
				m_bAdvance = true;
			}
		}
	}
}


void CBore::Draw( LPDIRECTDRAWSURFACE pdds )
{
	if( !m_bActive )
		return;

	RECT rc;
	rc.left = m_frame * m_width;
	rc.right = rc.left + m_width;
	rc.top = 0;
	rc.bottom = rc.top + m_height;

	pdds->BLT( m_x + OFFSETX, m_y + OFFSETY, m_bmp, &rc );
}



void CBore::Bore()
{
	m_bActive = true;
	m_bAdvance = true;
	m_frame = 0;
	m_time = m_pGame->GetTick();
}




//-----------------------------------------------------------------------------
// CBullet Construction/Destruction
//-----------------------------------------------------------------------------
CBullet::CBullet(CGame* pGame):CSprite(pGame)
{
	m_dir = DIR_UP;
	m_speed = 0;
	m_x2 = 0;
	m_y2 = 0;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_dir);
	pss->RegisterVar(m_speed);
	pss->RegisterVar(m_x2);
	pss->RegisterVar(m_y2);
}


bool CBullet::Move()
{
	if( !m_bActive )
		return false;

	m_x2 = m_x;
	m_y2 = m_y;

	switch( m_dir )
	{
	case DIR_UP:
		m_y -= m_speed;
		break;
	case DIR_DOWN:
		m_y += m_speed;
		break;
	case DIR_LEFT:
		m_x -= m_speed;
		break;
	case DIR_RIGHT:
		m_x += m_speed;
		break;
	}

	m_x2 += (int)((float)(m_x - m_x2) / 2.0f); 
	m_y2 += (int)((float)(m_y - m_y2) / 2.0f); 
	
	if( m_x >= 0 && m_x + m_width <= MAX_X &&
		m_y >= 0 && m_y + m_height <= MAX_Y )
		return true;

	if( m_x < 0 )
		m_x = 0;
	else 
		if( m_x + m_width > MAX_X )
			m_x = MAX_X - m_width;

	if( m_y < 0 )
		m_y = 0;
	else 
		if( m_y + m_height > MAX_Y )
			m_y = MAX_Y - m_height;

	return false;
}


void CBullet::Draw( LPDIRECTDRAWSURFACE pdds )
{
	if( !m_bActive )
		return;
	
	RECT rc;
	rc.left = m_dir * m_width;
	rc.top = 0;
	rc.right = rc.left + m_width;
	rc.bottom = m_height;
	
	pdds->BLT( m_x + OFFSETX, m_y + OFFSETY, m_bmp, &rc );
}



//-----------------------------------------------------------------------------
// CTank Construction/Destruction
//-----------------------------------------------------------------------------
CTank::CTank(CGame* pGame):CBullet(pGame)
{
	m_pBullet[0] = new CBullet(pGame);
	m_pBullet[1] = new CBullet(pGame);
	m_pBore = new CBore(pGame);

	m_pBullet[0]->Create( BULLET_W, BULLET_H, false );
	m_pBullet[1]->Create( BULLET_W, BULLET_H, false );	

	m_time = 0;
	m_frame = 0;
	m_type = 0;
	m_maxTime = 0;
	m_shieldTime = 0;
	m_flickerTime = 0;
	m_bBoring = false;
	m_bShield = false;
	m_bShield1 = false;
	
	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_time);
	pss->RegisterVar(m_frame);
	pss->RegisterVar(m_type);
	pss->RegisterVar(m_maxTime);
	pss->RegisterVar(m_shieldTime);
	pss->RegisterVar(m_flickerTime);
	pss->RegisterVar(m_bBoring);
	pss->RegisterVar(m_bShield);
	pss->RegisterVar(m_bShield1);
}


CTank::~CTank()
{
	SAFE_DEL(m_pBullet[0]);
	SAFE_DEL(m_pBullet[1]);
	SAFE_DEL(m_pBore);
}


void CTank::Create( int w, int h, bool active )
{
	CBullet::Create( w, h, active );
	m_pBore->Create( 32, 32, true );
}	



void CTank::BitBlt( LPDIRECTDRAWSURFACE tank, LPDIRECTDRAWSURFACE bullet,
			LPDIRECTDRAWSURFACE shield, LPDIRECTDRAWSURFACE bore )
{
	m_bmp = tank;
	m_bmpShield = shield;
	m_pBullet[0]->BitBlt( bullet );
	m_pBullet[1]->BitBlt( bullet );
	m_pBore->BitBlt( bore );
}



void CTank::ChangeDirection( DIRECTION dir )
{
	if( m_dir != dir )
	{
		m_dir = dir;
		int row = m_y / TILE_H;
		int col = m_x / TILE_W;
		int xoffset = m_x % TILE_W;
		int yoffset = m_y % TILE_H;
		if( xoffset <= 10 )
			m_x = (col * TILE_W + 2);
		else if( xoffset < TILE_W - 6 )
			m_x = (col * TILE_W + 18);
		else
			m_x = (col * TILE_W + 34);

		if( yoffset <= 10 )
			m_y = (row * TILE_H + 2);
		else if( yoffset < TILE_H - 6 )
			m_y = (row * TILE_H + 18);
		else
			m_y = (row * TILE_H + 34);
	}
}



bool CTank::HitTest( CTank* pSprite, int oldx, int oldy )
{
	if( !m_bActive || !pSprite->m_bActive || m_bBoring || pSprite->m_bBoring )
		return false;
	
	int x1 = pSprite->m_x;
	int y1 = pSprite->m_y;
	int x2 = x1 + pSprite->m_width;
	int y2 = y1 + pSprite->m_height;
	int newx = m_x;
	int newy = m_y;
	switch( m_dir )
	{
	case DIR_UP:
		if( newx <= x2 && newx + m_width >= x1 &&
			oldy >= y2 && newy <= y2 )
			return true;
		break;
	case DIR_DOWN:
		if( newx <= x2 && newx + m_width >= x1 &&
			oldy + m_height <= y1 && newy + m_height >= y1 )
			return true;
		break;
	case DIR_RIGHT:
		if( newy <= y2 && newy + m_height >= y1 &&
			oldx + m_width <= x1 && newx + m_width >= x1 )
			return true;
		break;
	case DIR_LEFT:
		if( newy <= y2 && newy + m_height >= y1 &&
			oldx >= x2 && newx <= x2 )
			return true;
		break;
	}

	return false;
}


void CTank::UpdateShield()
{
	if( m_pGame->GetTick() - m_flickerTime > m_pGame->GetApp()->GetGameFreq() / 20 )
	{
		m_bShield1 = !m_bShield1;
		m_flickerTime = m_pGame->GetTick();
	}

	if( m_pGame->GetTick() - m_shieldTime > m_maxTime )
		m_bShield = false;
}



void CTank::DrawShield( LPDIRECTDRAWSURFACE pdds )
{
	RECT rc;
	rc.left = 0;
	rc.top = m_bShield1 * 32;
	rc.right = rc.left + 32;
	rc.bottom = rc.top + 32;
	
	pdds->BLT( m_x - 2 + OFFSETX, m_y - 2 + OFFSETY, m_bmpShield, &rc );
}



void CTank::Shield( DWORD time )
{
	m_bShield1 = true;
	m_bShield = true;
	m_maxTime = time;
	m_shieldTime = m_flickerTime = m_pGame->GetTick();
}



//-----------------------------------------------------------------------------
// CEnemy Construction/Destruction
//-----------------------------------------------------------------------------
CEnemy::CEnemy( CGame* pGame ):CTank(pGame) 
{
	m_dwRedTime = 0;
	m_nLevel = 0;
	m_bBonus = false;
	m_bShowRed = false;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_dwRedTime);
	pss->RegisterVar(m_nLevel);
	pss->RegisterVar(m_bBonus);
	pss->RegisterVar(m_bShowRed);
}


void CEnemy::ChangeDirection()
{
	CTank::ChangeDirection( (DIRECTION)(m_pGame->Rand() % 4) );
}


bool CEnemy::Fire()
{
	if( m_pBullet[0]->m_bActive ||
		m_pGame->GetTick() - m_time < m_pGame->GetApp()->GetGameFreq() / 6 )
		return false;

	m_time = m_pGame->GetTick();
	m_pBullet[0]->m_bActive = true;
	m_pBullet[0]->m_speed = 3;
	m_pBullet[0]->m_dir = m_dir;
	
	switch( m_dir )
	{
	case DIR_UP:
		m_pBullet[0]->m_x = m_x + m_width/2 - 4;
		m_pBullet[0]->m_y = m_y;
		break;
	case DIR_DOWN:
		m_pBullet[0]->m_x = m_x + m_width/2 - 4;
		m_pBullet[0]->m_y = m_y + m_height - 8;
		break;
	case DIR_LEFT:
		m_pBullet[0]->m_x = m_x;
		m_pBullet[0]->m_y = m_y + m_height/2 - 4;
		break;
	case DIR_RIGHT:
		m_pBullet[0]->m_x = m_x + m_width - 8;
		m_pBullet[0]->m_y = m_y + m_height/2 - 4;
		break;
	}

	return true;
}


void CEnemy::Reborn()
{ 
	m_bActive = true;
	m_bShield = false;
	m_bBoring = true;
	m_pBore->m_x = m_x - 2;
	m_pBore->m_y = m_y - 2;
	m_pBore->Bore();
	m_shieldTime = m_pGame->GetTick();
}


void CEnemy::Update()
{
	if( !m_bActive )
		return;

	if( m_bBoring )
	{
		m_pBore->Update();
		if( m_pGame->GetTick() - m_shieldTime > m_pGame->GetApp()->GetGameFreq() )
			m_bBoring = false;
		return;
	}

	if( m_bBonus &&
		m_pGame->GetTick() - m_dwRedTime > m_pGame->GetApp()->GetGameFreq() / 12 )
	{	
		m_dwRedTime = m_pGame->GetTick();
		m_bShowRed = !m_bShowRed;
	}

	if( m_bShield )
		UpdateShield();
}


void CEnemy::Draw( LPDIRECTDRAWSURFACE pdds )
{
	m_pBullet[0]->Draw( pdds );

	if( !m_bActive )
		return;

	if( m_bBoring )
	{
		m_pBore->Draw( pdds );
		return;
	}

	int x = m_x + OFFSETX;
	int y = m_y + OFFSETY;
	RECT rc = {0,0,0,0};

	if( m_bBonus && m_bShowRed )
	{
		switch( m_type )
		{
		case 0:
		case 1:
			rc.top = m_dir * m_height;
			rc.left = (2 + m_frame + 4*m_type) * m_width;
			break;
		case 2:
			rc.top = (m_dir + 4) * m_height;
			rc.left = 6 * m_width;
			break;
		}

		rc.right = rc.left + m_width;
		rc.bottom = rc.top + m_height;
		pdds->BLT( x, y, m_bmp, &rc );
	}
	else
	{
		switch( m_type )
		{
		case 0:
		case 1:
			rc.top = m_dir * m_height;
			rc.left = (m_type * 4 + m_frame) * m_width;
			break;
		case 2:
			rc.left = ((2-m_nLevel)*2 + m_frame) * m_width;
			rc.top = (m_dir + 4) * m_height;
			break;
		}
		rc.right = rc.left + m_width;
		rc.bottom = rc.top + m_height;
		pdds->BLT( x, y, m_bmp, &rc );
	}

	if( m_bShield )
		DrawShield( pdds );
}




//-----------------------------------------------------------------------------
// CPlayer Construction/Destruction
//-----------------------------------------------------------------------------
CPlayer::CPlayer( CGame* pGame ):CTank(pGame)
{
	m_nLife = 0;
	m_nScore = 0;
	m_bLocked = false;
	m_bShow = false;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_nLife);
	pss->RegisterVar(m_nScore);
	pss->RegisterVar(m_bLocked);
	pss->RegisterVar(m_bShow);	
}


bool CPlayer::Fire()
{
	int speed=0;
	switch( m_type )
	{
	case 0:
		speed = 6;
		break;
	case 1:
		speed = 7;
		break;
	case 2:
	case 3:
		speed = 8;
		break;
	}

	if( m_pGame->GetTick() - m_time < m_pGame->GetApp()->GetGameFreq() / 5 )
		return false;
	
	int i=0;
	for( i = 0; i < 2; i ++ )
		if( !m_pBullet[i]->m_bActive )
			break;
	
	if( i >= 2 || i != 0 && m_type < 2)
		return false;

	m_time = m_pGame->GetTick();
	m_pBullet[i]->m_bActive = true;
	m_pBullet[i]->m_speed = speed;
	m_pBullet[i]->m_dir = m_dir;
	
	switch( m_dir )
	{
	case DIR_UP:
		m_pBullet[i]->m_x = m_x + m_width/2 - 4;
		m_pBullet[i]->m_y = m_y + 4;
		break;
	case DIR_DOWN:
		m_pBullet[i]->m_x = m_x + m_width/2 - 4;
		m_pBullet[i]->m_y = m_y + m_height - 12;
		break;
	case DIR_LEFT:
		m_pBullet[i]->m_x = m_x + 4;
		m_pBullet[i]->m_y = m_y + m_height/2 - 4;
		break;
	case DIR_RIGHT:
		m_pBullet[i]->m_x = m_x + m_width - 12;
		m_pBullet[i]->m_y = m_y + m_height/2 - 4;
	}

	m_pBullet[i]->m_x2 = m_pBullet[i]->m_x;
	m_pBullet[i]->m_y2 = m_pBullet[i]->m_y;
	return true;
}



bool CPlayer::ProcessInput( BYTE byInput )
{
	if( !m_bLocked )
	{
		DIRECTION dir=DIR_UP;

		if( byInput & KEY_UP )
			dir = DIR_UP;
		else if( byInput & KEY_DOWN )
			dir = DIR_DOWN;
		else if( byInput & KEY_LEFT )
			dir = DIR_LEFT;
		else if( byInput & KEY_RIGHT )
			dir = DIR_RIGHT;
	
		if( byInput & KEY_DIRECTION )
		{
			if( m_dir == dir )
				Move();
			else
				ChangeDirection( dir );
		}
	}

	if( byInput & KEY_FIRE )
		return Fire();

	return false;
}



void CPlayer::Reborn()
{ 
	m_bActive = true;
	m_bShield = true;
	m_bBoring = true;
	m_bLocked = false;
	m_pBore->m_x = m_x - 2;
	m_pBore->m_y = m_y - 2;
	m_pBore->Bore();
	m_shieldTime = m_pGame->GetTick();
	m_dir = DIR_UP;
	m_speed = 2;
}



void CPlayer::Lock()
{ 
	m_bLocked = true;
	m_shieldTime = m_flickerTime = m_pGame->GetTick();
}


void CPlayer::Update()
{
	if( !m_bActive )
		return;
	
	if( m_bBoring )
	{
		m_pBore->Update();
		if( m_pGame->GetTick() - m_shieldTime > m_pGame->GetApp()->GetGameFreq() )
		{
			m_bBoring = false;
			Shield( 3 * m_pGame->GetApp()->GetGameFreq() );
		}
		return;
	}

	if( m_bLocked )
	{
		if( m_pGame->GetTick() - m_flickerTime > m_pGame->GetApp()->GetGameFreq() / 5 )
		{
			m_bShow = !m_bShow;
			m_flickerTime = m_pGame->GetTick();
		}
		else if( m_pGame->GetTick() - m_shieldTime > m_pGame->GetApp()->GetGameFreq() * 5 )
			m_bLocked = false;
	}
	
	if( m_bShield )
		UpdateShield();
}


void CPlayer::Draw( LPDIRECTDRAWSURFACE pdds )
{
	if( !m_bActive )
		return;
	
	if( m_bBoring )
	{
		m_pBore->Draw( pdds );
		return;
	}

	if( !m_bLocked || m_bShow )
	{
		RECT rc;
		rc.left = (m_type * 2 + m_frame) * m_width;
		rc.top = m_dir * m_height;
		rc.right = rc.left + m_width;
		rc.bottom = rc.top + m_height;

		pdds->BLT( m_x + OFFSETX, m_y + OFFSETY, m_bmp, &rc );
	}
		
	if( m_bShield )
		DrawShield( pdds );

	m_pBullet[0]->Draw( pdds );
	m_pBullet[1]->Draw( pdds );
}