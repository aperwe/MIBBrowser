/*
	Implementation of the element lookup features.

	Creation date: 2001-11-08
	Created by: Artur Perwenis

	Last modification date: 2001-11-08
	Last modified by: Artur Perwenis

	Modification history (most recent first):
------	
	Version: 001
	Version date: 2001-11-08
	Version description:
		First version.
*/

#include "MIBFile.h"
#include "MainWnd.h"
#include "Externs.h"


//Iterates through the list to find the first occurence of
//an object, whose m_ElementString equals to pElement->m_Syntax
//Returns a pointer to such an object or NULL if no object
//could be found.
//Note: This function assumes the m_pParentFile pointer is valid
//and has been initialized by a prior call to CMIBFile::ParseFile ().
CMIBElement *LookupElementSyntaxElement (CMIBElement *pElement)
{
	CMIBFile *pFile;
	CMIBElement *pLookedUpElement;
	CMIBElementList *pElementList;
	POSITION pos;

	//DEBUG
	assert (pElement);
	assert (pElement->m_pParentFile);
	assert (pElement->m_pParentFile->m_pFlatElementList);

	if (pElement->m_Syntax == "")
	{
		return NULL;
	}

	pFile = pElement->m_pParentFile;

	if (!pFile)
	{
		//the object is not attached to the file!
		//This is an erroneous situation.
		return NULL;
	}

	pElementList = pFile->m_pFlatElementList;
	if (!pElementList)
	{
		//The file has an empty element list.
		//Has it been parsed?
		//This is an erroneous situation.
		return NULL;
	}

	pos = pElementList->GetHeadPosition ();

	while (pos)
	{
		pLookedUpElement = pElementList->GetNextElement (pos);
		if (pLookedUpElement->m_ElementString.Compare (pElement->m_Syntax))
		{
			//Nothing to do. Proceed with iteration
		}
		else
		{
			//This is the element!
			return pLookedUpElement;
		}
	}

	return NULL;
}



/*
	This function searches through the tree, starting with the MIB file 
	that contains hStartFromItem.
	If the element whose name contains the lpszFindString is found,
	it returns a HTREEITEM related to that CMIBElement.
	Otherwise NULL is returned.
	NOTE: If hStartFromItem is NULL, the program searches from the beginning
	of the first MIB loaded.
*/
HTREEITEM LookupFirstElementByName (const char *lpszFindString, HTREEITEM hStartFromItem, 
									CTreeCtrl *pTreeCtrl, CMIBFileList *pFileList,
									bool bCaseSensitive)
{
	CMIBFile *pFile;
	CMIBElement *pStartingElement;
	CMIBElement *pLookedUpElement;
	CMIBElementList *pElementList;

	pStartingElement = NULL;

	if (hStartFromItem)
	{
		pStartingElement = (CMIBElement *)pTreeCtrl->GetItemData (hStartFromItem);
	}
	else
	{
		//Searching must start from the beginning
		HTREEITEM hRootItem;

		hRootItem = pTreeCtrl->GetRootItem ();
		if (hRootItem)
		{
			pStartingElement = (CMIBElement *)pTreeCtrl->GetItemData (hRootItem);
		}
	}

	if (!pStartingElement)
	{
		//The selected element is not a MIB element.
		//Iterate through the list of MIBFiles
		if (pFileList)
		{
			POSITION pos;

			pos = pFileList->GetTailPosition ();

			if (pos)
			{
				pFile = pFileList->GetPrevFile (pos);

				if (pFile)
				{
					pElementList = pFile->m_pFlatElementList;
					//Proceed with pElementList validation outside the if's
				}
				else
				{
					//There is no MIB file loaded into the tree.
					//Nothing to search through.
					return NULL;
				}
			}
			else
			{
				//The list is empty (contains 0 elements).
				//Nothing to search through.
				return NULL;
			}
		}
		else
		{
			//No file list is available
			return NULL;
		}
	}
	else
	{

		pFile = pStartingElement->m_pParentFile;

		if (!pFile)
		{
			//the object is not attached to the file!
			//This is an erroneous situation.
			return NULL;
		}

		pElementList = pFile->m_pFlatElementList;
	}

	if (!pElementList)
	{
		//The file has an empty element list.
		//Has it been parsed?
		//This is an erroneous situation.
		return NULL;
	}

	pLookedUpElement = LookupFirstElementSubstring (lpszFindString, pElementList, bCaseSensitive);


	if (pLookedUpElement)
	{
		//Found an element with the given substring.
		return pLookedUpElement->m_hTreeItem;
	}
	else
	{
		//Element not found
		return NULL;
	}
	return NULL;
}


