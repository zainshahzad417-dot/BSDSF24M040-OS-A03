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
