/**
*	@file		MainWnd.cpp
*	@brief		Main window class implementation file.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/


#include "StdAfx.h"
#include "resource.h"
#include "MainWnd.h"
#include "SetEnvDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern void UpdateSyntaxInfo (CSyntaxDlg *pSyntaxDlg, CMIBElement *pElement);
extern void UpdateUtilityInfo (CUtilitiesDlg *pUtilitiesDlg, CMIBElement *pElement);


CMainWnd::CMainWnd(CWnd* pParent /*=NULL*/)
	: CDialog(CMainWnd::IDD, pParent)
{
	TVINSERTSTRUCT	TreeItem;

	//{{AFX_DATA_INIT(CMainWnd)
	m_StatusBox = _T("");
	m_AccessLabel = _T("");
	m_DescriptionLabel = _T("");
	m_IndexLabel = _T("");
	m_NumericOIDLabel = _T("");
	m_StatusLabel = _T("");
	m_SyntaxLabel = _T("");
	m_TextualOIDLabel = _T("");
	m_ExtraInfoLabel = _T("");
	m_ShortOIDLabel = _T("");
	m_sNameAutocompleteString = _T("");
	//}}AFX_DATA_INIT
	AfxInitRichEdit ();

	m_bWindowCreated = false;
	m_bShowCheckboxes = false;

	m_hIcon = AfxGetApp ()->LoadIcon (IDR_APPICON);

	Create (CMainWnd::IDD, NULL);

	//	Loading bitmaps from resources and placing them into pImageList object.

	CBitmap *pBitmap;
	CImageList *imageList;

	//Initialize imageList object
	imageList = new CImageList;
	imageList->Create (ICON_CX, ICON_CY, ILC_MASK | ILC_COLOR32, 10, 10);

	// Load bitmaps

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP1));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP2));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP3));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP4));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP5));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP6));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP7));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP8));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP9));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP10));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP11));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP12));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	pBitmap = new CBitmap;
	pBitmap->LoadBitmap (MAKEINTRESOURCE (IDB_BITMAP13));
	imageList->Add (pBitmap, RGB(255, 255, 255));
	delete pBitmap;

	m_MIBTree.SetItemHeight (ICON_CY);

	m_MIBTree.SetImageList (imageList, TVSIL_NORMAL);

	//Initialize root item in the TreeCtrl
/*
 typedef struct tagTVINSERTSTRUCT {
    HTREEITEM hParent;
    HTREEITEM hInsertAfter;
#if (_WIN32_IE >= 0x0400)
    union
    {
        TVITEMEX itemex;
        TVITEM item;
    } DUMMYUNIONNAME;
#else
    TVITEM item;
#endif
} TVINSERTSTRUCT, FAR *LPTVINSERTSTRUCT;


typedef struct tagTVITEM{
    UINT      mask;
    HTREEITEM hItem;
    UINT      state;
    UINT      stateMask;
    LPTSTR    pszText;
    int       cchTextMax;
    int       iImage;
    int       iSelectedImage;
    int       cChildren;
    LPARAM    lParam;
} TVITEM, FAR *LPTVITEM;
	*/
	TreeItem.hParent = NULL;
	TreeItem.hInsertAfter = TVI_SORT;
	TreeItem.item.iImage = ETYPE_MIB_ROOT;
	TreeItem.item.iSelectedImage = ETYPE_MIB_ROOT;
	TreeItem.item.pszText = "Loaded MIB modules";
	TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;
	m_hRootElement = this->m_MIBTree.InsertItem (&TreeItem);

	//Initialize dialog object pointers
	m_pLoadDialog = NULL;
	m_pSyntaxDlg = NULL;
	m_pFindDlg = NULL;
	m_pUtilitiesDlg = NULL;

	//Initialize the list of loaded MIBs
	m_pMIBFileList = new CMIBFileList;

	//Initial name of the environment variable
	this->m_EnvVar.LoadString (IDS_ENV_STRING);

}


