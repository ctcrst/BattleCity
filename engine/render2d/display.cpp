//-----------------------------------------------------------------------------
// File: Display
// Desc: ddraw interface
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "..\gtdefine.h"
#include "..\debug\debug.h"

#include "display.h"


//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
CDisplay::CDisplay()
{
	m_bFullScreen = FALSE;
}



//-----------------------------------------------------------------------------
// init direct draw
//-----------------------------------------------------------------------------
bool CDisplay::DDInit(HWND hWnd, DWORD dwWidth, DWORD dwHeight, bool bFullScreen)
{
	HRESULT hr;

	m_bFullScreen = bFullScreen;
	hr = DirectDrawCreate( NULL, &m_pDD, NULL );
	if( FAILED(hr) )
	{
		Debug->Trace( "Error Create DirectDraw\n" );
		return FALSE;
	}

	if( m_bFullScreen )
		m_pDD->SetCooperativeLevel( hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
	else
		m_pDD->SetCooperativeLevel( hWnd, DDSCL_NORMAL );

	if( m_bFullScreen )
	{
		if( FAILED(m_pDD->SetDisplayMode( 640, 480, 16 )) )
		{
			Debug->Trace( "Error SetDiaplayMode\n" );
			return FALSE;
		}
	}

	DDSURFACEDESC	ddsd;
	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS;
	ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if( m_bFullScreen )
	{
		ddsd.dwFlags |= DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps |= DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
	}

	hr = m_pDD->CreateSurface( &ddsd, &m_pddsFrontBuffer, NULL );
	if( FAILED(hr) )
	{
		Debug->Trace( "Error Create Front Buffer\n" );
		return FALSE;
	}

	if( m_bFullScreen )
	{
		DDSCAPS ddscaps;
		ZeroMemory( &ddscaps, sizeof(ddscaps) );
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
		m_pddsFrontBuffer->GetAttachedSurface( &ddscaps, &m_pddsBackBuffer );
	}
	else
	{
		m_pddsBackBuffer = DDCreateSurface( dwWidth, dwHeight );

		// create clipper
		LPDIRECTDRAWCLIPPER pClipper = NULL; 
		m_pDD->CreateClipper(0, &pClipper, NULL);
		pClipper->SetHWnd(0, hWnd);
		m_pddsFrontBuffer->SetClipper(pClipper);
		pClipper->Release();
	}


	if( !m_bFullScreen )
	{
		SetWindowPos( hWnd, 0, 0, 0, 646, 505, SWP_NOMOVE | SWP_NOZORDER );
		m_rcWindow.right = m_rcWindow.left + 640;
		m_rcWindow.bottom = m_rcWindow.top + 480;
	}

	return TRUE;
}




//-----------------------------------------------------------------------------
// Êä³öÎÄ×Ö
//-----------------------------------------------------------------------------
void CDisplay::OutputText( int x, int y, LPCTSTR string )
{
	HDC hdc;
	if( m_pddsBackBuffer && 
		m_pddsBackBuffer->GetDC(&hdc) == DD_OK )
	{
		SetBkMode( hdc, TRANSPARENT );
		SetTextColor( hdc, RGB(255,255,0) );
		TextOut( hdc, x, y, string, lstrlen(string) );
		m_pddsBackBuffer->ReleaseDC( hdc );
	}
}




//-----------------------------------------------------------------------------
// Name: DDLoadBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource.
//-----------------------------------------------------------------------------
LPDIRECTDRAWSURFACE CDisplay::DDLoadBitmap( LPCSTR szBitmap, int dx, int dy)
{
    HBITMAP                 hbm;
    BITMAP                  bm;
    DDSURFACEDESC			ddsd;
    LPDIRECTDRAWSURFACE		pdds;

    //  Try to load the bitmap as a resource, if that fails, try it as a file
    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx,
                              dy, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return NULL;

    // Get size of the bitmap
    GetObject(hbm, sizeof(bm), &bm);

    // Create a DirectDrawSurface for this bitmap
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;
    if (m_pDD->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
        return NULL;
    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);
    return pdds;
}




//-----------------------------------------------------------------------------
// Name: DDCopyBitmap()
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CDisplay::DDCopyBitmap( LPDIRECTDRAWSURFACE pdds, 
						HBITMAP hbm, int x, int y, 
						int dx, int dy)
{
    HDC                     hdcImage;
    HDC                     hdc;
    BITMAP                  bm;
    DDSURFACEDESC			ddsd;
    HRESULT                 hr;

    if (hbm == NULL || pdds == NULL)
        return E_FAIL;

    // Make sure this surface is restored.
    pdds->Restore();

    // Select bitmap into a memoryDC so we can use it.
    hdcImage = CreateCompatibleDC(NULL);
    
	if (!hdcImage)
        Debug->Trace("createcompatible dc failed\n");
    SelectObject(hdcImage, hbm);

    // Get size of the bitmap
    GetObject(hbm, sizeof(bm), &bm);
    dx = dx == 0 ? bm.bmWidth : dx;     // Use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;
    
    // Get size of surface.
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);

    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y,
                   dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }
    DeleteDC(hdcImage);
    return hr;
}


