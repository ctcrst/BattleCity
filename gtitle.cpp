//-----------------------------------------------------------------------------
// File: gTitle
// Desc: display title of game
// Auth: Lyp
// Date: 2003/11/22
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "resource.h"

#include "gApp.h"
#include "gTitle.h"
#include "resmgr.h"


//-----------------------------------------------------------------------------
// construction/destruction
//-----------------------------------------------------------------------------
CTitle::CTitle(CApp* pApp)
{
	ZeroMemory(this, sizeof(*this));

	m_pApp = pApp;
	ASSERT(m_pApp);
}

CTitle::~CTitle()
{
}




//-----------------------------------------------------------------------------
// »æÖÆ±êÌâ
//-----------------------------------------------------------------------------
void CTitle::DrawTitle()
{
	HRESULT hr;
	CDisplay* pDisplay = m_pApp->GetDisplay();
	CResMgr* pResMgr = m_pApp->GetResMgr();
	
	pDisplay->DDClear( NULL, 0 );

	hr = pDisplay->GetBackBuffer()->BLT( 135, 130, pResMgr->GetBmp(10), NULL );
	if( hr == DDERR_SURFACELOST )
	{
		pDisplay->RestoreSurfaces();
		pResMgr->RestoreRes();
	}

	RECT rc;
	rc.left = 0;
	rc.right = 28;
	rc.top = 28;
	rc.bottom = 56;

	bool bServer = m_pApp->IsServer();

	pDisplay->GetBackBuffer()->BLT( 200, 300 + 30*(!bServer), pResMgr->GetBmp(1), &rc );

	if( false == pDisplay->FlipScreen() )
	{
		pDisplay->RestoreSurfaces();
		pResMgr->RestoreRes();
	}

}