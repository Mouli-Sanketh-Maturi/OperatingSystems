int stringCompare(char*,char*);
void executeCommand(char*, char**, int, char*);
void parseInput(char*, char**, char**, int*);

void main() {
        char input[256];
	char output[13312];
	char* command[64];
	char arguments[10];
	int sectorsRead;
        syscall(0,"SHELL > ");
	syscall(1,input);
        syscall(0, "\r\n");
        
        if(isCommand("type", input)) {
		syscall(3, input+5, output, &sectorsRead);
                if(sectorsRead <= 0) {
			syscall(0, "Cannot find the file\r\n");
                        return;
                }
                syscall(0, output);
                syscall(0, "\r\n");
        } else if(isCommand("exec", input)) {
                syscall(3, input+5, output, &sectorsRead);
                if(sectorsRead<=0) syscall(0, "Cannot find the file\r\n");
                else {
                	syscall(4, input+5);
                }
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

     return 1; // true
}
