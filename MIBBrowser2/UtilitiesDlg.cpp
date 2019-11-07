// UtilitiesDlg.cpp : implementation file
//

#include "StdAfx.h"
#include "Windows.h"
#include "resource.h"
#include "AppClass.h"
#include "UtilitiesDlg.h"
#include "MIBFile.h"
#include "Externs.h"
#ifdef USE_NEW_SNMP_INTERFACE
#include "SNMPInterface.h"
#endif //USE_NEW_SNMP_INTERFACE


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
//#import "Kernel32.dll" no_namespace
//__declspec (dllimport) HWND GetConsoleWindow (VOID);

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesDlg dialog


CUtilitiesDlg::CUtilitiesDlg (CWnd* pParent /*=NULL*/)
	: CDialog (CUtilitiesDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUtilitiesDlg)
	m_csIPAddr = _T("");
	m_csElementName = _T("");
	m_csDataValue = _T("");
	m_csOutputEdit = _T("");
	m_nSelectedDataType = -1;
	//}}AFX_DATA_INIT
	m_bShowingOutput = false;
}


void CUtilitiesDlg::DoDataExchange (CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUtilitiesDlg)
	DDX_Control(pDX, IDC_OUTPUT_EDIT, m_OutputEditCtrl);
	DDX_Control(pDX, IDC_IP_ADDRESS_COMBOBOX, m_IPAddrCtrl);
	DDX_CBString(pDX, IDC_IP_ADDRESS_COMBOBOX, m_csIPAddr);
	DDX_Text(pDX, IDC_ELEMENTNAME_EDIT, m_csElementName);
	DDX_Text(pDX, IDC_DATAVALUE_EDIT, m_csDataValue);
	DDX_Text(pDX, IDC_OUTPUT_EDIT, m_csOutputEdit);
	DDX_CBIndex(pDX, IDC_DATATYPE_COMBOBOX, m_nSelectedDataType);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUtilitiesDlg, CDialog)
	//{{AFX_MSG_MAP(CUtilitiesDlg)
	ON_WM_SHOWWINDOW()
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_GETTABLE_BTN, OnGettableBtn)
	ON_BN_CLICKED(IDC_GETONE_BTN, OnGetoneBtn)
	ON_BN_CLICKED(IDC_SETANY_BTN, OnSetanyBtn)
	ON_BN_CLICKED(IDC_CLEARALL_BTN, OnClearallBtn)
	ON_BN_CLICKED(IDCLOSE, OnCloseBtn)
	ON_BN_CLICKED(IDC_SHOWOUTPUT_BTN, OnShowoutputBtn)
	ON_BN_CLICKED(IDC_CLEAR_CONTENS_BTN, OnClearContensBtn)
	ON_BN_CLICKED(IDC_GETMANY_BTN, OnGetmanyBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUtilitiesDlg message handlers


//if p_bDirection == true, data from MRU is retrieved and dialog is updated with this data.
//if p_bDirection == false, data from dialog is retrieved and stored in MRU.
void CUtilitiesDlg::UpdateDialogData (bool p_bDirection)
{
	CString csMRUSection;
	CString csEntryFormat;
	CString csEntry;
	int nSectionSize;
	int nMRUIterator;

	csMRUSection.LoadString (IDS_IPADDR_SECTION_NAME);
	csEntryFormat.LoadString (IDS_IPADDR_SECTION_FORMAT);
	//Create a MRU list object for storage and retrieval of the IP addresses.
	CRecentFileList RecentFileList (0, (LPCSTR)csMRUSection, (LPCSTR)csEntryFormat, 8, 48);

	if (p_bDirection)
	{
		//Direction: MRU-->dialog
		m_IPAddrCtrl.ResetContent ();
		RecentFileList.ReadList ();
		nSectionSize = RecentFileList.GetSize ();
		
		for (nMRUIterator = 0; nMRUIterator < nSectionSize; nMRUIterator++)
		{
			csEntry = RecentFileList[nMRUIterator];
			if (! csEntry.IsEmpty ())
			{
				//This is a work-around since CRecentFileList adds a prefix
				//of the current directory to the MRU items automatically.
				CFile cfIPAddr;
				cfIPAddr.SetFilePath ((LPCSTR)csEntry);
				m_IPAddrCtrl.AddString ((LPCSTR)cfIPAddr.GetFileTitle ());
			}
		}
		UpdateData (false);
	}
	else
	{
		//Direction: Dialog-->MRU
		UpdateData (true);

		//Copy the IP address string from the edit box to the list box.
		if (!m_csIPAddr.IsEmpty ())
		{
			//Avoid duplicates
			if (m_IPAddrCtrl.FindStringExact (-1, (LPCSTR)m_csIPAddr) == CB_ERR)
			{
				m_IPAddrCtrl.AddString ((LPCSTR)m_csIPAddr);
			}
		}

		//Get the number of elements in the combo box
		nSectionSize = m_IPAddrCtrl.GetCount ();

		for (nMRUIterator = 0; nMRUIterator < nSectionSize; nMRUIterator++)
		{
			if (m_IPAddrCtrl.GetLBTextLen (nMRUIterator))
			{
				m_IPAddrCtrl.GetLBText (nMRUIterator, csEntry);
				if (! csEntry.IsEmpty ())
				{
					RecentFileList.Add ((LPCSTR)csEntry);
				}
			}
		}
		RecentFileList.WriteList ();
	}
}


//Called when the window is shown/hidden
void CUtilitiesDlg::OnShowWindow (BOOL bShow, UINT nStatus) 
{
	bool bNeedUpdate;

	CDialog::OnShowWindow (bShow, nStatus);

	bNeedUpdate = false;

	RecalcWindowSize ();
	UpdateData (true);
	SetDefID (IDC_GETONE_BTN);
	UpdateDialogData (true);

	//Copy the first entry from the list box to the edit box of IP addres field.
	if (m_IPAddrCtrl.GetCount () > 0)
	{
		if (m_IPAddrCtrl.GetLBTextLen (0))
		{
			m_IPAddrCtrl.GetLBText (0, m_csIPAddr);
			bNeedUpdate = true;
		}
	}

	//When hidding/showing window, make sure the data type is selected
	if (m_nSelectedDataType == -1)
	{
		m_nSelectedDataType = 0;
		bNeedUpdate = true;
	}

	if (bNeedUpdate)
	{
		UpdateData (false);
	}

	GotoDlgCtrl (GetDlgItem (IDC_IP_ADDRESS_COMBOBOX));
}


//Called when window is about to close
void CUtilitiesDlg::OnClose () 
{
	//Store dialog data in the INI file.
	UpdateDialogData (false);

	ShowWindow (SW_HIDE);
}


//Called when Get table button is pressed
void CUtilitiesDlg::OnGettableBtn ()
{
	UpdateDialogData (false);
	if (ExecuteCommand (CUtilitiesDlg::CT_GETTAB))
	{
		//The command was successful.
		//TO DO: Add additional code here.
	}
	else
	{
		//The command failed.
		//TO DO: Notify user of an error.
		AfxMessageBox (MSG_COMMANDFAILED, MB_OK | MB_ICONEXCLAMATION);
	}
}


//Called when Get button is pressed
void CUtilitiesDlg::OnGetoneBtn ()
{
	UpdateDialogData (false);
	if (ExecuteCommand (CUtilitiesDlg::CT_GETONE))
	{
		//The command was successful.
		//TO DO: Add additional code here.
	}
	else
	{
		//The command failed.
		//TO DO: Notify user of an error.
		AfxMessageBox (MSG_COMMANDFAILED, MB_OK | MB_ICONEXCLAMATION);
	}
}


//Called when Get many button is pressed
void CUtilitiesDlg::OnGetmanyBtn() 
{
	UpdateDialogData (false);
	if (ExecuteCommand (CUtilitiesDlg::CT_GETMANY))
	{
		//The command was successful.
		//TO DO: Add additional code here.
	}
	else
	{
		//The command failed.
		//TO DO: Notify user of an error.
		AfxMessageBox (MSG_COMMANDFAILED, MB_OK | MB_ICONEXCLAMATION);
	}
}


//Called when Set button is pressed
void CUtilitiesDlg::OnSetanyBtn ()
{
	UpdateDialogData (false);
	if (ExecuteCommand (CUtilitiesDlg::CT_SETANY))
	{
		//The command was successful.
		//TO DO: Add additional code here.
	}
	else
	{
		//The command failed.
		//TO DO: Notify user of an error.
		AfxMessageBox (MSG_COMMANDFAILED, MB_OK | MB_ICONEXCLAMATION);
	}
}


//Called when Clear all button is pressed
void CUtilitiesDlg::OnClearallBtn ()
{
	UpdateDialogData (false);
	m_csElementName.Empty ();
	m_csIPAddr.Empty ();
	m_csDataValue.Empty ();
	//Add additional resetting code here.
	UpdateData (false);
}


//Called when Clear contents button is pressed
void CUtilitiesDlg::OnClearContensBtn() 
{
	m_csOutputEdit = "  ";
	UpdateData (false);
	m_csOutputEdit.Empty ();
	UpdateData (false);
}


//Called when Show output button is pressed
void CUtilitiesDlg::OnShowoutputBtn () 
{
	CWnd *pWnd;
	CString csWindowText;

	pWnd = GetDlgItem (IDC_SHOWOUTPUT_BTN);

	if (m_bShowingOutput)
	{
		m_bShowingOutput = false;
		if (pWnd)
		{
			csWindowText.LoadString (IDS_SHOW_OUTPUT_STR);
			pWnd->SetWindowText (csWindowText);
		}
	}
	else
	{
		m_bShowingOutput = true;
		if (pWnd)
		{
			csWindowText.LoadString (IDS_HIDE_OUTPUT_STR);
			pWnd->SetWindowText (csWindowText);
		}
	}
	RecalcWindowSize ();
}


//Called when Close button is pressed
void CUtilitiesDlg::OnCloseBtn ()
{
	OnClose ();
}


//Called to set window size depending on the value of m_bShowingOutput
void CUtilitiesDlg::RecalcWindowSize (void)
{
	CRect MyRect;
	CWnd *pWnd;

	GetWindowRect (MyRect);

	if (m_bShowingOutput)
	{
		//The window should be 'taller'
		MyRect.bottom = MyRect.top + (632 - 58);
		//The output controls should be enabled.
		//Output edit control.
		pWnd = GetDlgItem (IDC_OUTPUT_EDIT);
		if (pWnd)
		{
			pWnd->ShowWindow (SW_SHOW);
			pWnd->EnableWindow (true);
		}
		//Clear edit button.
		pWnd = GetDlgItem (IDC_CLEAR_CONTENS_BTN);
		if (pWnd)
		{
			pWnd->ShowWindow (SW_SHOW);
			pWnd->EnableWindow (true);
		}
	}
	else
	{
		//The window should be 'shorter'
		MyRect.bottom = MyRect.top + (632 - 358);
		//The output controls should be disabled.
		//Output edit control.
		pWnd = GetDlgItem (IDC_OUTPUT_EDIT);
		if (pWnd)
		{
			pWnd->ShowWindow (SW_HIDE);
			pWnd->EnableWindow (false);
		}
		//Clear edit button.
		pWnd = GetDlgItem (IDC_CLEAR_CONTENS_BTN);
		if (pWnd)
		{
			pWnd->ShowWindow (SW_HIDE);
			pWnd->EnableWindow (false);
		}
	}
	MoveWindow (MyRect);
}




//-------------------------------------------------------------------------------------
//
// bool CUtilitiesDlg::ExecuteCommand ()
//
//-------------------------------------------------------------------------------------
bool CUtilitiesDlg::ExecuteCommand (CUtilitiesDlg::ECommandType p_eType)
{
	bool bRetVal = false;
	CString csHelperString;
	CString csCommunityName;
	CString csLeafName;
	char lpstrEnvBuffer[256];
	CSNMPInterface *pSnmpInterface;

	LockControls (true);

	csHelperString.LoadString (IDS_COMMUNITY_ENV_VAR);
	if (GetEnvironmentVariable ((LPCTSTR)csHelperString, (LPTSTR)lpstrEnvBuffer, sizeof (lpstrEnvBuffer)))
	{
		//The environment variable IDS_COMMUNITY_ENV_VAR is set in the environment.
		//Do not use the community name in command line.
		csCommunityName.Empty ();
	}
	else
	{
		//There is no community name specified in environment.
		//Set it in command line.
		csCommunityName.LoadString (IDS_COMMUNITY_NAME);
	}

	pSnmpInterface = new CSNMPInterface;

	//If the user does not enter an index in the leaf name,
	//it is assumed that ".0" should be appended.
	csLeafName = m_csElementName;
	if (csLeafName.Find ((TCHAR)'.') == -1)
	{
		//The '.' (dot) is not present in the string.
		csLeafName += ".0";
	}

	pSnmpInterface->SetTargetComponent (m_csIPAddr);
	if (pSnmpInterface->InitializeInterface ())
	{
	}

	LockControls (false);
	return bRetVal;
}


/*
The old version of the function is defined below.

//Called by either OnSetanyBtn, GetoneBtn, or GettableBtn
bool CUtilitiesDlg::ExecuteCommand (CUtilitiesDlg::ECommandType p_eType)
{
	bool bRetVal;
	CString csCommandLine;
	CString csHelperString;
	CString csConsoleTitle;
	CString csCommunityName;
	CString csOutputHeader;
	char lpstrEnvBuffer[256];

	//For Utilities EntryPoint_xxx function calls
	int nCounter;
	int argc;
	char *argv[8];	//8 entries should be enough for every possible case.
	CString csUtilParam0;
	CString csUtilParam1;
	CString csUtilParam2;
	CString csUtilParam3;
	CString csUtilParam4;
	CString csUtilParam5;
	CString csConsoleOutputBuffer;

	bRetVal = false;
	argc = 0;		//This will be set individually by every command initializer.
	for (nCounter = 0; nCounter < 8; nCounter++)
	{
		argv[nCounter] = NULL;
	}

	LockControls (true);

	//Init the commandline.
	csHelperString.LoadString (IDS_TOOLSDIRNAME);
	csCommandLine = "\"" + csHelperString + "\\";
	//Load community name as appropriate
	csHelperString.LoadString (IDS_COMMUNITY_ENV_VAR);
	if (GetEnvironmentVariable ((LPCTSTR)csHelperString, (LPTSTR)lpstrEnvBuffer, sizeof (lpstrEnvBuffer)))
	{
		//The environment variable IDS_COMMUNITY_ENV_VAR is set in the environment.
		//Do not use the community name in command line.
		csCommunityName.Empty ();
	}
	else
	{
		//There is no community name specified in environment.
		//Set it in command line.
		csCommunityName.LoadString (IDS_COMMUNITY_NAME);
	}

	switch (p_eType)
	{
	case CUtilitiesDlg::CT_UNKNOWN:
		{
			bRetVal = false;
		}
		break;
	case CUtilitiesDlg::CT_GETONE:
		{
			CString csLeafName;
			//If the user does not enter an index in the leaf name,
			//it is assumed that ".0" should be appended.
			csLeafName = m_csElementName;
			if (csLeafName.Find ((TCHAR)'.') == -1)
			{
				//The '.' (dot) is not present in the string.
				csLeafName += ".0";
			}
			csHelperString.LoadString (IDS_GETONE_EXE_NAME);

			//Put parameter 1 for the external code.
			csUtilParam0 = csHelperString;
			argv[argc++] = (char *)(LPCSTR)csUtilParam0;

			csCommandLine += csHelperString + "\"";
			//The format: -v1 [ip.ip.ip.ip] [community] [LeafName]
			csHelperString.Format (" -v%s %s %s %s", 
									"1",
									m_csIPAddr,
									csCommunityName,
									csLeafName);

			//Put a parameter for the external code.
			argv[argc++] = (char *)"-v1";
			csUtilParam1 = m_csIPAddr;
			argv[argc++] = (char *)(LPCSTR)csUtilParam1;
			if (!csCommunityName.IsEmpty ())
			{
				csUtilParam2 = csCommunityName;
				argv[argc++] = (char *)(LPCSTR)csUtilParam2;
			}
			csUtilParam3 = csLeafName;
			argv[argc++] = (char *)(LPCSTR)csUtilParam3;
			//At this point both argc and argv are initialized to pass to the external command.

			csCommandLine += csHelperString;
			//Init the remaining part of the execution environment
			csHelperString.LoadString (IDS_CONSOLETITLE_PREFIX);
			csConsoleTitle = csHelperString;
			csHelperString.LoadString (IDS_GETONE_EXE_NAME);
			csConsoleTitle += csHelperString;
			//Prepare output header line.
			AfxFormatString2 (csOutputHeader, IDS_GETONE_HEADER, (LPCSTR)m_csIPAddr, (LPCSTR)csLeafName);
			bRetVal = true;
		}
		break;
	case CUtilitiesDlg::CT_GETMANY:
		{
			csHelperString.LoadString (IDS_GETMANY_EXE_NAME);
			csCommandLine += csHelperString + "\"";
			//The format: -v1 [ip.ip.ip.ip] [community] [LeafName]
			csHelperString.Format (" -v%s %s %s %s", 
									"1",
									m_csIPAddr,
									csCommunityName,
									m_csElementName);
			csCommandLine += csHelperString;
			//Init the remaining part of the execution environment
			csHelperString.LoadString (IDS_CONSOLETITLE_PREFIX);
			csConsoleTitle = csHelperString;
			csHelperString.LoadString (IDS_GETMANY_EXE_NAME);
			csConsoleTitle += csHelperString;
			//Prepare output header line.
			AfxFormatString2 (csOutputHeader, IDS_GETMANY_HEADER, (LPCSTR)m_csIPAddr, (LPCSTR)m_csElementName);
			bRetVal = true;
		}
		break;
	case CUtilitiesDlg::CT_GETTAB:
		{
			csHelperString.LoadString (IDS_GETTABLE_EXE_NAME);
			csCommandLine += csHelperString + "\"";
			//The format: -v1 [ip.ip.ip.ip] [community] [TableName]
			csHelperString.Format (" -v%s %s %s %s", 
									"1",
									m_csIPAddr,
									csCommunityName,
									m_csElementName);
			csCommandLine += csHelperString;
			//Init the remaining part of the execution environment
			csHelperString.LoadString (IDS_CONSOLETITLE_PREFIX);
			csConsoleTitle = csHelperString;
			csHelperString.LoadString (IDS_GETTABLE_EXE_NAME);
			csConsoleTitle += csHelperString;
			//Prepare output header line.
			AfxFormatString2 (csOutputHeader, IDS_GETTABLE_HEADER, (LPCSTR)m_csIPAddr, (LPCSTR)m_csElementName);
			bRetVal = true;
		}
		break;
	case CUtilitiesDlg::CT_SETANY:
		{
			CString csLeafName;
			CString csVarType;
			CString csOutputHeader2;
			//If the user does not enter an index in the leaf name,
			//it is assumed that ".0" should be appended.
			csLeafName = m_csElementName;
			if (csLeafName.Find ((TCHAR)'.') == -1)
			{
				//The '.' (dot) is not present in the string.
				csLeafName += ".0";
			}
			csVarType = GetSelectedDataTypeString ();
			csHelperString.LoadString (IDS_SETANY_EXE_NAME);
			csCommandLine += csHelperString + "\"";
			//The format: -v1 [ip.ip.ip.ip] [community] [LeafName] [VarType] [Value]
			csHelperString.Format (" -v%s %s %s %s %s %s", 
									"1",
									m_csIPAddr,
									csCommunityName,
									csLeafName,
									csVarType,
									m_csDataValue);
			csCommandLine += csHelperString;
			//Init the remaining part of the execution environment
			csHelperString.LoadString (IDS_CONSOLETITLE_PREFIX);
			csConsoleTitle = csHelperString;
			csHelperString.LoadString (IDS_SETANY_EXE_NAME);
			csConsoleTitle += csHelperString;
			//Prepare output header line.
			AfxFormatString2 (csOutputHeader2, IDS_SETANY_HEADER, (LPCSTR)m_csIPAddr, (LPCSTR)csLeafName);
			AfxFormatString2 (csOutputHeader, IDS_SETANY_HEADER2, (LPCSTR)csOutputHeader2, (LPCSTR)m_csDataValue);
			bRetVal = true;
		}
		break;
	}

	if (bRetVal)
	{
		//csCommandLine contains a full line to be executed.
//		if (AllocConsole ())
		if (((CMIBApplication *)AfxGetApp ())->m_bConsoleAllocated)
		{
			//The DOS console has been successfully allocated.
			//typedef struct _STARTUPINFO { 
			//	DWORD   cb; 
			//	LPTSTR  lpReserved; 
			//	LPTSTR  lpDesktop; 
			//	LPTSTR  lpTitle; 
			//	DWORD   dwX; 
			//	DWORD   dwY; 
			//	DWORD   dwXSize; 
			//	DWORD   dwYSize; 
			//	DWORD   dwXCountChars; 
			//	DWORD   dwYCountChars; 
			//	DWORD   dwFillAttribute; 
			//	DWORD   dwFlags; 
			//	WORD    wShowWindow; 
			//	WORD    cbReserved2; 
			//	LPBYTE  lpReserved2; 
			//	HANDLE  hStdInput; 
			//	HANDLE  hStdOutput; 
			//	HANDLE  hStdError; 
			//} STARTUPINFO, *LPSTARTUPINFO;
			//
			//
			//---------------------------------
			//
			//
			//typedef struct _PROCESS_INFORMATION { 
			//	HANDLE hProcess; 
			//	HANDLE hThread; 
			//	DWORD dwProcessId; 
			//	DWORD dwThreadId; 
			//} PROCESS_INFORMATION;
			PROCESS_INFORMATION prInf;
			STARTUPINFO stInfo;
			//-----------------------------
			HANDLE hStdout;
//			HANDLE hOldStdout;
			char charBuffer[CONSOLE_WIDTH + 1];
			CString csBufferLine;
			COORD coordOutputSize;
			COORD coordBufSize; 
			COORD coordBufCoord; 
			BOOL fSuccess; 
			DWORD dwCharsRead;
			DWORD dwNumChars;
			int nRowIterator;
			int nEmptyRowCounter;

//			SetConsoleTitle ((LPCSTR)csConsoleTitle);

			ZeroMemory (&prInf, sizeof (prInf));
			ZeroMemory (&stInfo, sizeof (stInfo));
			stInfo.cb = sizeof (stInfo);

			hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
//			hStdout = CreateConsoleScreenBuffer ( 
//						   GENERIC_READ |           // read/write access 
//						   GENERIC_WRITE, 
//						   0,                       // not shared 
//						   NULL,                    // no security attributes 
//						   CONSOLE_TEXTMODE_BUFFER, // must be TEXTMODE 
//						   NULL);                   // reserved; must be NULL 
			if (hStdout != INVALID_HANDLE_VALUE)
			{
				//Move the cursor to top-left corner of the console window
				coordBufCoord.X = 0;
				coordBufCoord.Y = 0;
				SetConsoleCursorPosition (hStdout, coordBufCoord);
				//Fill the console with spaces (blank characters)
				dwNumChars = CONSOLE_WIDTH * CONSOLE_HEIGHT;
				FillConsoleOutputCharacter (hStdout, 
											(CHAR)' ', 
											dwNumChars, 
											coordBufCoord, 
											&dwCharsRead);
				//Again move the cursor to top-left corner of the console window
				SetConsoleCursorPosition (hStdout, coordBufCoord);
				//SetBufferSize
				coordOutputSize.X = CONSOLE_WIDTH;
				coordOutputSize.Y = CONSOLE_HEIGHT;
				SetConsoleScreenBufferSize (hStdout, coordOutputSize);
//				SetConsoleActiveScreenBuffer (hStdout);

//Uncomment the below adjoining lines for original code execution
//				if (CreateProcess (
//					NULL,									//Application name
//					(LPSTR)(LPCSTR)csCommandLine,			//CommandLine
//					NULL,									//lpProcessAttributes
//					NULL,									//lpThreadAttributes
//					true,									//bInheritHandles
//					0,										//dwCreationFlags
//					NULL,									//lpEnvironment
//					NULL,									//lpCurrentDirectory
//					&stInfo,								//lpStartupInfo
//					&prInf									//lpProcessInformation
//					))

//Comment out the below line for original code execution
				if (true)
				{
				    // Wait until child process exits.
					HCURSOR hWaitCursor;
					HCURSOR hOldCursor;
					hWaitCursor = LoadCursor (//AfxGetApp ()->m_hInstance
												NULL, IDC_WAIT);
					hOldCursor = SetCursor (hWaitCursor);

//Uncomment the below line for original code execution.
//					WaitForSingleObject (prInf.hProcess, INFINITE);
//Comment out the below line for original code execution
					p_csConsoleOutputString = &csConsoleOutputBuffer;
					char ptr[OUTPUT_LINE_LEN];
					pszOutputBuffer = ptr;

//Comment out the below line for original code execution
					EntryPoint_GetOne (argc, argv);
					//Restore the original cursor.
					SetCursor (hOldCursor);

					//Now, read output from the console

					// The buffer size is 1 rows x 80 columns. 
					coordBufSize.X = CONSOLE_WIDTH;
					coordBufSize.Y = 1;

					// The top left destination cell of the buffer is 
					// row 0, col 0. 
					coordBufCoord.X = 0; 
					coordBufCoord.Y = 0; 

					//This is used to estimate, when the output ends
					//if the empty row count in a row exceedes EMPTY_ROW_LIMIT
					//it is assumed that there is no more output to get from
					//the console.
					nEmptyRowCounter = 0;

					//Initialize output:
					m_csOutputEdit += csOutputHeader + "\n";
					m_csOutputEdit += "Commandline: " + csCommandLine + "\n";
//Comment out the below line for original code execution
					m_csOutputEdit += csConsoleOutputBuffer + "\n";
//Comment out the below line for original code execution
					if (false)
						//Read one line of the screen buffer at a time
						for (nRowIterator = 0; nRowIterator < CONSOLE_HEIGHT; nRowIterator++)
						{
							coordBufCoord.Y = nRowIterator;

							fSuccess = ReadConsoleOutputCharacter (
								hStdout,
								charBuffer,
								CONSOLE_WIDTH,
								coordBufCoord,
								&dwCharsRead
								);
							
							if (fSuccess)
							{
								if (dwCharsRead <= CONSOLE_WIDTH)
								{
									//NULL-terminate the string
									charBuffer[dwCharsRead] = 0;
									csBufferLine = (LPCSTR)charBuffer;
									//Trim spaces on the right
									csBufferLine.TrimRight ();
									if (!csBufferLine.IsEmpty ())
									{
										m_csOutputEdit += csBufferLine + "\n";
										nEmptyRowCounter = 0;
									}
									else
									{
										nEmptyRowCounter++;
										if (nEmptyRowCounter > EMPTY_ROW_LIMIT)
										{
											//End of output detected.
											break;
										}
									}
								}
							}
							else
							{
								//Error in readout detected.
								AfxMessageBox (MSG_CONSOLEREADERROR, MB_OK | MB_ICONEXCLAMATION);
								break;
							}
						} //for (nRowIterator = 0; nRowIterator < CONSOLE_HEIGHT; nRowIterator++)
					//Finalize output:
					csOutputHeader.LoadString (IDS_OUTPUT_FOOTER);
					m_csOutputEdit += csOutputHeader + "\n\n";

					UpdateData (false);
				}
				else
				{
					//Error: Process could not be created.
					AfxMessageBox (MSG_CREATEPROCFAILED, MB_OK | MB_ICONEXCLAMATION);
				}
			}
			else
			{
				//An error: invalid stdout or screen buffer could not be created.
				AfxMessageBox (MSG_CONSOLEHANDLEINVALID, MB_OK | MB_ICONEXCLAMATION);
			}
//			FreeConsole ();
		}
	}
	//Unlock controls
	LockControls (false);

	return bRetVal;
}
*/

//Called to obtain a string corresponding to the data type selected in Data type
//ComboBox. Returned string is in form "-x", where 'x' can be 'i', or 'd', and so on.
const CString CUtilitiesDlg::GetSelectedDataTypeString ()
{
	CString pcsTypeTable[10] =
	{
		"-i",
		"-o",
		"-d",
		"-a",
		"-c",
		"-g",
		"-t",
		"-u",
		"-D",
		"-N"
	};
	
	if (m_nSelectedDataType == -1)
	{
		return CString ("");
	}
	//It is assumed that UpdateData () has been called prior to calling this function.
	return pcsTypeTable[m_nSelectedDataType];
}

//Global variable
CString *p_csConsoleOutputString;
char *pszOutputBuffer;

extern "C" void OutputString (const char *p_buf)
{
	if (p_csConsoleOutputString)
	{
		if (p_buf)
		{
			(*p_csConsoleOutputString) += (LPCTSTR)p_buf;
		}
	}
}

//Called to lock or unlock the controls in the window.
//Used in long-lasting operations.
//p_bLock = true - locks controls
//p_bLock = false - unlocks controls.
void CUtilitiesDlg::LockControls (bool p_bLock)
{
	CWnd *pWnd;
	bool bControlLockState;

	bControlLockState = !p_bLock;

	if (this)
	{
		this->EnableWindow (bControlLockState);
	}
	//The output controls should be disabled.
	//Output edit control.
	pWnd = GetDlgItem (IDC_IP_ADDRESS_COMBOBOX);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_ELEMENTNAME_EDIT);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_DATATYPE_COMBOBOX);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_DATAVALUE_EDIT);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_OUTPUT_EDIT);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_CLEAR_CONTENS_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Button
	//Output edit control.
	pWnd = GetDlgItem (IDC_GETONE_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_GETMANY_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_GETTABLE_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_SETANY_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_CLEARALL_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDC_SHOWOUTPUT_BTN);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}
	//Output edit control.
	pWnd = GetDlgItem (IDCLOSE);
	if (pWnd)
	{
		pWnd->EnableWindow (bControlLockState);
	}

	RecalcWindowSize ();
}


//Called to set cursor position on the Output control at the end of text
void CUtilitiesDlg::UpdateCursorPosition ()
{
	int nLineCount, nFirstVisibleLine;

	UpdateData (false);
	nLineCount = m_OutputEditCtrl.GetLineCount ();
	nFirstVisibleLine = m_OutputEditCtrl.GetFirstVisibleLine ();
	m_OutputEditCtrl.LineScroll (nLineCount - nFirstVisibleLine - 10);
//	m_OutputEditCtrl.LineScroll (nLineCount);
}

