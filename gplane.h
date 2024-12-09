//-----------------------------------------------------------------------------
// File: gPlane
// Desc: background
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_PLANE_H_
#define _BC_PLANE_H_
#pragma once

#include <ddraw.h>

#define PLANE_W		13
#define PLANE_H		13
#define TILE_W		32
#define TILE_H		32

enum{
	OBJ_NULL = -1,
	OBJ_BRICK = 0,
	OBJ_CONCRETE = 1,
	OBJ_TREE = 2,
	OBJ_RIVER = 3,
	OBJ_HAWK = 5,
	OBJ_STONE
};

#pragma pack(push)
#pragma pack(1)


struct TILE
{
	char	obj;
	WORD	mask;
};


class CTank;
class CBullet;
class CGame;
//-----------------------------------------------------------------------------
// class cplane
//-----------------------------------------------------------------------------
class CPlane  
{
public:
	CPlane(CGame* pGame);
	virtual ~CPlane();

	bool Create(int nLevel );
	void BitBlt( LPDIRECTDRAWSURFACE bmp ){ m_bmp = bmp; }
	bool CanHit( int surface )
	{
		return surface==OBJ_BRICK || surface==OBJ_CONCRETE;
	}
	bool CanDestroy( int row, int col, bool bDestroyConcrete )
	{
		return (m_map[row][col].obj == OBJ_BRICK ||
			m_map[row][col].obj == OBJ_CONCRETE && bDestroyConcrete);
	}	
	int HitSurface( CBullet* pBullet, bool bDestroyConcrete=false );
	bool GetSurface( int row, int col, RECT& rcTank, int a, int b );
	int GetSurface( CTank* pTank );
	
	
	void Update(DWORD dwTick);
	void Render( LPDIRECTDRAWSURFACE pdds );


	void DrawTree( LPDIRECTDRAWSURFACE pdds );
	void Protect();
	void Unprotect();
	void Bare();

private:

	CGame*					m_pGame;
	LPDIRECTDRAWSURFACE		m_bmp;

	TILE					m_map[PLANE_H+1][PLANE_W+1];
	DWORD					m_time;
	DWORD					m_protectTime;
	bool					m_bProtected;
	bool					m_bRiver1;
};

#pragma pack(pop)

#endif // _BC_PLANE_H_
