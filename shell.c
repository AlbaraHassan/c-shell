#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <ctype.h>
#include <signal.h> 

void get_prompt() {
    char machine_name[256];
    char user_name[256];
    gethostname(machine_name, sizeof(machine_name));
    getlogin_r(user_name, sizeof(user_name));

    printf("~\033[1;36m%s\033[0m", machine_name); 
    printf("\033[1;33m@%s\033[0m", user_name);
    printf(":\033[1;32m[A&A]\033[0m$ ");
}


// Function to remove a file
void remove_file(char *file_name) {
    if (remove(file_name) == 0) {
        printf("File '%s' removed successfully.\n", file_name);
    } else {
        printf("Error: Unable to remove file '%s'.\n", file_name);
    }
}

// Function to display file content
void display_file_content(char *file_name) {
    FILE *file = fopen(file_name, "r");
    if (file != NULL) {
        char line[256];
        while (fgets(line, sizeof(line), file) != NULL) {
            printf("%s", line);
        }
        fclose(file);
        printf("\n");
    } else {
        printf("Error: Unable to open file '%s'.\n", file_name);
    }
}

void clear_screen() {
    printf("\033[H\033[J"); // ANSI escape sequence for clearing the screen
}

char encrypt_char(char ch, int shift) {
    if (isalpha(ch)) {
        char base = isupper(ch) ? 'A' : 'a';
        return (ch - base + shift) % 26 + base;
    }
    return ch;
}

void encrypt_file(char *file_name, int shift) {
    FILE *file = fopen(file_name, "r+");
    if (file != NULL) {
        int ch;
        while ((ch = fgetc(file)) != EOF) {
            char encrypted_ch = encrypt_char(ch, shift);
            fseek(file, -1, SEEK_CUR); 
            fputc(encrypted_ch, file);
        }
        fclose(file);
        printf("File encrypted successfully.\n");
    } else {
        printf("Error: Unable to open file '%s' for encryption.\n", file_name);
    }
}


char decrypt_char(char ch, int shift) {
    if (isalpha(ch)) {
        char base = isupper(ch) ? 'A' : 'a';
        return (ch - base - shift + 26) % 26 + base; // Add 26 to handle negative results
    }
    return ch;
}

void decrypt_file(char *file_name, int shift) {
    FILE *file = fopen(file_name, "r+");
    if (file != NULL) {
        int ch;
        while ((ch = fgetc(file)) != EOF) {
            char decrypted_ch = decrypt_char(ch, shift);
            fseek(file, -1, SEEK_CUR);  // Move back one position to overwrite the character
            fputc(decrypted_ch, file);
        }
        fclose(file);
        printf("File decrypted successfully.\n");
    } else {
        printf("Error: Unable to open file '%s' for decryption.\n", file_name);
    }
}

void cowsay(char *message) {
    printf(" _____________\n");
    printf("< %s >\n", message);
    printf(" -------------\n");
    printf("        \\   ^__^\n");
    printf("         \\  (oo)\\_______\n");
    printf("            (__)\\       )\\/\\\n");
    printf("                ||----w |\n");
    printf("                ||     ||\n");
}

void create_file(char *file_name, char *content) {
    FILE *file = fopen(file_name, "w"); 
    if (file != NULL) {
        if (content != NULL) {
            fputs(content, file); 
        }
        printf("File '%s' created successfully.\n", file_name);
        fclose(file); 
    } else {
        printf("Error: Unable to create file '%s'.\n", file_name);
    }
}

void replace_word(char* args[]){
    FILE *file = fopen(args[3], "r+");
    if (file != NULL) {
        printf("File opened successfully.\n");
        char line[256];
        char *temp_file = strdup("temp.txt");
        FILE *temp = fopen(temp_file, "w+");
        if (temp != NULL) {
            printf("Temporary file created successfully.\n");
            while (fgets(line, sizeof(line), file)) {
                char *pos;
                char *start = line;
                while ((pos = strstr(start, args[1])) != NULL) {
                    *pos = '\0';
                    fprintf(temp, "%s%s", line, args[2]); 
                    start = pos + strlen(args[1]); 
                }
                fputs(start, temp);
            }
            fclose(temp);
            fclose(file);
            remove(args[3]);
            rename(temp_file, args[3]);
            printf("String replaced successfully.\n");
        } else {
            printf("Error: Unable to create temporary file.\n");
        }
        free(temp_file);
    } else {
        printf("Error: Unable to open file '%s'.\n", args[3]);
    }
}


void executeRedirectCommand(char **args, int num_args) {

    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;
    FILE *saved_input_file = fdopen(dup(STDIN_FILENO), "r");

    for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "<") == 0) {
            FILE *input_file = fopen(args[i + 1], "r");
            if (input_file == NULL) {
                perror("fopen");
                return;
            }
            input_fd = fileno(input_file);
            if (input_fd == -1) {
                perror("fileno");
                return;
            }
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], ">") == 0) {
            FILE *output_file = fopen(args[i + 1], "w");
            if (output_file == NULL) {
                perror("fopen");
                return;
            }
            output_fd = fileno(output_file);
            if (output_fd == -1) {
                perror("fileno");
                return;
            }
            args[i] = NULL;
            i++;
        } else if (strcmp(args[i], ">>") == 0) {
            FILE *output_file = fopen(args[i + 1], "a");
            if (output_file == NULL) {
                perror("fopen");
                return;
            }
            output_fd = fileno(output_file);
            if (output_fd == -1) {
                perror("fileno");
                return;
            }
            args[i] = NULL;
            i++;
        }
    }

    int pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    } else if (pid == 0) {
        dup2(input_fd, STDIN_FILENO);
        dup2(output_fd, STDOUT_FILENO);
        close(input_fd);
        close(output_fd);
        execvp(args[0], args);
        perror(args[0]);
        exit(EXIT_FAILURE);
    } else {
        waitpid(pid, NULL, 0);
        dup2(fileno(saved_input_file), STDIN_FILENO);
        fclose(saved_input_file);
    }
}



