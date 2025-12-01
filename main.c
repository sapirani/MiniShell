#include <stdio.h>
#include <stdbool.h>

#define MAX_COMMAND_LENGTH 20
#define END_COMMAND "end"

void parseArguments(char *comman_input, char **argv)
{
    while (*comman_input != '\0')
    {
        while (*comman_input == ' ' || *comman_input == '\t' || *comman_input == '\n' || *comman_input == '|')
            *comman_input++ = '\0';
        *argv++ = comman_input;
        while (*comman_input != '\0' && *comman_input != ' ' &&
               *comman_input != '\t' && *comman_input != '\n')
            comman_input++;
    }
    *argv = '\0';
}

void executeCommand(char **argv)
{
    int i = 0;
    printf("Executing command with arguments:\n");
    while (argv[i] != NULL)
    {
        printf("Argument %d: %s\n", i, argv[i]);
        i++;
    }
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
        executeCommand(arg);

    } while (flag);
    return 0;
}