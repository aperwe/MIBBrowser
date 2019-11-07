// SyntaxDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "resource.h"
#include "SyntaxDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSyntaxDlg dialog


CSyntaxDlg::CSyntaxDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSyntaxDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSyntaxDlg)
	m_ElementName = _T("");
	m_ElementSyntax = _T("");
	m_SyntaxDef = _T("");
	//}}AFX_DATA_INIT
}


void CSyntaxDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSyntaxDlg)
	DDX_Text(pDX, IDC_ELEMENT_NAME, m_ElementName);
	DDX_Text(pDX, IDC_ELEMENT_SYNTAX, m_ElementSyntax);
	DDX_Text(pDX, IDC_SYNTAX_DEF, m_SyntaxDef);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSyntaxDlg, CDialog)
	//{{AFX_MSG_MAP(CSyntaxDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSyntaxDlg message handlers
