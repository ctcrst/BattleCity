//-----------------------------------------------------------------------------
// File: gSprite
// Desc: sprite controller
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_SPRITE_H_
#define _BC_SPRITE_H_
#pragma once

#include <ddraw.h>

#pragma pack(push)
#pragma pack(1)

enum BONUS
{
	BONUS_LIFE=0,
	BONUS_CLOCK,
	BONUS_SHOVEL,
	BONUS_BOMB,
	BONUS_STAR,
	BONUS_HELMET
};
enum DIRECTION
{
	DIR_UP=0,
	DIR_RIGHT,
	DIR_DOWN,
	DIR_LEFT
};


class CGame;
//-----------------------------------------------------------------------------
// csprite
//-----------------------------------------------------------------------------
class CSprite  
{
public:
	CSprite( CGame* pGame);
	void Create( int w, int h, bool active );
	void BitBlt( LPDIRECTDRAWSURFACE bmp ){ m_bmp = bmp; }
	bool HitTest( CSprite* pSprite );	
	void Draw( LPDIRECTDRAWSURFACE pdds );
	bool IsActive() { return m_bActive; }

public:

	CGame*	m_pGame;
	LPDIRECTDRAWSURFACE m_bmp;

	int		m_x;
	int		m_y;
	int		m_width;
	int		m_height;
	bool	m_bActive;
};




//-----------------------------------------------------------------------------
// bonus
//-----------------------------------------------------------------------------
class CBonus : public CSprite
{
public:

	CBonus(CGame* pGame);
	void Update();
	void Draw( LPDIRECTDRAWSURFACE pdds );

	BONUS	m_type;
	DWORD	m_dwLastTime;
	DWORD	m_dwFlickerTime;
	bool	m_bShow;
};



//-----------------------------------------------------------------------------
// explode
//-----------------------------------------------------------------------------
class CExplode: public CSprite
{
public:

	CExplode( CGame* pGame );
	void BitBlt( LPDIRECTDRAWSURFACE bmp1, LPDIRECTDRAWSURFACE bmp2 );

	void Update();
	void Draw( LPDIRECTDRAWSURFACE pdds );
	
	LPDIRECTDRAWSURFACE m_explode;

	DWORD	m_time;
	bool	m_bExplode;
	
};



//-----------------------------------------------------------------------------
// bore
//-----------------------------------------------------------------------------
class CBore : public CSprite
{
public:

	CBore(CGame* pGame);

	void Update();
	void Draw( LPDIRECTDRAWSURFACE pdds );
	void Bore();

	int	  m_frame;
	DWORD m_time;
	bool  m_bAdvance;
};






//-----------------------------------------------------------------------------
// bullet
//-----------------------------------------------------------------------------
class CBullet: public CSprite
{
public:
	CBullet(CGame* pGame);
	bool Move();
	void Draw( LPDIRECTDRAWSURFACE pdds );

	DIRECTION	m_dir;
	int			m_speed;
	int			m_x2;
	int			m_y2;
};




//-----------------------------------------------------------------------------
// tank
//-----------------------------------------------------------------------------
class CTank: public CBullet
{
public:
	CTank( CGame* pGame );
	~CTank();

	void Create( int w, int h, bool active );
	void BitBlt( LPDIRECTDRAWSURFACE tank, LPDIRECTDRAWSURFACE bullet,
				LPDIRECTDRAWSURFACE shield, LPDIRECTDRAWSURFACE bore );

	void ChangeDirection( DIRECTION dir );
	bool Move(){ m_frame = !m_frame; return CBullet::Move(); }
	bool HitTest( CTank* pSprite, INT oldx, INT oldy );
	bool HitTest( CSprite* pSprite ){ return CSprite::HitTest(pSprite); }
	void UpdateShield();
	void DrawShield( LPDIRECTDRAWSURFACE pdds );
	void Draw( LPDIRECTDRAWSURFACE pdds );
	void Shield( DWORD time );

	LPDIRECTDRAWSURFACE m_bmpShield;
	CBullet*	m_pBullet[2];
	CBore*		m_pBore;

	DWORD	m_time;
	int		m_frame;
	int		m_type;
	DWORD	m_maxTime;
	DWORD	m_shieldTime;
	DWORD	m_flickerTime;
	bool	m_bBoring;
	bool	m_bShield;
	bool	m_bShield1;
};




//-----------------------------------------------------------------------------
// enemy
//-----------------------------------------------------------------------------
class CEnemy : public CTank
{
public:

	CEnemy( CGame* pGame );

	void ChangeDirection();
	bool Fire();
	void Reborn();
	void Update();
	void Draw( LPDIRECTDRAWSURFACE pdds );

	DWORD	m_dwRedTime;
	int		m_nLevel;
	bool	m_bBonus;
	bool	m_bShowRed;
	
};




//-----------------------------------------------------------------------------
// player
//-----------------------------------------------------------------------------
class CPlayer : public CTank
{
public:

	CPlayer( CGame* pGame );

	bool ProcessInput( BYTE byInput );
	bool Fire();
	void Update();
	void Draw( LPDIRECTDRAWSURFACE pdds );
	void Reborn();
	void Lock();

	int		m_nLife;
	int		m_nScore;
	bool	m_bLocked;
	bool	m_bShow;
};


#pragma pack(pop)

#endif // _BC_SPRITE_H_
