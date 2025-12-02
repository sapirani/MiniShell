#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <process.h>
#include <windows.h>

#define MAX_COMMAND_LENGTH 1000
#define END_COMMAND "end"

void parseArguments(char *comman_input, char **argv)
{
    while (*comman_input != '\0')
    {
        while (*comman_input == ' ' || *comman_input == '\t' || *comman_input == '\n' || *comman_input == '|')
        {
            *comman_input++ = '\0';
        }
        *argv++ = comman_input;
        while (*comman_input != '\0' && *comman_input != ' ' &&
               *comman_input != '\t' && *comman_input != '\n')
        {
            comman_input++;
        }
    }
    *argv = '\0';
}

bool shouldUseCmd(char *command)
{
    if (strstr(command, ".exe") || strstr(command, ".bat") || strstr(command, ".com"))
        return false;
    return true;
}

char *buildCommand(char *currentCommand, char **argv, bool needsCmd)
{
    if (needsCmd)
    {
        // Example: cmd.exe /C echo
        strcat(currentCommand, "cmd.exe /C ");
        strcat(currentCommand, argv[0]); // NO QUOTES ON COMMAND NAME
    }
    else
    {
        // Executables need quoting
        strcat(currentCommand, "\"");
        strcat(currentCommand, argv[0]);
        strcat(currentCommand, "\"");
    }

    // Arguments start from index 1 always
    for (int i = 1; argv[i] != NULL; i++)
    {
        strcat(currentCommand, " \"");   // <-- leading space is CRITICAL
        strcat(currentCommand, argv[i]); // value
        strcat(currentCommand, "\"");    // end quote
    }

    return currentCommand;
}

void executeCommand(char **argv)
{
    bool needsCmd = shouldUseCmd(argv[0]);

    // --- Build command line ---
    size_t size = 50;
    for (int i = 0; argv[i] != NULL; i++)
        size += strlen(argv[i]) + 4;

    char *cmdLine = malloc(size);
    cmdLine[0] = '\0';
    buildCommand(cmdLine, argv, needsCmd);

    // ----------------------
    // Create pipe for stdout
    // ----------------------
    HANDLE hRead, hWrite;
    SECURITY_ATTRIBUTES sa = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};

    if (!CreatePipe(&hRead, &hWrite, &sa, 0))
    {
        printf("Pipe creation failed\n");
        free(cmdLine);
        return;
    }

    // Ensure read handle is not inherited (only write handle is)
    SetHandleInformation(hRead, HANDLE_FLAG_INHERIT, 0);

    // ----------------------
    // Prepare STARTUPINFO
    // ----------------------
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    si.dwFlags = STARTF_USESTDHANDLES;

    si.hStdOutput = hWrite; // child stdout redirected to pipe
    si.hStdError = hWrite;  // also redirect stderr
    si.hStdInput = NULL;    // no input redirection

    ZeroMemory(&pi, sizeof(pi));

    // ----------------------
    // Create the process
    // ----------------------
    if (!CreateProcessA(
            NULL,
            cmdLine,
            NULL,
            NULL,
            TRUE, // inherit handles!
            0,
            NULL,
            NULL,
            &si,
            &pi))
    {
        printf("Unknown command or failed: %s\n", argv[0]);
        CloseHandle(hRead);
        CloseHandle(hWrite);
        free(cmdLine);
        return;
    }

    // Parent doesn't write
    CloseHandle(hWrite);

    // ----------------------
    // Read child's output
    // ----------------------
    char buffer[4096];
    DWORD bytesRead;

    while (1)
    {
        BOOL success = ReadFile(hRead, buffer, sizeof(buffer) - 1, &bytesRead, NULL);
        if (!success || bytesRead == 0)
            break;

        buffer[bytesRead] = '\0'; // null-terminate
        printf("Command output:\n");
        printf("%s", buffer);
    }

    // Cleanup
    CloseHandle(hRead);
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    free(cmdLine);
}

int main(int argc, char *argv[])
{
    char *arg[MAX_COMMAND_LENGTH];
    char user_command[MAX_COMMAND_LENGTH];
    bool flag = true;
    do
    {
        printf("Enter command: ");
        fgets(user_command, sizeof(user_command), stdin);
        printf("You entered: %s", user_command);
        printf("Parsing arguments...\n");
        parseArguments(user_command, arg);

        if (!strcmp(arg[0], END_COMMAND))
        {
            flag = false;
            printf("Exiting program.\n");
            continue;
        }
        printf("Executing command...\n");
        executeCommand(arg);

    } while (flag);
    return 0;
}