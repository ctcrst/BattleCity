//-----------------------------------------------------------------------------
// File: debug
// Desc: Game Tool debug 2.0
// Auth: Lyp
// Date: 2003/12/11
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include <windows.h>
#include "debug.h"


HWND g_hWnd = NULL;
//-----------------------------------------------------------------------------
// 简单的TRACE
//-----------------------------------------------------------------------------
VOID CDebug::Trace(LPCTSTR lpFormat, ... )
{
	static char szBuf[2048];	// FIXME: make the array safe
	va_list vl;
	va_start(vl, lpFormat);
	wvsprintf(szBuf, lpFormat, vl);
	OutputDebugString( szBuf );
	va_end(vl);
}



//-----------------------------------------------------------------------------
// 显示简单的信息
//-----------------------------------------------------------------------------
INT CDebug::MsgBox(LPCTSTR lpFormat, ...)
{
	static TCHAR szBuf[2048];	// FIXME: make the array safe
	va_list argptr;
	va_start(argptr, lpFormat);
	wvsprintf(szBuf, lpFormat, argptr);
	va_end(argptr);
	
	return ::MessageBox(g_hWnd, szBuf, "Message", MB_OK);
}




//-----------------------------------------------------------------------------
// 显示错误信息,并接受用户处理
//-----------------------------------------------------------------------------
BOOL CDebug::ErrMsg(UINT uType, LPCTSTR lpFormat, ...)
{
	TCHAR szBuf[4096];	// FIXME: make the array safe
	LPVOID lpMsgBuf = NULL;

	DWORD dwErr = ::GetLastError();

	va_list argptr;
	va_start(argptr, lpFormat);
	wvsprintf(szBuf, lpFormat, argptr);
	va_end(argptr);

	// 检查是否可以从系统得到错误信息
	if( dwErr != 0 )
	{
		// Read Error message from system
		FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM 
			| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, dwErr,
			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
			(LPTSTR) &lpMsgBuf, 0, NULL);
		
		wsprintf(szBuf, "%s\nCode: %lu\nNews: %s", szBuf,
			dwErr, (const char*)lpMsgBuf);
		
		LocalFree( lpMsgBuf );
	}

	switch( ::MessageBox(g_hWnd, szBuf, " Error ", uType) )
	{
	case IDRETRY:
		return TRUE;
	case IDABORT:
		__asm int 3;
		break;
	}

	return FALSE;

}



//-----------------------------------------------------------------------------
// exception
//-----------------------------------------------------------------------------
CException::CException(LPSTR sMsg, LPSTR sFileName, INT nLineNum )
{
	strcpy(m_sMsg, sMsg);
	strcpy(m_sFileName, sFileName);
	m_nLineNum = nLineNum;

	wsprintf(m_sInfo, "%s\n%s Line %d", m_sMsg, m_sFileName, m_nLineNum);
}