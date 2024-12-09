//-----------------------------------------------------------------------------
// File: worldobj
// Desc: �����������
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
// CZoneObjSnapshot	Ϊ��������
//-----------------------------------------------------------------------------
CZoneObjSnapshot::CZoneObjSnapshot()
{
	m_dwDataSize = 0;
	m_pData = MemMgr::Alloc(32*1024);	// Ԥ��32K�ռ�
	m_nItemBuffer = 100;	// Ԥ��һ�ٸ�item�ռ�
	m_nItemNum = 0;
	ZeroMemory(m_pData, m_nItemBuffer * sizeof(ZoneObjVar));
}

CZoneObjSnapshot::~CZoneObjSnapshot()
{
	MemMgr::Free(m_pData);
}


//-----------------------------------------------------------------------------
// ע���µı���
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::RegisterVariable(VOID* pLocation, VOID* pFatherObj, DWORD dwSize)
{
	ZoneObjVar* p = (ZoneObjVar*)m_pData;
	DWORD dwPerItemSize = sizeof(ZoneObjVar);
	
	ASSERT( dwSize > 0 );

	if( m_nItemNum >= m_nItemBuffer )	// �ȼ��ռ��Ƿ��㹻
	{
		// �ռ䲻��,�ٶ�����16��Item�Ļ���ռ�
		m_pData = MemMgr::Resize( m_pData, dwPerItemSize*m_nItemBuffer, 
			dwPerItemSize*(m_nItemBuffer + 16) );
		
		p = (ZoneObjVar*)m_pData;

		// �õ��Ŀռ���Ҫ����
		ZeroMemory(&p[m_nItemBuffer], 16*dwPerItemSize);
		m_nItemBuffer += 16;
	}

	// ��һ���ط��Ž�ȥ
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
				// �������ָ��λ�õı���
				__asm int 3;
			}
/**/
			m_nItemNum++;
			return;
		}
	}
	
	ASSERT(0);	// �߼�����
	return;
}



//-----------------------------------------------------------------------------
// ע������
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::UnregisterVariable(VOID* pFatherObj)
{
	ASSERT(0);	// ��ʱ�벻����ΪʲôҪ���ô˺���

	ZoneObjVar* p = (ZoneObjVar*)m_pData;

	// �ҵ���ָ����ͬ��
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
// ע�����б���
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::UnregisterAll()
{
	m_dwDataSize = 0;
	m_nItemBuffer = 100;	// Ԥ��һ�ٸ�item�ռ�
	m_nItemNum = 0;
	ZeroMemory(m_pData, m_nItemBuffer * sizeof(ZoneObjVar));
}



//-----------------------------------------------------------------------------
// ����
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::Snapshot(VOID* pDest)
{
	ZoneObjVar* p = (ZoneObjVar*)m_pData;
	BYTE* pDestByte = (BYTE*)pDest;

	// Ϊÿ��Item����
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
// ����ӳ��
//-----------------------------------------------------------------------------
VOID CZoneObjSnapshot::LoadFrom(VOID* pSource)
{
	ZoneObjVar* p = (ZoneObjVar*)m_pData;
	BYTE* pSourceByte = (BYTE*)pSource;

	// Ϊÿ��Item����ӡ��
	for(int n=0; n<m_nItemBuffer; n++ )
	{
		if( p[n].pLocation != NULL )
		{
			memcpy( p[n].pLocation, pSourceByte, p[n].dwSize );
			pSourceByte += p[n].dwSize;
		}
	}
}
