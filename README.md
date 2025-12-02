# MiniShell
Basic command interpreter in C

This repository contains an implementation to command interperter, written in C language.
The implementation works as follows:
1. Ask the user for a command.
2. Parse the command into tokens.
3. Understand whether the command requires execution through the command line or not.
4. If it is a CMD command, add "cmd.exe /C" to the user's input.
5. Execute the updated command, and print it's output.

For now, this interpreter works in Windows only - hence supports only commands that work in windows.

In order to terminate the program, enter `end` as the command.
