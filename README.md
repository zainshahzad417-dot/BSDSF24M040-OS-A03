# Custom UNIX Shell — Operating Systems Assignment 03

### A Modular UNIX Command-Line Interpreter Written in C

**Roll Number:** BSDSF24M040
**Course:** Operating Systems
**Instructor:** Dr. Muhammad Arif Butt
**Platform:** Ubuntu Linux (Oracle VirtualBox)
**Language:** C
**Latest Release:** v8 — Shell Variables
**GitHub Owner:** [zainshahzad417-dot](https://github.com/zainshahzad417-dot)

---

## Project Overview

This project implements a custom UNIX shell in C, developed incrementally across eight software releases and a final collaborative Git workflow.

The shell supports external commands, built-ins, command history, GNU Readline, tab completion, I/O redirection, pipes, background jobs, conditional execution, and variables.

Feature 9 demonstrates collaborative development using a secondary GitHub account, a repository fork, an issue, a fix branch, a pull request, code review, and merging.

### Learning Objectives

- Understand the UNIX fork-exec-wait process lifecycle.
- Implement built-in and external command execution.
- Manage history and command-line input.
- Integrate GNU Readline.
- Manipulate file descriptors using open(), dup2(), and close().
- Implement inter-process communication using pipe().
- Execute sequential and background commands.
- Track and reap background processes.
- Implement conditional execution based on exit status.
- Store and expand shell variables.
- Practice professional Git and GitHub collaboration.

---

## Technologies

| Technology | Purpose |
|---|---|
| C | Shell implementation |
| GCC | Compilation |
| GNU Make | Build automation |
| GNU Readline | Input editing and completion |
| POSIX APIs | Process and file operations |
| Python 3 | Automated testing |
| Bash | Integration testing |
| Ubuntu Linux | Development environment |
| Oracle VirtualBox | Virtual machine |
| Git | Version control |
| GitHub | Repository and releases |
| GitHub CLI | Issues, releases, and pull requests |

---

## Project Structure

```text
BSDSF24M040-OS-A03/
├── src/
│   ├── main.c
│   ├── shell.c
│   ├── execute.c
│   ├── history.c
│   ├── completion.c
│   ├── io.c
│   ├── jobs.c
│   ├── multitask.c
│   ├── control.c
│   └── variables.c
├── include/
│   ├── shell.h
│   ├── history.h
│   ├── completion.h
│   ├── io.h
│   ├── jobs.h
│   ├── multitask.h
│   ├── control.h
│   └── variables.h
├── tests/
│   ├── test_history.py
│   ├── test_readline.sh
│   ├── test_io.py
│   ├── test_multitasking.py
│   ├── test_control.py
│   └── test_variables.py
├── Makefile
├── README.md
└── .gitignore
```

The Makefile generates the executable `bin/myshell`.

---

## Shell Execution Architecture

```text
START
  |
  v
Initialize Shell
  |
  v
Display FCIT> Prompt
  |
  v
Read Input with GNU Readline
  |
  v
Process History Expansion
  |
  v
Parse Command
  |
  +----> Conditional Control Structure
  |
  +----> Variable Assignment / Expansion
  |
  +----> Built-in Command
  |
  +----> External Command
              |
              v
       Handle Redirection
              |
              v
       Configure Pipes
              |
              v
       Fork Child Process
              |
              v
          execvp()
              |
              v
       Foreground / Background
              |
              v
       Wait / Track Jobs
              |
              v
       Return to Prompt
```

Built-in commands that change shell state run in the parent process. External commands are executed through child processes.

---

# Release History and Features

## Release 1 — The Base Shell

**Marks:** 5
**Tag:** `v1.0-base`

### Implemented

- Initial shell project setup
- Modular source and header directories
- Makefile compilation
- External command execution
- Parent and child process management
- Initial Git commit, tag, and GitHub release

### Example

```bash
ls -l
pwd
whoami
```

**System calls:** `fork()`, `execvp()`, `waitpid()`

[View Release 1](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v1.0-base)

---

## Release 2 — Built-in Commands

**Marks:** 10
**Tag:** `v2`
**Branch:** `feature-built-ins`

### Implemented

- `exit` — terminate the shell
- `cd` — change directory
- `help` — display supported commands
- `jobs` — initial placeholder
- Parent-process execution for built-ins
- Error handling for invalid directories

### Example

```bash
help
cd /tmp
pwd
cd
jobs
exit
```

**Core system call:** `chdir()`

[View Release 2](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v2)

---

## Release 3 — Command History

**Marks:** 10
**Tag:** `v3`
**Branch:** `feature-history`

### Implemented

- In-memory history storage
- Retention of the latest 20 commands
- Numbered history display
- Re-execution using `!n`
- Handling of invalid history references
- Preservation of previous functionality

### Example

```bash
pwd
whoami
history
!1
```

**Concepts:** Arrays, string manipulation, memory management.

[View Release 3](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v3)

---

## Release 4 — GNU Readline and Tab Completion

**Marks:** 15
**Tag:** `v4`
**Branch:** `feature-readline`

### Implemented

- GNU Readline integration
- Interactive line editing
- Up/Down Arrow history navigation
- Tab completion for commands
- Filename and directory completion
- Retention of custom numbered history
- Linking using `-lreadline`

### Examples

Type `whoam` and press Tab to complete `whoami`.

Type `cat README.m` and press Tab to complete `README.md`.

Use the Up and Down Arrow keys to navigate previously entered commands.

**Library functions:** `readline()`, `add_history()`, completion APIs.

[View Release 4](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v4)

---

## Release 5 — I/O Redirection and Pipes

**Marks:** 15
**Tag:** `v5`
**Branch:** `feature-io-redirection`

### Implemented

- Output redirection `>`
- Input redirection `<`
- Pipes `|`
- Multiple pipelines
- Combined pipes and redirection
- Error handling for files and commands

### Examples

```bash
ls -l > file_list.txt
sort < file_list.txt
cat /etc/passwd | grep root
cat /etc/passwd | grep root > root_users.txt
cat /etc/passwd | grep root | wc -l
```

**System calls:** `open()`, `close()`, `dup2()`, `pipe()`, `fork()`, `execvp()`, `waitpid()`.

[View Release 5](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v5)

---

## Release 6 — Command Chaining and Background Execution

**Marks:** 15
**Tag:** `v6`
**Branch:** `feature-multitasking`

### Implemented

- Sequential commands using `;`
- Background execution using `&`
- Background process tracking
- `jobs` command with process IDs
- Zombie-process cleanup
- Non-blocking `waitpid()` with `WNOHANG`
- Background pipeline support

### Examples

```bash
echo "First" ; sleep 2 ; echo "Second"
sleep 10 &
jobs
sleep 5 & echo "Next"
```

**Concepts:** Process management, job tracking, zombie processes.

[View Release 6](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v6)

---

## Release 7 — if-then-else-fi Control Structure

**Marks:** 10
**Tag:** `v7`
**Branch:** `feature-if-then-else`

### Implemented

- Multi-line `if` statements
- `then` blocks for successful conditions
- Optional `else` blocks
- `fi` to end the structure
- Exit-status-based conditional execution
- Multiple commands in branches
- Compatibility with previous features

### Successful Condition

```bash
if true
then
echo SUCCESS
else
echo FAILURE
fi
```

Expected: `SUCCESS`

### Failed Condition

```bash
if false
then
echo SHOULD_NOT_RUN
else
echo FAILURE_HANDLED
fi
```

Expected: `FAILURE_HANDLED`

### File Existence

```bash
if [ -f "myfile.txt" ]
then
echo FILE_EXISTS
else
echo FILE_MISSING
fi
```

**Concepts:** State management, `WIFEXITED()`, `WEXITSTATUS()`.

[View Release 7](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v7)

---

## Release 8 — Shell Variables

**Marks:** 10
**Tag:** `v8`
**Branch:** `feature-variables`

### Implemented

- Variable assignment using `NAME=value`
- Linked-list-based variable storage
- Variable expansion using `$NAME`
- Variable reassignment
- `set` built-in command
- Quoted values containing spaces
- Variables with command chaining
- Variables inside conditional structures
- Variables with pipes
- Invalid variable-name handling
- Dynamic memory management

### Assignment and Expansion

```bash
MSG="hello"
set
echo $MSG
```

Expected: `hello`

### Reassignment

```bash
MSG="goodbye"
echo $MSG
```

Expected: `goodbye`

### Values Containing Spaces

```bash
MESSAGE="Hello, World!"
echo $MESSAGE
```

Expected: `Hello, World!`

### Variables and Chaining

```bash
CITY=Lahore ; echo $CITY
```

Expected: `Lahore`

**Concepts:** Linked lists, string manipulation, `malloc()`, `realloc()`, `free()`.

[View Release 8](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases/tag/v8)

---

## Feature 9 — Collaborative Git Workflow

**Marks:** 10
**Completion:** Merged Pull Request #2

Feature 9 demonstrates collaboration between two GitHub accounts.

### Primary Repository

[zainshahzad417-dot/BSDSF24M040-OS-A03](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03)

### Secondary Contributor

GitHub account: `zainshahzad4125`

[Forked Repository](https://github.com/zainshahzad4125/BSDSF24M040-OS-A03)

### Completed Workflow

1. Created a secondary GitHub account.
2. Forked the original repository.
3. Created an issue for a help-command improvement.
4. Cloned the fork separately in Ubuntu.
5. Created a dedicated fix branch.
6. Updated the shell help output.
7. Compiled and tested the fix.
8. Committed and pushed the fix.
9. Submitted a pull request.
10. Reviewed and approved the proposed changes.
11. Merged Pull Request #2.
12. Synchronized the original Ubuntu project.
13. Verified the feature branches and tags.

### Implemented Improvement

The `help` command now displays:

```text
Variable usage: MSG=hello; echo $MSG
```

This explains how variables are assigned and expanded.

[View Merged Pull Request #2](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/pull/2)

Feature 9 is recorded through its merged pull request rather than an additional versioned release.

---

# Installation and Execution

## 1. Install Dependencies

On Ubuntu:

```bash
sudo apt update
sudo apt install build-essential make libreadline-dev python3 git
```

## 2. Clone the Repository

```bash
git clone https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03.git
```

## 3. Enter the Project Directory

```bash
cd BSDSF24M040-OS-A03
```

## 4. Build

```bash
make clean && make
```

## 5. Run

```bash
./bin/myshell
```

The shell displays:

```text
FCIT>
```

To exit:

```bash
exit
```

---

# Supported Built-in Commands

| Command | Description |
|---|---|
| `exit` | Exit the shell |
| `cd directory` | Change the current directory |
| `cd` | Change to the home directory |
| `help` | Display command help |
| `jobs` | Show background processes |
| `history` | Display command history |
| `!n` | Re-execute command number n |
| `set` | Display defined variables |
| `NAME=value` | Assign or modify a variable |

External commands such as `ls`, `pwd`, `whoami`, `cat`, `grep`, `sort`, `echo`, and `sleep` are also supported.

---

# Automated Testing

The project includes six regression and integration test suites.

### Feature 3 — Command History

```bash
python3 tests/test_history.py
```

### Feature 4 — GNU Readline

```bash
bash tests/test_readline.sh
```

### Feature 5 — Redirection and Pipes

```bash
python3 tests/test_io.py
```

### Feature 6 — Multitasking

```bash
python3 tests/test_multitasking.py
```

### Feature 7 — Conditional Execution

```bash
python3 tests/test_control.py
```

### Feature 8 — Shell Variables

```bash
python3 tests/test_variables.py
```

## Run All Tests

```bash
python3 tests/test_history.py && \
bash tests/test_readline.sh && \
python3 tests/test_io.py && \
python3 tests/test_multitasking.py && \
python3 tests/test_control.py && \
python3 tests/test_variables.py
```

All six automated test suites passed in the final development environment.

Interactive testing is additionally required to verify actual Tab completion and arrow-key navigation.

---

# Git Development Workflow

Features 2–8 followed this workflow:

```text
Update main
    |
    v
Create Feature Branch
    |
    v
Implement Feature
    |
    v
Build and Test
    |
    v
Commit and Push
    |
    v
Merge into main
    |
    v
Create Annotated Tag
    |
    v
Push Git Tag
    |
    v
Publish GitHub Release
    |
    v
Attach myshell Executable
```

## Development Branches

| Branch | Purpose |
|---|---|
| main | Integrated application |
| feature-built-ins | Built-in commands |
| feature-history | Command history |
| feature-readline | Readline integration |
| feature-io-redirection | Redirection and pipes |
| feature-multitasking | Background jobs |
| feature-if-then-else | Conditional execution |
| feature-variables | Shell variables |

The Feature 9 fix was implemented in a separate contributor branch on the secondary fork.

---

# Published Releases

| Release | Feature | Tag |
|---|---|---|
| 1 | Base Shell | `v1.0-base` |
| 2 | Built-in Commands | `v2` |
| 3 | Command History | `v3` |
| 4 | GNU Readline | `v4` |
| 5 | I/O Redirection and Pipes | `v5` |
| 6 | Background Execution and Chaining | `v6` |
| 7 | if-then-else-fi | `v7` |
| 8 | Shell Variables | `v8` |

Each release contains a compiled `myshell` executable as an asset.

[View All GitHub Releases](https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases)

---

# Git Commands

Check repository status:

```bash
git status
```

View branches:

```bash
git branch -a
```

View annotated tags:

```bash
git tag -n
```

View commit history:

```bash
git log --oneline --graph --all
```

Push main:

```bash
git push origin main
```

Push feature branches:

```bash
git push origin --all
```

Push tags:

```bash
git push origin --tags
```

---

# Learning Outcomes

## Operating Systems

- Process creation and execution
- Parent-child process relationships
- Process synchronization
- Foreground and background execution
- Zombie-process management
- File descriptors
- Redirection and piping
- Exit-status evaluation

## C Programming

- Modular program organization
- Arrays and linked lists
- Dynamic memory allocation
- String processing
- Input parsing
- Conditional execution
- Error handling

## Software Engineering

- Feature-based development
- Git branching and merging
- Annotated version tags
- GitHub release publishing
- Regression testing
- Forks and pull requests
- Issue tracking
- Code review
- Collaborative integration

---

# Project Scope

This is an educational UNIX shell, not a complete Bash or Zsh replacement.

Advanced features such as complete POSIX shell grammar, full terminal job control, persistent variables, and complex shell scripting are outside the assignment scope.

---

# Original Code and Acknowledgment

The starter shell code was provided by:

**Dr. Muhammad Arif Butt**

Original repository:

[arifpucit/OS-Codes](https://github.com/arifpucit/OS-Codes)

The project extends this original implementation through modular operating-system features and professional Git workflows.

---

# Assignment Summary

| Feature | Maximum Marks |
|---|---:|
| 1 — Base Shell | 5 |
| 2 — Built-in Commands | 10 |
| 3 — History | 10 |
| 4 — Readline | 15 |
| 5 — I/O and Pipes | 15 |
| 6 — Multitasking | 15 |
| 7 — Conditional Execution | 10 |
| 8 — Shell Variables | 10 |
| 9 — Git Workflow | 10 |
| **Total Available** | **100** |

Features 1–9 have been completed through development, testing, versioned releases, and the collaborative pull request.

Final grading is determined by the instructor.

---

## Project Links

**Repository:**
https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03

**Releases:**
https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/releases

**Merged Pull Request #2:**
https://github.com/zainshahzad417-dot/BSDSF24M040-OS-A03/pull/2

**Latest Versioned Release:** `v8`
**Feature 9:** GitHub collaboration completed