void executePipedCommand(char **args, int num_args) {
    int pid1, pid2, fd[2];
    int input_fd = STDIN_FILENO;
    int output_fd = STDOUT_FILENO;

    for (int i = 0; i < num_args; i++) {
        if (strcmp(args[i], "|") == 0) { // Handle piping
            if (pipe(fd) == -1) {
                perror("pipe");
                return;
            }
            args[i] = NULL;
            pid1 = fork();
            if (pid1 == -1) {
                perror("fork");
                return;
            } else if (pid1 == 0) {
                close(fd[0]); 
                dup2(output_fd, STDOUT_FILENO);
                close(output_fd);
                dup2(fd[1], STDOUT_FILENO);
                close(fd[1]); 
                execvp(args[0], args);
                perror(args[0]);
            } else {
                pid2 = fork();
                if (pid2 == -1) {
                    perror("fork");
                    return;
                } else if (pid2 == 0) {
                    close(fd[1]); 
                    dup2(input_fd, STDIN_FILENO);
                    close(input_fd);
                    dup2(fd[0], STDIN_FILENO); 
                    close(fd[0]); 
                    execvp(args[i+1], &args[i+1]);
                    perror(args[i+1]);
                } else {
                    close(fd[0]);
                    close(fd[1]);
                    waitpid(pid1, NULL, 0);
                    waitpid(pid2, NULL, 0);
                    input_fd = fd[0]; 
                    output_fd = STDOUT_FILENO;
                }
            }
        }
    }
}



void execute_command(char *args[]) {
    
    int num_args = 0;
    while (args[num_args] != NULL) {
        num_args++;
    }

    executePipedCommand(args, num_args);
    
    if (strcmp(args[0], "hello") == 0) {
        printf("Hello, World!\n");
    } else if (strcmp(args[0], "add") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            int num1 = atoi(args[1]);
            int num2 = atoi(args[2]);
            printf("Sum: %d\n", num1 + num2);
        } else {
            printf("Usage: add <num1> <num2>\n");
        }
    } else if (strcmp(args[0], "rm") == 0) {
        if (args[1] != NULL) {
            remove_file(args[1]);
        } else {
            printf("Usage: rm <file_name>\n");
        }
    } else if (strcmp(args[0], "cat") == 0) {
        if (args[1] != NULL) {
            display_file_content(args[1]);
        } else {
            printf("Usage: cat <file_name>\n");
        }
    } else if (strcmp(args[0], "clear") == 0) {
        clear_screen();
    } else if (strcmp(args[0], "cowsay") == 0) {
        if (args[1] != NULL) {
            cowsay(args[1]);
        } else {
            printf("Usage: cowsay <message>\n");
        }
    } else if (strcmp(args[0], "rep") == 0) {
        if (args[1] != NULL && args[2] != NULL && args[3] != NULL) {
            replace_word(args);
        } else {
            printf("Usage: replace <old_string> <new_string> <file_name>\n");
        }
    }
else if (strcmp(args[0], "enc") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            int shift = atoi(args[1]);
            char *file_name = args[2];
            encrypt_file(file_name, shift);
        } else {
            printf("Usage: encrypt <shift_amount> <file_name>\n");
        }
    } else if (strcmp(args[0], "dec") == 0) {
        if (args[1] != NULL && args[2] != NULL) {
            int shift = atoi(args[1]);
            char *file_name = args[2];
            decrypt_file(file_name, shift);
        } else {
            printf("Usage: decrypt <shift_amount> <file_name>\n");
        }
        
    } else if (strcmp(args[0], "mkf") == 0){
        if (args[1] != NULL) {
            char *file_name = args[1];
            char *content = args[2];
            create_file(file_name, content);
        } else {
            printf("Usage: create <file_name> [content]\n");
        }
    } else if (strcmp(args[0], "kill") == 0) {
        if (args[1] != NULL) {
            int pid = atoi(args[1]);
            kill(pid, SIGKILL);
        } else {
            printf("Usage: kill <pid>\n");
        }
    }
    else if (strcmp(args[0], "fork") == 0) {
        int pid = fork();
        if (pid < 0) {
            printf("Error: Fork failed.\n");
        } else if (pid == 0) {
            printf("Child process created with PID: %d\n", getpid());
            execv(args[1], args + 1);
            printf("Error: execv failed.\n");
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
            printf("Child process with PID %d finished.\n", pid);
        }
    }
    else {
        printf("Error: Command not found.\n");
    }
}


int containsRedirect(char** args, int num_args) {

    for (int i = 0; i < num_args; i++) {

        if (strcmp(args[i], "<") == 0 || strcmp(args[i], ">") == 0 ||
            strcmp(args[i], ">>") == 0) {
            return 1;
        }
    }
    return 0;
}

int main() {
    char command[256];
    char *args[64];
    int arg_count;

    while (1) {
        get_prompt();

        if (fgets(command, 256, stdin) == NULL) {
            printf("\n");
            break;
        }

        // Tokenize command into arguments
        char *token = strtok(command, " \n");
        arg_count = 0;
        while (token != NULL && arg_count < 64 - 1) {
            args[arg_count++] = token;
            token = strtok(NULL, " \n");
        }
        args[arg_count] = NULL;

        if (arg_count > 0) {
            if (containsRedirect(args, arg_count)) {
                executeRedirectCommand(args, arg_count);

            }
            execute_command(args);
        }

    }
        return 0;
}