void CMainWnd::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMainWnd)
	DDX_Control(pDX, IDD_CLOSE_BTN, m_CtrlCloseBtn);
	DDX_Control(pDX, IDC_TEXTUAL_OID_LABEL, m_CtrlTextualOIDLabel);
	DDX_Control(pDX, IDC_SYNTAX_LABEL, m_CtrlSyntaxLabel);
	DDX_Control(pDX, IDC_STATUS_LABEL, m_CtrlStatusLabel);
	DDX_Control(pDX, IDC_STATUS_BOX, m_CtrlStatusBox);
	DDX_Control(pDX, IDC_NUMERIC_OID_LABEL, m_CtrlNumericOIDLabel);
	DDX_Control(pDX, IDC_INDEX_LABEL, m_CtrlIndexLabel);
	DDX_Control(pDX, IDC_EXTRAINFO_LABEL, m_CtrlExtraInfoLabel);
	DDX_Control(pDX, IDC_DESCRIPTION_LABEL, m_CtrlDescriptionLabel);
	DDX_Control(pDX, IDC_ACCESS_LABEL, m_CtrlAccessLabel);
	DDX_Control(pDX, IDC_MIB_TREE, m_MIBTree);
	DDX_Text(pDX, IDC_STATUS_BOX, m_StatusBox);
	DDX_Text(pDX, IDC_ACCESS_LABEL, m_AccessLabel);
	DDX_Text(pDX, IDC_DESCRIPTION_LABEL, m_DescriptionLabel);
	DDX_Text(pDX, IDC_INDEX_LABEL, m_IndexLabel);
	DDX_Text(pDX, IDC_NUMERIC_OID_LABEL, m_NumericOIDLabel);
	DDX_Text(pDX, IDC_STATUS_LABEL, m_StatusLabel);
	DDX_Text(pDX, IDC_SYNTAX_LABEL, m_SyntaxLabel);
	DDX_Text(pDX, IDC_TEXTUAL_OID_LABEL, m_TextualOIDLabel);
	DDX_Text(pDX, IDC_EXTRAINFO_LABEL, m_ExtraInfoLabel);
	DDX_Text(pDX, IDC_SHORT_OID_LABEL, m_ShortOIDLabel);
	DDX_CBString(pDX, IDC_AUTOCOMPLETE_CBX, m_sNameAutocompleteString);
	//}}AFX_DATA_MAP
}

 
BEGIN_MESSAGE_MAP (CMainWnd, CDialog)
	//{{AFX_MSG_MAP(CMainWnd)
	ON_BN_CLICKED(IDD_CLOSE_BTN, OnCloseBtn)
	ON_NOTIFY(NM_CLICK, IDC_MIB_TREE, OnClickMibTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_MIB_TREE, OnSelchangedMibTree)
	ON_COMMAND(ID_FILE_LOAD_NEW_MIB, OnFileLoadNewMib)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_UNLOADSELECTEDMIB, OnFileUnloadselectedmib)
	ON_COMMAND(ID_FILE_UNLOADALLMIBS, OnFileUnloadallmibs)
	ON_WM_SIZE()
	ON_WM_SIZING()
	ON_WM_PAINT()
	ON_BN_CLICKED(IDC_CLEAR_STAT_BTN, OnClearStatBtn)
	ON_COMMAND(ID_BATCH_SETENVIRONMENTVARIABLENAME, OnBatchSetenvironmentvariablename)
	ON_COMMAND(ID_BATCH_LOADMIBSFROMTHEVARIABLE, OnBatchLoadmibsfromthevariable)
	ON_COMMAND(ID_EDIT_SHOWELEMENTSYNTAX, OnEditShowElementSyntax)
	ON_COMMAND(ID_EDIT_FIND_ELEMENTBYNAME, OnEditFindElementbyname)
	ON_BN_CLICKED(IDC_FIND_BTN, OnFindBtn)
	ON_BN_CLICKED(IDC_SYNTAX_BTN, OnSyntaxBtn)
	ON_COMMAND(ID_EDIT_UTILITIES, OnEditUtilities)
	ON_BN_CLICKED(IDC_UTILITIES_BTN, OnUtilitiesBtn)
	ON_UPDATE_COMMAND_UI(ID_MISCELLANEOUS_SHOWCHECKBOXES, OnUpdateMiscellaneousShowcheckboxes)
	ON_COMMAND(ID_MISCELLANEOUS_SHOWCHECKBOXES, OnMiscellaneousShowcheckboxes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP ()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers


BOOL CMainWnd::PreTranslateMessage (MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if ((pMsg->wParam == VK_F) && (GetAsyncKeyState (VK_CONTROL) && 0x8000))
		{
			OnEditFindElementbyname ();
			return TRUE;
		}
	}
	return CDialog::PreTranslateMessage (pMsg);
}


void CMainWnd::OnCloseBtn ()
{
	OnFileExit ();
}


void CMainWnd::OnCancel ()
{
	OnFileExit ();
}


void CMainWnd::OnOK ()
{
	//Prevent the window from destroying after the user presses ENTER.
}


//Adds status message to the status box and a new line sequence
void CMainWnd::StatusMessage (CString message)
{
	m_StatusBox += message + "\n";
	UpdateData (false);
}



void CMainWnd::OnClickMibTree (NMHDR* pNMHDR, LRESULT* pResult) 
{
	this->UpdateData (false);

	*pResult = 0;
}


HTREEITEM CMainWnd::AddFileToTree (CMIBFile *MIBFile)
{
	CMIBElementList *pFileTree;
	CString InfoTip;

	assert (MIBFile);

	pFileTree = MIBFile->m_pTreeElementList;

	assert (pFileTree);

//	StatusMessage ("Reading in tree structure...");

	//Add this file list to tree.
	MIBFile->m_hTreeItem = AddListToTree (m_hRootElement, pFileTree);
	
	//And add it to the MIBFileList
	m_pMIBFileList->AddHead (MIBFile);
	InfoTip.Format ("This file contains %d recognized elements.", MIBFile->m_pFlatElementList->GetCount ());
	
	StatusMessage (InfoTip);
//	StatusMessage ("File analysis complete.");

	return MIBFile->m_hTreeItem;
}


