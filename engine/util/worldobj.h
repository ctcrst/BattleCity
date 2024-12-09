//-----------------------------------------------------------------------------
// File: worldobj
// Desc: 世界物件管理
// Auth: Lyp
// Date: 2003/12/18
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_WORLD_OBJ_H_
#define _GT_WORLD_OBJ_H_
#pragma once


#pragma	pack(push)
#pragma pack(1)
struct ZoneObjVar
{
	VOID*	pFatherObj;
	VOID*	pLocation;
	DWORD	dwSize;
};
#pragma pack(pop)


#define RegisterVar(n)		RegisterVariable(&n, this, sizeof(n))
//-----------------------------------------------------------------------------
// CZoneObjSnapshot	为区域拍照
//-----------------------------------------------------------------------------
class CZoneObjSnapshot
{
public:
	
	VOID RegisterVariable(VOID* pLocation, VOID* pFatherObj, DWORD dwSize);
	VOID UnregisterVariable(VOID* pFatherObj);
	VOID UnregisterAll();

	DWORD	GetAllSize() { return m_dwDataSize; }
	VOID	Snapshot(VOID* pDest);
	VOID	LoadFrom(VOID* pSource);

	CZoneObjSnapshot();
	~CZoneObjSnapshot();

private:

	DWORD	m_dwDataSize;
	VOID*	m_pData;
	INT		m_nItemBuffer;
	INT		m_nItemNum;
};



#endif // _GT_WORLD_OBJ_TPL_H_