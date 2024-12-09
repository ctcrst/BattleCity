//-----------------------------------------------------------------------------
// File: gPlane
// Desc: background
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

#define OFFSETX		100
#define OFFSETY		48
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CPlane::CPlane(CGame* pGame)
{
	m_pGame = pGame;

	ZeroMemory(m_map, sizeof(m_map));
	m_time = 0;
	m_protectTime = 0;
	m_bProtected = false;
	m_bRiver1 = true;

	CZoneObjSnapshot* pss = m_pGame->GetApp()->GetSnapshot();
	pss->RegisterVar(m_map);
	pss->RegisterVar(m_time);
	pss->RegisterVar(m_protectTime);
	pss->RegisterVar(m_bProtected);
	pss->RegisterVar(m_bRiver1);
}

CPlane::~CPlane()
{
}


//-----------------------------------------------------------------------------
// 创建地图
//-----------------------------------------------------------------------------
bool CPlane::Create( int nLevel )
{
	HRSRC hResInfo = 0;
	hResInfo = FindResource(NULL, MAKEINTRESOURCE(124+nLevel), "Map");
	if( hResInfo == 0 )
		return false;

	HGLOBAL hMem = LoadResource(NULL, hResInfo);
	BYTE* pMap = (BYTE*)LockResource(hMem);
	BYTE* p = pMap;

	for( int i = 0; i < PLANE_H; i ++ )
	{
		for( int j = 0; j < PLANE_W; j ++ )
		{
			m_map[i][j].obj = *p++;
			m_map[i][j].obj --;
			m_map[i][j].mask = 0xffff;
			if( m_map[i][j].obj == OBJ_BRICK ||
				m_map[i][j].obj == OBJ_CONCRETE )
			{
				switch( *p++ )
				{
				case 0:
					m_map[i][j].mask = 0xff00;
					break;
				case 1:
					m_map[i][j].mask = 0x00ff;
					break;
				case 2:
					m_map[i][j].mask = 0xcccc;
					break;
				case 3:
					m_map[i][j].mask = 0x3333;
					break;
				}
			}
			else if( m_map[i][j].obj == OBJ_NULL )
				m_map[i][j].mask = 0;
		}
	}

	
	Unprotect();
	return true;
}



