//-----------------------------------------------------------------------------
// File: MemMgr
// Desc: game tool tread safe mem pool manager 1.0
// Auth: Lyp
// Date: 2003-1-8	
// Last: 2003-12-11
//
// Copyright (c) 2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_MEM_MANAGER_
#define _GT_MEM_MANAGER_
#pragma once

#define TSMemMgr				CTSMemMgr	// for short
#define MemMgr					CMemMgr		// for short

#define GT_MAX_CACHE_PER_TYPE	32

enum EMMCacheType	// 规格枚举
{
	MMCT_1k=0,	// 预开16个
	MMCT_64k,	// 预开2个
	MMCT_256k,	// 预开1个
	MMCT_1024k,	// 预开0个
	MMCT_4096k,	// 预开0个
	MMCT_8192k,	// 预开0个
	MMCT_End,
};


// 每块缓冲的描述
struct tagCache
{
	void* pMem;
	DWORD dwSize;		// 缓冲大小
	DWORD dwUsage;		// 使用了多少
	DWORD dwMaxUsage;	// 最大使用空间
	DWORD dwUseTime;	// 总共使用次数

	bool bBeUsed;		// 现在被使用
	bool bAllocated;	// 内存指针有效吗
	bool bEverBeenUsed;	// 曾被使用过
};


//-----------------------------------------------------------------------------
// 提供线程安全的内存池管理接口
//-----------------------------------------------------------------------------
class CTSMemMgr
{
public:

	static VOID* Alloc(DWORD dwBytes);
	static VOID  Free(VOID* pMem);
	
	~CTSMemMgr();
	CTSMemMgr();
	
private:

	static tagCache			m_Pool[MMCT_End][GT_MAX_CACHE_PER_TYPE];
	static CRITICAL_SECTION	m_PoolLock;

};



//-----------------------------------------------------------------------------
// 提供一般的内存池管理接口
//-----------------------------------------------------------------------------
class CMemMgr
{
public:

	static VOID* Alloc(DWORD dwBytes);
	static VOID  Free(VOID* pMem);
	static VOID* Resize(VOID* pMem, DWORD dwOriginSize, DWORD dwDesireSize);
	
	~CMemMgr();
	CMemMgr();
	
private:

	static tagCache			m_Pool[MMCT_End][GT_MAX_CACHE_PER_TYPE];
};




#endif	// _GT_MEM_MANAGER_