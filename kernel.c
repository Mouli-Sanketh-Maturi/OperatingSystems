// Author - Mouli Sanketh Maturi
void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*,int);
void handleInterrupt21(int, int, int, int);

void main() {
	char line [80];
	char buffer[512];
	
	printString("Enter a line: ");
	//readString(line);
	//printString(line);
	
	//readSector(buffer, 30);
	//printString(buffer);

	makeInterrupt21();
	interrupt(0x21,1,line,0,0);
	printString("\nYour String is: ");
	interrupt(0x21,0,line,0,0);
	interrupt(0x21,2,buffer,30,0);
	interrupt(0x21,0,buffer,0,0);
	while(1);
}

void printString(char* chars) {
	int i=0;
	while(chars[i] != 0x0) {
		//interrupt(0x10, 0xe*256+chars[i++], 0, 0, 0);
		printChar(chars[i++]);
	}
}

void printChar(char c) {
	interrupt(0x10, 0xe*256+c, 0, 0, 0);
}

void readString(char* chars) {
	char inp = interrupt(0x16, 0, 0, 0, 0);
	int i=0;
	while(inp != 0xd) {
		if(inp == 0x08 || inp == 0x7f) {
			if(i > 0) {
				i--;
				printChar(0x08);
				printChar(' ');
				printChar(0x08);
				//printChar(0x08);
				//printChar(inp);
			}
		} else {
			printChar(inp);
			chars[i++] = inp;
			inp = interrupt(0x16, 0, 0, 0, 0);
		}
	}
	chars[i++] = 0xa;
	chars[i++] = 0x0;
}	

void readSector(char* chars, int sector) {
	int AH = 2;
	int AL = 1;
	int AX = AH*256+AL;
	int BX = chars;
	int CH = 0;
	int CL = sector+1;
	int CX = CH*256+CL;
	int DH = 0;
	int DL = 0x80;
	int DX = DH*256+DL;

	interrupt(0x13, AX, BX, CX, DX);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if( ax == 0) {
		printString(bx);
	} else if(ax == 1) {
		readString(bx);
	} else if(ax == 2) {
		readSector(bx, cx);
	} else {
		printString("AX must be one of 1,2,3");
	}
}	
