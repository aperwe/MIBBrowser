// SplashWnd.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "SplashWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd dialog


CSplashWnd::CSplashWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CSplashWnd::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSplashWnd)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	Create (CSplashWnd::IDD, NULL);
}


void CSplashWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSplashWnd)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

void CSplashWnd::OnCancel (void)
{
	//Prevent the window from destroying after the user presses ESC.
}


void CSplashWnd::OnOK (void)
{
	//Prevent the window from destroying after the user presses ENTER.
}

BEGIN_MESSAGE_MAP(CSplashWnd, CDialog)
	//{{AFX_MSG_MAP(CSplashWnd)
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSplashWnd message handlers

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CDialog::OnCreate(lpCreateStruct) == -1)
		return -1;
	ModifyStyleEx (0, WS_EX_OVERLAPPEDWINDOW | WS_EX_TOPMOST | WS_EX_TOOLWINDOW);

	if (SetTimer (MY_TIMER, MY_TIMER_TIMEOUT, NULL) != MY_TIMER) return -1;

	return 0;
}

void CSplashWnd::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == MY_TIMER)
	{
		//Bring the main application window to front
		if (AfxGetApp ()->m_pMainWnd)
		{
			AfxGetApp ()->m_pMainWnd->BringWindowToTop ();
			AfxGetApp ()->m_pMainWnd->SetActiveWindow();
		}

		DestroyWindow ();
		AfxEndThread (0L);
	}

	CDialog::OnTimer(nIDEvent);
}