/*
	This function searches through the tree, starting with hStartFromItem.
	if the element whose name contains the lpszFindString is found,
	it returns a HTREEITEM related to that CMIBElement.
	Otherwise NULL is returned.
	NOTE: If hStartFromItem is NULL, the program searches from the beginning
	of the first MIB loaded.
*/
HTREEITEM LookupNextElementByName (const char *lpszFindString, HTREEITEM hStartFromItem, 
									CTreeCtrl *pTreeCtrl, CMIBFileList *pFileList,
									bool bCaseSensitive)
{
	POSITION pos;
	CMIBFile *pFile;
	CMIBElement *pStartingElement;
	CMIBElement *pLookedUpElement;
	CMIBElementList *pElementList;

	pStartingElement = NULL;

	if (hStartFromItem)
	{
		pStartingElement = (CMIBElement *)pTreeCtrl->GetItemData (hStartFromItem);
	}
	else
	{
		//Searching must start from the beginning
		HTREEITEM hRootItem;

		hRootItem = pTreeCtrl->GetRootItem ();
		if (hRootItem)
		{
			pStartingElement = (CMIBElement *)pTreeCtrl->GetItemData (hRootItem);
		}
	}

	if (!pStartingElement)
	{
		//The selected element is not a MIB element.
		//Iterate through the list of MIBFiles
		if (pFileList)
		{
			POSITION pos;

			pos = pFileList->GetTailPosition ();

			if (pos)
			{
				pFile = pFileList->GetPrevFile (pos);

				if (pFile)
				{
					pElementList = pFile->m_pFlatElementList;
					//Proceed with pElementList validation outside the if's
				}
				else
				{
					//There is no MIB file loaded into the tree.
					//Nothing to search through.
					return NULL;
				}
			}
			else
			{
				//The list is empty (contains 0 elements).
				//Nothing to search through.
				return NULL;
			}
		}
		else
		{
			//No file list is available
			return NULL;
		}
	}
	else
	{

		pFile = pStartingElement->m_pParentFile;

		if (!pFile)
		{
			//the object is not attached to the file!
			//This is an erroneous situation.
			return NULL;
		}

		pElementList = pFile->m_pFlatElementList;
	}

	if (!pElementList)
	{
		//The file has an empty element list.
		//Has it been parsed?
		//This is an erroneous situation.
		return NULL;
	}


	//TO DO: Implement iteration through the list of MIB files
	//to search for the string.
	//At this point pFile contains a pointer to the MIB file
	//that contains the selected item.
	pos = pFileList->Find (pFile);

	while (pos)
	{
		pLookedUpElement = LookupNextElementSubstring (lpszFindString, pElementList, pStartingElement, bCaseSensitive);
		if (pLookedUpElement)
		{
			return pLookedUpElement->m_hTreeItem;
		}
		else
		{
			//Ensure the next iteration will ignore the pStartingElement value
			//and will search from the beginning of the list.
			pStartingElement = NULL;
		}
		//Get data about the next file.
		pFile = pFileList->GetPrevFile (pos);
		pElementList = pFile->m_pFlatElementList;
	}

	//The element has not been found.
	return NULL;
}



/*
	This function iterates through the specified list to search for the first
	element whose name contains a substring of lpszFindString.
	On successful search this function returns the pointer to the element.
	Otherwise false is returned.
	To perform a subsequent search, call LookupNextElementSubstring ().
*/
CMIBElement *LookupFirstElementSubstring (const char *lpszFindString, CMIBElementList *pElementList, bool bCaseSensitive)
{
	POSITION pos;
	CMIBElement *pLookedUpElement;

	if (!pElementList)
	{
		//No list to search specified.
		return NULL;
	}

	if (!lpszFindString)
	{
		//No string to search for.
		return NULL;
	}

	pos = pElementList->GetHeadPosition ();
	if (bCaseSensitive)
	{
		while (pos)
		{
			pLookedUpElement = pElementList->GetNextElement (pos);
			if (pLookedUpElement->m_ElementString.Find (lpszFindString) == -1)
			{
				//Nothing to do. Proceed with iteration
			}
			else
			{
				//This element contains the substring
				return pLookedUpElement;
			}
		}
	}
	else
	{
		CString CaseInsensitiveString;
		CString CaseInsensitiveMatch;

		CaseInsensitiveMatch = CString (lpszFindString);
		CaseInsensitiveMatch.MakeLower ();

		while (pos)
		{
			pLookedUpElement = pElementList->GetNextElement (pos);
			CaseInsensitiveString = pLookedUpElement->m_ElementString;
			CaseInsensitiveString.MakeLower ();
			if (CaseInsensitiveString.Find (CaseInsensitiveMatch) == -1)
			{
				//Nothing to do. Proceed with iteration
			}
			else
			{
				//This element contains the substring
				return pLookedUpElement;
			}
		}
	}
	return NULL;
}



