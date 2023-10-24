#Author - Mouli Sanketh Maturi
bcc -ansi -c kernel.c -o kernel_c.o
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o
dd if=kernel of=diskc.img bs=512 conv=notrunc seek=3
