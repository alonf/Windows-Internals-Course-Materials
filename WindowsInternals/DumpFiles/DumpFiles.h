// ----------------------------------------------------------------------
// <copyright file="DumpFiles.h" company="CodeValue">
//     Copyright (c) 2011 by CodeValue Ltd. All rights reserved
// </copyright>
//
// http://codevalue.com
// Licensed under the Educational Community License version 1.0 (http://www.opensource.org/licenses/ecl1)
// This example was written as a demonstration of principles only
//
// ------------------------------------------------------------------------

#pragma once

#include "resource.h"

DWORD FailFastFilter(EXCEPTION_POINTERS *exceptionPointer);
void ShowRestartMessage(LPTSTR lpCmdLine);
INT_PTR CALLBACK DialogProc(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
UINT_PTR HandleDialogCommands(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam);
void InitiateRestartManagerTimer(HWND hWnd);
void UpdateRestartManagerTimer(HWND hWnd);
UINT_PTR HandleDivideByZero();
UINT_PTR HandleAccessViolation();
UINT_PTR HandleHang();
UINT_PTR HandleDump();
UINT_PTR HandleRegisterRestartManager(HWND hDlg);
UINT_PTR HandleWERExclude(HWND hDlg);
