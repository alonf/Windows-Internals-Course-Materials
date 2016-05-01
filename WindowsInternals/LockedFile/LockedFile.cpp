// ----------------------------------------------------------------------
// <copyright file="LockedFile.cpp" company="CodeValue">
//     Copyright (c) 2011 by CodeValue Ltd. All rights reserved
// </copyright>
//
// http://codevalue.com
// Licensed under the Educational Community License version 1.0 (http://www.opensource.org/licenses/ecl1)
// This example was written as a demonstration of principles only
//
// ------------------------------------------------------------------------

// LockedFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include <assert.h>
#include <string>
#include <memory>
using namespace std;

static wstring CreateTempFileName()
{
	
	//Get the temp directory of the current user
	DWORD result = GetTempPathW(0, L""); //Get the buffer size
	assert(result > 0);

	wchar_t *tempPath = make_shared<wchar_t>(result + 2).get(); 
	result = GetTempPath(result, tempPath);
	assert(result > 0);
	wstring path(tempPath);
		//Get a temp file name
	wchar_t tempFileName[MAX_PATH]; 
	result = GetTempFileName(path.c_str(), L"LockedFile_", 0, tempFileName);
	assert(result > 0);
	auto fileName = tempFileName + L".txt"s;
	return fileName;
}

int main()
{
	wstring tempFileName = CreateTempFileName();
	
	HANDLE hFile = CreateFileW(tempFileName.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		DWORD nError = GetLastError();
		wcout << L"Error creating file: " << nError << endl;
		return nError;
	}
	DWORD nBytes;
	WriteFile(hFile, L"Succeeded", 20, &nBytes, nullptr);

	ShellExecuteW(nullptr, L"open", tempFileName.c_str(), L"", L"", SW_SHOWNORMAL);
	wcout << L"The fIle " << tempFileName << L" is loecked... try to close the exlusive handle. Press the Enter key to try reopen." << endl;
	cin.get();
	ShellExecuteW(nullptr, L"open", tempFileName.c_str(), L"", L"", SW_SHOWNORMAL);
	CloseHandle(hFile);

	return 0;
}

