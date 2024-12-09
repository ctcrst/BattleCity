//-----------------------------------------------------------------------------
// File: Util.cpp
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include <tchar.h>

#include "..\debug\debug.h"
#include "..\debug\log.h"
#include "Util.h"



static CUtil	g_util;

volatile DWORD	CUtil::m_dwIDHolder=1;		// 记录ID使用状态
volatile INT	CUtil::m_nRandHolder=0;
volatile INT	CUtil::m_nRandCallCount=0;
//-----------------------------------------------------------------------------
// 得到下一个空闲的ID
//-----------------------------------------------------------------------------
DWORD CUtil::GetFreeID()
{
	if (m_dwIDHolder >= 0x7fffffff)	// 此种情况应该永远不会发生
		m_dwIDHolder = 1;
	return m_dwIDHolder++;
}


/*
//-----------------------------------------------------------------------------
// 用给定的随机数种子初始化随机数发生器
//-----------------------------------------------------------------------------
VOID CUtil::Randomize(DWORD dwSeed)
{
	m_nRandHolder = (int)dwSeed;
	m_nRandCallCount = 0;
}


//-----------------------------------------------------------------------------
// 得到随机数
//-----------------------------------------------------------------------------
INT CUtil::Rand()
{
	static bool b=false;
	m_nRandCallCount++;
	if( m_nRandHolder == 973215232 )
		b=true;

	if( b )
	{
		__asm int 3;
	}

//	if(b)
//	{
//		LOG->Write("%lu", m_nRandHolder);
//	}

	return (((m_nRandHolder = m_nRandHolder * 214013L + 2531011L) >> 16) & 0x7fff);
}


//-----------------------------------------------------------------------------
// 得到随机数
//-----------------------------------------------------------------------------
INT CUtil::Random( int min, int max )
{
	return (min + Rand() % (max - min + 1));
}
*/

//-----------------------------------------------------------------------------
// IntersectionRect 在Win98操作系统下有BUG, 这里我们重写一个
//-----------------------------------------------------------------------------
BOOL CUtil::RectIntersection(RECT& rcIntersection, RECT rc1, RECT rc2)
{

	if (rc1.left > rc2.right || rc2.left > rc1.right 
		|| rc1.top > rc2.bottom || rc2.top > rc1.bottom)
		return FALSE;	// 两矩形不相交

	if (rc1.left > rc2.left)
		rcIntersection.left = rc1.left;
	else
		rcIntersection.left = rc2.left;
	
	if (rc1.right < rc2.right)
		rcIntersection.right = rc1.right;
	else
		rcIntersection.right = rc2.right;
	
	if (rc1.top > rc2.top)
		rcIntersection.top = rc1.top;
	else
		rcIntersection.top = rc2.top;
	
	if (rc1.bottom < rc2.bottom)
		rcIntersection.bottom = rc1.bottom;
	else
		rcIntersection.bottom = rc2.bottom;
	
	return TRUE;
	
}



