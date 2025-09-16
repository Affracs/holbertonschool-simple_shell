#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#define MAX_COMMAND_LENGTH 1024
/**
 * main - simple shell that executes commands from input
 *
 * Return: 0 on success
 */
int main(void)
{
	char command_path[MAX_COMMAND_LENGTH];
	pid_t pid;
	int status;
	char *envp[] = {NULL};
	while (1)
	{
		printf("simple_shell> ");
		if (fgets(command_path, MAX_COMMAND_LENGTH, stdin) == NULL)
		{
			printf("\nExiting simple_shell.\n");
			break;
		}
		command_path[strcspn(command_path, "\n")] = 0;
		if (strcmp(command_path, "exit") == 0)
		{
			printf("Exiting simple_shell.\n");
			break;
		}
		pid = fork();
		if (pid == -1)
		{
			perror("fork");
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			char *command_path_value = "";
			char *argv[2];
			argv[0] = command_path_value;
			argv[1] = NULL;
			if (execve(command_path, argv, envp) == -1)
			{
				perror("execve");
				exit(EXIT_FAILURE);
			}
		}
		else
		{
			if (waitpid(pid, &status, 0) == -1)
			{
				perror("waitpid");
				exit(EXIT_FAILURE);
			}
		}
	}
	return (0);
}
