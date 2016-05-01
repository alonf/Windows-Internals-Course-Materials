// ----------------------------------------------------------------------
// <copyright file="ExceptionHandling.cpp" company="CodeValue">
//     Copyright (c) 2011 by CodeValue Ltd. All rights reserved
// </copyright>
//
// http://codevalue.com
// Licensed under the Educational Community License version 1.0 (http://www.opensource.org/licenses/ecl1)
// This example was written as a demonstration of principles only
//
// ------------------------------------------------------------------------

// ExceptionHandling.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <functional>
#include <iostream>

using namespace std;


void CheckException(int depth)
{
	__try
	{
		if (depth == 0)
			RaiseException(5, 0, 0, nullptr);
		__try
		{
			CheckException(depth - 1);
		}
		__finally
		{
			wcout << L"Called in the finally statement, depth: " << depth << L"  Abnormal Termination:" <<
				(AbnormalTermination() ? L"True" : L"False") << endl;
		}
		wcout << L"*** This is never written" << endl;
	}
	__except (EXCEPTION_CONTINUE_SEARCH)
	{
	}

}

void Test(PWSTR message)
{
	wcout << endl << endl;
	wcout << "============================================" << endl;
	wcout << "=========   N E W    T E S T   =============" << endl;
	wcout << "============================================" << endl;
	__try
	{
		CheckException(10);
	}
	__except (EXCEPTION_EXECUTE_HANDLER)
	{
		wcout << L"Test, message:" << message << endl;;
	}
}

LONG WINAPI VectoredHandler(_EXCEPTION_POINTERS *ExceptionInfo)
{
	wcout << "Vectored Handler" << endl;
	return EXCEPTION_EXECUTE_HANDLER;
}

int main()
{
	__try
	{
		Test(L"First");

		PVOID hHandler = AddVectoredExceptionHandler(2, VectoredHandler);

		Test(L"After adding exception handler");

		RemoveVectoredExceptionHandler(hHandler);

		Test(L"Last");
	}
	__finally
	{
		wcout << L"Called in the finally statement of main. Abnormal Termination:" <<
			(AbnormalTermination() ? L"True" : L"False") << endl;
	}
	return 0;
}

