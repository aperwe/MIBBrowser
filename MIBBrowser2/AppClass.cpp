/**
*	@file		AppClass.cpp
*	@brief		Program application class.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/


#include "AppClass.h"
#include "MainWnd.h"
#include "SplashThread.h"
#ifdef USE_NEW_SNMP_INTERFACE
#include "SNMPInterface.h"
#endif //USE_NEW_SNMP_INTERFACE
#include "Externs.h"


///////////////////////////////////////////////////////////////////////////////
//The definition of CMIBApplication::InitInstance () method (function body)
///////////////////////////////////////////////////////////////////////////////
BOOL CMIBApplication::InitInstance ()
{
	CString csHelperString;
	HWND hConsoleWindow;
	HANDLE hStdout;
	COORD coordOutputSize;

	
//	SetDialogBkColor (RGB (222, 214, 206));
	SetDialogBkColor ();
	m_bConsoleAllocated = false;

	//Initialize the console for the application
	//Do this before the splash screen appears.

	if (AllocConsole ())
	{
		csHelperString.LoadString (IDS_CONSOLE_TITLE);
		SetConsoleTitle ((LPCSTR)csHelperString);
		hConsoleWindow = FindWindow (NULL, (LPCSTR)csHelperString);
		if (hConsoleWindow)
		{
			//Hide console window
			ShowWindow (hConsoleWindow, SW_HIDE);
		}
		else
		{
			//Wait a second and retry to find window.
			Sleep (MY_FINDCONSOLEWND_TIMEOUT);
			hConsoleWindow = FindWindow (NULL, (LPCSTR)csHelperString);
			if (hConsoleWindow)
			{
				//Again try to hide console window
				ShowWindow (hConsoleWindow, SW_HIDE);
			}
		}
		hStdout = GetStdHandle (STD_OUTPUT_HANDLE);
		if (hStdout != INVALID_HANDLE_VALUE)
		{
			//SetBufferSize
			coordOutputSize.X = CONSOLE_WIDTH;
			coordOutputSize.Y = CONSOLE_HEIGHT;
			SetConsoleScreenBufferSize (hStdout, coordOutputSize);
//			SetConsoleActiveScreenBuffer (hStdout);
		}
		m_bConsoleAllocated = true;
	}

	p_csConsoleOutputString = NULL;

	//Version 1.1 update:
	//Create a separate thread to display and destroy a splash screen
	m_pSplashThreadRTC = RUNTIME_CLASS (CSplashThread);

	
	AfxBeginThread (m_pSplashThreadRTC
					//int nPriority = THREAD_PRIORITY_NORMAL, 
					//UINT nStackSize = 0, 
					//DWORD dwCreateFlags = 0, 
					//LPSECURITY_ATTRIBUTES lpSecurityAttrs = NULL 
					);

	m_pMainWnd = new CMainWnd; //Create the main wnd

	//Init dialog box title and background color
	csHelperString.LoadString (IDS_MAINWINDOWTITLE);
	m_pMainWnd->SetWindowText (csHelperString);
	SetDialogBkColor ();
	m_pMainWnd->ShowWindow (m_nCmdShow); //Show the main window
	m_pMainWnd->BringWindowToTop ();
	m_pMainWnd->SetActiveWindow();

	return true;
}

//Create the object to execute the program

CMIBApplication MIBApp;


