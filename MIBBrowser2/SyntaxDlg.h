#if !defined(AFX_SYNTAXDLG_H__86000E05_D41B_11D5_953A_0050BA0F5206__INCLUDED_)
#define AFX_SYNTAXDLG_H__86000E05_D41B_11D5_953A_0050BA0F5206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SyntaxDlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CSyntaxDlg dialog

class CSyntaxDlg : public CDialog
{
// Construction
public:
	CSyntaxDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSyntaxDlg)
	enum { IDD = IDD_SYNTAX_DLG };
	CString	m_ElementName;
	CString	m_ElementSyntax;
	CString	m_SyntaxDef;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSyntaxDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSyntaxDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SYNTAXDLG_H__86000E05_D41B_11D5_953A_0050BA0F5206__INCLUDED_)
