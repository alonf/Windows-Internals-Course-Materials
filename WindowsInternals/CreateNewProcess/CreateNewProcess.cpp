// CreateNewProcess.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>

using namespace std;

int main()
{
	STARTUPINFO si = { sizeof(si) };
	PROCESS_INFORMATION pi;

	wchar_t name[] = L"Notepad.exe";
	BOOL created = CreateProcess(nullptr, name, nullptr, 0, FALSE, 0, nullptr, nullptr, &si, &pi);
	if (!created)
	{
		DWORD dwErrorCode = GetLastError();
		wcerr << L"Failed to start process (error number = " << dwErrorCode << ")" << endl;
		return dwErrorCode;
	}
	//else
	wcout << L"Process started... pid = " << pi.dwProcessId
		<< L" Main thread id = " << pi.dwThreadId << endl;
	DWORD result = WaitForSingleObject(pi.hProcess, 10000);
	if (result == WAIT_TIMEOUT)
		wcout << L"Process still running..." << endl;
	else
		wcout << L"Process exited." << endl;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

