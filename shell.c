int stringCompare(char*,char*);
void executeCommand(char*, char**, int, char*);
void parseInput(char*, char**, char**, int*);
void extractStringAtIndex(char*, int, char*);

void main() {
        char input[256];
	char output[13312];
	char* command[64];
	char argument1[10];
        char argument2[10];
	int sectorsRead;
        char txtInput[512];
        char filename[7];
        int txtLength, i, j, k,process, processID;;
        enableInterrupts();
        syscall(0,"SHELL > ");
	syscall(1,input);
        syscall(0, "\r\n");
        
        if(isCommand("type\0", input)) {
		syscall(3, input+5, output, &sectorsRead);
                if(sectorsRead <= 0) {
			syscall(0, "Cannot find the file\r\n");
                        return;
                }
                syscall(0, output);
                syscall(0, "\r\n");
        } else if(isCommand("execb\0", input)) {
                syscall(3, input+6, output, &sectorsRead);
                if(sectorsRead<=0) syscall(0, "Cannot find the file\r\n");
                else {
                        syscall(4, input+6, &processID);
                }
        } else if(isCommand("exec\0", input)) {
                syscall(3, input+5, output, &sectorsRead);
                if(sectorsRead<=0) syscall(0, "Cannot find the file\r\n");
                else {
                        syscall(4, input+5, &processID);
                }
                syscall(10, processID);
        } else if(isCommand("dir\0", input)) {
                syscall(2, output, 2);
			for (i=0; i<512; i+=32) {
			if (output[i] == '\0') continue;
			for (j=0; j<6; j++) {
				filename[j] = output[i+j];
			}
			filename[6] = '\0';
			syscall(0, filename);
			syscall(0, "\r\n");
	        }
        } else if(isCommand("del\0", input)) {
                syscall(7,input+4);
        } else if(isCommand("create\0", input)) {
			// create a text file
			txtLength = 0;
			for(i=0; i<26; i++) { // max file length is 26 sectors
				// clear line input
				for(j=0; j<512; j++) {
					txtInput[j] = '\0';
				}

				// read line
				syscall(1, txtInput);
				if(txtInput[0]=='\0' || txtInput[0]=='\r') break;
				
				// add carriage return and newline to end of line
				for(j=0; j<510; j++) { // cap at 510 bytes to leave room for a carriage return and a newline
					if(txtInput[j]=='\0') {
						break;
					}
				}
				txtInput[j++] = '\r';
				txtInput[j++] = '\n';	

				// add line to buffer
				for(k=0; k<j; k++) {
					output[txtLength+k] = txtInput[k];
				}
				txtLength += j;
			}
			syscall(8, output, input+7, i);
	} else if(isCommand("kill\0", input)) {
                process = input[5] - '0';
		syscall(9, process); 
        } else if(isCommand("copy\0", input)) {
                extractStringAtIndex(input, 1, argument1);
                extractStringAtIndex(input, 2, argument2);
                syscall(11, argument1, argument2);
        } else {
                syscall(0, "Invalid command entered\r\n");
		return;
        }
}

int isCommand(char *command, char *input) {
    while (*command != '\0' && *command != ' ' && *input != '\0' && *input != ' ') {
        if (*command != *input) {
            return 0; // false
        }
        command++;
        input++;
     }
     if(*command == '\0') {
        return 1;
     }
     return 0; // true
}

void extractStringAtIndex(char* input, int index, char* output) {
    int i = 0, j = 0, currentWord = 0;
    int start, end;

    // Initialize output
    output[0] = '\0';

    // Skip leading spaces
    while (input[i] == ' ') i++;

    // Iterate through the input string
    while (input[i] != '\0') {
        // Check if we're at the beginning of a word
        if ((i == 0 || input[i - 1] == ' ') && input[i] != ' ') {
            // Check if it's the word at the desired index
            if (currentWord == index) {
                start = i;
                // Find the end of the word
                while (input[i] != ' ' && input[i] != '\0') i++;
                end = i;

                // Copy the word into output
                for (j = start; j < end; j++) {
                    output[j - start] = input[j];
                }
                output[j - start] = '\0';
                return;
            }
            currentWord++;
        }
        i++;
    }
}
