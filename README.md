# Custom UNIX Shell - OS Assignment 03

**Roll Number:** BSDSF24M040

## Description
A custom UNIX shell written in C for Operating Systems Assignment 03.

## Release 1 - The Base Shell
Initial starter shell code provided by Dr. Muhammad Arif Butt.

## Project Structure
- src/main.c
- src/shell.c
- src/execute.c
- include/shell.h
- Makefile
- README.md

## Build
make

## Run
./bin/myshell

## Test Commands
ls -l
pwd
whoami

## Instructor Repository
https://github.com/arifpucit/OS-Codes

## Version
v1.0-base

## Release 3 - Command History

Feature 3 introduces an in-memory command history.

### Features
- Stores the 20 most recent commands
- Displays numbered command history
- Re-executes previous commands using !n
- Handles invalid or unavailable history numbers
- Retains existing built-in and external commands

### Example
pwd
whoami
history
!1

### Version
v3

## Release 4 - GNU Readline Integration

Feature 4 adds GNU Readline to the custom shell.

### New Features
- Up and Down Arrow command navigation
- Tab completion for commands
- Tab completion for filenames and directories
- Interactive command-line editing
- Previous built-in commands preserved
- Previous command history and !n preserved

### Dependency
sudo apt install libreadline-dev

### Build
make clean && make

### Run
./bin/myshell

### Version
v4

## Release 5 - I/O Redirection and Pipes

Feature 5 adds input redirection, output redirection,
and inter-process communication through pipes.

### Features
- Output redirection using >
- Input redirection using <
- Pipes using |
- Multiple connected commands
- Combined pipes and redirection
- File and command error handling
- Previous built-in commands and history preserved

### Examples
ls -l > file_list.txt
sort < file_list.txt
cat /etc/passwd | grep "root"
cat /etc/passwd | grep root > root_users.txt

### System Calls
- open()
- dup2()
- close()
- pipe()
- fork()
- execvp()
- waitpid()

### Build
make clean && make

### Run
./bin/myshell

### Version
v5

## Release 6 - Command Chaining and Background Execution

Feature 6 adds multitasking capabilities to the custom shell.

### New Features
- Sequential execution using ;
- Background execution using &
- Background job tracking with PIDs
- Working jobs built-in command
- Zombie process cleanup using waitpid() with WNOHANG
- Support for background pipelines
- Previous features preserved

### Examples
echo "First" ; sleep 2 ; echo "Second"
sleep 10 &
jobs
sleep 5 & echo "Next"
cat /etc/passwd | grep root

### System Calls
- fork()
- waitpid()
- WNOHANG
- execvp()
- pipe()
- dup2()

### Build
make clean && make

### Run
./bin/myshell

### Version
v6

## Release 7 - if-then-else-fi Control Structure

Feature 7 implements multi-line conditional execution.

### Features
- if condition checking
- then block for successful commands
- Optional else block
- fi to terminate the structure
- Conditional execution based on exit status
- Multiple commands in each branch

### Example
if true
then
echo "Success"
else
echo "Failure"
fi

### Build
make clean && make

### Version
v7

## Release 8 - Shell Variables

Feature 8 adds shell variable assignment, storage,
expansion, reassignment, and display.

### Features
- Store variables using a linked list
- Assign variables with VARNAME=value
- Expand variables using $VARNAME
- Reassign existing variables
- Display stored variables using set
- Support quoted values containing spaces
- Use variables with command chaining and conditions
- Manage dynamically allocated memory
- Preserve all previous shell features

### Examples
MSG="hello"
set
echo $MSG
MSG="goodbye"
echo $MSG

### Implementation
- src/variables.c
- include/variables.h

### Build
make clean && make

### Run
./bin/myshell

### Version
v8