////////////////////////////////////////////////////////////////////////////////////////////
// CMainWnd::AddListToTree ()
////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM CMainWnd::AddListToTree (HTREEITEM hParentItem, CMIBElementList *pElementList)
{
	CMIBElement *pElement;
	POSITION ListPos;
	TVINSERTSTRUCT	TreeItem;
	HTREEITEM hThisItem;	//HTREEITEM value of the ETYPE_MIB_ROOT element in the list, or NULL if no such element is found.

	//If the list is empty, simply return false
	//For iteration purposes, this means the element has no child elements.
	if (!pElementList) return false;

	hThisItem = NULL;
	
	//Start iterating from the beginning of the structure.
	ListPos = pElementList->GetHeadPosition ();

	while (ListPos)
	{
		pElement = pElementList->GetNextElement (ListPos);
		if (!pElement) break;

		switch (pElement->m_nElementType)
		{
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_MIB_ROOT:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_MIB_ROOT;
				TreeItem.item.iSelectedImage = ETYPE_MIB_ROOT;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
				hThisItem = pElement->m_hTreeItem;
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_MIB_ROOT_KEY:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_MIB_ROOT;
				TreeItem.item.iSelectedImage = ETYPE_MIB_ROOT;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_IMPORTS_SECTION:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_CLOSED_FOLDER;
				TreeItem.item.iSelectedImage = ETYPE_OPEN_FOLDER;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_IMPORT_MODULE:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_MIB_ROOT;
				TreeItem.item.iSelectedImage = ETYPE_MIB_ROOT;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_IMPORTED_ELEMENT:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_READ_ONLY_ENTRY;
				TreeItem.item.iSelectedImage = ETYPE_READ_ONLY_ENTRY;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_OID:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_CLOSED_FOLDER;
				TreeItem.item.iSelectedImage = ETYPE_OPEN_FOLDER;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_TEXTUAL_CONV:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_TEXTUAL_CONV;
				TreeItem.item.iSelectedImage = ETYPE_TEXTUAL_CONV;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_TABLE_ROOT:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_TABLE_ROOT;
				TreeItem.item.iSelectedImage = ETYPE_TABLE_ROOT;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_TABLE_ITEM:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_TABLE_ITEM;
				TreeItem.item.iSelectedImage = ETYPE_TABLE_ITEM;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_TABLE_INDEX_FIELD:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_TABLE_INDEX_FIELD;
				TreeItem.item.iSelectedImage = ETYPE_TABLE_INDEX_FIELD;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_TABLE_EXTERNAL_INDEX_FIELD:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_TABLE_EXTERNAL_INDEX_FIELD;
				TreeItem.item.iSelectedImage = ETYPE_TABLE_EXTERNAL_INDEX_FIELD;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_READ_ONLY_ENTRY:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_READ_ONLY_ENTRY;
				TreeItem.item.iSelectedImage = ETYPE_READ_ONLY_ENTRY;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_READ_WRITE_ENTRY:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_READ_WRITE_ENTRY;
				TreeItem.item.iSelectedImage = ETYPE_READ_WRITE_ENTRY;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		case ETYPE_TRAP:
			{
				TreeItem.hParent = hParentItem;
				TreeItem.hInsertAfter = TVI_LAST;
				TreeItem.item.iImage = ETYPE_TRAP;
				TreeItem.item.iSelectedImage = ETYPE_TRAP;
				TreeItem.item.pszText = (char *)LPCSTR (pElement->m_ElementString);
				TreeItem.item.lParam = (LPARAM)pElement;
				TreeItem.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
				pElement->m_hTreeItem = m_MIBTree.InsertItem (&TreeItem);	
			}
			break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
		}

		//Here we need to perform a recursive addition of the list in
		//case an element contains SubObjectList
		AddListToTree (pElement->m_hTreeItem, pElement->m_pSubobjectList);
	}
	return hThisItem;
}



