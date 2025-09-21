#ifndef SHELL_H
#define SHELL_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

extern char **environ;


void execute_cmd(char *cmd, char *prog_name);
void trim_newline(char *line);
char *find_path(void);

#endif
