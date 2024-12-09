//-----------------------------------------------------------------------------
// File: MemMgr
// Desc: game tool mem manager 1.0
// Auth: Lyp
// Date: 2003-1-8	
// Last: 2003-12-11
//
// Copyright (c) 2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "..\gtdefine.h"

#include "..\debug\debug.h"
#include "..\debug\log.h"
#include "MemMgr.h"


// 缓存大小规格
DWORD g_dwCacheSize[MMCT_End] = { 1024, 64*1024, 256*1024, 1024*1024, 4096*1024, 8192*1024 };
int   g_nCachePreCreate[MMCT_End] = { 16, 2, 1, 0, 0, 0 };	// 预先每种缓冲个数

CRITICAL_SECTION	CTSMemMgr::m_PoolLock;
tagCache			CTSMemMgr::m_Pool[MMCT_End][GT_MAX_CACHE_PER_TYPE];
tagCache			CMemMgr::m_Pool[MMCT_End][GT_MAX_CACHE_PER_TYPE];

static	CTSMemMgr		g_TSMemMgr;
static	CMemMgr			g_MemMgr;
//-----------------------------------------------------------------------------
// construction / destruction
//-----------------------------------------------------------------------------
CTSMemMgr::CTSMemMgr()
{
	ZeroMemory( m_Pool, sizeof(m_Pool) );
	InitializeCriticalSection(&m_PoolLock);

	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<g_nCachePreCreate[n]; m++ )
		{
			m_Pool[n][m].pMem = malloc(g_dwCacheSize[n]);
			if( m_Pool[n][m].pMem != NULL)
			{
				m_Pool[n][m].bAllocated = true;
				m_Pool[n][m].dwSize = g_dwCacheSize[n];
			}
		}
	}
}


CTSMemMgr::~CTSMemMgr()
{
	DeleteCriticalSection(&m_PoolLock);

	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].bAllocated )
			{
				free(m_Pool[n][m].pMem);
				m_Pool[n][m].bAllocated = false;
			}
		}
	}

	// log
	LOG->Write("Memory usage(by Thread Safe MemMgr)");
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].bEverBeenUsed )
				LOG->Write("%luk-%d, max usage : %lu  use time : %lu", 
				g_dwCacheSize[n]/1024, m,
				m_Pool[n][m].dwMaxUsage, 
				m_Pool[n][m].dwUseTime);
		}
	}
}




//-----------------------------------------------------------------------------
// 申请内存
//-----------------------------------------------------------------------------
void* CTSMemMgr::Alloc(DWORD dwBytes)
{
	void* pMem = NULL;
	EMMCacheType eMemType = MMCT_End;
	
	// 搜索申请内存大小类型
	ASSERT( dwBytes <= g_dwCacheSize[MMCT_End-1] );
	for( int n=0; n<MMCT_End; n++ )
	{
		if( dwBytes <= g_dwCacheSize[n] )
		{
			eMemType = (EMMCacheType)n;
			break;
		}
	}
	
	if( eMemType < MMCT_End )
	{
		// 在此进入锁定区
		EnterCriticalSection(&m_PoolLock);

		// 检查空闲的缓存
		for( int n=0; n<GT_MAX_CACHE_PER_TYPE; n++ )
		{
			if( false == m_Pool[eMemType][n].bBeUsed )
			{
				if( false == m_Pool[eMemType][n].bAllocated )
					m_Pool[eMemType][n].pMem = malloc(g_dwCacheSize[eMemType]);
				
				if( m_Pool[eMemType][n].pMem )
				{
					pMem = m_Pool[eMemType][n].pMem;
					m_Pool[eMemType][n].bAllocated = true;
					m_Pool[eMemType][n].bBeUsed = true;
					m_Pool[eMemType][n].dwSize = g_dwCacheSize[eMemType];
					m_Pool[eMemType][n].bEverBeenUsed = true;
					m_Pool[eMemType][n].dwUsage = dwBytes;
					m_Pool[eMemType][n].dwUseTime ++;
					if( dwBytes > m_Pool[eMemType][n].dwMaxUsage )
						m_Pool[eMemType][n].dwMaxUsage = dwBytes;
					
					break;
				}
			}
		}
		
		// 离开锁定区
		LeaveCriticalSection(&m_PoolLock);
	}
	
	if( NULL == pMem )
		pMem = malloc(dwBytes);

	if( NULL == pMem )
	{
		FATAL_ERR("CE_CANNOT_ALLOC_MEM");
		Debug->MsgBox("申请%lu字节内存失败", dwBytes);
	}

	return pMem;
}




//-----------------------------------------------------------------------------
// 释放内存
//-----------------------------------------------------------------------------
VOID CTSMemMgr::Free(VOID* pMem)
{
	ASSERT(pMem);

	// 搜索对应缓存
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].pMem == pMem )
			{
				m_Pool[n][m].dwUsage = 0;
				m_Pool[n][m].bBeUsed = false;
				return;
			}
		}
	}

	free( pMem );	// not found
	return;
}





