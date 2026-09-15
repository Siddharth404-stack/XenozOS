/*
 * SiddOS v0.1 — a minimal 64-bit UEFI operating system
 *
 * This is a UEFI application that the firmware loads and runs DIRECTLY
 * on power-on — before Windows, GRUB, or any other OS gets involved.
 * That makes it a genuine (if tiny) operating system: it owns the CPU,
 * talks to the firmware-provided console and input drivers, and decides
 * what happens next (echo keys, reboot, shut down).
 *
 * Build with gnu-efi (see Makefile). Boot it from a USB stick as
 * EFI/BOOT/BOOTX64.EFI — see README.md for full instructions.
 */

#include <efi.h>
#include <efilib.h>

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    ST->ConOut->ClearScreen(ST->ConOut);

    Print(L"=========================================\r\n");
    Print(L"        SiddOS v0.1 -- a UEFI OS\r\n");
    Print(L"=========================================\r\n\r\n");
    Print(L"You are now running code with no Windows,\r\n");
    Print(L"no Linux, and no bootloader underneath it --\r\n");
    Print(L"just the UEFI firmware and this program.\r\n\r\n");
    Print(L"Type anything -- keys echo below.\r\n");
    Print(L"  ESC  = power off\r\n");
    Print(L"  F1   = reboot\r\n\r\n");
    Print(L"> ");

    EFI_INPUT_KEY key;
    UINTN index;

    for (;;) {
        BS->WaitForEvent(1, &ST->ConIn->WaitForKey, &index);
        ST->ConIn->ReadKeyStroke(ST->ConIn, &key);

        if (key.ScanCode == SCAN_ESC) {
            Print(L"\r\n\r\nShutting down...\r\n");
            RT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
        } else if (key.ScanCode == SCAN_F1) {
            Print(L"\r\n\r\nRebooting...\r\n");
            RT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
        } else if (key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            Print(L"\r\n> ");
        } else if (key.UnicodeChar != 0) {
            Print(L"%c", key.UnicodeChar);
        }
    }

    return EFI_SUCCESS;
}
