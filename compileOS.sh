#Author - Mouli Sanketh Maturi & Miray Dalgin
dd if=/dev/zero of=diskc.img bs=512 count=1000
dd if=bootload of=diskc.img bs=512 count=1 conv=notrunc
bcc -ansi -c kernel.c -o kernel_c.o
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o
bcc -ansi -c shell.c -o shell_c.o
as86 userlib.asm -o userlib.o
ld86 -o shell -d shell_c.o userlib.o
./loadFile tstpr1
./loadFile kernel
./loadFile message.txt
./loadFile shell
