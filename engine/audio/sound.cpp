//-----------------------------------------------------------------------------
// File: sound
// Desc: interface for directx sound
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "Sound.h"
#include "wavread.h"
#include "..\debug\debug.h"

#define SOUND_PATH "resource\\sound\\"
static char szSoundEffects[EFFECT_NUM][80] =
{
	SOUND_PATH"gunfire.wav",
	SOUND_PATH"bang.wav",
	SOUND_PATH"hit.wav",
	SOUND_PATH"peow.wav",
	SOUND_PATH"fanfare.wav",
};


//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CDirectSound::CDirectSound():m_pDS(NULL)
{
	for( int i = 0; i < EFFECT_NUM; i ++ )
		m_pdsBuffer[i] = NULL;
}

CDirectSound::~CDirectSound()
{
	Destroy();
}



//-----------------------------------------------------------------------------
// Init
//-----------------------------------------------------------------------------
bool CDirectSound::Init( HWND hWnd )
{
	// Create DirectSound
	if( FAILED(DirectSoundCreate( NULL, &m_pDS, NULL )) )
		return false;

	m_pDS->SetCooperativeLevel( hWnd, DSSCL_NORMAL );

	// Create Sound buffer
	CWaveSoundRead waveSound;

	for( int i = 0; i < EFFECT_NUM; i ++ )
	{
		if( FAILED(waveSound.Open( szSoundEffects[i] )) )
			continue;
		
		waveSound.Reset();
		
		DSBUFFERDESC dsbd;
		ZeroMemory( &dsbd, sizeof(dsbd) );
		dsbd.dwSize = sizeof(dsbd);
		dsbd.dwFlags = DSBCAPS_STATIC;
		dsbd.dwBufferBytes = waveSound.m_ckIn.cksize;
		dsbd.lpwfxFormat = waveSound.m_pwfx;

		if( FAILED(m_pDS->CreateSoundBuffer(&dsbd, &m_pdsBuffer[i], NULL)) )
		{
			waveSound.Close();
			Destroy();
			return false;
		}

		LPVOID lpvAudio;
		DWORD  dwBytes;
		m_pdsBuffer[i]->Lock(0, 0, &lpvAudio, &dwBytes,	NULL, NULL,
				DSBLOCK_ENTIREBUFFER);
		
		UINT cbBytesRead;
		waveSound.Read( dwBytes, (LPBYTE)lpvAudio, &cbBytesRead );
		m_pdsBuffer[i]->Unlock( lpvAudio, dwBytes, NULL, 0 );
		waveSound.Close();
	}

	return true;
}



//-----------------------------------------------------------------------------
// destroy
//-----------------------------------------------------------------------------
void CDirectSound::Destroy()
{
	for( int i = 0; i < EFFECT_NUM; i ++ )
		SAFE_RELEASE( m_pdsBuffer[i] );
	SAFE_RELEASE( m_pDS );
}



//-----------------------------------------------------------------------------
// load effect
//-----------------------------------------------------------------------------
bool CDirectSound::LoadSound( EFFECT sfx )
{
	int idx = (int)sfx;

	CWaveSoundRead waveSound;
	if( FAILED(waveSound.Open( szSoundEffects[idx] )) )
		return false;

	waveSound.Reset();
	
	LPVOID lpvAudio;
	DWORD  dwBytes;
	m_pdsBuffer[idx]->Lock(0, 0, &lpvAudio,	&dwBytes, NULL,	NULL,
			DSBLOCK_ENTIREBUFFER);
		
	UINT cbBytesRead;
	waveSound.Read( dwBytes, (LPBYTE)lpvAudio, &cbBytesRead );
	m_pdsBuffer[idx]->Unlock( lpvAudio, dwBytes, NULL, 0 );
	waveSound.Close();

	return true;
}



//-----------------------------------------------------------------------------
// play
//-----------------------------------------------------------------------------
bool CDirectSound::Play( EFFECT sfx )
{
	return false;

	HRESULT hr;
	int idx = (int)sfx;
	if( !m_pdsBuffer[idx] )
		return false;

	m_pdsBuffer[idx]->SetCurrentPosition(0);
	hr = m_pdsBuffer[idx]->Play( 0, 0, 0 );
	if( hr == DSERR_BUFFERLOST )
	{
		hr = m_pdsBuffer[idx]->Restore();
		LoadSound( sfx );
	}

	if( FAILED(hr) )
		return false;

	return true;
}



//-----------------------------------------------------------------------------
// stop
//-----------------------------------------------------------------------------
void CDirectSound::Stop( EFFECT sfx )
{
	int idx = (int)sfx;
	if( !m_pdsBuffer[idx] )
		return;

	m_pdsBuffer[idx]->Stop();
}