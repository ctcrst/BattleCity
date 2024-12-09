//-----------------------------------------------------------------------------
// File: debug
// Desc: Game Tool debug 2.0
// Auth: Lyp
// Date: 2003/12/11
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_DEBUG_H_
#define	_GT_DEBUG_H_
#pragma once

#include "..\gtdefine.h"
#include <windows.h>
#include <assert.h>
//-----------------------------------------------------------------------------
// for short
//-----------------------------------------------------------------------------
#define Debug	CDebug::Inst()
#define Msg		CDebug::MsgBox
#define DbgMsg	CDebug::DebugMsgBox
#ifndef ASSERT
#define ASSERT(f) assert((f))
#endif


#define FATAL_ERR(m)		CDebug::Inst()->ErrMsg(MB_ICONERROR | MB_OK, \
							"Info: "m"\nFile: %s\nLine: %d", __FILE__, \
							__LINE__)


#define EQUAL_ERR(a,b,m)	while((a)==(b) && CDebug::Inst()->ErrMsg( MB_APPLMODAL | \
							MB_ICONERROR | MB_ABORTRETRYIGNORE | MB_DEFBUTTON2, \
							"Info: "m"\nFile: %s\nLine: %d", __FILE__, \
							__LINE__))


#define	USER_BREAKPOINT		{ __asm int 3 }
//-----------------------------------------------------------------------------
// cdebug
//-----------------------------------------------------------------------------
class CDebug
{
public:
	VOID Trace(LPCTSTR format, ... );
	INT  MsgBox(LPCTSTR lpFormat, ...);
	BOOL ErrMsg(UINT uType, LPCTSTR lpFormat, ...);

	static CDebug* Inst() { static CDebug inst; return &inst; }
};





//-----------------------------------------------------------------------------
// 异常处理类
//-----------------------------------------------------------------------------
#define GT_MAX_EXCPTSTR_LEN	256


// 供外部使用的产生异常对象的宏
#define EXCEPTION(err_info) CException(err_info, __FILE__, __LINE__);

class CException
{
public:
	CException(LPSTR sMsg, LPSTR sFileName, INT nLineNum);
	LPSTR GetString() { return m_sInfo; }

protected:
	CHAR	m_sMsg[GT_MAX_EXCPTSTR_LEN];
	CHAR	m_sFileName[GT_MAX_EXCPTSTR_LEN];
	CHAR	m_sInfo[GT_MAX_EXCPTSTR_LEN*3];
	INT		m_nLineNum;
};




#endif	// _GT_DEBUG_H_