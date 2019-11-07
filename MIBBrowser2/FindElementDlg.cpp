// FindElementDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "FindElementDlg.h"
#include "MainWnd.h"
#include "Externs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CFindElementDlg dialog


CFindElementDlg::CFindElementDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFindElementDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CFindElementDlg)
	m_FindString = _T("");
	m_bCaseSensitiveSwitch = FALSE;
	//}}AFX_DATA_INIT
}


void CFindElementDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CFindElementDlg)
	DDX_Text(pDX, IDC_FIND_STRING_EDIT_BOX, m_FindString);
	DDX_Check(pDX, IDC_CASE_SENSITIVE_CHK, m_bCaseSensitiveSwitch);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CFindElementDlg, CDialog)
	//{{AFX_MSG_MAP(CFindElementDlg)
	ON_BN_CLICKED(IDCLOSE, OnClose)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(ID_FIND_BTN, OnFindBtn)
	ON_BN_CLICKED(ID_FIND_NEXT_BTN, OnFindNextBtn)
	ON_BN_CLICKED(IDC_CASE_SENSITIVE_CHK, OnCaseSensitiveChk)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFindElementDlg message handlers
void CFindElementDlg::OnOK ()
{
	//Prevent the window from destroying after the user presses ENTER.
	InitLookupFirstElementByName ();
}



void CFindElementDlg::OnClose () 
{
	ShowWindow (SW_HIDE);
}



void CFindElementDlg::OnShowWindow (BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow (bShow, nStatus);

	//Set the Case-sensitivity switch into position
	UpdateData (true);

	SetDefID (ID_FIND_BTN);
	
	GotoDlgCtrl (GetDlgItem (IDC_FIND_STRING_EDIT_BOX));
}



//This function inits and executes the actual searching for
//element name.
void CFindElementDlg::InitLookupFirstElementByName (void)
{
	HTREEITEM hLookedUpItem;

	CMainWnd *pMainWnd;
	UpdateData (true);

	pMainWnd = (CMainWnd *)GetParent ();

	if (!pMainWnd)
	{
		//Why doesn't this window have its parent?
		//This is an error.
		return;
	}

	hLookedUpItem = LookupFirstElementByName (this->m_FindString,
		pMainWnd->m_MIBTree.GetSelectedItem (), 
		&(pMainWnd->m_MIBTree),
		pMainWnd->m_pMIBFileList,
		(bool)m_bCaseSensitiveSwitch);

	if (hLookedUpItem)
	{
		pMainWnd->m_MIBTree.SelectItem (hLookedUpItem);
		pMainWnd->SetActiveWindow ();
		pMainWnd->m_MIBTree.SetFocus ();
	}
	else
	{
		MessageBox ("Could not find an element whose name contains the specified substring.", "Substring not found");
	}
}



//This function inits and executes the actual searching for
//the next element name.
void CFindElementDlg::InitLookupNextElementByName (void)
{
	HTREEITEM hLookedUpItem;

	CMainWnd *pMainWnd;
	UpdateData (true);

	pMainWnd = (CMainWnd *)GetParent ();

	if (!pMainWnd)
	{
		//Why doesn't this window have its parent?
		//This is an error.
		return;
	}

	hLookedUpItem = LookupNextElementByName (this->m_FindString,
		pMainWnd->m_MIBTree.GetSelectedItem (), 
		&(pMainWnd->m_MIBTree),
		pMainWnd->m_pMIBFileList,
		(bool)m_bCaseSensitiveSwitch);

	if (hLookedUpItem)
	{
		pMainWnd->m_MIBTree.SelectItem (hLookedUpItem);
		pMainWnd->SetActiveWindow ();
		pMainWnd->m_MIBTree.SetFocus ();
	}
	else
	{
		MessageBox ("No more elements with the specified substring found.", "Next occurence not found");
	}
}



void CFindElementDlg::OnFindBtn () 
{
	InitLookupFirstElementByName ();
}




void CFindElementDlg::OnFindNextBtn () 
{
	InitLookupNextElementByName ();
}


void CFindElementDlg::OnCaseSensitiveChk() 
{
	UpdateData (true);
}
