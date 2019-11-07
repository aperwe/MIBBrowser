// SplashThread.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "CommonDefs.h"
#include "SplashThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CSplashThread

IMPLEMENT_DYNCREATE(CSplashThread, CWinThread)

CSplashThread::CSplashThread()
{
}

CSplashThread::~CSplashThread()
{
}

//A function that handles timer message
void CSplashThread::MyTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
	if (idEvent == MY_TIMER)
	{
		m_pSplashWnd->DestroyWindow ();
		m_pTopMostWindow->DestroyWindow ();
	}
	//Quit the thread regardless of whether the event was our timer event
	PostQuitMessage (0L);
}

void TimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime)
{
}

BOOL CSplashThread::InitInstance()
{

	LPCSTR lpszSplashClassName;

	lpszSplashClassName = AfxRegisterWndClass (
		0,
		0,
		0,
		0);

	m_pTopMostWindow = new CWnd;

	m_pTopMostWindow->CreateEx (WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
		lpszSplashClassName,
		"Splash screen title",
		WS_POPUP | WS_VISIBLE,
		0,
		0,
		1,
		1,
		NULL,
		NULL,
		NULL);

	m_pSplashWnd = new CSplashWnd;
	if (m_pSplashWnd)
	{
	}

	return TRUE;

}

int CSplashThread::ExitInstance()
{
	// TODO:  perform any per-thread cleanup here
	if (m_pSplashWnd)
	{
		delete m_pSplashWnd;
		m_pSplashWnd = NULL;
	}

	if (m_pTopMostWindow)
	{
		delete m_pTopMostWindow;
		m_pTopMostWindow = NULL;
	}

	return CWinThread::ExitInstance();
}

BEGIN_MESSAGE_MAP(CSplashThread, CWinThread)
	//{{AFX_MSG_MAP(CSplashThread)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashThread message handlers
