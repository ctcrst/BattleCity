//-----------------------------------------------------------------------------
// File: gDefine
// Desc: game define of Battle City LAN
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _BC_DEFINE_H_
#define _BC_DEFINE_H_
#pragma once

#define RAND_SEED		90100891

#define BC_PORT			6222
#define BC_SYNC_PORT	7000
#define BC_MAX_PLAYER	2


#define SCREEN_W		640
#define SCREEN_H		480
#define OFFSETX			100
#define OFFSETY			48
#define PLAYER1_STARTX	130
#define PLAYER1_STARTY	386
#define PLAYER2_STARTX	258
#define PLAYER2_STARTY	386


#define MAX_PLAYER		2
#define NUM_BITMAPS		15
#define NUM_EXPLODES	20
#define NUM_ENEMYS		6



#define THREAD_SCHEDULE_MS	5

enum GAMESTATE
{
	GS_SPLASH,
	GS_ACTIVE,
	GS_WIN,
	GS_OVER,
};


#endif	// _BC_DEFINE_H_