//-----------------------------------------------------------------------------
// 子弹与地形的碰撞检测
//-----------------------------------------------------------------------------
int CPlane::HitSurface( CBullet* pBullet, bool bDestroyConcrete )
{
	int x = pBullet->m_x2 + pBullet->m_width / 2;
	int y = pBullet->m_y2 + pBullet->m_height / 2;
	int row = y / TILE_H;
	int col = x / TILE_W;

	WORD mask = 0, flag = 0;
	int shift = 0;
	int hit = OBJ_NULL;
	int nTimes = 0;
	
_re_test:

	ASSERT( row>=0 && row<PLANE_H );
	ASSERT( col>=0 && col<PLANE_W );

	switch( pBullet->m_dir )
	{
	case DIR_UP:
	case DIR_DOWN:
		if( !bDestroyConcrete )
			shift = (3 - (y - row*TILE_H) / 8) * 4;
		else
			shift = (1 - (y - row*TILE_H) / 16) * 8;
		
		if( x % TILE_W )
		{
			if( !bDestroyConcrete )
				flag = 0x000f << shift;
			else
				flag = 0x00ff << shift;

			if( m_map[row][col].mask & flag )
			{
				hit = m_map[row][col].obj;
				if( CanDestroy(row, col, bDestroyConcrete) )
					m_map[row][col].mask &= ~flag;
			}
		}
		else
		{
			if( !bDestroyConcrete )
				flag = 0x000c << shift;
			else
				flag = 0x00cc << shift;

			if( m_map[row][col].mask & flag )
			{
				hit = m_map[row][col].obj;
				if( CanDestroy(row, col, bDestroyConcrete) )
					m_map[row][col].mask &= ~flag;
			}

			if( !bDestroyConcrete )
				flag = 0x0003 << shift;
			else
				flag = 0x0033 << shift;

			if( col >= 1 && m_map[row][col-1].mask & flag )
			{
				if( !CanHit( hit ) )
					hit = m_map[row][col-1].obj;
				if( CanDestroy(row, col-1, bDestroyConcrete) )
					m_map[row][col-1].mask &= ~flag;
			}
		}

		if( col >= 1 && m_map[row][col-1].mask == 0 )
			m_map[row][col-1].obj = OBJ_NULL;
		break;

	case DIR_LEFT:
	case DIR_RIGHT:
		if( !bDestroyConcrete )
			shift = (x - col*TILE_W) / 8;
		else
			shift = (x - col*TILE_W) / 16;
		if( y % TILE_H )
		{
			if( !bDestroyConcrete )
			{
				if( shift == 0 )
					mask = 0x8888;
				else if( shift == 1 )
					mask = 0x4444;
				else if( shift == 2 )
					mask = 0x2222;
				else
					mask = 0x1111;
			}
			else
			{
				if( shift == 0 )
					mask = 0xcccc;
				else
					mask = 0x3333;
			}
			if( m_map[row][col].mask & mask )
			{
				hit = m_map[row][col].obj;
				if( CanDestroy(row, col, bDestroyConcrete) )
					m_map[row][col].mask &= ~mask;
			}
		}
		else
		{
			if( !bDestroyConcrete )
			{
				if( shift == 0 )
					mask = 0x8800;
				else if( shift == 1 )
					mask = 0x4400;
				else if( shift == 2 )
					mask = 0x2200;
				else
					mask = 0x1100;
			}
			else
			{
				if( shift == 0 )
					mask = 0xcc00;
				else
					mask = 0x3300;
			}

			if( m_map[row][col].mask & mask )
			{
				hit = m_map[row][col].obj;
				if( CanDestroy(row, col, bDestroyConcrete) )
					m_map[row][col].mask &= ~mask;
			}
			mask >>= 8;

			if( row >= 1 && m_map[row-1][col].mask & mask )
			{
				if( !CanHit( hit ) )
					hit = m_map[row-1][col].obj;
				if( CanDestroy(row-1, col, bDestroyConcrete) )
					m_map[row-1][col].mask &= ~mask;
			}
		}
		if( row >= 1 && m_map[row-1][col].mask == 0 )
			m_map[row-1][col].obj = OBJ_NULL;
		break;
	}

	if( m_map[row][col].mask == 0 )
		m_map[row][col].obj = OBJ_NULL;

	if( hit == OBJ_HAWK )
		m_map[12][6].obj = OBJ_STONE;

	if( hit == OBJ_NULL && nTimes <= 0 )
	{
		x = pBullet->m_x + pBullet->m_width / 2;
		y = pBullet->m_y + pBullet->m_height / 2;
		nTimes++;
		goto _re_test;
	}


	return hit;
}


bool CPlane::GetSurface( int row, int col, RECT& rcTank, int a, int b)
{
	ASSERT( row>=0 && row<PLANE_H+1 );
	ASSERT( col>=0 && col<PLANE_W+1 );

	int obj = m_map[row][col].obj;
	if( obj == OBJ_NULL || obj == OBJ_TREE )
		return false;
	
	WORD mask = m_map[row][col].mask;
	WORD flag = 0;

	for( int i = 0; i < 4; i ++ )
	{
		if( (i == a || i == b) || (a == -1 && b == -1) )
		{
			RECT rc, temp;
			rc.left = col * TILE_W + i % 2 * 16;
			rc.right = rc.left + 16;
			rc.top = row * TILE_H + i / 2 * 16;
			rc.bottom = rc.top + 16;
			temp.left = temp.right = temp.top = temp.bottom = 0;

			switch( i )
			{
			case 0:
				flag = 0xcc00;
				break;
			case 1:
				flag = 0x3300;
				break;
			case 2:
				flag = 0x00cc;
				break;
			case 3:
				flag = 0x0033;
				break;
			}

			if( (mask & flag) && Util::RectIntersection( temp, rcTank, rc ) )
				return true;
		}
	}

	return false;
}


