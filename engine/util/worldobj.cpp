//-----------------------------------------------------------------------------
// File: worldobj
// Desc: 世界物件管理
// Auth: Lyp
// Date: 2003/12/18
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "..\debug\debug.h"
#include "..\debug\log.h"
#include "..\mem\memmgr.h"

#include "worldobj.h"
//-----------------------------------------------------------------------------
// CZoneObjSnapshot	为区域拍照
//-----------------------------------------------------------------------------
CZoneObjSnapshot::CZoneObjSnapshot()
{
	m_dwDataSize = 0;
	m_pData = MemMgr::Alloc(32*1024);	// 预开32K空间
	m_nItemBuffer = 100;	// 预开一百个item空间
	m_nItemNum = 0;
	ZeroMemory(m_pData, m_nItemBuffer * sizeof(ZoneObjVar));
}

CZoneObjSnapshot::~CZoneObjSnapshot()
{
	MemMgr::Free(m_pData);
}


//-----------------------------------------------------------------------------
// 注册新的变量
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::RegisterVariable(VOID* pLocation, VOID* pFatherObj, DWORD dwSize)
{
	ZoneObjVar* p = (ZoneObjVar*)m_pData;
	DWORD dwPerItemSize = sizeof(ZoneObjVar);
	
	ASSERT( dwSize > 0 );

	if( m_nItemNum >= m_nItemBuffer )	// 先检查空间是否足够
	{
		// 空间不足,再多申请16个Item的缓冲空间
		m_pData = MemMgr::Resize( m_pData, dwPerItemSize*m_nItemBuffer, 
			dwPerItemSize*(m_nItemBuffer + 16) );
		
		p = (ZoneObjVar*)m_pData;

		// 得到的空间需要清零
		ZeroMemory(&p[m_nItemBuffer], 16*dwPerItemSize);
		m_nItemBuffer += 16;
	}

	// 找一个地方放进去
	for(int n=0; n<m_nItemBuffer; n++ )
	{
		if( p[n].pLocation == NULL )
		{
			p[n].pLocation = pLocation;
			p[n].pFatherObj = pFatherObj;
			p[n].dwSize = dwSize;
			
			m_dwDataSize+=dwSize;

/*
			if( m_dwDataSize >= 1007 )
			{
				// 用来检查指定位置的变量
				__asm int 3;
			}
/**/
			m_nItemNum++;
			return;
		}
	}
	
	ASSERT(0);	// 逻辑错误
	return;
}



//-----------------------------------------------------------------------------
// 注销变量
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::UnregisterVariable(VOID* pFatherObj)
{
	ASSERT(0);	// 暂时想不出来为什么要调用此函数

	ZoneObjVar* p = (ZoneObjVar*)m_pData;

	// 找到父指针相同的
	for(int n=0; n<m_nItemBuffer; n++ )
	{
		if( p[n].pFatherObj == pFatherObj )
		{
			m_dwDataSize -= p[n].dwSize;
			m_nItemNum--;

			p[n].pLocation = NULL;
			p[n].pFatherObj = NULL;
			p[n].dwSize = 0;
		}
	}

	return;
}



//-----------------------------------------------------------------------------
// 注销所有变量
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::UnregisterAll()
{
	m_dwDataSize = 0;
	m_nItemBuffer = 100;	// 预开一百个item空间
	m_nItemNum = 0;
	ZeroMemory(m_pData, m_nItemBuffer * sizeof(ZoneObjVar));
}



//-----------------------------------------------------------------------------
// 拍照
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::Snapshot(VOID* pDest)
{
	ZoneObjVar* p = (ZoneObjVar*)m_pData;
	BYTE* pDestByte = (BYTE*)pDest;

	// 为每个Item拍照
	for(int n=0; n<m_nItemBuffer; n++ )
	{
		if( p[n].pLocation != NULL )
		{
			memcpy( pDestByte, p[n].pLocation, p[n].dwSize );
			pDestByte += p[n].dwSize;
		}
	}
}



//-----------------------------------------------------------------------------
// 载入映象
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::LoadFrom(VOID* pSource)
{
	ZoneObjVar* p = (ZoneObjVar*)m_pData;
	BYTE* pSourceByte = (BYTE*)pSource;

	// 为每个Item读入印象
	for(int n=0; n<m_nItemBuffer; n++ )
	{
		if( p[n].pLocation != NULL )
		{
			memcpy( p[n].pLocation, pSourceByte, p[n].dwSize );
			pSourceByte += p[n].dwSize;
		}
	}
}
