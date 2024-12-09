//-----------------------------------------------------------------------------
// File: Util.h
// Desc: Game System Util 2.0
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_UTIL_H_
#define	_GT_UTIL_H_
#pragma once


#define Util	CUtil
//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
class CUtil
{
public:

	static DWORD	GetFreeID();
	
	/*
	// �ø�������������ӳ�ʼ�������������
	static VOID		Randomize(DWORD dwSeed);
	static DWORD	GetRandHolder() { return m_nRandHolder; }

	// �õ������
	static INT		Rand();
	static INT		Random(int min, int max);
	*/

	// IntersectionRect ��Win98����ϵͳ����BUG, ����������дһ��
	static BOOL RectIntersection(RECT& rcIntersection, RECT rc1, RECT rc2);

	CUtil(){ZeroMemory(this, sizeof(*this));}
	~CUtil(){};


private:
	
	static volatile DWORD	m_dwIDHolder;		// ��¼IDʹ��״̬
	static volatile INT		m_nRandHolder;
	static volatile INT		m_nRandCallCount;
};


#endif	// _GT_UTIL_H_