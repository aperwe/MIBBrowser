#if !defined(AFX_UTILITIESDLG_H__6E5A9353_FF74_11D5_955F_0050BA0F5206__INCLUDED_)
#define AFX_UTILITIESDLG_H__6E5A9353_FF74_11D5_955F_0050BA0F5206__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UtilitiesDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesDlg dialog

class CUtilitiesDlg : public CDialog
{
// Construction
public:
	CUtilitiesDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUtilitiesDlg)
	enum { IDD = IDD_UTILITIES_DLG };
	CRichEditCtrl	m_OutputEditCtrl;
	CComboBox	m_IPAddrCtrl;
	CString	m_csIPAddr;
	CString	m_csElementName;
	CString	m_csDataValue;
	CString	m_csOutputEdit;
	int		m_nSelectedDataType;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUtilitiesDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	//if p_bDirection == true, data from MRU is retrieved and dialog is updated with this data.
	//if p_bDirection == false, data from dialog is retrieved and stored in MRU.
	void UpdateDialogData (bool p_bDirection);

private:
	enum ECommandType {
		CT_UNKNOWN,
		CT_GETTAB,
		CT_GETONE,
		CT_GETMANY,
		CT_SETANY
	};
	//returns true on successful command execution or false if
	//the execution was unsuccessful.
	bool ExecuteCommand (CUtilitiesDlg::ECommandType p_eType);
	//Called to set window size depending on the value of m_bShowingOutput
	void RecalcWindowSize (void);
	//Called to obtain a string corresponding to the data type selected in Data type
	//ComboBox. Returned string is in form "-x", where 'x' can be 'i', or 'd', and so on.
	const CString CUtilitiesDlg::GetSelectedDataTypeString ();
	//Called to lock or unlock the controls in the window.
	//Used in long-lasting operations
	void LockControls (bool p_bLock);
	//Called to set cursor position on the Output control at the end of text
	void UpdateCursorPosition ();

//Members
private:
	bool	m_bShowingOutput;

protected:

	// Generated message map functions
	//{{AFX_MSG(CUtilitiesDlg)
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnGettableBtn();
	afx_msg void OnGetoneBtn();
	afx_msg void OnSetanyBtn();
	afx_msg void OnClearallBtn();
	afx_msg void OnCloseBtn();
	afx_msg void OnShowoutputBtn();
	afx_msg void OnClearContensBtn();
	afx_msg void OnGetmanyBtn();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UTILITIESDLG_H__6E5A9353_FF74_11D5_955F_0050BA0F5206__INCLUDED_)
