# Simple Shell - OS 

This repository contains my implementation of a simple shell in the OS Lab course. The shell is designed to run basic Linux commands by taking user input, forking one or more child processes, and executing the commands using the `exec` system call. It also incorporates the functionality to reap dead children using the `wait` system call.

## Features
- Executes simple Linux commands like `ls`, `cat`, `echo`, and `sleep`
- Implements shell functionality using `fork`, `exec`, and `wait` system calls
- Does not rely on library functions like `system` to invoke Linux shell commands

## Getting Started
To run the shell, follow these steps:
1. Clone this repository to your local machine.
2. Compile the shell source code using a C compiler.
3. Run the compiled executable on a Linux environment.
4. Enter Linux commands and observe the shell's execution.

## Usage
Once the shell is running, you can enter commands supported by the shell, such as `ls`, `cat`, `echo`, and `sleep`. The shell will execute these commands by invoking the corresponding Linux executables.