/*
	This method is called whenever a selection on the MIB tree changes.
*/
void CMainWnd::OnSelchangedMibTree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = (NMTREEVIEW*)pNMHDR;
	// TODO: Add your control notification handler code here
	// TO DO: Write a handler routine to respond to item change in the
	// TreeCtrl
	//
	// typedef struct tagNMTREEVIEW {
	//    NMHDR hdr; 
	//    UINT action; 
	//    TVITEM itemOld; 
	//    TVITEM itemNew; 
	//    POINT ptDrag; 
	//} NMTREEVIEW, FAR *LPNMTREEVIEW; 
	//
	//typedef struct tagTVITEM{
	//    UINT      mask;
	//    HTREEITEM hItem;
	//    UINT      state;
	//    UINT      stateMask;
	//    LPTSTR    pszText;
	//    int       cchTextMax;
	//    int       iImage;
	//    int       iSelectedImage;
	//    int       cChildren;
	//    LPARAM    lParam;
	//} TVITEM, FAR *LPTVITEM;
	CMIBElement *pElement;

	pElement = (CMIBElement *)pNMTreeView->itemNew.lParam;

	m_ShortOIDLabel.Empty ();
	m_TextualOIDLabel.Empty ();
	m_NumericOIDLabel.Empty ();
	m_SyntaxLabel.Empty ();
	m_AccessLabel.Empty ();
	m_IndexLabel.Empty ();
	m_StatusLabel.Empty ();
	m_DescriptionLabel.Empty ();
	m_ExtraInfoLabel = "   ";
	UpdateData (false);

	//This can be no element
	if (!pElement) 
	{
		return;
	}


	switch (pElement->m_nElementType)
	{
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_MIB_ROOT:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = "";
			m_SyntaxLabel = "";
			m_AccessLabel = "";
			m_IndexLabel = "";
			m_StatusLabel = "";
			m_DescriptionLabel = "ETYPE_MIB_ROOT";
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_MIB_ROOT_KEY:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = "";
			m_SyntaxLabel = "";
			m_AccessLabel = "";
			m_IndexLabel = "";
			m_StatusLabel = "";
			m_DescriptionLabel = "ROOT KEY";
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_OID:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_READ_ONLY_ENTRY:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_TABLE_ROOT:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_TABLE_ITEM:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_TABLE_INDEX_FIELD:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_TABLE_EXTERNAL_INDEX_FIELD:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = "This is an external index of the table.\n" + pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_READ_WRITE_ENTRY:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_TextualOIDLabel = pElement->BuildFullElementName ();
			m_NumericOIDLabel = pElement->BuildFullElementValue ();
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_IMPORTS_SECTION:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = "";
			m_SyntaxLabel = "";
			m_AccessLabel = "";
			m_IndexLabel = "";
			m_StatusLabel = "";
			m_DescriptionLabel = "Imported elements";
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_IMPORT_MODULE:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = "";
			m_SyntaxLabel = "";
			m_AccessLabel = "";
			m_IndexLabel = "";
			m_StatusLabel = "";
			m_DescriptionLabel = "Import module";
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_IMPORTED_ELEMENT:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = "";
			m_SyntaxLabel = "";
			m_AccessLabel = "";
			m_IndexLabel = "";
			m_StatusLabel = "";
			m_DescriptionLabel = "Imported element.";
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_TEXTUAL_CONV:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = "";
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	case ETYPE_TRAP:
		{
			m_ShortOIDLabel = pElement->m_ElementString;
			m_NumericOIDLabel = pElement->m_ElementValue;
			m_SyntaxLabel = pElement->m_Syntax;
			m_AccessLabel = pElement->m_Access;
			m_IndexLabel = pElement->m_Index;
			m_StatusLabel = pElement->m_Status;
			m_DescriptionLabel = pElement->m_Description;
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	default:
		{
			m_ShortOIDLabel = "";
			m_TextualOIDLabel = "";
			m_NumericOIDLabel = "";
			m_SyntaxLabel = "";
			m_AccessLabel = "";
			m_IndexLabel = "";
			m_StatusLabel = "";
			m_DescriptionLabel = "The type of this element has not been recognized! (Internal bug.)";
			m_ExtraInfoLabel = pElement->m_ExtraInfo;
		}
		break;
	/////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////
	}

	if (m_pSyntaxDlg)
	{
		//If the syntax window is open, update its contents with
		//syntax definition of currently selected element.
		UpdateSyntaxInfo (m_pSyntaxDlg, pElement);
	}
	if (m_pUtilitiesDlg)
	{
		//If the utilities window is open, update its contents with
		//the data of currently selected element.
		UpdateUtilityInfo (m_pUtilitiesDlg, pElement);
	}
	UpdateData (false);

	*pResult = 0;
}


void CMainWnd::OnFileLoadNewMib() 
{
	// TODO: Add your command handler code here
 	char lpszFileTypes[] = "MIB database files\0*.mib\0All files\0*.*\0\0";
	if (!m_pLoadDialog)
	{
		m_pLoadDialog = new CFileDialog (true);
		m_pLoadDialog->m_ofn.lpstrFilter = lpszFileTypes;
		m_pLoadDialog->m_ofn.nFilterIndex = 1;	//Select the first file type filter
		m_pLoadDialog->m_ofn.lpstrTitle = "Load a new MIB database file";
	}
	char szFullFileName[512];

	szFullFileName[0] = 0;

	m_pLoadDialog->m_ofn.lpstrFile = szFullFileName;

	if (m_pLoadDialog->DoModal () == IDOK)
	{
		//Check if such a module has already been loaded
		if (m_pMIBFileList->FindElement (szFullFileName))
		{
			//Notify the user that such a file has already been loaded.
			MessageBox ("The MIB you are trying to open has already been loaded.", "MIBBrowser question");
		}
		else
		{
			if (LoadMIBModule (szFullFileName))
			{
				CleanUp ();
			}
		}
	}
}


