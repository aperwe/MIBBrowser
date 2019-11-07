/**
*	@file		MainWnd.h
*	@brief		Main window class definition.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/


#ifndef ___MAIN_WND_H__
#define ___MAIN_WND_H__


#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MainWnd.h : header file
//
#include "resource.h"
#include "StdAfx.h"
#include "MIBFile.h"
#include "SyntaxDlg.h"
#include "FindElementDlg.h"
#include "UtilitiesDlg.h"

/////////////////////////////////////////////////////////////////////////////
// CMainWnd dialog

/**
*	@brief		Main window class.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CMainWnd : public CDialog
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief	Standard constructor.
	*	@param	pParent		Parent window.
	*/
	CMainWnd(CWnd* pParent = NULL);
	/**
	*	@brief	Adds the supplied message string to the status box.
	*	@param	message		Message to be added.
	*/
	void StatusMessage (CString message);
	/**
	*	@brief	A workaround function allowing a dialog to react to Ctrl+key presses.
	*	@param	pMsg		Windows message to be translated.
	*	@return				Boolean.
	*/
	BOOL PreTranslateMessage (MSG* pMsg);
	/**
	*	@brief	Adds the file tree to the main window TreeCtrl.
	*	@param	*MIBFile	A file to be added.
	*	@return				HTREEITEM of the tree node corresponding to the added file.
	*/
	HTREEITEM AddFileToTree (CMIBFile *MIBFile);
	/**
	*	@brief	This function returns a HTREEITEM when it detects the ETYPE_MIB_ROOT among
	*			the list elements. Note that it returns HTREEITEM value of 
	*			the last ETYPE_MIB_ROOT element in the list, when more than one
	*			ETYPE_MIB_ROOT elements exist in this list. When no such element is found
	*			this function returns false.
	*	@param	hParentItem		A parent item for the list to be added to.
	*	@param	*pElementList	A list that is to be added.
	*	@return					HTREEITEM of the tree node corresponding to the loaded list.
	*/
	HTREEITEM AddListToTree (HTREEITEM hParentItem, CMIBElementList *pElementList);
	/**
	*	@brief	Loads the specified module. Displays error messages as appropriate.
	*	@param	*szFullFileName		The name of a file to load.
	*	@return						Boolean value indicating the status of load process.
	*/
	bool LoadMIBModule (const char *szFullFileName);
	void CleanUp ();	//Cleans up lists etc. After the file list has been modified.


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainWnd)
public:
	virtual BOOL DestroyWindow();
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CMainWnd)
	afx_msg void OnCloseBtn();
	afx_msg void OnClickMibTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSelchangedMibTree(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnFileLoadNewMib();
	afx_msg void OnFileExit();
	afx_msg void OnFileUnloadselectedmib();
	afx_msg void OnFileUnloadallmibs();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnSizing (UINT nSide, LPRECT lpRect);
	afx_msg void OnPaint();
	virtual BOOL OnInitDialog();
	afx_msg void OnClearStatBtn();
	afx_msg void OnBatchSetenvironmentvariablename();
	afx_msg void OnBatchLoadmibsfromthevariable();
	afx_msg void OnEditShowElementSyntax();
	afx_msg void OnEditFindElementbyname();
	afx_msg void OnFindBtn();
	afx_msg void OnSyntaxBtn();
	afx_msg void OnEditUtilities();
	afx_msg void OnUtilitiesBtn();
	afx_msg void OnUpdateMiscellaneousShowcheckboxes(CCmdUI* pCmdUI);
	afx_msg void OnMiscellaneousShowcheckboxes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	virtual void OnCancel ();
	virtual void OnOK ();


//////////////
// Members
//////////////

public:
	/**
	*	@brief		The root element in the TreeCtrl.
	*/
	HTREEITEM m_hRootElement;

	////////////////////////////////////////////////
	////                                        ////
	////  Any dialogs used by the application.  ////
	////                                        ////
	////////////////////////////////////////////////

	/**
	*	@brief		The File dialog for loading MIBs.
	*/
	CFileDialog *m_pLoadDialog;
	/**
	*	@brief		Application's own find dialog. Constructed on first use.
	*/
	CFindElementDlg *m_pFindDlg;
	/**
	*	@brief		A dialog used to display syntax of the selected element. Constructed on first use.
	*/
	CSyntaxDlg *m_pSyntaxDlg;
	/**
	*	@brief		A dialog used to execute SNMP utilities. Constructed on first use.
	*/
	CUtilitiesDlg *m_pUtilitiesDlg;
	/**
	*	@brief		The list of loaded MIBs.
	*/
	CMIBFileList *m_pMIBFileList;
	/**
	*	@brief		Flag used during application initialization to make learn when the window structure is actually displayed.
	*/
	bool m_bWindowCreated;
	bool m_bShowCheckboxes;
	/**
	*	@brief		The application icon.
	*/
	HICON m_hIcon;
	/**
	*	@brief		The environment variable string. (The default is 'MIB_DIR').
	*/
	CString m_EnvVar;


// Dialog Data
	//{{AFX_DATA(CMainWnd)
	enum { IDD = IDD_MAIN_DIALOG };
	CButton	m_CtrlCloseBtn;
	CEdit	m_CtrlTextualOIDLabel;
	CEdit	m_CtrlSyntaxLabel;
	CEdit	m_CtrlStatusLabel;
	CRichEditCtrl	m_CtrlStatusBox;
	CEdit	m_CtrlNumericOIDLabel;
	CEdit	m_CtrlIndexLabel;
	CRichEditCtrl	m_CtrlExtraInfoLabel;
	CRichEditCtrl	m_CtrlDescriptionLabel;
	CEdit	m_CtrlAccessLabel;
	CTreeCtrl	m_MIBTree;
	CString	m_StatusBox;
	CString	m_AccessLabel;
	CString	m_DescriptionLabel;
	CString	m_IndexLabel;
	CString	m_NumericOIDLabel;
	CString	m_StatusLabel;
	CString	m_SyntaxLabel;
	CString	m_TextualOIDLabel;
	CString	m_ExtraInfoLabel;
	CString	m_ShortOIDLabel;
	CString	m_sNameAutocompleteString;
	//}}AFX_DATA

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //___MAIN_WND_H__
