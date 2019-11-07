/**
*	@file		AppClass.h
*	@brief		Application class.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*
*	@history
*	@item		18-Jan-2002		Artur Perwenis		Documented.
*/


#ifndef ___APPCLASS_H__
#define ___APPCLASS_H__

#include "StdAfx.h"
#include "SplashThread.h"


/**
*	@brief		Windows application class that will act as entry point for the program.
*	@author		Artur Perwenis
*	@date		18-Jan-2002
*/
class CMIBApplication : public CWinApp
{

//////////////
// Methods
//////////////

public:
	/**
	*	@brief				Overrider.
	*	@return				Initialization success status.
	*/
	BOOL InitInstance ();


//////////////
// Members
//////////////

public:
	/**
	*	@brief		Indicates whether the console has been allocated for the application.
	*	@date		18-Jan-2002
	*/
	bool m_bConsoleAllocated;
private:
	/**
	*	@brief		Holds a runtime class instance pointer for the thread
	*				that manages the display of the splash screen.
	*	@date		18-Jan-2002
	*/
	CRuntimeClass *m_pSplashThreadRTC;
};


#endif //___APPCLASS_H__
