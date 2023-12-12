// Author - Mouli Sanketh Maturi
#define SECTOR_SIZE 512
#define PROGRAM_LOAD_SEGMENT 0x2000
#define MAP_SECTOR 1
#define DIRECTORY_SECTOR 2
#define BOOTLOADER_SECTOR 3
#define MAX_FILE_NAME 6
#define MAX_SECTORS 26
#define MAX_FILE_SIZE 13312
#define MAX_LINES 50
#define MAX_LINE_LENGTH 80

void printChar(char);
void printString(char*);
void readString(char*);
void readSector(char*,int);
void writeSector(char*,int);
void listDirectory();
void handleInterrupt21(int, int, int, int);
void readFile(char*, char*, int*);
void writeFile(char*, char*, int);
void copyFile(char*, char*);
int strlen(char*);
int compareFileName(char*, char*);
void executeProgram(char*);
void terminate();
void deleteFile(char*);

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
        //interrupt(0x21,8,"this is a test message","testmg",3);
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

void writeSector(char* chars, int sector) {
        int AH = 3;
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

void writeFile(char* buffer, char* filename, int numberOfSectors) {
    char map[SECTOR_SIZE];
    char directory[SECTOR_SIZE];
    int dirEntry, sector, bufferOffset, i;

    // Step 1: Load the Map and Directory into char arrays
    readSector(map, MAP_SECTOR);
    readSector(directory, DIRECTORY_SECTOR);

    // Step 2: Find a free directory entry
    for (dirEntry = 0; dirEntry < SECTOR_SIZE; dirEntry += 32) {
        if (directory[dirEntry] == '\0') {
            // Step 3: Copy the name to that directory entry
            for (i = 0; i < MAX_FILE_NAME; i++) {
                if (i < strlen(filename)) {
                    directory[dirEntry + i] = filename[i];
                } else {
                    directory[dirEntry + i] = '\0'; // Fill remaining with '\0'
                }
            }

            bufferOffset = 0;
            for (i = 0; i < numberOfSectors; i++) {
                // Step 4: Find a free sector
                for (sector = BOOTLOADER_SECTOR; sector < SECTOR_SIZE; sector++) {
                    if (map[sector] == 0) {
                        // Step 5: Set that sector to 0xFF in the Map
                        map[sector] = 0xFF;

                        // Step 6: Add that sector number to the file's directory entry
                        directory[dirEntry + 6 + i] = sector;

                        // Step 7: Write 512 bytes from the buffer to that sector
                        writeSector(buffer + bufferOffset, sector);
                        bufferOffset += SECTOR_SIZE;
                        break;
                    }
                }

                if (sector == SECTOR_SIZE) {
                    // No free sectors
                    return;
                }
            }

            // Step 8: Fill in the remaining bytes in the directory entry to 0
            for (i = numberOfSectors + 6; i < MAX_SECTORS; i++) {
                directory[dirEntry + i] = 0;
            }

            // Step 9: Write the Map and Directory back to the disk
            writeSector(map, MAP_SECTOR);
            writeSector(directory, DIRECTORY_SECTOR);

            return;
        }
    }

    // No free directory entries
}

void copyFile(char* filename1, char* filename2) {
    char buffer[MAX_FILE_SIZE];  // Buffer to hold file data
    int sectorsRead;

    // Read the contents of filename1
    interrupt(0x21, 3, filename1, buffer, &sectorsRead); // 0x21, 3 is readFile in the system

    if (sectorsRead > 0) {
        // Write the contents to filename2
        interrupt(0x21, 8, buffer, filename2, sectorsRead); // 0x21, 8 is writeFile in the system
    } else {
        // Handle the case where the file couldn't be read (e.g., doesn't exist or read error)
        printString("Error reading file\r\n");
    }
}

int strlen(char *str) {
    int length = 0;
    while (*str != '\0') {
        length++;
        str++;
    }
    return length;
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
    if(i != 0) {
        return 1;  // Match
    }
    return 0;
}

void listDirectory() {
    char directory[SECTOR_SIZE];
    int i, j;
    int sectorCount;

    // Read the directory sector
    readSector(directory, 4); // Assuming the directory is in sector 2

    // Iterate over each directory entry
    for (i = 0; i < SECTOR_SIZE; i += 32) {
        // Check if the file is not deleted (first character is not '\0')
        printString("Files:\r\n");
        if (directory[i] != '\0') {
            // Print the file name (first 6 bytes)
            for (j = 0; j < 6; j++) {
                if (directory[i + j] != '\0') {
                    printChar(directory[i + j]);
                } else {
                    break;
                }
            }
        }
    }
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

void deleteFile(char* filename) {
    char map[SECTOR_SIZE];
    char directory[SECTOR_SIZE];
    int dirEntry, sectorNum, i;

    // Step 1: Load the Directory and Map into 512 byte character arrays
    readSector(map, MAP_SECTOR);
    readSector(directory, DIRECTORY_SECTOR);

    // Step 2: Search through the directory for the file name
    for (dirEntry = 0; dirEntry < SECTOR_SIZE; dirEntry += 32) {
        if (compareFileName(directory + dirEntry, filename)) {
            // Step 3: Set the first byte of the file name to '\0'
            directory[dirEntry] = '\0';

            // Step 4: Step through the sector numbers listed as belonging to the file
            for (i = 6; i < 32; i++) {
                sectorNum = directory[dirEntry + i];
                if (sectorNum == 0) break; // No more sectors
                map[sectorNum] = 0; // Free the sector in the map
            }
            break; // Exit the loop after deleting the file
        }
    }

    // Step 5: Write the modified Directory and Map back to their sectors
    writeSector(map, MAP_SECTOR);
    writeSector(directory, DIRECTORY_SECTOR);
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
        } else if(ax == 6) {
                writeSector(bx, cx);
        } else if(ax == 7) {
                deleteFile((char*)bx);
        } else if(ax == 8) {
                writeFile((char*)bx, (char*)cx, dx);
        } else if(ax == 10) {
                listDirectory();
        } else if(ax == 11) {
                copyFile((char*)bx, (char*)cx);
        } else {
		printString("AX must be one of 1,2,3");
	}
}	