bool CMainWnd::LoadMIBModule (const char *szFullFileName)
{
	//This is the new MIB (not loaded yet).
	//Try to load it.
	CMIBFile *newMIB;
	bool bRetVal;

	bRetVal = false;

	newMIB = new CMIBFile;

	newMIB->m_FileName = szFullFileName;

	StatusMessage ("Loading MIB: " + newMIB->m_FileName);

	if (newMIB->ParseFile ())
	{
		StatusMessage ("MIB module has been successfuly parsed.");
		AddFileToTree (newMIB);
		bRetVal = true;
	}
	else
	{
		CString pErrorString;
		CFile DummyFile;
		DummyFile.SetFilePath (newMIB->m_FileName);

		pErrorString.Format ("MIB module \"%s\" could not be parsed successfuly.", (char *)(LPCSTR)DummyFile.GetFileTitle ());
		if (newMIB->m_pLastFlatElement)
		{
			pErrorString += "\nThe probable element that caused the problem is: \"" + newMIB->m_pLastFlatElement->m_ElementString + "\".";
		}
		pErrorString += "\nProbable keyword: \"" + newMIB->m_Token + "\".";
		pErrorString += "\n\nThat part of the contents of the file that has been parsed correctly will be displayed anyway.";

		MessageBox ((LPCSTR)pErrorString, "Loading error");
		AddFileToTree (newMIB);
	}
	StatusMessage ("Done.");
	m_MIBTree.Expand (m_hRootElement, TVE_EXPAND);
	return bRetVal;
}



//Exits the application
void CMainWnd::OnFileExit() 
{
	// TODO: Add your command handler code here
	if (MessageBox ("Are you sure you want to quit MIBBrowser?", "Confirmation", MB_YESNO) == IDYES)
	{
		DestroyWindow ();
	}
}



//Unloads the MIB if selected
void CMainWnd::OnFileUnloadselectedmib() 
{
	HTREEITEM hSelection;
	POSITION pos;
	CMIBFile *pUnloadedFile;

	hSelection = m_MIBTree.GetSelectedItem ();

	if (pUnloadedFile = m_pMIBFileList->FindElement (hSelection))
	{
		//Such file is loaded. We can attempt to unload it
		if (MessageBox ("Are you sure you want to unload this MIB?", "Confirmation", MB_YESNO) == IDYES)
		{
			m_MIBTree.DeleteItem (hSelection);
			pos = m_pMIBFileList->Find (pUnloadedFile);
			m_pMIBFileList->RemoveAt (pos);
			delete pUnloadedFile;
			CleanUp ();
		}
	}
	else
	{
		MessageBox ("Please select a module to unload first.");
	}
}



//Unloads all the MIBs
void CMainWnd::OnFileUnloadallmibs() 
{
	CMIBFile *pUnloadedFile;

	if (m_pMIBFileList->GetCount () == 0)
	{
		MessageBox ("There are no modules to unload.");
		return;
	}

	if (MessageBox ("Are you sure you want to unload all loaded MIBs?", "Confirmation", MB_YESNO) != IDYES)
	{
		return;
	}

	while (m_pMIBFileList->GetCount ())
	{
		pUnloadedFile = m_pMIBFileList->GetHead ();
		m_MIBTree.DeleteItem (pUnloadedFile->m_hTreeItem);
		m_pMIBFileList->RemoveHead ();
		delete pUnloadedFile;
	}
	CleanUp ();
}