int CPlane::GetSurface( CTank* pTank )
{
	int x1 = pTank->m_x;
	int y1 = pTank->m_y;
	int x2 = x1 + pTank->m_width;
	int y2 = y1 + pTank->m_height;
	int x = x1 + pTank->m_width / 2;
	int y = y1 + pTank->m_height / 2;
	int row = 0, col = 0;
	
	RECT rcTank;
	rcTank.left = pTank->m_x;
	rcTank.right = rcTank.left + pTank->m_width;
	rcTank.top = pTank->m_y;
	rcTank.bottom = rcTank.top + pTank->m_height;

	ASSERT( row>=0 && row<PLANE_H );
	ASSERT( col>=0 && col<PLANE_W );

	if( pTank->m_dir == DIR_UP || pTank->m_dir == DIR_DOWN )
	{
		col = x / TILE_W;
		if( pTank->m_dir == DIR_UP )
			row = y1 / TILE_H;
		else
			row = y2 / TILE_H;

		if( x % TILE_W )
		{
			if( GetSurface( row, col, rcTank, -1, -1 ) )
				return m_map[row][col].obj;
		}
		else
		{
			if( GetSurface( row, col-1, rcTank, 1, 3 ) )
				return m_map[row][col-1].obj;
			if( GetSurface( row, col, rcTank, 0, 2 ) )
				return m_map[row][col].obj;
		}
	}
	else
	{
		row = y / TILE_H;
		if( pTank->m_dir == DIR_LEFT )
			col = x1 / TILE_W;
		else
			col = x2 / TILE_W;

		if( y % TILE_H )
		{
			if( GetSurface( row, col, rcTank, -1, -1 ) )
				return m_map[row][col].obj;
		}
		else
		{
			if( GetSurface( row-1, col, rcTank, 2, 3 ) )
				return m_map[row-1][col].obj;
			if( GetSurface( row, col, rcTank, 0, 1 ) )
				return m_map[row][col].obj;
		}
	}
	return OBJ_NULL;
}



//-----------------------------------------------------------------------------
// update
//-----------------------------------------------------------------------------
void CPlane::Update(DWORD dwTick)
{
	if( dwTick - m_time > m_pGame->GetApp()->GetGameFreq() / 2 )
	{
		m_bRiver1 = !m_bRiver1;
		m_time = dwTick;
	}

	if( m_bProtected &&
		dwTick - m_protectTime > 20 * m_pGame->GetApp()->GetGameFreq() )
	{
		Unprotect();
	}
}



//-----------------------------------------------------------------------------
// render
//-----------------------------------------------------------------------------
void CPlane::Render( LPDIRECTDRAWSURFACE pdds )
{
	int x = 0, y = 0;
	WORD mask = 0;
	RECT rc = { 0,0,0,0 };
	int obj = 0;
	int i = 0, j = 0;
	
	for( i = 0; i < PLANE_H; i ++ )
	{
		for( j = 0; j < PLANE_W; j ++ )
		{
			obj = m_map[i][j].obj;
			if( obj == OBJ_NULL || obj == OBJ_TREE )
				continue;

			x = j * TILE_W + OFFSETX;
			y = i * TILE_H + OFFSETY;
			mask = m_map[i][j].mask;

			rc.left = obj * TILE_W;
			rc.right = rc.left + TILE_W;
			rc.top = 0;
			rc.bottom = TILE_H;

			if( mask == 0xffff )
			{
				if( obj == OBJ_RIVER && !m_bRiver1 )
				{
					RECT rcRiver;
					rcRiver.left = rc.left + TILE_W;
					rcRiver.right = rcRiver.left + TILE_W;
					rcRiver.top = 0;
					rcRiver.bottom = TILE_H;
					pdds->BLT( x, y, m_bmp, &rcRiver );
				}
				else
					pdds->BLT( x, y, m_bmp, &rc );
			}
			else
			{
				for( int cnt = 0; cnt < 16; cnt ++ )
				{
					if( mask & 0x8000 )
					{	
						rc.left = obj * TILE_W + cnt % 4 * 8;
						rc.top = cnt / 4 * 8;
						rc.right = rc.left + 8;
						rc.bottom = rc.top + 8;
						pdds->BLT( x + cnt%4*8, y + cnt/4*8, m_bmp, &rc );
					}
					mask <<= 1;
				}
			}

		} // for j
	} // for i

}





