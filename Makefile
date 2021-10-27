all: boot1 boot2
boot2: boot2.exe
boot1: boot2.exe boot1.asm
	nasm -l boot1.list -DENTRY=` ./getaddr.sh main` boot1.asm
boot2.exe: boot2_c.o boot2_s.o
	ld -g -melf_i386 -Ttext 0x10000 -e main -o boot2.exe boot2_s.o boot2_c.o
	objcopy -j .text* -j .data* -j .rodata* -S -O binary boot2.exe boot2
boot2_s.o: boot2.s
	gcc -g -m32 -c -masm=intel -o boot2_s.o boot2.s
boot2_c.o: boot2.c
	gcc -g -m32 -c -o boot2_c.o boot2.c
install:
	bximage -mode=create -fd=1.44M -q a.img
	/sbin/mkdosfs a.img
	dd if=boot1 of=a.img bs=1 count=512 conv=notrunc
	mcopy -o boot2 a:BOOT2
debug:
	qemu-system-i386 -S -s -boot a -fda a.img
run:
	qemu-system-i386 -boot a -fda a.img
clean:
	rm -f *.o *.exe a.img boot1 boot2