void CMainWnd::OnSize (UINT nType, int cx, int cy) 
{
	// TO DO: Write code for moving elements around the window as it resizes

	if (m_bWindowCreated)
	{
		RECT rect;
		//Initial window size is: 776x583
		//Calculate what is the current window size relative to the original size.
		//positive value means enlargement, negative means shrinkage.

		//Initial control coordinates are: (455, 213) (767, 236)
		m_CtrlAccessLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlAccessLabel.MoveWindow (&rect, true);

		//Initial control coordinates are: (692, 548) (767, 571)
		m_CtrlCloseBtn.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 692);
		rect.right = cx - (776 - 767);
		rect.top = cy - (583 - 548);
		rect.bottom = cy - (583 - 571);
		this->m_CtrlCloseBtn.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 72) (767, 95)
		m_CtrlTextualOIDLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlTextualOIDLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 166) (767, 189)
		m_CtrlSyntaxLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlSyntaxLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 309) (767, 332)
		m_CtrlStatusLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlStatusLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (236, 380) (548, 443)
		m_CtrlStatusBox.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 236);
		rect.right = cx - (776 - 548);
		this->m_CtrlStatusBox.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 117) (767, 140)
		m_CtrlNumericOIDLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlNumericOIDLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 262) (767, 285)
		m_CtrlIndexLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlIndexLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 356) (767, 428)
		m_CtrlExtraInfoLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		this->m_CtrlExtraInfoLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (455, 455) (767, 543)
		m_CtrlDescriptionLabel.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		rect.bottom = cy - (583 - 543);
		this->m_CtrlDescriptionLabel.MoveWindow (&rect, true);


		//Initial control coordinates are: (11, 39) (424, 541)
		m_MIBTree.GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.right = cx - (776 - 424);
		rect.bottom = cy - (583 - 541);
		this->m_MIBTree.MoveWindow (&rect, true);

		CWnd *pLabel;

		pLabel = GetDlgItem (IDC_STATIC_STATUS_MESSAGES);
		//Initial control coordinates are: (234, 361) (318, 374)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 234);
		rect.right = cx - (776 - 318);
		pLabel->MoveWindow (&rect, true);

		pLabel = GetDlgItem (IDC_STATIC_OID_TEXT);
		//Initial control coordinates are: (453, 54) (572, 67)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 453);
		rect.right = cx - (776 - 572);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_OID_NUM);
		//Initial control coordinates are: (455, 99) (578, 112)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 578);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_SYNTAX);
		//Initial control coordinates are: (455, 148) (524, 161)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 524);
		pLabel->MoveWindow (&rect, true);

		pLabel = GetDlgItem (IDC_STATIC_ACCESS);
		//Initial control coordinates are: (455, 195) (551, 208)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 551);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_INDEX);
		//Initial control coordinates are: (455, 244) (485, 257)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 485);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_OBJECT_STATUS);
		//Initial control coordinates are: (455, 291) (521, 304)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 453);
		rect.right = cx - (776 - 519);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_DESCRIPTION);
		//Initial control coordinates are: (453, 436) (543, 449)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 453);
		rect.right = cx - (776 - 543);
//		rect.top = cy - (583 - 436);
//		rect.bottom = cy - (583 - 449);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_EXTRA_INFO);
		//Initial control coordinates are: (455, 336) (664, 349)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 664);
//		rect.top = cy - (583 - 336);
//		rect.bottom = cy - (583 - 349);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_SHORT_OID_LABEL);
		//Initial control coordinates are: (455, 28) (767, 51)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 767);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_STATIC_SHORT_OID_TEXT);
		//Initial control coordinates are: (455, 11) (559, 24)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 455);
		rect.right = cx - (776 - 559);
		pLabel->MoveWindow (&rect, true);


		pLabel = GetDlgItem (IDC_CLEAR_STAT_BTN);
		//This is not a label. This is a button.
		//Initial control coordinates are: (158, 548) (269, 571)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 158);
		rect.right = cx - (776 - 269);
		rect.top = cy - (583 - 548);
		rect.bottom = cy - (583 - 571);
		pLabel->MoveWindow (&rect, true);

		pLabel = GetDlgItem (IDC_UTILITIES_BTN);
		//This is not a label. This is a button.
		//Initial control coordinates are: (437, 548) (512, 571)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 437);
		rect.right = cx - (776 - 512);
		rect.top = cy - (583 - 548);
		rect.bottom = cy - (583 - 571);
		pLabel->MoveWindow (&rect, true);

		pLabel = GetDlgItem (IDC_SYNTAX_BTN);
		//This is not a label. This is a button.
		//Initial control coordinates are: (522, 548) (597, 571)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 522);
		rect.right = cx - (776 - 597);
		rect.top = cy - (583 - 548);
		rect.bottom = cy - (583 - 571);
		pLabel->MoveWindow (&rect, true);

		pLabel = GetDlgItem (IDC_FIND_BTN);
		//This is not a label. This is a button.
		//Initial control coordinates are: (608, 548) (683, 571)
		pLabel->GetWindowRect (&rect);
		this->ScreenToClient (&rect);
		rect.left = cx - (776 - 608);
		rect.right = cx - (776 - 683);
		rect.top = cy - (583 - 548);
		rect.bottom = cy - (583 - 571);
		pLabel->MoveWindow (&rect, true);

		this->RedrawWindow ();

	}

	CDialog::OnSize(nType, cx, cy);
}