//-----------------------------------------------------------------------------
// Name: DDReLoadBitmap()
// Desc: Load a bitmap from a file or resource into a directdraw surface.
//       normaly used to re-load a surface after a restore.
//-----------------------------------------------------------------------------
HRESULT CDisplay::DDReLoadBitmap(LPDIRECTDRAWSURFACE pdds, LPCSTR szBitmap)
{
    HBITMAP                 hbm;
    HRESULT                 hr;

    //
    //  Try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0,
                              0, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
    {
        OutputDebugString("handle is null\n");
        return E_FAIL;
    }
    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
    {
        OutputDebugString("ddcopybitmap failed\n");
    }
    DeleteObject(hbm);
    return hr;
}


//-----------------------------------------------------------------------------
// Name: DDColorMatch()
// Desc: Convert a RGB color to a pysical color.
//       We do this by leting GDI SetPixel() do the color matching
//       then we lock the memory and see what it got mapped to.
//-----------------------------------------------------------------------------
DWORD CDisplay::DDColorMatch(LPDIRECTDRAWSURFACE pdds, COLORREF rgb)
{
    COLORREF                rgbT;
    HDC                     hdc;
    DWORD                   dw = CLR_INVALID;
    DDSURFACEDESC			ddsd;
    HRESULT                 hres;

    //
    //  Use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, rgb);       // Set our value
        pdds->ReleaseDC(hdc);
    }
    //
    // Now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING)
        ;
    if (hres == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface;                 // Get DWORD
        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  // Mask it to bpp
        pdds->Unlock(NULL);
    }
    //
    //  Now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}



DWORD CDisplay::DDColorMatch(COLORREF color)
{
	return DDColorMatch(m_pddsBackBuffer, color);
}

//-----------------------------------------------------------------------------
// Name: DDSetColorKey()
// Desc: Set a color key for a surface, given a RGB.
//       If you pass CLR_INVALID as the color key, the pixel
//       in the upper-left corner will be used.
//-----------------------------------------------------------------------------
HRESULT CDisplay::DDSetColorKey(LPDIRECTDRAWSURFACE pdds, COLORREF rgb)
{
    DDCOLORKEY              ddck;

    ddck.dwColorSpaceLowValue = DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}



void CDisplay::DDTerm()
{
	if( !m_bFullScreen )
		SAFE_RELEASE( m_pddsBackBuffer );
	SAFE_RELEASE( m_pddsFrontBuffer );
	SAFE_RELEASE( m_pDD );
}



bool CDisplay::FlipScreen()
{
	HRESULT hr;
	
	if( m_bFullScreen )
		hr = m_pddsFrontBuffer->Flip( NULL, DDFLIP_WAIT );
	else
	{
		hr = m_pddsFrontBuffer->Blt( &m_rcWindow, m_pddsBackBuffer,
				NULL, DDBLT_WAIT, NULL );
	}

	if( hr == DDERR_SURFACELOST )
		return false;
	else
		return true;
}





LPDIRECTDRAWSURFACE CDisplay::DDCreateSurface( int width, int height )
{
	DDSURFACEDESC	ddsd;
	ZeroMemory( &ddsd, sizeof(ddsd) );
	ddsd.dwSize = sizeof(ddsd);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
	ddsd.dwWidth = width;
	ddsd.dwHeight = height;

	LPDIRECTDRAWSURFACE pdds;
	if( FAILED(m_pDD->CreateSurface( &ddsd, &pdds, NULL )) )
		return NULL;

	return pdds;
}


void CDisplay::RestoreSurfaces()
{
	m_pddsBackBuffer->Restore();
	m_pddsFrontBuffer->Restore();


}


void CDisplay::DDClear( RECT* prc, DWORD dwFillColor )
{
	DDBLTFX	ddbfx;
	ZeroMemory( &ddbfx, sizeof(ddbfx) );
	ddbfx.dwSize = sizeof(ddbfx);
	ddbfx.dwFillColor = dwFillColor;
	
	m_pddsBackBuffer->Blt( prc, NULL, NULL, 
		DDBLT_COLORFILL | DDBLT_WAIT, &ddbfx );
}

