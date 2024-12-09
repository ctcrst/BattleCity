//-----------------------------------------------------------------------------
// File: gApp
// Desc: main entrance of Battle City Lan
// Auth: Lyp
// Date: 2003/11/13
//
// Copyright (c) 1985-2003 CTCRST Entertainment All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "resource.h"

#include "gApp.h"
#include "gTitle.h"
#include "gGame.h"
#include "resmgr.h"
#include "netSession.h"
//-----------------------------------------------------------------------------
// Construction/Destruction
//-----------------------------------------------------------------------------
CApp::CApp():Trunk<CApp>(this)
{
	m_bSpeedup = false;		// 是否服务器要求加速主循环
	m_bSlowdown = false;	// 减速

	m_pGame = NULL;

	m_pDisplay = new CDisplay;
	m_pDirectSound = new CDirectSound;
	m_pDirectInput = new CDirectInput;
	m_pResMgr = new CResMgr;
	m_pSnapshot = new CZoneObjSnapshot;

	m_pSession = new CSession( this );
	m_pTitle = new CTitle( this );

	m_hInst = NULL;
	m_hWnd = NULL;
	m_bActive = false;
	m_bFullScreen = false;
	m_bServer = true;
	m_bGameStarted = false;

	::QueryPerformanceFrequency(&m_qWinFreq);	// 高精度计时器频率
	::QueryPerformanceCounter(&m_qLastWinTick);	// 高精度计时器Tick
	::QueryPerformanceCounter(&m_qWinTick);		// 高精度计时器Tick
	
	m_dwGameFreq = 60;

	// 计算每一个游戏Tick是多少高精度计时器Tick
	m_qWinTickPerGameTick.QuadPart = m_qWinFreq.QuadPart / (LONGLONG)m_dwGameFreq;

	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup( wVersionRequested, &wsaData );
}

CApp::~CApp()
{
	SAFE_DEL( m_pDisplay );
	SAFE_DEL( m_pDirectSound );
	SAFE_DEL( m_pDirectInput );
	SAFE_DEL( m_pResMgr );
	SAFE_DEL( m_pSnapshot );
	SAFE_DEL( m_pTitle );

	WSACleanup();
}



//-----------------------------------------------------------------------------
// 应用程序窗口消息处理
//-----------------------------------------------------------------------------
LRESULT CApp::MsgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch( uMsg )
	{
	case WM_ACTIVATEAPP:
		if( m_pDirectInput && (m_bActive = (wParam != 0)) )
			m_pDirectInput->Acquire();
		break;

	case WM_SETCURSOR:
		if( m_bFullScreen )
		{
			SetCursor( NULL );
			return TRUE;
		}
		break;

	case WM_CLOSE:
		PostQuitMessage(0);
		return 0;
		break;

	case WM_MOVE:
		m_bActive = TRUE;
		GetClientRect( hWnd, m_pDisplay->GetWindowRect() );
		ClientToScreen( hWnd, (LPPOINT)m_pDisplay->GetWindowRect() );
		ClientToScreen( hWnd, (LPPOINT)m_pDisplay->GetWindowRect() + 1 );
		return 0;
		break;

	case WM_MOVING:
		m_bActive = FALSE;
		break;

	case WM_KEYDOWN:
		switch( wParam )
		{
		case VK_ESCAPE:
			if( m_bGameStarted )
				m_pSession->SendCutLineMsg();
			else
				PostQuitMessage(0);
			break;

		case VK_F4:

			DestroySupport();
			m_bFullScreen = !m_bFullScreen;
			InitSupport();
			if( m_bGameStarted )
				m_pGame->Init();
			return 0;

		case VK_F5:
			if( m_bGameStarted )
				m_pGame->SetShowState(!m_pGame->IsShowState());
			return 0;
			
		case VK_DOWN:
		case VK_UP:
			if( !m_bGameStarted )
				m_bServer = !m_bServer;
			break;

		case VK_RETURN:
			if( !m_bGameStarted )
			{
				m_bGameStarted = true;
				m_pSession->Init(m_bServer);
				
				m_pGame = new CGame(this);
				m_pGame->Randomize( RAND_SEED );
				m_pGame->Init();
				m_pGame->ResetGame();
			}
			break;
		}
		break;
	}

	return DefWindowProc( hWnd, uMsg, wParam, lParam );
}





//-----------------------------------------------------------------------------
// 初始化整个应用程序
//-----------------------------------------------------------------------------
bool CApp::Initialize( HINSTANCE hInst )
{
	m_hInst = hInst;

	if( !InitWindow() || 
		!InitSupport() )
	{
		DestroySupport();
		return false;
	}

	return true;
}