void CMainWnd::OnSizing (UINT nSide, LPRECT lpRect)
{
	//Allow shrinking window to only the initial size.
	//Initial size of the window is its minimal allowed size.
	//The initial window size is: 784x628
	eWindowSide nWindowSide;

	nWindowSide = (eWindowSide)nSide;

	switch (nWindowSide)
	{
	case EWS_LEFT:
		//Left side is changing
		lpRect->left = min (lpRect->right - MAINWND_MINWIDTH, lpRect->left);
		break;
	case EWS_RIGHT:
		//Right side is changing
		lpRect->right = max (lpRect->left + MAINWND_MINWIDTH, lpRect->right);
		break;
	case EWS_TOP:
		//Top side is changing
		lpRect->top = min (lpRect->bottom - MAINWND_MINHEIGHT, lpRect->top);
		break;
	case EWS_LEFTTOP:
		//Left-top corner is changing
		lpRect->left = min (lpRect->right - MAINWND_MINWIDTH, lpRect->left);
		lpRect->top = min (lpRect->bottom - MAINWND_MINHEIGHT, lpRect->top);
		break;
	case EWS_RIGHTTOP:
		//Right-top corner is changing
		lpRect->right = max (lpRect->left + MAINWND_MINWIDTH, lpRect->right);
		lpRect->top = min (lpRect->bottom - MAINWND_MINHEIGHT, lpRect->top);
		break;
	case EWS_BOTTOM:
		//Bottom side is changing
		lpRect->bottom = max (lpRect->top + MAINWND_MINHEIGHT, lpRect->bottom);
		break;
	case EWS_LEFTBOTTOM:
		//Left-bottom corner is changing
		lpRect->left = min (lpRect->right - MAINWND_MINWIDTH, lpRect->left);
		lpRect->bottom = max (lpRect->top + MAINWND_MINHEIGHT, lpRect->bottom);
		break;
	case EWS_RIGHTBOTTOM:
		//Right-bottom corner is changing
		lpRect->right = max (lpRect->left + MAINWND_MINWIDTH, lpRect->right);
		lpRect->bottom = max (lpRect->top + MAINWND_MINHEIGHT, lpRect->bottom);
		break;
	}
}




void CMainWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
	
	// Do not call CDialog::OnPaint() for painting messages
}


BOOL CMainWnd::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon (m_hIcon, TRUE);		// Set big icon
	SetIcon (m_hIcon, FALSE);		// Set small icon

	m_sNameAutocompleteString.LoadString (IDS_INACTIVE_CONTROL_STRING);
	UpdateData (FALSE);

	//The window has been created, so we can set a flag for OnSize ()
	//indicating we can resize window.
	m_bWindowCreated = true;

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



void CMainWnd::OnClearStatBtn() 
{
	m_StatusBox = "  ";
	UpdateData (false);
	m_StatusBox.Empty ();
	UpdateData (false);
}



//This method retrieves the new environment variable name from the user,
//using the appropriate dialog box.
void CMainWnd::OnBatchSetenvironmentvariablename() 
{
	CSetEnvDialog SetEnvDialog;

	SetEnvDialog.m_EnvName = m_EnvVar;

	if (SetEnvDialog.DoModal () == IDOK)
	{
		m_EnvVar = SetEnvDialog.m_EnvName;
	}
}



//This method expands the environment variable name to a directory path
//and tries to load all *.mib files contained in the folder.
//Note: UNC paths are supported.
void CMainWnd::OnBatchLoadmibsfromthevariable() 
{
	char pszDirectory[1024];
	CString VarString;
	CString DirPath;
	CString InfoMessage;
	CString FullMIBFileName;
	WIN32_FIND_DATA FindDataStruct;
	HANDLE hFindHandle;
	int nFilesLoaded, nTotalMatchingFiles;

	VarString = CString ("%") + m_EnvVar + CString ("%");

	if (!ExpandEnvironmentStrings (VarString, pszDirectory, sizeof (pszDirectory)))
	{
		MessageBox ("Cannot expand the specified environment variable. Make sure the variable is set prior to calling this function.", "Environment variable");
		return;
	}

	//If variable expanding gives the unchanged string, that means
	//the variable is most probably undefined
	if (!VarString.Compare (pszDirectory))
	{
		InfoMessage.Format ("\"%s\" environment variable is not defined.\n"
			"Therefore the batch processing cannot be performed.\n"
			"Make sure to define the variable, then try again.",
			m_EnvVar);

		MessageBox (InfoMessage, "Batch processing problem");
		return;
	}


	//Proceed with batch processing. The variable has been successfully expanded.
	DirPath = pszDirectory;
	DirPath += "\\*.mib";

	//We want to count how many files have been loaded.
	nFilesLoaded = 0;
	//And we want to count how many files (in total) match the file pattern.
	nTotalMatchingFiles = 0;

	hFindHandle = FindFirstFile (DirPath, &FindDataStruct);

	while (hFindHandle != INVALID_HANDLE_VALUE)
	{
		//The handle is valid, which means, the file name can be retrieved.
		nTotalMatchingFiles ++;
		
		FullMIBFileName.Format ("%s\\%s",
			pszDirectory,
			FindDataStruct.cFileName);

		//Check if such a module has already been loaded
		if (m_pMIBFileList->FindElement (FullMIBFileName))
		{
			//We do not want to load given module again,
			//but also we do not display any error message,
			//since it is a batch processing.
		}
		else
		{
			if (LoadMIBModule (FullMIBFileName))
			{
				nFilesLoaded ++;
			}
		}

		if (!FindNextFile (hFindHandle, &FindDataStruct))
		{
			break;
		}
	}

	//Cleanup.
	if (hFindHandle != INVALID_HANDLE_VALUE)
	{
		FindClose (hFindHandle);
	}

	InfoMessage.Format ("Loading MIB modules from %s directory complete.\n"
		"Total MIB files found: %d;\n"
		"Successfuly parsed files: %d.",
		pszDirectory, nTotalMatchingFiles, nFilesLoaded);

	MessageBox (InfoMessage, "Batch processing summary");
	CleanUp ();
}



