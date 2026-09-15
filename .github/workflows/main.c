/*
 * SiddOS v0.3 — now with color
 *
 * Uses UEFI's text console color attributes (EFI_TEXT_ATTR) to give
 * the shell a real visual identity: a colored banner, a colored
 * prompt, and color-coded command output (green for success/info,
 * red for errors, cyan for headers).
 *
 * Build with gnu-efi (see Makefile). Boot it from a USB stick as
 * EFI/BOOT/BOOTX64.EFI — see README.md for full instructions.
 */

#include <efi.h>
#include <efilib.h>

#define INPUT_BUFFER_SIZE 128

static int
str_eq (CHAR16 *a, CHAR16 *b)
{
    while (*a && *b) {
        if (*a != *b) return 0;
        a++;
        b++;
    }
    return *a == *b;
}

static void
set_color (UINTN fg, UINTN bg)
{
    ST->ConOut->SetAttribute(ST->ConOut, EFI_TEXT_ATTR(fg, bg));
}

static void
print_banner (void)
{
    set_color(EFI_LIGHTCYAN, EFI_BLACK);
    Print(L"  _____ _     _     _  ___  _____\r\n");
    Print(L" / ____(_)   | |   | |/ _ \\/ ____|\r\n");
    Print(L"| (___  _  __| | __| | | | | (___\r\n");
    Print(L" \\___ \\| |/ _` |/ _` | | | |\\___ \\\r\n");
    Print(L" ____) | | (_| | (_| | |_| |____) |\r\n");
    Print(L"|_____/|_|\\__,_|\\__,_|\\___/|_____/\r\n\r\n");

    set_color(EFI_WHITE, EFI_BLACK);
    Print(L"v0.3 -- a UEFI operating system\r\n");
    set_color(EFI_LIGHTGRAY, EFI_BLACK);
    Print(L"No Windows. No Linux. Just firmware and this.\r\n\r\n");

    set_color(EFI_YELLOW, EFI_BLACK);
    Print(L"Type 'help' and press Enter to see commands.\r\n");
    set_color(EFI_LIGHTGRAY, EFI_BLACK);
}

static void
print_prompt (void)
{
    Print(L"\r\n");
    set_color(EFI_LIGHTGREEN, EFI_BLACK);
    Print(L"siddos");
    set_color(EFI_WHITE, EFI_BLACK);
    Print(L"> ");
}

static void
run_command (CHAR16 *line)
{
    Print(L"\r\n");

    if (str_eq(line, L"help")) {
        set_color(EFI_LIGHTCYAN, EFI_BLACK);
        Print(L"Available commands:\r\n");
        set_color(EFI_LIGHTGRAY, EFI_BLACK);
        Print(L"  help     - show this list\r\n");
        Print(L"  about    - about this OS\r\n");
        Print(L"  clear    - clear the screen\r\n");
        Print(L"  color    - preview all text colors\r\n");
        Print(L"  reboot   - restart the machine\r\n");
        Print(L"  poweroff - shut the machine down\r\n");
    } else if (str_eq(line, L"about")) {
        set_color(EFI_LIGHTCYAN, EFI_BLACK);
        Print(L"SiddOS v0.3\r\n");
        set_color(EFI_LIGHTGRAY, EFI_BLACK);
        Print(L"A minimal UEFI operating system, built from scratch.\r\n");
        Print(L"No Windows, no Linux, no bootloader underneath -\r\n");
        Print(L"just the UEFI firmware and this program.\r\n");
    } else if (str_eq(line, L"color")) {
        set_color(EFI_RED, EFI_BLACK);          Print(L"red ");
        set_color(EFI_LIGHTRED, EFI_BLACK);     Print(L"lightred ");
        set_color(EFI_GREEN, EFI_BLACK);        Print(L"green ");
        set_color(EFI_LIGHTGREEN, EFI_BLACK);   Print(L"lightgreen ");
        set_color(EFI_YELLOW, EFI_BLACK);       Print(L"yellow ");
        set_color(EFI_BLUE, EFI_BLACK);         Print(L"blue ");
        set_color(EFI_LIGHTBLUE, EFI_BLACK);    Print(L"lightblue ");
        set_color(EFI_MAGENTA, EFI_BLACK);      Print(L"magenta ");
        set_color(EFI_LIGHTMAGENTA, EFI_BLACK); Print(L"lightmagenta ");
        set_color(EFI_CYAN, EFI_BLACK);         Print(L"cyan ");
        set_color(EFI_LIGHTCYAN, EFI_BLACK);    Print(L"lightcyan ");
        set_color(EFI_WHITE, EFI_BLACK);        Print(L"white\r\n");
        set_color(EFI_LIGHTGRAY, EFI_BLACK);
    } else if (str_eq(line, L"clear")) {
        ST->ConOut->ClearScreen(ST->ConOut);
        return;
    } else if (str_eq(line, L"reboot")) {
        set_color(EFI_YELLOW, EFI_BLACK);
        Print(L"Rebooting...\r\n");
        RT->ResetSystem(EfiResetCold, EFI_SUCCESS, 0, NULL);
    } else if (str_eq(line, L"poweroff")) {
        set_color(EFI_YELLOW, EFI_BLACK);
        Print(L"Shutting down...\r\n");
        RT->ResetSystem(EfiResetShutdown, EFI_SUCCESS, 0, NULL);
    } else if (line[0] == 0) {
        return;
    } else {
        set_color(EFI_LIGHTRED, EFI_BLACK);
        Print(L"Unknown command: %s\r\n", line);
        set_color(EFI_LIGHTGRAY, EFI_BLACK);
        Print(L"Type 'help' for a list of commands.\r\n");
    }
}

EFI_STATUS
EFIAPI
efi_main (EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE *SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    ST->ConOut->ClearScreen(ST->ConOut);
    print_banner();

    CHAR16 buffer[INPUT_BUFFER_SIZE];
    UINTN pos = 0;
    buffer[0] = 0;

    print_prompt();

    EFI_INPUT_KEY key;
    UINTN index;

    for (;;) {
        BS->WaitForEvent(1, &ST->ConIn->WaitForKey, &index);
        ST->ConIn->ReadKeyStroke(ST->ConIn, &key);

        if (key.UnicodeChar == CHAR_CARRIAGE_RETURN) {
            buffer[pos] = 0;
            run_command(buffer);
            pos = 0;
            buffer[0] = 0;
            print_prompt();
        } else if (key.UnicodeChar == CHAR_BACKSPACE) {
            if (pos > 0) {
                pos--;
                buffer[pos] = 0;
                Print(L"\b \b");
            }
        } else if (key.UnicodeChar != 0 && pos < INPUT_BUFFER_SIZE - 1) {
            buffer[pos] = key.UnicodeChar;
            pos++;
            buffer[pos] = 0;
            set_color(EFI_WHITE, EFI_BLACK);
            Print(L"%c", key.UnicodeChar);
        }
    }

    return EFI_SUCCESS;
}
