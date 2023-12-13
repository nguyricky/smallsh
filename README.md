# Smallsh - Your Own Shell in C
Smallsh is a simple shell implementation in C that aims to provide a subset of features found in well-known shells like bash. This project is part of an assignment where you'll build your own shell with the following features:

## Features
**1) Prompt for Running Commands:** Smallsh provides a user-friendly prompt for running shell commands.

**2) Handling Blank Lines and Comments:** Smallsh gracefully handles blank lines and comments, which are lines beginning with the # character. These lines are ignored when processing commands.

**3) Variable Expansion:** Smallsh supports the expansion of the $$ variable, allowing you to reference the current process ID.

**4) Built-in Commands:** You can use three built-in commands:

* exit: To exit the shell.
* cd: To change the current working directory.
* status: To display information about the exit status of the last foreground process.

**5) Executing Other Commands:** Smallsh can execute other shell commands by creating new processes using functions from the exec family of functions.

**6) Input and Output Redirection:** Smallsh supports input and output redirection. You can redirect the standard input and output for processes using the < and > operators, respectively.

**7) Foreground and Background Processes:** Smallsh allows you to run commands in both foreground and background processes. You can use the & operator to run a command in the background.

**8) Signal Handling:** Smallsh implements custom handlers for two signals:

* SIGINT (Ctrl+C): Sends an interrupt signal to the currently running foreground process.
* SIGTSTP (Ctrl+Z): Toggles the foreground-only mode. In this mode, the shell will prevent background processes from being run in the background.

## How to Use
**1) Compilation:** To compile Smallsh, you can use the provided Makefile. Open a terminal and navigate to the directory containing the Makefile and the smallsh.c source code, and then run the following command:
```
make
```
This will compile the smallsh program. If you want to enable debugging information, you can set the DEBUG variable as follows:
```
make DEBUG=1
```
**2) Running Smallsh:** After compiling, you can run the Smallsh shell as follows:
```
./smallsh
```
Use the shell prompt to enter commands and take advantage of the listed features.

**3) Using the Shell:** Use the shell prompt to enter commands and take advantage of the listed features.

**4) Exiting the Shell:** To exit the shell, simply type exit and press Enter.