BOOL CMainWnd::DestroyWindow ()
{
	// Cleanup code executed just before the application exits
	if (m_pMIBFileList)
	{
		delete m_pMIBFileList;
		m_pMIBFileList = NULL;
	}
	m_EnvVar.FreeExtra ();
	if (m_pLoadDialog)
	{
		delete m_pLoadDialog;
		m_pLoadDialog = NULL;
	}

	return CDialog::DestroyWindow();
}


//Opens the syntax dialog box.
void CMainWnd::OnEditShowElementSyntax ()
{
	HTREEITEM hSelectedTreeItem;
	CMIBElement *pSelectedElement;

	if (m_pSyntaxDlg)
	{
		m_pSyntaxDlg->ShowWindow (SW_SHOW);
		return;
	}

	m_pSyntaxDlg = new CSyntaxDlg;

	m_pSyntaxDlg->Create (CSyntaxDlg::IDD, this);
	m_pSyntaxDlg->ShowWindow (SW_SHOW);

	//The window is newly opened, so we need to update its data.
	hSelectedTreeItem = m_MIBTree.GetSelectedItem ();
	//Ensure any item is selected.
	if (hSelectedTreeItem)
	{
		pSelectedElement = (CMIBElement *)m_MIBTree.GetItemData (hSelectedTreeItem);
		if (m_pSyntaxDlg)
		{
			UpdateSyntaxInfo (m_pSyntaxDlg, pSelectedElement);
		}
	}
}


//Duplicates menu Edit/Show element syntax functionality
void CMainWnd::OnSyntaxBtn ()
{
	OnEditShowElementSyntax ();
}


//Use this function to permit the user to search for element name.
void CMainWnd::OnEditFindElementbyname ()
{
	if (m_pFindDlg)
	{
		m_pFindDlg->ShowWindow (SW_SHOW);
		return;
	}

	m_pFindDlg = new CFindElementDlg;

	m_pFindDlg->Create (CFindElementDlg::IDD, this);
	m_pFindDlg->ShowWindow (SW_SHOW);
}


//Duplicates menu Edit/Find functionality
void CMainWnd::OnFindBtn ()
{
	OnEditFindElementbyname ();
}



void CMainWnd::OnEditUtilities ()
{
	HTREEITEM hSelectedTreeItem;
	CMIBElement *pSelectedElement;

	if (m_pUtilitiesDlg)
	{
		m_pUtilitiesDlg->ShowWindow (SW_SHOW);
		return;
	}

	m_pUtilitiesDlg = new CUtilitiesDlg;

	m_pUtilitiesDlg->Create (CUtilitiesDlg::IDD, this);
	m_pUtilitiesDlg->ShowWindow (SW_SHOW);

	//The window is newly opened, so we need to update its data.
	hSelectedTreeItem = m_MIBTree.GetSelectedItem ();
	//Ensure any item is selected.
	if (hSelectedTreeItem)
	{
		pSelectedElement = (CMIBElement *)m_MIBTree.GetItemData (hSelectedTreeItem);
		if (m_pUtilitiesDlg)
		{
			UpdateUtilityInfo (m_pUtilitiesDlg, pSelectedElement);
		}
	}
	CleanUp ();
}


//Duplicates menu Edit/Utilities functionality
void CMainWnd::OnUtilitiesBtn() 
{
	OnEditUtilities ();
}


///////////////////////////////////////////////////////////////////////////////
//
// void CMainWnd::CleanUp ()
// This code is not functional in version 1.3.6
//
///////////////////////////////////////////////////////////////////////////////
void CMainWnd::CleanUp ()
{
//	if ((m_pUtilitiesDlg) && (m_pMIBFileList))
//	{
//		m_pUtilitiesDlg->ReloadAllElementNames (*m_pMIBFileList);
//	}
//	ReloadElementNamesDataBase (*m_pMIBFileList, m_csQuickSearchCB);
}


void CMainWnd::OnUpdateMiscellaneousShowcheckboxes (CCmdUI* pCmdUI) 
{
	CString sMenuString;
	// TODO: Add your command update UI handler code here
	if (m_bShowCheckboxes)
	{
		sMenuString.LoadString (MNU_SHOW_CHECK);
		pCmdUI->SetCheck (0);
	}
	else
	{
		sMenuString.LoadString (MNU_HIDE_CHECK);
		pCmdUI->SetCheck (1);
	}
	pCmdUI->SetText (sMenuString);
	m_bShowCheckboxes = !m_bShowCheckboxes;
}


void CMainWnd::OnMiscellaneousShowcheckboxes ()
{
	if (m_bWindowCreated)
	{
		if (m_bShowCheckboxes)
		{
			m_MIBTree.ModifyStyle (0, TVS_CHECKBOXES);
		}
		else
		{
			m_MIBTree.ModifyStyle (TVS_CHECKBOXES, 0);
		}
	}
}
