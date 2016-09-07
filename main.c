#include <efi.h>
#include <efilib.h>

#define SEC_TO_USEC(value) ((value) * 1000 * 1000)

EFI_GUID gEfiPciRootBridgeIoProtocolGuid = { 0x2f707ebb, 0x4a1a, 0x11d4, {0x9a, 0x38, 0x00, 0x90, 0x27, 0x3f, 0xc1, 0x4d } };

EFI_STATUS
EFIAPI
efi_main (
    EFI_HANDLE          ImageHandle,
    EFI_SYSTEM_TABLE    *SystemTable)
{
    EFI_STATUS 		Status = EFI_SUCCESS;
    VOID 		*PciRootBridge = NULL;

    InitializeLib (ImageHandle, SystemTable);

    Print (L"Hello, world!\n");

    Status = uefi_call_wrapper (BS->LocateProtocol, 3, &gEfiPciRootBridgeIoProtocolGuid, NULL, (VOID **) &PciRootBridge);
    Print (L"Status = %r\n", Status);

    uefi_call_wrapper (BS->Stall, 1, SEC_TO_USEC (5));

    return (EFI_SUCCESS);
}
