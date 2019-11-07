/**
*	@file		FindElementDlg.h
*	@brief		Find element dialog class definition.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/

#ifndef ___FIND_ELEMENT_DLG_H__
#define ___FIND_ELEMENT_DLG_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FindElementDlg.h : header file
//


/**
*	@brief		Find element dialog class.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CFindElementDlg : public CDialog
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief	Standard constructor.
	*	@param	pParent		Parent window.
	*/
	CFindElementDlg(CWnd* pParent = NULL);
	void InitLookupFirstElementByName (void);
	void InitLookupNextElementByName (void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFindElementDlg)
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

	/**
	*	@brief	Function called when default (OK) button is pressed.
	*/
	virtual void OnOK ();

//////////////////
// Dialog members
//////////////////

public:
	//{{AFX_DATA(CFindElementDlg)
	enum { IDD = IDD_FIND_DIALOG };
	CString	m_FindString;
	BOOL	m_bCaseSensitiveSwitch;
	//}}AFX_DATA


// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFindElementDlg)
	afx_msg void OnClose();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnFindBtn();
	afx_msg void OnFindNextBtn();
	afx_msg void OnCaseSensitiveChk();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP ()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //___FIND_ELEMENT_DLG_H__
