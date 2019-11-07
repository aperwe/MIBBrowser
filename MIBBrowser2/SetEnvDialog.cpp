// SetEnvDialog.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "SetEnvDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSetEnvDialog dialog


CSetEnvDialog::CSetEnvDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetEnvDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSetEnvDialog)
	m_EnvName = _T("");
	//}}AFX_DATA_INIT
}


void CSetEnvDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetEnvDialog)
	DDX_Text(pDX, IDC_ENV_EDIT, m_EnvName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetEnvDialog, CDialog)
	//{{AFX_MSG_MAP(CSetEnvDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetEnvDialog message handlers
