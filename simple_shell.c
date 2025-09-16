#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;

#define MAX_COMMAND_LENGTH 1024

int main(int argc, char **argv)
{
	char command_path[MAX_COMMAND_LENGTH];
	pid_t pid;
	int status;
	size_t len;
	unsigned long line_no = 0;
	int interactive = isatty(STDIN_FILENO);

	(void) argc;

	while (1)
	{
		if (interactive)
			printf("simple_shell> ");

		if (fgets(command_path, MAX_COMMAND_LENGTH, stdin) == NULL)
		{
			if (interactive)
				printf("\n");
			break;
		}

		line_no++;
		len = strcspn(command_path, "\n");
		command_path[len] = '\0';

		if (command_path[0] == '\0')
			continue;

		if (strcmp(command_path, "exit") == 0)
			break;

		pid = fork();
		if (pid == -1)
		{
			perror(argv[0]);
			exit(EXIT_FAILURE);
		}
		else if (pid == 0)
		{
			char *child_argv[] = {command_path, NULL};

			if (execve(command_path, child_argv, environ) == -1)
			{
				if (strchr(command_path, '/') != NULL)
				{
					fprintf(stderr, "%s: %s: %s\n", argv[0], command_path,
						(errno == ENOENT) ? "No such file or directory" : "Permission denied");
				}
				else
				{
					fprintf(stderr, "%s: %lu: %s: not found\n", argv[0], line_no, command_path);
				}
				exit(127);
			}
		}
		else
		{
			if (waitpid(pid, &status, 0) == -1)
			{
				perror(argv[0]);
				exit(EXIT_FAILURE);
			}
		}
	}

	return (0);
}
