// ----------------------------------------------------------------------
// <copyright file="COMInit.cpp" company="CodeValue">
//     Copyright (c) 2011 by CodeValue Ltd. All rights reserved
// </copyright>
//
// http://codevalue.com
// Licensed under the Educational Community License version 1.0 (http://www.opensource.org/licenses/ecl1)
// This example was written as a demonstration of principles only
//
// ------------------------------------------------------------------------

// COMInit.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <shobjidl.h>
#include <Shlobj.h>
#include <iostream>
#include <Knownfolders.h>
#include <Propkey.h>

using namespace std;

//Create a new shell library
IShellLibrary *CreateLibrary(const wstring &libraryName)
{
	IShellLibrary *shellLibrary = nullptr;
	IShellItem *savedTo = nullptr;

	//Create the shell library COM object
	HRESULT hr = SHCreateLibrary(IID_PPV_ARGS(&shellLibrary));
	if (FAILED(hr))
	{
		wcerr << L"Create: Can't create Shell Library COM object. Error: " << hr << endl;
		exit(hr);
	}

	//Save the new library under the user's Libraries folder.
	//If a library with the same name is already exists, add a number to the
	//name to create unique name
	hr = shellLibrary->SaveInKnownFolder(FOLDERID_UsersLibraries, libraryName.c_str(),
		LSF_MAKEUNIQUENAME, &savedTo);
	if (FAILED(hr))
	{
		wcerr << L"Create: Can't create Shell Library. Error: " << hr << endl;
		exit(hr);
	}

	if (shellLibrary != nullptr)
		shellLibrary->Release();

	if (savedTo != nullptr)
		savedTo->Release();

	return shellLibrary;
}

// Get the library COM object and the full path of a library. 
// If shellLibrary is NULL, don't return it. 
// If libraryFullPath is NULL, don't return it. 
// Call release on the shellLibrary object. Call CoTaskMemFree on the 
// libraryFullPath 
HRESULT GetLibraryFromLibrariesFolder(PCWSTR libraryName, IShellLibrary **shellLibrary, bool openRead = true, PWSTR *libraryFullPath = nullptr)
{
	//Create the real library file name 
	wstring realLibraryName(libraryName);
	realLibraryName += L".library-ms";

	IShellItem2 *shellItem = nullptr;
	//Get the shell item that represent the library 
	HRESULT hr = SHCreateItemInKnownFolder(FOLDERID_UsersLibraries, KF_FLAG_DEFAULT_PATH | KF_FLAG_NO_ALIAS, realLibraryName.c_str(), IID_PPV_ARGS(&shellItem));
	if (FAILED(hr))
	{
		return hr;
	} //In case a file-system full path is needed

	  //extract the information from the Shell Item ParsingPath property 
	if (libraryFullPath != nullptr)
	{
		hr = shellItem->GetString(PKEY_ParsingPath, libraryFullPath);
	}
	//Get the shellLibrary object from the shell item with a read/write 
	//permissions 
	if (shellLibrary != nullptr)
	{
		hr = SHLoadLibraryFromItem(shellItem, openRead ? STGM_READ : STGM_READWRITE, IID_PPV_ARGS(shellLibrary));
	}

	if (shellItem != nullptr)
		shellItem->Release();
	return hr;
}

void ShowManageLibraryUI(const wstring &libraryName)
{
	PWSTR libraryFullPath = nullptr;
	HRESULT  hr = GetLibraryFromLibrariesFolder(libraryName.c_str(), nullptr, false, &libraryFullPath);

	if (FAILED(hr))
	{
		wcerr << L"ManageUI: Can't get library." << endl;
		exit(hr);
	}

	IShellItem  *shellItem = nullptr;
	hr = SHCreateItemFromParsingName(libraryFullPath, nullptr, IID_PPV_ARGS(&shellItem));

	if (FAILED(hr))
	{
		wcerr << L"ManageUI: Can't create COM object." << endl;
		exit(hr);
	}

	SHShowManageLibraryUI(shellItem, nullptr, libraryName.c_str(), L"Manage Library folders and settings", LMD_ALLOWUNINDEXABLENETWORKLOCATIONS);
	shellItem->Release();
	CoTaskMemFree(libraryFullPath);
}


int main()
{
	//CoInitializeEx(0, COINIT_APARTMENTTHREADED);

	CreateLibrary(L"My Library");

	ShowManageLibraryUI(L"My Library");
	return 0;
}

