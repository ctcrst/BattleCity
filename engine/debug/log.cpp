//-----------------------------------------------------------------------------
// File: log
// Desc: Game Tool log 1.0
// Auth: Lyp
// Date: 2003/12/18
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include <windows.h>
#include "..\gtdefine.h"
#include "debug.h"
#include "log.h"


//-----------------------------------------------------------------------------
// construction
//-----------------------------------------------------------------------------
CLog::CLog()
{
#ifndef LOG_ON
	return;
#endif

	HANDLE hFile = CreateFile(LOG_FILE, GENERIC_WRITE, FILE_SHARE_WRITE, 
		NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hFile == INVALID_HANDLE_VALUE )
		return;

	CloseHandle(hFile);
}


//-----------------------------------------------------------------------------
// 输出简单的信息
//-----------------------------------------------------------------------------
INT CLog::Write(LPCTSTR lpFormat, ...)
{
#ifndef LOG_ON
	return 0;
#endif


	// The maximum size of the buffer which "wvsprintf" take is 1024 bytes
	static char szBuf[1024];
	char pReturn[2] = {0x0d, 0x0a};
	DWORD dwBytes = 0;

	va_list argptr;
	va_start(argptr, lpFormat);
	int nReturn = wvsprintf(szBuf, lpFormat, argptr);	
	va_end(argptr);

	if( nReturn >= 1024 )
		ASSERT(0);

	HANDLE hFile = CreateFile(LOG_FILE, GENERIC_WRITE, FILE_SHARE_WRITE, 
		NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if( hFile == INVALID_HANDLE_VALUE )
		return GT_INVALID;

	SetFilePointer(hFile, 0, NULL, FILE_END);
	WriteFile(hFile, szBuf, strlen(szBuf), &dwBytes, NULL);
	WriteFile(hFile, pReturn, 2, &dwBytes, NULL);
	CloseHandle(hFile);

	return 0;
}




