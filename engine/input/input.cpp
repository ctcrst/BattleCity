//-----------------------------------------------------------------------------
// File: input
// Desc: interface for directx input
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "..\debug\debug.h"
#include "Input.h"

//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CDirectInput::CDirectInput():m_pDI(NULL), m_pKeyboard(NULL)
{}

CDirectInput::~CDirectInput()
{
	Destroy();
}


//-----------------------------------------------------------------------------
// create
//-----------------------------------------------------------------------------
bool CDirectInput::Create( HINSTANCE hInst, HWND hWnd )
{
	HRESULT hr;

	// Create DirectInput object
//	hr = DirectInput8Create( hInst, DIRECTINPUT_VERSION, IID_IDirectInput8A, 
//		(void**)(&m_pDI), NULL );

	hr = DirectInputCreate( hInst, DIRECTINPUT_VERSION, (IDirectInputA **)(&m_pDI), NULL );


	if( FAILED(hr) )
		return false;

	// Initialize keyboard...
	hr = m_pDI->CreateDevice( GUID_SysKeyboard, &m_pKeyboard, NULL );
	if( FAILED(hr) )
	{
		Destroy();
		return false;
	}

	m_pKeyboard->SetDataFormat( &c_dfDIKeyboard );
	m_pKeyboard->SetCooperativeLevel( hWnd, 
			DISCL_FOREGROUND | DISCL_NONEXCLUSIVE );

	// acquire keyboard
	m_pKeyboard->Acquire();
	return true;
}


//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
void CDirectInput::Destroy()
{
	if( m_pKeyboard )
		m_pKeyboard->Unacquire();

	SAFE_RELEASE( m_pKeyboard );
	SAFE_RELEASE( m_pDI );
}


//-----------------------------------------------------------------------------
// acquire
//-----------------------------------------------------------------------------
bool CDirectInput::Acquire()
{
	return ( m_pKeyboard && m_pKeyboard->Acquire() );
}



#define KEYDOWN(key)	(buffer[key] & 0x80)
//-----------------------------------------------------------------------------
// getkey
//-----------------------------------------------------------------------------
bool CDirectInput::GetKey( BYTE& byInput )
{
	HRESULT hr;
	static char buffer[256];
	
	// Read keyboard data...
	hr = m_pKeyboard->GetDeviceState( sizeof(buffer), &buffer );
	if( FAILED(hr) )
	{
		if( hr == DIERR_INPUTLOST )
			hr = Acquire();
		return false;
	}

	byInput = 0;
	if( KEYDOWN( DIK_UP ) )
		byInput = KEY_UP;
	if( KEYDOWN( DIK_DOWN ) )
		byInput = KEY_DOWN;
	if( KEYDOWN( DIK_LEFT ) )
		byInput = KEY_LEFT;
	if( KEYDOWN( DIK_RIGHT ) )
		byInput = KEY_RIGHT;

	if( KEYDOWN( DIK_Z ) )
		byInput |= KEY_FIRE;

	return true;
}
