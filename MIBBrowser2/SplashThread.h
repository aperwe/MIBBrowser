#if !defined(AFX_SPLASHTHREAD_H__DA981A12_D36D_11D5_953A_0050BA0F5206__INCLUDED_)
#define AFX_SPLASHTHREAD_H__DA981A12_D36D_11D5_953A_0050BA0F5206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SplashThread.h : header file
//
#include "CommonDefs.h"
#include "SplashWnd.h"


/////////////////////////////////////////////////////////////////////////////
// CSplashThread thread

class CSplashThread : public CWinThread
{
	DECLARE_DYNCREATE(CSplashThread)
protected:
	CSplashThread();           // protected constructor used by dynamic creation

// Attributes
public:
	CSplashWnd *m_pSplashWnd;
	CWnd *m_pTopMostWindow;	//This is a fake window that has to be open in order that the actual splash window is displayed as top-most window.

// Operations
public:
	void CALLBACK MyTimerProc(HWND hwnd, UINT uMsg, UINT idEvent, DWORD dwTime);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSplashThread)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CSplashThread();

	// Generated message map functions
	//{{AFX_MSG(CSplashThread)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SPLASHTHREAD_H__DA981A12_D36D_11D5_953A_0050BA0F5206__INCLUDED_)
