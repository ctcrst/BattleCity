//-----------------------------------------------------------------------------
// File: gTitle
// Desc: display title of game
// Auth: Lyp
// Date: 2003/11/22
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_TITLE_H_
#define _BC_TITLE_H_
#pragma once

#include "gDefine.h"

class CApp;
class CPlayer;
class CSprite;
class CEnemy;
class CPlane;
class CExplode;
class CBonus;


//-----------------------------------------------------------------------------
// title of game
//-----------------------------------------------------------------------------
class CTitle
{
public:
	CTitle(CApp* pApp);
	~CTitle();

	void	DrawTitle();


private:

	CApp*		m_pApp;
	CPlane*		m_pPlane;

};


#endif	// _BC_TITLE_H_