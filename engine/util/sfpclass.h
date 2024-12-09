//-----------------------------------------------------------------------------
// File: SfpTpl
// Desc: ���������̬����ָ��Ļ���
// Auth: Lyp
// Date: 2003/12/01
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#ifndef _GT_SFP_TPL_H_
#define _GT_SFP_TPL_H_
#pragma once

#include <list>
#include <map>

#include "emap.h"
//-----------------------------------------------------------------------------
// ��֧��INTEL�ܹ�32λCPU �Լ� VC++������
//-----------------------------------------------------------------------------
template<typename your_class> class Trunk
{
public:
	// �������޲����ĺ���
	template<typename T> DWORD (WINAPI* FP0(T f))()
	{
		LPBYTE pByte = m_mapFP.Peek(*(DWORD*)(VOID*)&f);
		if( VALID(pByte) )
			return (DWORD(WINAPI*)())pByte;

		pByte = AllocCodeSpace();
		if( pByte )
		{
			m_fpList.push_back(*(LPVOID*)&f);
			m_mapFP.Add(*(DWORD*)(VOID*)&f, pByte);
			memcpy(pByte, CODE0, sizeof(CODE0));
			*(DWORD*)&pByte[2] = (DWORD)&m_pObj;
			*(DWORD*)&pByte[10] = (DWORD)&m_fpList.back();
		}
		return (DWORD(WINAPI*)())pByte;
	}

	// ������1�����ĺ���
	template<typename T> DWORD (WINAPI* FP1(T f))(DWORD)
	{
		LPBYTE pByte = m_mapFP.Peek(*(DWORD*)(VOID*)&f);
		if( VALID(pByte) )
			return (DWORD(WINAPI*)(DWORD))pByte;

		pByte = AllocCodeSpace();
		if( pByte )
		{
			m_fpList.push_back(*(LPVOID*)&f);
			m_mapFP.Add(*(DWORD*)(VOID*)&f, pByte);
			memcpy(pByte, CODE1, sizeof(CODE1));
			*(DWORD*)&pByte[2] = (DWORD)&m_pObj;
			*(DWORD*)&pByte[14] = (DWORD)&m_fpList.back();
		}
		return (DWORD(WINAPI*)(DWORD))pByte;
	}

	// ������1�����ĺ���,����cdecl�ͺ���,����Lua�ű�ע�ắ��ʹ��
	template<typename T> DWORD (__cdecl* FP1_cdecl(T f))(DWORD)	
	{
		LPBYTE pByte = m_mapFP.Peek(*(DWORD*)(VOID*)&f);
		if( VALID(pByte) )
			return (DWORD(__cdecl*)(DWORD))pByte;

		pByte = AllocCodeSpace();
		if( pByte )
		{
			m_fpList.push_back(*(LPVOID*)&f);
			m_mapFP.Add(*(DWORD*)(VOID*)&f, pByte);
			memcpy(pByte, CODE1, sizeof(CODE1));
			*(DWORD*)&pByte[2] = (DWORD)&m_pObj;
			*(DWORD*)&pByte[14] = (DWORD)&m_fpList.back();
			pByte[19] = 0;
		}

		return (DWORD(__cdecl*)(DWORD))pByte;
	}

	// ������2�����ĺ���
	template<typename T> DWORD (WINAPI* FP2(T f))(DWORD, DWORD)
	{
		LPBYTE pByte = m_mapFP.Peek(*(DWORD*)(VOID*)&f);
		if( VALID(pByte) )
			return (DWORD(WINAPI*)(DWORD,DWORD))pByte;

		pByte = AllocCodeSpace();
		if( pByte )
		{
			m_fpList.push_back(*(LPVOID*)&f);
			m_mapFP.Add(*(DWORD*)(VOID*)&f, pByte);
			memcpy(pByte, CODE2, sizeof(CODE2));
			*(DWORD*)&pByte[10] = (DWORD)&m_pObj;
			*(DWORD*)&pByte[16] = (DWORD)&m_fpList.back();
		}
		return (DWORD(WINAPI*)(DWORD,DWORD))pByte;
	}

	// ������3�����ĺ���
	template<typename T> DWORD (WINAPI* FP3(T f))(DWORD, DWORD, DWORD)
	{
		LPBYTE pByte = m_mapFP.Peek(*(DWORD*)(VOID*)&f);
		if( VALID(pByte) )
			return (DWORD(WINAPI*)(DWORD,DWORD,DWORD))pByte;

		pByte = AllocCodeSpace();
		if( pByte )
		{
			m_fpList.push_back(*(LPVOID*)&f);
			m_mapFP.Add(*(DWORD*)(VOID*)&f, pByte);
			memcpy(pByte, CODE3, sizeof(CODE3));
			*(DWORD*)&pByte[18] = (DWORD)&m_pObj;
			*(DWORD*)&pByte[24] = (DWORD)&m_fpList.back();

		}
		return (DWORD(WINAPI*)(DWORD,DWORD,DWORD))pByte;
	}

	// ������4�����ĺ���
	template<typename T> DWORD (WINAPI* FP4(T f))(DWORD, DWORD, DWORD, DWORD)
	{
		LPBYTE pByte = m_mapFP.Peek(*(DWORD*)(VOID*)&f);
		if( VALID(pByte) )
			return (DWORD(WINAPI*)(DWORD,DWORD,DWORD,DWORD))pByte;

		pByte = AllocCodeSpace();
		if( pByte )
		{
			m_fpList.push_back(*(LPVOID*)&f);
			m_mapFP.Add(*(DWORD*)(VOID*)&f, pByte);
			memcpy(pByte, CODE4, sizeof(CODE4));
			*(DWORD*)&pByte[20] = (DWORD)&m_pObj;
			*(DWORD*)&pByte[28] = (DWORD)&m_fpList.back();
		}
		return (DWORD(WINAPI*)(DWORD,DWORD,DWORD,DWORD))pByte;
	}

	// ��������
	VOID Clear() { m_mapFP.ResetIterator(); LPBYTE p; while(m_mapFP.PeekNext(p)){ FreeCodeSpace(p); } m_mapFP.Clear();}
	Trunk(your_class* pThis) { m_pObj = pThis; }
	~Trunk() { Clear(); }

private:
	your_class* m_pObj;
	std::list<LPVOID> m_fpList;
	EMap<DWORD, LPBYTE>	m_mapFP;

	static CONST INT MAX_CODE_SIZE	= 10;
	static CONST DWORD CODE0[MAX_CODE_SIZE];
	static CONST DWORD CODE1[MAX_CODE_SIZE];
	static CONST DWORD CODE2[MAX_CODE_SIZE];
	static CONST DWORD CODE3[MAX_CODE_SIZE];
	static CONST DWORD CODE4[MAX_CODE_SIZE];

	LPBYTE AllocCodeSpace()
	{
		DWORD dwPageSize;             // amount of memory to allocate.
		SYSTEM_INFO sSysInfo;         // useful information about the system
		GetSystemInfo(&sSysInfo);     // initialize the structure
		dwPageSize = sSysInfo.dwPageSize;
		return (LPBYTE)VirtualAlloc(NULL, dwPageSize, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	}

	VOID FreeCodeSpace(LPVOID pMem) { VirtualFree(pMem, 0, MEM_RELEASE); }
};



template<typename your_class> CONST DWORD Trunk<your_class>::CODE0[MAX_CODE_SIZE] = 
{	0x53800d8b, 0x90900041, 0x67e015ff, 0x00c20041, 0x90909000, 0x90909090 }; 

template<typename your_class> CONST DWORD Trunk<your_class>::CODE1[MAX_CODE_SIZE] = 
{	0x53800d8b, 0x5a580041, 0x90525052, 0x67e015ff, 0x04c20041, 0x90909000, 0x90909090, 0x90909090 };

template<typename your_class> CONST DWORD Trunk<your_class>::CODE2[MAX_CODE_SIZE] = 
{	0x525a5859, 0x50525150, 0x00000d8b, 0x15ff0000, 0x00000000, 0x900008c2, 0x90909090, 0x90909090 };

template<typename your_class> CONST DWORD Trunk<your_class>::CODE3[MAX_CODE_SIZE] = 
{	0x0c24448b, 0x08244c8b, 0x0424548b, 0x90525150, 0x00000d8b, 0x15ff0000, 0x00000000, 0x90000cc2 };

template<typename your_class> CONST DWORD Trunk<your_class>::CODE4[MAX_CODE_SIZE] = 
{	0x1024448b, 0x0c244c8b, 0x0824548b, 0x24448b50, 0x0d8b5108, 0x00416494, 0x15ff5052, 0x00000000, 0x900010c2, 0x90909090 };





#endif // _GT_SFP_CLASS_H_