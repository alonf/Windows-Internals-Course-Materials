// NativeKernelTransaction.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <Windows.h>
#include <ktmw32.h>
#include <TxDtc.h>
#include <xolehlp.h>
#include <cassert>

#pragma comment (lib, "ktmw32.lib")
#pragma comment (lib, "xolehlp.lib")

int main()
{
	CoInitialize(nullptr);
	ITransactionDispenser *pITransactionDispenser;
	ITransaction *pITransaction;
	IKernelTransaction *pKernelTransaction;
	HANDLE hTransactionHandle;

	HRESULT hr = DtcGetTransactionManagerEx(nullptr, nullptr,
		IID_ITransactionDispenser, OLE_TM_FLAG_NONE,
		nullptr, reinterpret_cast<void**>(&pITransactionDispenser));
	assert(hr == S_OK);
	
	hr = pITransactionDispenser->BeginTransaction(
		nullptr, ISOLATIONLEVEL_READCOMMITTED,
		ISOFLAG_RETAIN_BOTH, nullptr, &pITransaction);

	assert(hr == S_OK);

	hr = pITransaction->QueryInterface(
		IID_IKernelTransaction,
		reinterpret_cast<void**>(&pKernelTransaction));

	assert(hr == S_OK);

	hr = pKernelTransaction->GetHandle(&hTransactionHandle);

	HANDLE hAppend = CreateFileTransacted(L"test.txt",
		FILE_APPEND_DATA, FILE_SHARE_READ,
		nullptr, OPEN_ALWAYS,
		FILE_ATTRIBUTE_NORMAL, nullptr,
		hTransactionHandle, nullptr, nullptr);

	assert(hr == S_OK);

	DWORD nWritten;
	wchar_t text[] = L"Transaction Rocks!";
	WriteFile(hAppend, text, sizeof(text), &nWritten, nullptr);

	//Check write...

	hr = pITransaction->Commit(FALSE, XACTTC_SYNC_PHASEONE, 0);

	assert(hr == S_OK);

	CloseHandle(hAppend);
	pKernelTransaction->Release();
	pITransaction->Release();
	pITransactionDispenser->Release();

	CoUninitialize();
	return 0;
}

