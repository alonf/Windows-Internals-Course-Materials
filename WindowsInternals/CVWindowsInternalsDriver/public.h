/*++

Module Name:

public.h

Abstract:

This module contains the common declarations shared by driver
and user applications.

Environment:

user and kernel

--*/

//
// Define an Interface Guid so that app can find the device and talk to it.
//

DEFINE_GUID(GUID_DEVINTERFACE_CVWindowsInternals,
	0x1f8281d6, 0xa7c0, 0x4327, 0x82, 0xf7, 0x9a, 0x6c, 0x99, 0x98, 0x14, 0x19);
// {1f8281d6-a7c0-4327-82f7-9a6c99981419}
