//-----------------------------------------------------------------------------
// File: resMgr
// Desc: resource manager
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_RES_MGR_H_
#define	_BC_RES_MGR_H_
#pragma once

#include <ddraw.h>
#include "gDefine.h"


class CDisplay;

class CResMgr
{
	
	CDisplay*				m_pDisplay;
	LPDIRECTDRAWSURFACE		m_pBmpList[NUM_BITMAPS];

public:

	LPDIRECTDRAWSURFACE GetBmp(int n) { ASSERT(n<NUM_BITMAPS); return m_pBmpList[n]; }

	bool	Init(CDisplay* pDisplay);
	bool	LoadRes();
	bool	RestoreRes();


	bool	UnloadRes();

	CResMgr();
	~CResMgr();
};


#endif	// _BC_RES_MGR_H_