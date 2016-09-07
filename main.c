#include <efi.h>
#include <efilib.h>

#include "PciRootBridgeIo.h"

#define SEC_TO_USEC(value) ((value) * 1000 * 1000)

EFI_GUID gEfiPciRootBridgeIoProtocolGuid = EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL_GUID;

EFI_STATUS
EFIAPI
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable)
{
    EFI_STATUS 		Status = EFI_SUCCESS;
    EFI_PCI_ROOT_BRIDGE_IO_PROTOCOL *PciRootBridge = NULL;

    InitializeLib (ImageHandle, SystemTable);

    Print (L"Hello, world!\n");

    Status = uefi_call_wrapper (BS->LocateProtocol, 3, &gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **) &PciRootBridge);
    Print (L"Status = %r\n", Status);

    uefi_call_wrapper (BS->Stall, 1, SEC_TO_USEC (5));

    return (EFI_SUCCESS);
}

