// ----------------------------------------------------------------------
// <copyright file="SimpleDeadlock.cpp" company="CodeValue">
//     Copyright (c) 2011 by CodeValue Ltd. All rights reserved
// </copyright>
//
// http://codevalue.com
// Licensed under the Educational Community License version 1.0 (http://www.opensource.org/licenses/ecl1)
// This example was written as a demonstration of principles only
//
// ------------------------------------------------------------------------

// SimpleDeadlock.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <ctime>
#include <Windows.h>
#include <process.h>

using namespace std;

HANDLE hMutex1;
HANDLE hMutex2;

DWORD WINAPI ThreadFunc1(LPVOID lpThreadParameter)
{
	WaitForSingleObject(hMutex1, INFINITE);
	Sleep(rand() % 200);
	WaitForSingleObject(hMutex2, INFINITE);
	ReleaseMutex(hMutex1);
	Sleep(rand() % 200);
	ReleaseMutex(hMutex2);

	return 0;
}

void ThreadFunc2()
{
	WaitForSingleObject(hMutex2, INFINITE);
	Sleep(rand() % 200);
	WaitForSingleObject(hMutex1, INFINITE);
	ReleaseMutex(hMutex2);
	Sleep(rand() % 200);
	ReleaseMutex(hMutex1);
}

int _tmain()
{
	srand(static_cast<unsigned>(time(nullptr)));

	hMutex1 = CreateMutexW(nullptr, FALSE, nullptr);
	hMutex2 = CreateMutexW(nullptr, FALSE, nullptr);

	while (true)
	{
		HANDLE hThread = CreateThread(nullptr, 0, ThreadFunc1, nullptr, 0, nullptr);
		CloseHandle(hThread);
		ThreadFunc2();
	}
}