/*
	This function iterates through the specified list to search for the first
	element whose name contains a substring of lpszFindString.
	On successful search this function returns the pointer to the element.
	Otherwise false is returned.
	To perform a subsequent search, call LookupNextElementSubstring ().
*/
CMIBElement *LookupNextElementSubstring (const char *lpszFindString, CMIBElementList *pElementList, CMIBElement *pStartingElement, bool bCaseSensitive)
{
	POSITION pos;
	CMIBElement *pLookedUpElement;

	if (!pElementList)
	{
		//No list to search specified.
		return NULL;
	}

	if (!lpszFindString)
	{
		//No string to search for.
		return NULL;
	}

	if (pStartingElement)
	{
		pos = pElementList->Find (pStartingElement);
		if (!pos)
		{
			//Such element has not been found in this list.
			//Fetch the position corresponding to the first element in this list.
			pos = pElementList->GetHeadPosition ();
		}
		else
		{
			//Skip the element that we start from.
			pElementList->GetNextElement (pos);
		}
	}
	else
	{
		pos = pElementList->GetHeadPosition ();
	}

	if (bCaseSensitive)
	{
		while (pos)
		{
			pLookedUpElement = pElementList->GetNextElement (pos);
			if (pLookedUpElement->m_ElementString.Find (lpszFindString) == -1)
			{
				//Nothing to do. Proceed with iteration
			}
			else
			{
				//This element contains the substring
				return pLookedUpElement;
			}
		}
	}
	else
	{
		CString CaseInsensitiveString;
		CString CaseInsensitiveMatch;

		CaseInsensitiveMatch = CString (lpszFindString);
		CaseInsensitiveMatch.MakeLower ();

		while (pos)
		{
			pLookedUpElement = pElementList->GetNextElement (pos);

			CaseInsensitiveString = pLookedUpElement->m_ElementString;
			CaseInsensitiveString.MakeLower ();
			if (CaseInsensitiveString.Find (CaseInsensitiveMatch) == -1)
			{
				//Nothing to do. Proceed with iteration
			}
			else
			{
				//This element contains the substring
				return pLookedUpElement;
			}
		}
	}
	return NULL;
}



void UpdateSyntaxInfo (CSyntaxDlg *pSyntaxDlg, CMIBElement *pElement)
{
	CMIBElement *pSyntaxElement;

	//DEBUG
	assert (pSyntaxDlg);

	//First we need to zero the dialog fields
	pSyntaxDlg->m_ElementName = "   ";		//fake code
	pSyntaxDlg->m_ElementName.Empty ();
	pSyntaxDlg->m_ElementSyntax = "   ";	//fake code
	pSyntaxDlg->m_ElementSyntax.Empty ();
	pSyntaxDlg->m_SyntaxDef = "   ";		//fake code
	pSyntaxDlg->m_SyntaxDef.Empty ();
	pSyntaxDlg->UpdateData (false);

	//Now, init the fields.
	if (!pElement)
	{
		//No tree element is selected, so there is no syntax element
		//available.
		return;
	}

	pSyntaxElement = LookupElementSyntaxElement (pElement);

	if (!pSyntaxElement)
	{
		//No syntax element has been defined in this file.
		pSyntaxDlg->m_ElementName = pElement->m_ElementString;
		pSyntaxDlg->m_ElementSyntax = pElement->m_Syntax;
		pSyntaxDlg->UpdateData (false);
		return;
	}

	//The syntax element has been found.
	//Extract the information from it.
	pSyntaxDlg->m_ElementName = pElement->m_ElementString;
	pSyntaxDlg->m_ElementSyntax = pSyntaxElement->m_ElementString;
	pSyntaxDlg->m_SyntaxDef = pSyntaxElement->m_Syntax;
	pSyntaxDlg->UpdateData (false);
}


//This function is called to update the contents of the CUtilitiesDlg
//when the TreeCtrl selection changes.
//If the window is not opened, nothing changes.
void UpdateUtilityInfo (CUtilitiesDlg *pUtilitiesDlg, CMIBElement *pElement)
{
	//DEBUG
	assert (pUtilitiesDlg);

	pUtilitiesDlg->UpdateData (true);

	//First we need to zero the dialog fields
	pUtilitiesDlg->m_csElementName = "   "; //fake code
	pUtilitiesDlg->m_csElementName.Empty ();
	//Add additional resetting code here.
	pUtilitiesDlg->UpdateData (false);

	//Now, init the fields.
	if (!pElement)
	{
		//No tree element is selected, so there is no syntax element
		//available.
		return;
	}

	pUtilitiesDlg->m_csElementName = pElement->m_ElementString;
	pUtilitiesDlg->UpdateData (false);
}


