#Author - Mouli Sanketh Maturi
dd if=/dev/zero of=diskc.img bs=512 count=1000
dd if=bootload of=diskc.img bs=512 count=1 conv=notrunc
bcc -ansi -c kernel.c -o kernel_c.o
as86 kernel.asm -o kernel_asm.o
ld86 -o kernel -d kernel_c.o kernel_asm.o
bcc -ansi -c shell.c -o shell_c.o
as86 userlib.asm -o userlib.o
ld86 -o shell -d shell_c.o userlib.o
bcc -ansi -c tstpr1.c -o tstpr1_c.o
bcc -ansi -c tstpr2.c -o tstpr2_c.o
bcc -ansi -c number.c -o number_c.o
bcc -ansi -c letter.c -o letter_c.o
ld86 -o tstpr1 -d tstpr1_c.o userlib.o
ld86 -o tstpr2 -d tstpr2_c.o userlib.o
ld86 -o number -d number_c.o userlib.o
ld86 -o letter -d letter_c.o userlib.o
./loadFile kernel
./loadFile message.txt
./loadFile shell
./loadFile tstpr1
./loadFile tstpr2
./loadFile number
./loadFile letter
