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

	(void)argc;
while (1)
{
	if (interactive)
		printf("$ ");

	nread = getline(&line, &len, stdin);
	if (nread == -1)
	{
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
	char *argv[64];
	int status;
	int argc = 0;
	char *token;
	char *path_copy;
	char *dir;
	char fullpath[1024];

	token = strtok(cmd, " ");
	while (token != NULL && argc < 63)
	{
		argv[argc] = token;
		token = strtok(NULL, " ");
		argc++;
	}
	argv[argc] = NULL;
	if (argc > 0 && strcmp(argv[0], "exit") == 0)
	{
		exit(0);
	}

	if (argc > 0 && strcmp(argv[0], "env") == 0)
	{
		int i = 0;

		while (environ[i] != NULL)
		{
			printf("%s\n", environ[i]);
			i++;
		}
		return;

	}

	if (access(argv[0], X_OK) != 0)
	{
		if (argv[0][0] != '/')
		{
			path_copy = strdup(getenv("PATH"));
			if (path_copy != NULL)
			{
			dir = strtok(path_copy, ":");
			while (dir != NULL)
			{
				fullpath[0] = '\0';
				if (strlen(dir) + 1 + strlen(argv[0]) + 1 < sizeof(fullpath))
				{
				strcpy(fullpath, dir);
				strcat(fullpath, "/");
				strcat(fullpath, argv[0]);

				if (access(fullpath, X_OK) == 0)
				{
					argv[0] = strdup(fullpath);
					break;
				}
				dir = strtok(NULL, ":");
			}
			free(path_copy);
		}
	}
	}
}
	

	if (access(argv[0], X_OK) != 0)
	{
	fprintf(stderr, "%s: 1: %s: not found\n", prog_name, argv[0]);
	return;
	}

	pid = fork();
	if (pid < 0)
	{
		printf("Error: fork failed\n");
		return;
	}
	if (pid == 0)
	{
		if (execve(argv[0], argv, environ) == -1)
		{
			printf("%s: %s: command does not exist\n", prog_name, argv[0]);
			_exit(1);
		}
	}
	else
	{
		waitpid(pid, &status, 0);
	}
}
