#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include "main.h"


/**
 * main - simple shell
 *@argc: argument count
 *@argv: argument vector
 *
 * Return: 0 on success
 */
int main(int argc, char **argv)
{
	int interactive = isatty(STDIN_FILENO);
	char *line = NULL;
	size_t len = 0;
	ssize_t nread;


	(void) argc;
	(void)argv;


while (1)
{
	if (interactive)
		printf("$ ");

	nread = getline(&line, &len, stdin);
	if (nread == -1)
	{
		printf("\n");
		free(line);
		exit(0);
	}

	if (line[nread - 1] == '\n')
		line[nread - 1] = '\0';

	if (line[0] == '\0')
		continue;

	execute_cmd(line, argv[0]);
}
free(line);
return (0);
}

/**
 * execute_cmd - executes the command written by the user
 * @cmd: command to execute
 * @prog_name: program name
 */
void execute_cmd(char *cmd, char *prog_name)
{
	pid_t pid;
	char *argv_exec[2];
	int status;

	argv_exec[0] = cmd;
	argv_exec[1] = NULL;

	pid = fork();
	if (pid < 0)
	{
		printf("Error: fork failed\n");
		return;
	}
	if (pid == 0)
	{
		if (execve(cmd, argv_exec, environ) == -1)
		{
			printf("%s: command does not exist\n",prog_name, cmd);
			_exit(1);
		}
	}
	else
	{
		waitpid(pid, &status, 0);
	}
}