//-----------------------------------------------------------------------------
// CMemMgr construction / destruction
//-----------------------------------------------------------------------------
CMemMgr::CMemMgr()
{
	ZeroMemory( m_Pool, sizeof(m_Pool) );
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<g_nCachePreCreate[n]; m++ )
		{
			m_Pool[n][m].pMem = malloc(g_dwCacheSize[n]);
			if( m_Pool[n][m].pMem != NULL)
			{
				m_Pool[n][m].bAllocated = true;
				m_Pool[n][m].dwSize = g_dwCacheSize[n];
			}
		}
	}
}


CMemMgr::~CMemMgr()
{
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].bAllocated )
			{
				free(m_Pool[n][m].pMem);
				m_Pool[n][m].bAllocated = false;
			}
		}
	}

	// log
	LOG->Write("Memory usage(by MemMgr)");
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].bEverBeenUsed )
				LOG->Write("%luk-%d, max usage : %lu  use time : %lu", 
				g_dwCacheSize[n]/1024, m,
				m_Pool[n][m].dwMaxUsage, 
				m_Pool[n][m].dwUseTime);
		}
	}
}




//-----------------------------------------------------------------------------
// 申请内存
//-----------------------------------------------------------------------------
void* CMemMgr::Alloc(DWORD dwBytes)
{
	void* pMem = NULL;
	EMMCacheType eMemType = MMCT_End;
	
	// 搜索申请内存大小类型
	ASSERT( dwBytes <= g_dwCacheSize[MMCT_End-1] );
	for( int n=0; n<MMCT_End; n++ )
	{
		if( dwBytes <= g_dwCacheSize[n] )
		{
			eMemType = (EMMCacheType)n;
			break;
		}
	}
	
	if( eMemType < MMCT_End )
	{
		// 检查空闲的缓存
		for( int n=0; n<GT_MAX_CACHE_PER_TYPE; n++ )
		{
			if( false == m_Pool[eMemType][n].bBeUsed )
			{
				if( false == m_Pool[eMemType][n].bAllocated )
					m_Pool[eMemType][n].pMem = malloc(g_dwCacheSize[eMemType]);
				
				if( m_Pool[eMemType][n].pMem )
				{
					pMem = m_Pool[eMemType][n].pMem;
					m_Pool[eMemType][n].bAllocated = true;
					m_Pool[eMemType][n].bBeUsed = true;
					m_Pool[eMemType][n].dwSize = g_dwCacheSize[eMemType];
					m_Pool[eMemType][n].bEverBeenUsed = true;
					m_Pool[eMemType][n].dwUsage = dwBytes;
					m_Pool[eMemType][n].dwUseTime ++;
					if( dwBytes > m_Pool[eMemType][n].dwMaxUsage )
						m_Pool[eMemType][n].dwMaxUsage = dwBytes;
					
					break;
				}
			}
		}
	}
	
	if( NULL == pMem )
		pMem = malloc(dwBytes);

	if( NULL == pMem )
	{
		FATAL_ERR("CE_CANNOT_ALLOC_MEM");
		Debug->MsgBox("申请%lu字节内存失败", dwBytes);
	}

	return pMem;
}




//-----------------------------------------------------------------------------
// 释放内存
//-----------------------------------------------------------------------------
VOID CMemMgr::Free(VOID* pMem)
{
	ASSERT(pMem);

	// 搜索对应缓存
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].pMem == pMem )
			{
				m_Pool[n][m].dwUsage = 0;
				m_Pool[n][m].bBeUsed = false;
				return;
			}
		}
	}

	free( pMem );	// not found
	return;
}



//-----------------------------------------------------------------------------
// 释放内存
//-----------------------------------------------------------------------------
VOID* CMemMgr::Resize(VOID* pMem, DWORD dwOriginSize, DWORD dwDesireSize)
{
	ASSERT(pMem);
	ASSERT(dwDesireSize > dwOriginSize);
	VOID* pMemNew = NULL;

	// 搜索对应缓存
	for( int n=0; n<MMCT_End; n++ )
	{
		for( int m=0; m<GT_MAX_CACHE_PER_TYPE; m++ )
		{
			if( m_Pool[n][m].pMem == pMem )
			{
				if( m_Pool[n][m].dwSize >= dwDesireSize )
					goto _resize_end;
				
				pMemNew = Alloc(dwDesireSize);
				memcpy(pMemNew, pMem, dwOriginSize);
				pMem = pMemNew;

				m_Pool[n][m].dwUsage = 0;
				m_Pool[n][m].bBeUsed = false;
				goto _resize_end;
			}
		}
	}
	
	// 可能 pMem 并不是在池中分配的, 所以找不到
	pMemNew = Alloc(dwDesireSize);
	memcpy(pMemNew, pMem, dwOriginSize);
	Free(pMem);
	pMem = pMemNew;

_resize_end:

	return pMem;
}
