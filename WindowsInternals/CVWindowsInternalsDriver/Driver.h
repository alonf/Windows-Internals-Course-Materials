/*++

Module Name:

driver.h

Abstract:

This file contains the driver definitions.

Environment:

Kernel-mode Driver Framework

--*/

#define INITGUID

#include <ntddk.h>
#include <wdf.h>

#include "device.h"
#include "queue.h"

EXTERN_C_START
DRIVER_INITIALIZE DriverEntry;
EVT_WDF_DRIVER_DEVICE_ADD CVWindowsInternalsEvtDeviceAdd;
EVT_WDF_OBJECT_CONTEXT_CLEANUP CVWindowsInternalsEvtDriverContextCleanup;

EXTERN_C_END
