//-----------------------------------------------------------------------------
// File: gtdefine
// Desc: base define
// Auth: Lyp
// Date: 2003/12/11
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_DEFINE_H_
#define	_GT_DEFINE_H_
#pragma once


#include <windows.h>
//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define GT_INVALID				-1
#define SAFE_DEL(p)				{ if(p) { delete (p);		(p) = NULL; } }
#define SAFE_DELETE(p)			{ if(p) { delete (p);		(p) = NULL; } }
#define SAFE_RELEASE(p)			{ if(p) { (p)->Release();	(p) = NULL; } }
#define SAFE_CLOSE_HANDLE(h)	{ if(h) { CloseHandle(h);	(h) = NULL; } }

typedef  unsigned int (__stdcall * THREADPROC)(void *);

//-----------------------------------------------------------------------------
// VALID∂®“Â
//-----------------------------------------------------------------------------
#define INVALID				(-1)
#define INVALID_POINTER		((LPVOID)(LONG_PTR)-1)
#define INVALID_HANDLE		(INVALID_HANDLE_VALUE)

__forceinline bool VALID(BYTE n)		{ return n != BYTE(-1); }
__forceinline bool VALID(WORD n)		{ return n != WORD(-1); }
__forceinline bool VALID(INT32 n)	{ return n != (-1); }
__forceinline bool VALID(DWORD n)	{ return INT32(n) != (-1); }
__forceinline bool VALID(INT64 n)	{ return n != (-1); }
__forceinline bool VALID(LPVOID n)	{ return (INVALID_POINTER != n) && (NULL != n); }
__forceinline bool VALID(LPCVOID n)	{ return (INVALID_POINTER != n) && (NULL != n); }

#endif	// _GT_DEFINE_H_