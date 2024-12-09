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

enum EMMCacheType	// ���ö��
{
	MMCT_1k=0,	// Ԥ��16��
	MMCT_64k,	// Ԥ��2��
	MMCT_256k,	// Ԥ��1��
	MMCT_1024k,	// Ԥ��0��
	MMCT_4096k,	// Ԥ��0��
	MMCT_8192k,	// Ԥ��0��
	MMCT_End,
};


// ÿ�黺�������
struct tagCache
{
	void* pMem;
	DWORD dwSize;		// �����С
	DWORD dwUsage;		// ʹ���˶���
	DWORD dwMaxUsage;	// ���ʹ�ÿռ�
	DWORD dwUseTime;	// �ܹ�ʹ�ô���

	bool bBeUsed;		// ���ڱ�ʹ��
	bool bAllocated;	// �ڴ�ָ����Ч��
	bool bEverBeenUsed;	// ����ʹ�ù�
};


//-----------------------------------------------------------------------------
// �ṩ�̰߳�ȫ���ڴ�ع���ӿ�
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
// �ṩһ����ڴ�ع���ӿ�
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