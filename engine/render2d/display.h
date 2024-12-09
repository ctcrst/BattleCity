//-----------------------------------------------------------------------------
// File: Display
// Desc: ddraw interface
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_2DDISPLAY_H_
#define	_GT_2DDISPLAY_H_
#pragma once

#include <ddraw.h>


#define BLT(a,b,c,d) BltFast(a,b,c,d,DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT)
//-----------------------------------------------------------------------------
// ddraw interface
//-----------------------------------------------------------------------------
class CDisplay
{
public:

	LPDIRECTDRAWSURFACE	GetBackBuffer() { return m_pddsBackBuffer; }
	RECT*				GetWindowRect() { return &m_rcWindow; }

	bool DDInit(HWND hWnd, DWORD dwWidth, DWORD dwHeight, bool bFullScreen);
	void DDTerm();

	bool FlipScreen();
	void RestoreSurfaces();

	__forceinline void 
		DrawPic(int x, int y, LPDIRECTDRAWSURFACE pSurface, RECT* pRect=NULL)
	{
		m_pddsBackBuffer->BLT(x, y, pSurface, pRect);
	}
	void OutputText(int x, int y, LPCTSTR string);


	LPDIRECTDRAWSURFACE DDLoadBitmap( LPCSTR szBitmap, int dx=0, int dy=0 );
	LPDIRECTDRAWSURFACE DDCreateSurface( int width, int height );
	HRESULT DDCopyBitmap( LPDIRECTDRAWSURFACE, HBITMAP hbm, int x, int y, int dx, int dy);
	HRESULT DDReLoadBitmap(LPDIRECTDRAWSURFACE, LPCSTR szBitmap);
	HRESULT DDSetColorKey(LPDIRECTDRAWSURFACE, COLORREF);
	DWORD DDColorMatch(LPDIRECTDRAWSURFACE, COLORREF);
	DWORD DDColorMatch(COLORREF);
	void DDClear( RECT* prc, DWORD dwFillColor );

	CDisplay();


private:

	LPDIRECTDRAW			m_pDD;
	LPDIRECTDRAWSURFACE		m_pddsFrontBuffer;
	LPDIRECTDRAWSURFACE		m_pddsBackBuffer;
	bool					m_bFullScreen;
	RECT					m_rcWindow;

};









#endif // _GT_2DDISPLAY_H