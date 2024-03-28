# Group Members
- Albara Hassan
- Andrei Baranov

## Directory Listing
- `shell.c` : The C program for the shell implementation.

## Assignment Answers

### Question 1.5.1:
The `mmap()` function in C is used to map files or devices into memory. It allows the programmer to treat a file or device as if it were an array in memory, enabling direct access to its contents. This function is commonly used for memory-mapped I/O operations and for creating shared memory regions between processes.

### Question 1.5.2:
During a context switch, the operating system saves the state of a process (such as its CPU registers, program counter, and stack pointer) and restores the state of another process, allowing multiple processes to share a single CPU core efficiently. We generally want the OS to perform as few context switches as possible because each context switch incurs overhead in terms of CPU time and system resources. Minimizing context switches improves overall system performance and responsiveness. However, context switches are necessary for multitasking and concurrency.

### Question 1.5.3:

When the fork() system call is used, it creates a child process. The child process attempts an exec() call, which fails, so the child process prints "child". Meanwhile, the parent process waits for the child to finish and then prints "parent". The output will be:
```
child
parent
```
The wait() ensures "child" is printed before "parent".

## Assignment Outline
- Implemented a basic shell program in C.
- Included functionalities for file manipulation, text processing, and process management.
- Used system calls and library functions to interact with the operating system.
- Provided user-friendly prompts and error messages for better usability.

## Compilation Instructions
To compile the C program, run the following command in the terminal:
```bash
gcc -o shell shell.c
```
To run the C program:
```bash
./shell
```

Additionally you should download jp2a to be able to display images in the shell. You can do this by running the following command:
```bash
sudo apt-get install jp2a
```

## Any challenges you encountered along the way
- Implementing the shell piping and handling redirection of the files.

## Any sources/tutorials you have used to complete the assignment:

[Guide to Code a Simple Shell in C](https://medium.com/@winfrednginakilonzo/guide-to-code-a-simple-shell-in-c-bd4a3a4c41cd)
