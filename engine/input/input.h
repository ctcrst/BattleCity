//-----------------------------------------------------------------------------
// File: input
// Desc: interface for directx input
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_DX8INPUT_H_
#define	_GT_DX8INPUT_H_
#pragma once

#define DIRECTINPUT_VERSION 0x0500	// 此行必须在前
#include <dinput.h>


#define KEY_UP				0x01
#define	KEY_RIGHT			0x02
#define KEY_DOWN			0x04
#define KEY_LEFT			0x08
#define KEY_DIRECTION		0x0f
#define KEY_FIRE			0x10


//-----------------------------------------------------------------------------
// dinput interface
//-----------------------------------------------------------------------------
class CDirectInput  
{
public:

	bool	Create( HINSTANCE hInst, HWND hWnd );
	void	Destroy();
	bool	Acquire();

	bool	GetKey( BYTE& byInput );


	CDirectInput();
	~CDirectInput();

private:
	LPDIRECTINPUT			m_pDI;
	LPDIRECTINPUTDEVICE		m_pKeyboard;
};

#endif // _GT_DX8INPUT_H_
