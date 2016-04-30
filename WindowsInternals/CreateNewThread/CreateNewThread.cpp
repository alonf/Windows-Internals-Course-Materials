// CreateNewThread.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <windows.h> //Windows API
#include <iostream> //C++ I/O
#include <process.h> //VC Runtime
#include <thread> //C++ 11 threads
#include <cassert>

using namespace std;

void CreateThreadWithCreateThreadAPI();
void CreateThreadWithCreateRemoteThreadAPI();
void CreateThreadUsingWithBeginThreadCRuntimeFunction();
void CreateThreadUsingCPP11Threads();

int main()
{
	CreateThreadWithCreateThreadAPI();
	CreateThreadWithCreateRemoteThreadAPI();
	CreateThreadUsingWithBeginThreadCRuntimeFunction();
	CreateThreadUsingCPP11Threads();
    return 0;
}

DWORD WINAPI Thread(PVOID param)
{
	wcout << L"Thread started id = " << GetCurrentThreadId() << endl;
	
	PROCESSOR_NUMBER processorNumber;
	GetThreadIdealProcessorEx(GetCurrentThread(), &processorNumber);
	wcout << "Ideal processor is " << processorNumber.Number << endl;

	for (int i = 1; i <= 10; i++) 
	{
		cout << i << endl;
		Sleep(rand() % 2000);
	}
	return 10;
}

void CreateThreadWithCreateThreadAPI()
{
	DWORD id;
	HANDLE hThread = CreateThread(nullptr, 0, Thread, nullptr, 0, &id);
	wcout << L"Thread " << GetCurrentThreadId() << L" free as a thread..." << endl;
	WaitForSingleObject(hThread, INFINITE);

	DWORD code;
	GetExitCodeThread(hThread, &code);
	wcout << "Thread finished. result = " << code << endl;
	CloseHandle(hThread);
}

	
void CreateThreadWithCreateRemoteThreadAPI()
{
	DWORD id;
	LPPROC_THREAD_ATTRIBUTE_LIST pAttrlist;
	SIZE_T attrsize;
	
	//Get the required attribute list size
	InitializeProcThreadAttributeList(nullptr, 1, 0, &attrsize);
	
	pAttrlist = static_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(_malloca(attrsize));
	memset(pAttrlist, 0, attrsize);
	BOOL bOk = InitializeProcThreadAttributeList(pAttrlist, 1, 0, &attrsize);
	assert(bOk);

	PROCESSOR_NUMBER processorNumber;
	processorNumber.Group = 0;
	processorNumber.Number = 0;
	processorNumber.Reserved = 0;
	
	bOk = UpdateProcThreadAttribute(pAttrlist, 0, PROC_THREAD_ATTRIBUTE_IDEAL_PROCESSOR, &processorNumber,
		sizeof(processorNumber), nullptr, nullptr);
	assert(bOk);

	//Create new thread with Ideal processor 0
	HANDLE hThread = CreateRemoteThreadEx(GetCurrentProcess(), nullptr, 0, 
		Thread, nullptr, 0, pAttrlist, &id);
	wcout << L"Thread " << GetCurrentThreadId() << L" free as a thread..." << endl;
	WaitForSingleObject(hThread, INFINITE);

	DWORD code;
	GetExitCodeThread(hThread, &code);
	wcout << "Thread finished. result = " << code << endl;
	CloseHandle(hThread);
}

void CreateThreadUsingWithBeginThreadCRuntimeFunction()
{
	HANDLE hThread = HANDLE(_beginthreadex(nullptr, 0, _beginthreadex_proc_type(Thread), nullptr, 0, nullptr));
	wcout << L"Thread " << GetCurrentThreadId() << L" free as a thread..." << endl;
	WaitForSingleObject(hThread, INFINITE);

	DWORD code;
	GetExitCodeThread(hThread, &code);
	wcout << "Thread finished. result = " << code << endl;
	CloseHandle(hThread);
}

void CreateThreadUsingCPP11Threads()
{
	thread t([] {Thread(nullptr); });
	wcout << L"Thread " << t.get_id() << L" free as a thread..." << endl;
	t.join();
	wcout << "Thread finished." << endl;
}
