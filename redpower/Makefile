
# RedPower bootloader for asm2bf.
# Copyright (C) by Kamila Szewczyk, Licensed under the terms of MIT license.

.PHONY: redpower test clean

redpower: redpower.img

test: redpower.img
	qemu-system-i386 -enable-kvm -hda redpower.img

clean:
	rm -f redpower.img redpower.hex kernel/kernel.b kernel/kernel.p

redpower.hex: redpower.asm kernel/kernel.b
	nasm redpower.asm -f bin -o redpower.hex

redpower.img: redpower.hex
	dd if=/dev/zero of=redpower.img count=1008 bs=512
	dd conv=notrunc if=redpower.hex of=redpower.img

kernel/kernel.b: kernel/kernel.asm
	~/.asmbf/bfmake -s -o kernel/kernel.b kernel/kernel.asm
	~/.asmbf/bfmake -p -s -o kernel/kernel.p kernel/kernel.asm
