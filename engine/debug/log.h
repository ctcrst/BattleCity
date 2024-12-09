//-----------------------------------------------------------------------------
// File: log
// Desc: Game Tool log 1.0
// Auth: Lyp
// Date: 2003/12/18
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_LOG_H_
#define	_GT_LOG_H_
#pragma once


//-----------------------------------------------------------------------------
// for short
//-----------------------------------------------------------------------------
// #define LOG_ON		
#define LOG_FILE	"log.txt"
#define LOG			CLog::Inst()




//-----------------------------------------------------------------------------
// clog
//-----------------------------------------------------------------------------
class CLog
{
public:
	INT Write(LPCTSTR lpFormat, ...);

	static CLog* Inst() { static CLog inst; return &inst; }
	CLog();
};





#endif	// _GT_LOG_H_