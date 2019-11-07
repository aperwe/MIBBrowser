#if !defined(AFX_SETENVDIALOG_H__96AE44C3_3F9C_4613_92A6_76F0A7056312__INCLUDED_)
#define AFX_SETENVDIALOG_H__96AE44C3_3F9C_4613_92A6_76F0A7056312__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SetEnvDialog.h : header file
//
#include "CommonDefs.h"

/////////////////////////////////////////////////////////////////////////////
// CSetEnvDialog dialog

class CSetEnvDialog : public CDialog
{
// Construction
public:
	CSetEnvDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSetEnvDialog)
	enum { IDD = IDD_ENV_VAR_DLG };
	CString	m_EnvName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetEnvDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetEnvDialog)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SETENVDIALOG_H__96AE44C3_3F9C_4613_92A6_76F0A7056312__INCLUDED_)
