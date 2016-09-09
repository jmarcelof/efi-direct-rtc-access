#include <efi.h>
#include <efilib.h>

#include "PciRootBridgeIo.h"

#define SEC_TO_USEC(Value) ((Value) * 1000 * 1000)

EFI_GUID gEfiPciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridge = NULL;

VOID
PrintTime (
    EFI_TIME            *Time);

VOID
ReadTime (
    EFI_TIME            *Time);

UINT16
BcdToDecimal (
    UINT16              Value);

VOID
WaitStopUpdate ();

UINT8
ReadRTCPort (
    UINT64              Port);

VOID
WritePort8 (
    UINT64              Port,
    UINT8               Value);

UINT8
ReadPort8 (
    UINT64              Port);

EFI_STATUS
EFIAPI
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable)
{
    EFI_STATUS 		Status = EFI_SUCCESS;
    EFI_TIME            Time;

    InitializeLib (ImageHandle, SystemTable);

    Print (L"Hello, world!\n");

    Status = uefi_call_wrapper (BS->LocateProtocol, 3, &gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **) & PciRootBridge);
    Print (L"Status = %r\n", Status);

    ReadTime (&Time);
    PrintTime (&Time);

    uefi_call_wrapper (BS->Stall, 1, SEC_TO_USEC (5));

    return (EFI_SUCCESS);
}

VOID
PrintTime (
    EFI_TIME            *Time)
{

    Print (L"%2hhd:%02hhd:%02hhd\n",
        Time->Hour, Time->Minute, Time->Second);
}

VOID
ReadTime (
    EFI_TIME            *Time)
{

    WaitStopUpdate ();
    Time->Second = ReadRTCPort (0x00);
    Time->Minute = ReadRTCPort (0x02);
    Time->Hour = ReadRTCPort (0x04);
    // is BCD coded, then convert to decimal
    if ((ReadRTCPort (0x0B) & 0x04) == 0x00)
    {
        Time->Second = BcdToDecimal (Time->Second);
        Time->Minute = BcdToDecimal (Time->Minute);
        Time->Hour = BcdToDecimal (Time->Hour);
    }
    // is 12h format, then convert to 24h format
    if (((ReadRTCPort (0x0B) & 0x02) == 0x00) && (Time->Hour & 0x80))
    {
        Time->Hour = ((Time->Hour & 0x7F) + 24) % 24;
    }
}

UINT16
BcdToDecimal (
    UINT16              Value)
{

    return ((Value & 0x0F) + ((Value >> 4) * 10));
}

VOID
WaitStopUpdate ()
{
    // Wait flag become set
    while (ReadRTCPort (0x0A) & 0x80 == 0x00)
        continue;
    // Wait flag become clear
    while (ReadRTCPort (0x0A) & 0x80 == 0x80)
        continue;
}

UINT8
ReadRTCPort (
    UINT64              Port)
{
    UINT8               Value;

    WritePort8 (0x70, Port);
    Value = ReadPort8 (0x71);

    return (Value);
}

VOID
WritePort8 (
    UINT64              Port,
    UINT8               Value)
{
    EFI_STATUS          Status = EFI_SUCCESS;

    Status = uefi_call_wrapper (PciRootBridge->Io.Write, 5, PciRootBridge, EfiPciWidthUint8, Port, 1, &Value);
}

UINT8
ReadPort8 (
    UINT64              Port)
{
    UINT8               Value;
    EFI_STATUS          Status = EFI_SUCCESS;

    Status = uefi_call_wrapper (PciRootBridge->Io.Read, 5, PciRootBridge, EfiPciWidthUint8, Port, 1, &Value);

    return (Value);
}
