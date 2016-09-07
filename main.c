#include <efi.h>
#include <efilib.h>

#include "PciRootBridgeIo.h"

#define SEC_TO_USEC(value) ((value) * 1000 * 1000)

EFI_GUID gEfiPciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;
EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridge = NULL;

VOID
PrintTime (
  EFI_TIME            *time);

VOID
ReadTime (
  EFI_TIME            *time);

VOID
WaitStopUpdate ();

UINT8
ReadRTCPort (
  UINT64              port);

VOID
WritePort8 (
  UINT64              port,
  UINT8               value);

UINT8
ReadPort8 (
  UINT64              port);

EFI_STATUS
EFIAPI
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable)
{
    EFI_STATUS 		       Status = EFI_SUCCESS;
    EFI_TIME             time;

    InitializeLib (ImageHandle, SystemTable);

    Print (L"Hello, world!\n");

    Status = uefi_call_wrapper (BS->LocateProtocol, 3, &gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **) &PciRootBridge);
    Print (L"Status = %r\n", Status);

    ReadTime (&time);
    PrintTime (&time);

    uefi_call_wrapper (BS->Stall, 1, SEC_TO_USEC (5));

    return (EFI_SUCCESS);
}

VOID
PrintTime (
  EFI_TIME            *time)
{

    Print (L"%2uh%02umin%02useg - %02u/%02u/%04u\n",
      time->Hour, time->Minute, time->Second,
      time->Month, time->Day, time->Year);
}

VOID
ReadTime (
  EFI_TIME            *time)
{

    WaitStopUpdate ();
    time->Second = ReadRTCPort(0x00);
    time->Minute = ReadRTCPort(0x02);
    time->Hour = ReadRTCPort(0x04);
    time->Day = ReadRTCPort(0x07);
    time->Month = ReadRTCPort(0x08);
    time->Year = ReadRTCPort(0x09);
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
    UINT64              port)
{
    UINT8               Value;

    WritePort8 (0x70, port);
    Value = ReadPort8 (0x71);

    return (Value);
}

VOID
WritePort8 (
    UINT64              port,
    UINT8               value)
{
    EFI_STATUS      Status = EFI_SUCCESS;

    Status = uefi_call_wrapper (PciRootBridge->Io.Write, 5, PciRootBridge, EfiPciWidthUint8, port, 1, &value);
    Print (L"WritePort8 Status = %r\n", Status);
}

UINT8
ReadPort8 (
    UINT64              port)
{
    UINT8               Value;
    EFI_STATUS      Status = EFI_SUCCESS;

    Status = uefi_call_wrapper (PciRootBridge->Io.Read, 5, PciRootBridge, EfiPciWidthUint8, port, 1, &Value);
    Print (L"ReadPort8 Status = %r\n", Status);

    return (Value);
}
