# SiddOS v0.1 — build & boot on real hardware

This builds a minimal 64-bit UEFI operating system and boots it straight
from a USB stick on your Dell Inspiron 15 3541 — no Windows or Linux
involved once it's running.

**Important:** this only touches the USB stick. Your internal hard drive
and Windows installation are never written to, as long as you select the
USB drive in the one-time boot menu (not "boot order" in BIOS settings).

---

## 1. Build it

You need a Linux machine (or a Linux VM) to build this — Dell's laptop
itself works fine if you dual-boot or have any Linux installed, but you
can also build on any other PC/VM and just carry the .efi file over on
the same USB stick.

```bash
sudo apt update
sudo apt install gnu-efi build-essential
make
```

This produces `main.efi`.

---

## 2. Prepare the USB stick

**Back up anything on the USB stick first — this erases it.**

Find your USB device name (careful — picking the wrong disk here can
wipe your real drive):

```bash
lsblk
```

Look for your USB stick by its size (e.g. `sdb`). Then:

```bash
sudo parted /dev/sdX --script mklabel gpt mkpart primary fat32 1MiB 100%
sudo mkfs.fat -F32 /dev/sdX1

sudo mkdir -p /mnt/usb
sudo mount /dev/sdX1 /mnt/usb
sudo mkdir -p /mnt/usb/EFI/BOOT
sudo cp main.efi /mnt/usb/EFI/BOOT/BOOTX64.EFI
sudo umount /mnt/usb
```

Replace `/dev/sdX` with your actual device (e.g. `/dev/sdb`) and
`/dev/sdX1` with its first partition (e.g. `/dev/sdb1`).

The filename **must** be exactly `EFI/BOOT/BOOTX64.EFI` — that's the
standard path UEFI firmware looks for on a removable drive with no
boot entry registered.

---

## 3. Boot it on the Inspiron 3541

1. Plug the USB stick into the laptop and restart it.
2. Tap **F12** repeatedly right after power-on to bring up Dell's
   one-time boot menu.
3. If the USB drive doesn't appear, you may need to disable Secure Boot:
   tap **F2** at startup to enter BIOS setup → Boot/Secure Boot → set
   Secure Boot to Disabled → Save & Exit (this is needed because our
   .efi file isn't cryptographically signed).
4. Select the USB drive (often listed as "UEFI: [your USB brand]") from
   the F12 boot menu.
5. SiddOS should boot directly — you'll see the welcome banner, and
   keys you type will echo on screen. ESC powers off, F1 reboots.

---

## What this actually is

The moment the firmware hands control to `main.efi`, nothing else is
running underneath it — no kernel, no OS, no drivers except the ones
UEFI itself provides (console output, keyboard input, power control).
Everything this program does — print text, read keys, reboot, shut
down — it does by calling UEFI firmware services directly. That's a
real (extremely minimal) operating system.

## Where to go next

- **Graphics**: use the UEFI Graphics Output Protocol (GOP) to draw
  pixels instead of relying on the firmware's text console.
- **A real shell**: parse the typed line after Enter and implement
  commands (`help`, `clear`, `time`, etc.) instead of just echoing.
- **Memory management**: query the UEFI memory map (`GetMemoryMap`)
  and build your own simple allocator instead of relying on UEFI's.
- **Disk/filesystem access**: read other files off the USB stick using
  the Simple File System Protocol, so your "OS" can load extra code.
- **Two-stage design**: eventually split this into a small bootloader
  that loads a separate kernel binary, which is how most real OS
  projects (and osdev.org tutorials) are structured.

Good communities/resources if you want to keep going: osdev.org wiki
and forums, and the "OSDev" Discord — very active hobbyist OS-dev
community.
