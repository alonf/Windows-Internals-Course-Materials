// SecurityPrivilegeMapFileToLargePage.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

const int BUF_SIZE = 65536;

const wchar_t szName[] = L"LARGEPAGE";
typedef int(*GETLARGEPAGEMINIMUM)(void);

void DisplayError(wchar_t* pszAPI, DWORD dwError)
{
	LPVOID lpvMessageBuffer;

	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		nullptr, dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
		LPWSTR(&lpvMessageBuffer), 0, nullptr);

	//... now display this string
	wcerr << "Error API = " << pszAPI << endl;
	wcerr << "Error Code = " << dwError << endl;
	wcerr << "Message = " << wstring(static_cast<wchar_t *>(lpvMessageBuffer)) << endl;
	
	// Free the buffer allocated by the system
	LocalFree(lpvMessageBuffer);

	ExitProcess(GetLastError());
}

void Privilege(wchar_t* pszPrivilege, BOOL bEnable)
{
	HANDLE           hToken;
	TOKEN_PRIVILEGES tp;
	BOOL             status;
	DWORD            error;

	// open process token
	if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
		DisplayError(L"OpenProcessToken", GetLastError());

	// get the luid
	if (!LookupPrivilegeValue(nullptr, pszPrivilege, &tp.Privileges[0].Luid))
		DisplayError(L"LookupPrivilegeValue", GetLastError());

	tp.PrivilegeCount = 1;

	// enable or disable privilege
	if (bEnable)
		tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
	else
		tp.Privileges[0].Attributes = 0;

	// enable or disable privilege
	status = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, nullptr, nullptr);

	// It is possible for AdjustTokenPrivileges to return TRUE and still not succeed.
	// So always check for the last error value.
	error = GetLastError();
	if (!status || (error != ERROR_SUCCESS))
		DisplayError(L"AdjustTokenPrivileges", GetLastError());

	// close the handle
	if (!CloseHandle(hToken))
		DisplayError(L"CloseHandle", GetLastError());
}

int main()
{
	HANDLE hMapFile;
	LPCTSTR pBuf;
	DWORD size;
	GETLARGEPAGEMINIMUM pGetLargePageMinimum;
	HINSTANCE  hDll;

	// call succeeds only on Windows Server 2003 SP1 or later
	hDll = LoadLibrary(L"kernel32.dll");
	if (hDll == nullptr)
		DisplayError(L"LoadLibrary", GetLastError());

	pGetLargePageMinimum = reinterpret_cast<GETLARGEPAGEMINIMUM>(GetProcAddress(hDll,
	                                                                            "GetLargePageMinimum"));
	if (pGetLargePageMinimum == nullptr)
		DisplayError(L"GetProcAddress", GetLastError());

	size = (*pGetLargePageMinimum)();

	FreeLibrary(hDll);

	wcout << L"Page Size: " << size << endl;

	Privilege(L"SeLockMemoryPrivilege", TRUE);

	hMapFile = CreateFileMapping(
		INVALID_HANDLE_VALUE,    // use paging file
		nullptr,                    // default security
		PAGE_READWRITE | SEC_COMMIT | SEC_LARGE_PAGES,
		0,                       // max. object size
		size,                    // buffer size
		szName);                 // name of mapping object

	if (hMapFile == nullptr)
		DisplayError(L"CreateFileMapping", GetLastError());
	else
		wcout << L"File mapping object successfully created." << endl;

	Privilege(L"SeLockMemoryPrivilege", FALSE);

	pBuf = static_cast<LPWSTR>(MapViewOfFile(hMapFile,   // handle to map object
	                                         FILE_MAP_ALL_ACCESS, // read/write permission
	                                         0,
	                                         0,
	                                         BUF_SIZE));

	if (pBuf == nullptr)
		DisplayError(L"MapViewOfFile", GetLastError());
	else
		wcout << L"View of file successfully mapped." << endl;

	// do nothing, clean up an exit
	UnmapViewOfFile(pBuf);
	CloseHandle(hMapFile);
}