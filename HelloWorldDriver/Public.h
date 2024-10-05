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
// Define an Interface Guid so that apps can find the device and talk to it.
//

DEFINE_GUID (GUID_DEVINTERFACE_HelloWorldDriver,
    0x82f80c04,0x22a1,0x4fff,0x9e,0xd7,0xa3,0x4b,0x7f,0x29,0x2f,0x6e);
// {82f80c04-22a1-4fff-9ed7-a34b7f292f6e}
