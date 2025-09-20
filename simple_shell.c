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
 * @argc: argument count
 * @argv: argument vector
 *
 * Return: 0 on success
 */
int	main(int argc, char **argv)
{
	int	interactive = isatty(STDIN_FILENO);
	char	*line = NULL;
	size_t	len = 0;
	ssize_t	nread;

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
 * find_path - return path from environ
 *
 * Return: path to value or NULL if not found
 */
char *find_path(void)
{
	int i = 0;

	while (environ[i])
	{
		if (strncmp(environ[i], "PATH=", 5) == 0)
			return (environ[i] + 5);
		i++;
	}
	return (NULL);
}

/**
 * execute_cmd - executes the command written by the user
 * @cmd: command to execute
 * @prog_name: program name
 */
void	execute_cmd(char *cmd, char *prog_name)
{
	pid_t	pid;
	char	*argv[64];
	int		status;
	int		argc = 0;
	char	*token;
	char	*path_copy;
	char	*dir;
	char	fullpath[1024];
	char	*full_cmd = NULL;

	token = strtok(cmd, " ");
	while (token != NULL && argc < 63)
	{
		argv[argc] = token;
		token = strtok(NULL, " ");
		argc++;
	}
	argv[argc] = NULL;

	if (argc > 0 && strcmp(argv[0], "exit") == 0)
		exit(0);

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

	if (access(argv[0], X_OK) != 0 && argv[0][0] != '/')
	{

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
						full_cmd = strdup(fullpath);
						break;
					}
				}
				dir = strtok(NULL, ":");
			}
			free(path_copy);
		}

		if (full_cmd != NULL)
			argv[0] = full_cmd;
	}

	if (access(argv[0], X_OK) != 0)
	{
		fprintf(stderr, "%s: 1: %s: not found\n", prog_name, argv[0]);
		if (full_cmd != NULL)
			free(full_cmd);
		return;
	}

	pid = fork();
	if (pid < 0)
	{
		printf("Error: fork failed\n");
		if (full_cmd != NULL)
			free(full_cmd);
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
		waitpid(pid, &status, 0);

	if (full_cmd != NULL)
		free(full_cmd);
}

