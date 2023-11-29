// Author - Mouli Sanketh Maturi
#define SECTOR_SIZE 512
#define PROGRAM_LOAD_SEGMENT 0x2000

void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*,int);
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*, int*);
int compareFileName(char*, char*);
void executeProgram(char*);
void terminate();

void main() {
	//char line [80];
	char buffer[13312];
	int sectorsRead;
	
	//printString("Enter a line: ");
	//readString(line);
	//printString(line);
	
	//readSector(buffer, 30);
	//printString(buffer);

	makeInterrupt21();
	interrupt(0x21,4,"shell",0,0);
	
	//printString("\nYour String is: ");
	//interrupt(0x21,0,line,0,0);
	//interrupt(0x21,2,buffer,30,0);
	//interrupt(0x21,0,buffer,0,0);
	//if (sectorsRead>0)
	//	interrupt(0x21, 0, buffer, 0, 0);   /*print out the file*/ 
	//else
	//	interrupt(0x21, 0, "messag not found\r\n", 0, 0);  /*no sectors read? then print an error*/

	while(1);
}

void printString(char* chars) {
	int i=0;
	while(chars[i] != 0x0) {
		interrupt(0x10, 0xe*256+chars[i++], 0, 0, 0);
		//printChar(chars[i++]);
	}
}

void printChar(char c) {
	interrupt(0x10, 0xe*256+c, 0, 0, 0);
}


void readString(char* str) {
    int index = 0;
    while (1) {
        char key = interrupt(0x16, 0, 0, 0, 0);
        if (key == 0xd) {
            interrupt(0x10, 0xE * 256 + 0xa, 0, 0, 0);
            interrupt(0x10, 0xE * 256, 0, 0, 0);
            str[index] = 0xa;
            str[index + 1] = 0x0;
            return;
        } else if (key == 0x8) {
            if (index > 0) {
                interrupt(0x10, 0xE * 256 + 0x8, 0, 0, 0);
                interrupt(0x10, 0xE * 256 + ' ', 0, 0, 0);
                interrupt(0x10, 0xE * 256 + 0x8, 0, 0, 0);
                index--;
            }
        } else {
            interrupt(0x10, 0xE * 256 + key, 0, 0, 0);
            str[index] = key;
            index++;
	}
    }
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

// The readFile function
void readFile(char* filename, char* buffer, int* sectorsRead) {
    char dir[SECTOR_SIZE];  // Directory sector
    int fileFound = 0;
    int fileEntry = 0;
    int i = 0;

    // Load directory sector
    readSector(dir, 2);  // Assuming sector 2 is the directory sector

    // Search for the file
    for (fileEntry = 0; fileEntry < SECTOR_SIZE; fileEntry += 32) {
        if (compareFileName(dir + fileEntry, filename)) {
            fileFound = 1;
            *sectorsRead = 0;
            for (i = 6; i < 32; i++) {
                char sectorNum = dir[fileEntry + i];
                if (sectorNum == 0) break;
                readSector(buffer + (*sectorsRead) * SECTOR_SIZE, sectorNum);
                (*sectorsRead)++;
            }
            break;
        }
    }

    if (!fileFound) {
        *sectorsRead = 0;
    }
}

int compareFileName(char* filename, char* dirEntry) {
    int i = 0;
    for (i = 0; i < 6; i++) {
        if (filename[i] == '\0') {
		break;
	}
        if (filename[i] != dirEntry[i]) {
            return 0;  // Not a match
        }
    }
    return 1;  // Match
}

void executeProgram(char* name) {
    char buffer[13312];
    int sectorsRead;
    int i=0;

    readFile(name, buffer, &sectorsRead);
    if (sectorsRead > 0) {
        // Transfer the program into memory segment
        for (i = 0; i < sectorsRead * SECTOR_SIZE; ++i) {
            putInMemory(PROGRAM_LOAD_SEGMENT, i, buffer[i]);
        }

        // Launch the program
        launchProgram(PROGRAM_LOAD_SEGMENT);
    }
}

void terminate() {
	char shellname[6]; 
	shellname[0]='s';
	shellname[1]='h';
	shellname[2]='e';
	shellname[3]='l';
	shellname[4]='l';
	shellname[5]='\0';
	executeProgram(shellname);
}

void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if( ax == 0) {
		printString(bx);
	} else if(ax == 1) {
		readString(bx);
	} else if(ax == 2) {
		readSector(bx, cx);
	} else if(ax == 3) {
		readFile(bx, cx, dx);
	} else if(ax == 4) {
                executeProgram((char*)bx);
        } else if(ax == 5) {
                terminate();
        } else {
		printString("AX must be one of 1,2,3");
	}
}	