//-----------------------------------------------------------------------------
// 显示树，因为树要显示在坦克之上，所以单独提取出来
//-----------------------------------------------------------------------------
void CPlane::DrawTree( LPDIRECTDRAWSURFACE pdds )
{
	int obj = 0;
	int x = 0, y = 0;
	int i = 0, j = 0;
	RECT rc = { 0,0,0,0 };

	for( i = 0; i < PLANE_H; i ++ )
	{
		for( j = 0; j < PLANE_W; j ++ )
		{
			obj = m_map[i][j].obj;
			if( obj == OBJ_TREE )
			{
				x = j * TILE_W + OFFSETX;
				y = i * TILE_H + OFFSETY;
				
				rc.left = obj * TILE_W;
				rc.right = rc.left + TILE_W;
				rc.top = 0;
				rc.bottom = TILE_H;
				pdds->BLT( x, y, m_bmp, &rc );
			}
		}
	}
}


//-----------------------------------------------------------------------------
// 基地被保护
//-----------------------------------------------------------------------------
void CPlane::Protect()
{
	m_map[12][5].obj = OBJ_CONCRETE;
	m_map[12][5].mask = 0x3333;
	m_map[11][5].obj = OBJ_CONCRETE;
	m_map[11][5].mask = 0x0033;
	m_map[11][6].obj = OBJ_CONCRETE;
	m_map[11][6].mask = 0x00ff;
	m_map[11][7].obj = OBJ_CONCRETE;
	m_map[11][7].mask = 0x00cc;
	m_map[12][7].obj = OBJ_CONCRETE;
	m_map[12][7].mask = 0xcccc;

	m_protectTime = m_pGame->GetTick();
	m_bProtected = true;
}


//-----------------------------------------------------------------------------
// 基地取消保护
//-----------------------------------------------------------------------------
void CPlane::Unprotect()
{
	m_map[12][5].obj = OBJ_BRICK;
	m_map[12][5].mask = 0x3333;
	m_map[11][5].obj = OBJ_BRICK;
	m_map[11][5].mask = 0x0033;
	m_map[11][6].obj = OBJ_BRICK;
	m_map[11][6].mask = 0x00ff;
	m_map[11][7].obj = OBJ_BRICK;
	m_map[11][7].mask = 0x00cc;
	m_map[12][7].obj = OBJ_BRICK;
	m_map[12][7].mask = 0xcccc;
	m_bProtected = false;
}


//-----------------------------------------------------------------------------
// 基地周围清空
//-----------------------------------------------------------------------------
void CPlane::Bare()
{
	m_map[12][5].obj = OBJ_NULL;
	m_map[12][5].mask = 0;
	m_map[11][5].obj = OBJ_NULL;
	m_map[11][5].mask = 0;
	m_map[11][6].obj = OBJ_NULL;
	m_map[11][6].mask = 0;
	m_map[11][7].obj = OBJ_NULL;
	m_map[11][7].mask = 0;
	m_map[12][7].obj = OBJ_NULL;
	m_map[12][7].mask = 0;
	m_bProtected = false;
}