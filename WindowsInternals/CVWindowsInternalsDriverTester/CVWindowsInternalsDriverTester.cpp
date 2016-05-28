// CVWindowsInternalsDriverTester.cpp : Defines the entry point for the console application.
//

#include <Windows.h>
#include <iostream>
#include <cfgmgr32.h>
#include <strsafe.h>
#include <stdio.h>
#pragma comment (lib, "cfgmgr32.lib")

#define NUM_ASYNCH_IO   100
#define BUFFER_SIZE     (40*1024)

#define READER_TYPE   1
#define WRITER_TYPE   2

#define MAX_DEVPATH_LENGTH                       256

BOOLEAN G_PerformAsyncIo;
BOOLEAN G_LimitedLoops;
ULONG G_AsyncIoLoopsNum;
WCHAR G_DevicePath[MAX_DEVPATH_LENGTH];


using namespace std;

extern "C" const GUID GUID_DEVINTERFACE_CVWindowsInternalsDriver = { 0x1f8281d6, 0xa7c0, 0x4327,{ 0x82, 0xf7, 0x9a, 0x6c, 0x99, 0x98, 0x14, 0x19 } };

BOOL
GetDevicePath(
	IN  LPGUID InterfaceGuid,
	_Out_writes_(BufLen) PWCHAR DevicePath,
	_In_ size_t BufLen
);

int SendDeviceIoControl(DWORD controlCode);

int main()
{
	char c;
	cout << "Choose error control code to get a blue screen!!!" << endl;
	cout << "1. 0xBADC0DE0 - DebugCheckEx with 1,2,3,4 parameters" << endl;
	cout << "2. 0xBADC0DE1 - Access Vilation - trying to write to memory location 0" << endl;
	cout << "3. 0xBADC0DE2 - Allocating pool memory in high IRQ level" << endl;

	cin >> c;

	DWORD code = 0xBADC0DE0 + c - '0';
	return SendDeviceIoControl(code);
}

int SendDeviceIoControl(DWORD controlCode)
{

	if (!GetDevicePath(
		const_cast<LPGUID>(&GUID_DEVINTERFACE_CVWindowsInternalsDriver),
		G_DevicePath,
		sizeof(G_DevicePath) / sizeof(G_DevicePath[0])))
	{
		cerr << "Can't find device interface: " << hex << GetLastError() << endl;
		return -1;
	}

	printf("DevicePath: %ws\n", G_DevicePath);

	HANDLE hDevice = CreateFile(G_DevicePath, GENERIC_READ | GENERIC_WRITE, 0, nullptr,
		OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
	if (hDevice == INVALID_HANDLE_VALUE) // Was the device opened?
	{
		cerr << "Can't connect to the driver, error code: " << hex << GetLastError() << endl;
		return -1;
	}
	DWORD byteReturned;


	BOOL result = DeviceIoControl(hDevice, controlCode, nullptr, 0, nullptr, 0, &byteReturned, nullptr);
	if (!result)
	{
		cerr << "DeviceIoControl returned error " << hex << GetLastError() << endl;
		return -1;
	}
	CloseHandle(hDevice);
	return 0;
}

BOOL
GetDevicePath(
	_In_ LPGUID InterfaceGuid,
	_Out_writes_(BufLen) PWCHAR DevicePath,
	_In_ size_t BufLen
)
{
	CONFIGRET cr = CR_SUCCESS;
	PWSTR deviceInterfaceList = nullptr;
	ULONG deviceInterfaceListLength = 0;
	PWSTR nextInterface;
	HRESULT hr = E_FAIL;
	BOOL bRet = TRUE;

	cr = CM_Get_Device_Interface_List_Size(
		&deviceInterfaceListLength,
		InterfaceGuid,
		nullptr,
		CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
	if (cr != CR_SUCCESS) {
		printf("Error 0x%x retrieving device interface list size.\n", cr);
		goto clean0;
	}

	if (deviceInterfaceListLength <= 1) {
		bRet = FALSE;
		printf("Error: No active device interfaces found.\n"
			" Is the sample driver loaded?");
		goto clean0;
	}

	deviceInterfaceList = static_cast<PWSTR>(malloc(deviceInterfaceListLength * sizeof(WCHAR)));
	if (deviceInterfaceList == nullptr) {
		printf("Error allocating memory for device interface list.\n");
		goto clean0;
	}
	ZeroMemory(deviceInterfaceList, deviceInterfaceListLength * sizeof(WCHAR));

	cr = CM_Get_Device_Interface_List(
		InterfaceGuid,
		nullptr,
		deviceInterfaceList,
		deviceInterfaceListLength,
		CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
	if (cr != CR_SUCCESS) {
		printf("Error 0x%x retrieving device interface list.\n", cr);
		goto clean0;
	}

	nextInterface = deviceInterfaceList + wcslen(deviceInterfaceList) + 1;
	if (*nextInterface != UNICODE_NULL) {
		printf("Warning: More than one device interface instance found. \n"
			"Selecting first matching device.\n\n");
	}

	hr = StringCchCopy(DevicePath, BufLen, deviceInterfaceList);
	if (FAILED(hr)) {
		bRet = FALSE;
		printf("Error: StringCchCopy failed with HRESULT 0x%x", hr);
		goto clean0;
	}

clean0:
	if (deviceInterfaceList != nullptr) {
		free(deviceInterfaceList);
	}
	if (CR_SUCCESS != cr) {
		bRet = FALSE;
	}

	return bRet;
}
