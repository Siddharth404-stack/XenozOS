# Makefile for SiddOS — a minimal UEFI application
# Requires: gnu-efi, build-essential  (sudo apt install gnu-efi build-essential)

ARCH      = x86_64
TARGET    = main.efi

EFIINC    = /usr/include/efi
EFIINCS   = -I$(EFIINC) -I$(EFIINC)/$(ARCH) -I$(EFIINC)/protocol
LIB       = /usr/lib
EFILIB    = /usr/lib
EFI_CRT_OBJS = $(EFILIB)/crt0-efi-$(ARCH).o
EFI_LDS      = $(EFILIB)/elf_$(ARCH)_efi.lds

CFLAGS  = $(EFIINCS) -fno-stack-protector -fpic -fshort-wchar -mno-red-zone -Wall -DEFI_FUNCTION_WRAPPER
LDFLAGS = -nostdlib -znocombreloc -T $(EFI_LDS) -shared -Bsymbolic -L $(LIB) $(EFI_CRT_OBJS)

all: $(TARGET)

main.o: main.c
	gcc $(CFLAGS) -c main.c -o main.o

main.so: main.o
	ld $(LDFLAGS) main.o -o main.so -lefi -lgnuefi

main.efi: main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic \
		-j .dynsym -j .rel -j .rela -j .reloc \
		--target=efi-app-$(ARCH) main.so main.efi

clean:
	rm -f *.o *.so *.efi
