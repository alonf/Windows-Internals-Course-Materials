// ----------------------------------------------------------------------
// <copyright file="DumpFiles.cpp" company="CodeValue">
//     Copyright (c) 2011 by CodeValue Ltd. All rights reserved
// </copyright>
//
// http://codevalue.com
// Licensed under the Educational Community License version 1.0 (http://www.opensource.org/licenses/ecl1)
// This example was written as a demonstration of principles only
//
// ------------------------------------------------------------------------

// DumpFiles.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DumpFiles.h"
#include <Werapi.h>
#include <commctrl.h>
#include <Shlobj.h>
#include <Dbghelp.h>
#define MAX_LOADSTRING 100

#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "Wer.lib")
#pragma comment(lib, "DbgHelp.lib")
#pragma comment(lib, "Shell32.lib")

int zero = 0;

// Global Variables:
HINSTANCE hInst;								// current instance

BOOL				InitInstance(HINSTANCE, int);

int APIENTRY wWinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPTSTR    lpCmdLine,
	int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	ShowRestartMessage(lpCmdLine);

	DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG_DUMPFILES), ::GetDesktopWindow(), DialogProc);

	return 0;
}

void ShowRestartMessage(LPTSTR lpCmdLine)
{
	if (lstrcmpi(lpCmdLine, L"Restarted") == 0)
	{
		MessageBox(GetDesktopWindow(), L"Restarted", L"DumpFiles", MB_OK);
	}
}

// Message handler for about box.
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	__try
	{
		UNREFERENCED_PARAMETER(lParam);
		switch (message)
		{
		case WM_INITDIALOG:
			InitiateRestartManagerTimer(hDlg);
			WerRemoveExcludedApplication(L"DumpFiles.exe", FALSE);
			return static_cast<INT_PTR>(TRUE);

		case WM_CLOSE:
			EndDialog(hDlg, 0);
			return static_cast<INT_PTR>(TRUE);

		case WM_TIMER:
			UpdateRestartManagerTimer(hDlg);
			return static_cast<INT_PTR>(TRUE);

		case WM_COMMAND:
			return HandleDialogCommands(hDlg, message, wParam, lParam);

		}

	}
	__except (FailFastFilter(GetExceptionInformation()))
	{
	}
	return static_cast<INT_PTR>(FALSE);
}

DWORD FailFastFilter(EXCEPTION_POINTERS *exceptionPointer)
{
	RaiseFailFastException(exceptionPointer->ExceptionRecord, exceptionPointer->ContextRecord, 0);
	return EXCEPTION_EXECUTE_HANDLER;
}

void InitiateRestartManagerTimer(HWND hWnd)
{
	INITCOMMONCONTROLSEX InitCtrlEx;

	InitCtrlEx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	InitCtrlEx.dwICC = ICC_PROGRESS_CLASS;
	InitCommonControlsEx(&InitCtrlEx);

	HWND hProgressBar = GetDlgItem(hWnd, IDC_PROGRESS_RESTART_MANAGER_TIMER);
	SetWindowLong(hProgressBar, GWL_EXSTYLE, PBS_SMOOTHREVERSE);

	SendMessage(hProgressBar, PBM_SETRANGE, 0,
		MAKELPARAM(0, 60));
	SendMessage(hProgressBar, PBM_SETSTEP, WPARAM(1), 0);

	SetTimer(hWnd, 1, 1000, nullptr);
}

void UpdateRestartManagerTimer(HWND hWnd)
{
	static int clock = 0;

	if (++clock > 60)
	{
		KillTimer(hWnd, 1);
		return;
	}

	HWND hProgressBar = GetDlgItem(hWnd, IDC_PROGRESS_RESTART_MANAGER_TIMER);
	SendMessage(hProgressBar, PBM_STEPIT, 0, 0);
}



UINT_PTR HandleDialogCommands(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	WORD command = LOWORD(wParam);

	switch (command)
	{
	case IDC_BUTTON_DIVIDE_BY_ZERO:
		return HandleDivideByZero();

	case IDC_BUTTON_ACCESS_VIOLATION:
		return HandleAccessViolation();

	case IDC_BUTTON_HANG:
		return HandleHang();

	case IDC_BUTTON_DUMP:
		return HandleDump();

	case IDC_CHECK_REGISTER_RESTART_MANAGER:
		return HandleRegisterRestartManager(hDlg);

	case IDC_CHECK_EXCLUDE_WER:
		return HandleWERExclude(hDlg);

	}

	return static_cast<INT_PTR>(FALSE);
}


UINT_PTR HandleDivideByZero()
{
	// ReSharper disable CppEntityNeverUsed
	int x = 100 / zero;
	// ReSharper restore CppEntityNeverUsed

	return static_cast<UINT_PTR>(TRUE);
}

UINT_PTR HandleAccessViolation()
{
	int *p = reinterpret_cast<int *>(0xFF);
	*p = 42;

	return static_cast<UINT_PTR>(TRUE);
}

UINT_PTR HandleHang()
{
	SuspendThread(GetCurrentThread());
	return static_cast<UINT_PTR>(TRUE);
}


UINT_PTR HandleDump()
{
	TCHAR strPath[MAX_PATH];

	SHGetSpecialFolderPath(nullptr, strPath, CSIDL_DESKTOPDIRECTORY, FALSE);

	lstrcat(strPath, L"\\DumpFile.dmp");
	HANDLE hFile = CreateFile(strPath, GENERIC_READ | GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hFile == INVALID_HANDLE_VALUE)
	{
		return static_cast<UINT_PTR>(TRUE);
	}

	MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MiniDumpWithFullMemory, nullptr, nullptr, nullptr);
	CloseHandle(hFile);
	return static_cast<UINT_PTR>(TRUE);
}

UINT_PTR HandleRegisterRestartManager(HWND hDlg)
{
	BOOL check = IsDlgButtonChecked(hDlg, IDC_CHECK_REGISTER_RESTART_MANAGER);
	if (check)
		RegisterApplicationRestart(L"Restarted", RESTART_NO_PATCH | RESTART_NO_REBOOT);
	else
		UnregisterApplicationRestart();

	return static_cast<UINT_PTR>(TRUE);
}


UINT_PTR HandleWERExclude(HWND hDlg)
{
	BOOL check = IsDlgButtonChecked(hDlg, IDC_CHECK_EXCLUDE_WER);
	if (check)
		WerAddExcludedApplication(L"DumpFiles.exe", FALSE);
	else
		WerRemoveExcludedApplication(L"DumpFiles.exe", FALSE);

	return static_cast<UINT_PTR>(TRUE);
}
