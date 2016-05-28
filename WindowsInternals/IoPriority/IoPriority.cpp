// IoPriorityDemo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <iostream>
#include<fstream>
#include <ppl.h>

using namespace std;
using namespace Concurrency;

#define MAX_FILE_SIZE 10048576
#define FILE_DRIVE L"C:\\\\temp\\"
#define FILE_PATH L"PriorityTestFile.txt"

volatile bool bEnd = false;

void Write(HANDLE hFile, wchar_t marker);

wstring GetFileName(wchar_t marker)
{
	wstring fileName(FILE_DRIVE);
	fileName += marker;
	fileName += FILE_PATH;

	return fileName;
}

HANDLE CreateShareWriteFileHandle(wchar_t marker)
{
	wstring fileName = GetFileName(marker);

	HANDLE hFile = CreateFile(fileName.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_WRITE_THROUGH, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		wcerr << L"Error opening file: " << hex << GetLastError() << endl;
		ExitProcess(-1);
	}

	return hFile;
}


void ThreadBackgroundModePriority(HANDLE hReadyToWrite, HANDLE hStartWrite)
{
	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_BEGIN);
	HANDLE hFile = CreateShareWriteFileHandle(L'B');
	SetEvent(hReadyToWrite);
	WaitForSingleObject(hStartWrite, INFINITE);
	Write(hFile, L'B');
	SetThreadPriority(GetCurrentThread(), THREAD_MODE_BACKGROUND_END);
	CloseHandle(hFile);
}

void ThreadNormalPriority(HANDLE hReadyToWrite, HANDLE hStartWrite)
{
	HANDLE hFile = CreateShareWriteFileHandle(L'N');
	SetEvent(hReadyToWrite);
	WaitForSingleObject(hStartWrite, INFINITE);
	Write(hFile, L'N');
	CloseHandle(hFile);
}

void HandleVeryLowPriotiry(HANDLE hReadyToWrite, HANDLE hStartWrite)
{
	HANDLE hFile = CreateShareWriteFileHandle(L'L');
	__declspec(align(8)) FILE_IO_PRIORITY_HINT_INFO priorityInfo;
	priorityInfo.PriorityHint = IoPriorityHintVeryLow;

	BOOL result = SetFileInformationByHandle(hFile, FileIoPriorityHintInfo, &priorityInfo, sizeof(FILE_IO_PRIORITY_HINT_INFO));
	if (!result)
	{
		wcerr << L"Error setting priority hint: " << hex << GetLastError() << endl;
		ExitProcess(-1);
	}
	SetEvent(hReadyToWrite);
	WaitForSingleObject(hStartWrite, INFINITE);
	Write(hFile, L'L');
	CloseHandle(hFile);
}

void Write(HANDLE hFile, wchar_t marker)
{
	DWORD nWritten;

	while (true)
	{
		BOOL result = WriteFile(hFile, &marker, sizeof(marker), &nWritten, nullptr);
		if (!result)
		{
			wcerr << L"Error writing to the file: " << hex << GetLastError() << endl;
			ExitProcess(-1);
		}
		LARGE_INTEGER fileSize;

		GetFileSizeEx(hFile, &fileSize);
		if (fileSize.QuadPart >= MAX_FILE_SIZE || bEnd)
			return;
	}
}

void StartProgress(HANDLE hReadyToWrite[3], HANDLE hStartWrite)
{
	WaitForMultipleObjects(3, hReadyToWrite, TRUE, INFINITE);

	HANDLE hBFile = CreateShareWriteFileHandle(L'B');
	HANDLE hNFile = CreateShareWriteFileHandle(L'N');
	HANDLE hLFile = CreateShareWriteFileHandle(L'L');
	LARGE_INTEGER fileSize;

	//Start the write process
	SetEvent(hStartWrite);

	long long totalSize = 0;

	do
	{
		GetFileSizeEx(hBFile, &fileSize);
		totalSize += fileSize.QuadPart;

		GetFileSizeEx(hNFile, &fileSize);
		totalSize += fileSize.QuadPart;

		GetFileSizeEx(hLFile, &fileSize);
		totalSize += fileSize.QuadPart;

		wcout << L"File size: " << totalSize << endl;
		Sleep(1000);
	} while (totalSize < MAX_FILE_SIZE);
	bEnd = true;
}

HANDLE CreateManualResetEvent()
{
	HANDLE hEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);

	if (hEvent == nullptr)
	{
		wcerr << L"Error creating Win32 ManualResetEvent: " << hex << GetLastError() << endl;
		ExitProcess(-1);
	}

	return hEvent;
}

int main()
{
	DeleteFile(GetFileName(L'L').c_str());
	DeleteFile(GetFileName(L'B').c_str());
	DeleteFile(GetFileName(L'N').c_str());

	HANDLE hStartWrite = CreateManualResetEvent();
	HANDLE hBackgroundModeReadyToWrite = CreateManualResetEvent();
	HANDLE hNormalPriorityReadyToWrite = CreateManualResetEvent();
	HANDLE hVeryLowPriotiryReadyToWrite = CreateManualResetEvent();

	HANDLE readyToWrite[] = { hBackgroundModeReadyToWrite, hNormalPriorityReadyToWrite, hVeryLowPriotiryReadyToWrite };

	parallel_invoke(
		[&]() { StartProgress(readyToWrite, hStartWrite); },
		[&]() { ThreadBackgroundModePriority(hBackgroundModeReadyToWrite, hStartWrite); },
		[&]() { ThreadNormalPriority(hNormalPriorityReadyToWrite, hStartWrite); },
		[&]() { HandleVeryLowPriotiry(hVeryLowPriotiryReadyToWrite, hStartWrite); });


	HANDLE hBFile = CreateShareWriteFileHandle(L'B');
	HANDLE hNFile = CreateShareWriteFileHandle(L'N');
	HANDLE hLFile = CreateShareWriteFileHandle(L'L');
	LARGE_INTEGER fileSize;

	GetFileSizeEx(hLFile, &fileSize);
	wcout << L"Low File Size: " << fileSize.QuadPart << endl;

	GetFileSizeEx(hBFile, &fileSize);
	wcout << L"Background File Size: " << fileSize.QuadPart << endl;

	GetFileSizeEx(hNFile, &fileSize);
	wcout << L"Normal File Size: " << fileSize.QuadPart << endl;

	CloseHandle(hBFile);
	CloseHandle(hNFile);
	CloseHandle(hLFile);

	return 0;
}


