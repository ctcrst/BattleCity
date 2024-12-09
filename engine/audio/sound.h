//-----------------------------------------------------------------------------
// File: sound
// Desc: interface for directx input
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_DX8SOUND_H_
#define	_GT_DX8SOUND_H_
#pragma once

#include <dsound.h>


enum EFFECT
{
	EFFECT_FIRE,
	EFFECT_EXPLODE,
	EFFECT_HIT,
	EFFECT_PICK,
	EFFECT_LIFE,
	EFFECT_NUM
};


class CDirectSound  
{
public:
	CDirectSound();
	~CDirectSound();

	bool	Init( HWND hWnd );
	void	Destroy();
	bool	Play( EFFECT sfx );
	void	Stop( EFFECT sfx );
	bool	LoadSound( EFFECT sfx );

private:
	LPDIRECTSOUND			m_pDS;
	LPDIRECTSOUNDBUFFER		m_pdsBuffer[EFFECT_NUM];
};

#endif // _GT_DX8SOUND_H_