//-----------------------------------------------------------------------------
// 建立底层支持
//-----------------------------------------------------------------------------
bool CApp::InitSupport()
{
	// initialize display
	m_pDisplay->DDInit( m_hWnd, SCREEN_W, SCREEN_H, m_bFullScreen );
	
	// initialize input devices
	m_pDirectInput->Create( m_hInst, m_hWnd );
	
	// initialize sound effects
// 	m_pDirectSound->Init( m_hWnd );

	// initizlize resource
	m_pResMgr->Init( m_pDisplay );

	// load resource
	if( !m_pResMgr->LoadRes() )
		return FALSE;

	return TRUE;
}



//-----------------------------------------------------------------------------
// 销毁底层支持
//-----------------------------------------------------------------------------
void CApp::DestroySupport()
{
	m_pResMgr->UnloadRes();
	m_pDisplay->DDTerm();
	m_pDirectInput->Destroy();
	m_pDirectSound->Destroy();
}


extern HWND g_hWnd;
//-----------------------------------------------------------------------------
// 初始化应用程序窗口
//-----------------------------------------------------------------------------
bool CApp::InitWindow()
{
	char szClassName[] = "BATTLECITYLAN";
	char szTitle[] = "Version Alpha - CTCRST";

	WNDCLASS wc;
	wc.hInstance = m_hInst;
	wc.lpszClassName = szClassName;
	wc.lpfnWndProc = (WNDPROC)FP4(&CApp::MsgProc);
	wc.style = CS_DBLCLKS;
	wc.hIcon = NULL;
	wc.hCursor = LoadCursor( NULL, IDC_ARROW );
	wc.lpszMenuName = NULL;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject( BLACK_BRUSH );

	if( !RegisterClass( &wc ) )
	{
		Debug->Trace( "Error In RegisterClassEx\n" );
		return false;
	}

	g_hWnd = m_hWnd = CreateWindow( szClassName,	szTitle,
				WS_OVERLAPPED, CW_USEDEFAULT, CW_USEDEFAULT,
				646, 505, NULL,	NULL, m_hInst, NULL );
	
	if( !m_hWnd )
	{
		Debug->Trace( "Error In CreateWindow\n" );
		return false;
	}
				
	UpdateWindow( m_hWnd );
	ShowWindow( m_hWnd, SW_NORMAL );

	return true;
}




//-----------------------------------------------------------------------------
// 游戏主循环
//-----------------------------------------------------------------------------
int CApp::MainLoop()
{
	MSG	msg;

    for(;;)
    {
        if( PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE) )
        {
            if( !GetMessage( &msg, NULL, 0, 0) )
                break;
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
		
		LARGE_INTEGER qSleepTick, q;
		DWORD dwSleepMs;
				
		// 对游戏的主循环进行定时
		QueryPerformanceCounter(&m_qWinTick);
		q.QuadPart = m_qWinTickPerGameTick.QuadPart;

		// 如果要求加快或减慢，则相应变更此循环的时间
		if ( m_bSpeedup ) q.QuadPart = q.QuadPart * 6 / 8;
		if ( m_bSlowdown ) q.QuadPart = q.QuadPart * 9 / 8;
		m_bSpeedup = m_bSlowdown = false;
				
		while( m_qWinTick.QuadPart < q.QuadPart + m_qLastWinTick.QuadPart )
		{
			qSleepTick.QuadPart = 
				q.QuadPart - (m_qWinTick.QuadPart - m_qLastWinTick.QuadPart);

			dwSleepMs = (DWORD)(qSleepTick.QuadPart / (m_qWinFreq.QuadPart / 1000));

			if( dwSleepMs >= THREAD_SCHEDULE_MS )
				::Sleep( THREAD_SCHEDULE_MS - 1 );

			QueryPerformanceCounter(&m_qWinTick);
		}
		m_qLastWinTick.QuadPart = m_qWinTick.QuadPart;

		if( false == m_pSession->Update() )	// 断线
		{
			delete m_pGame;
			m_pSession->Destroy();
			m_bGameStarted = false;
		}

		if( m_bGameStarted )
			m_pGame->Render();
		else
			m_pTitle->DrawTitle();
    }
	
	DestroySupport();
	return msg.wParam;
}




//-----------------------------------------------------------------------------
// Main entry point for win32 application
//-----------------------------------------------------------------------------
int WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR, int )
{
	CApp app;
	app.Initialize( hInst );
	return app.MainLoop();
}



