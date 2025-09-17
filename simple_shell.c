include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
extern char **environ;
#define PROMPT "$ "
int is_interactive_mode(void);
void print_prompt(void);
char *trim_newline(char *s);
void run_command(char *cmd, char *prog_name, unsigned long line_no);
int is_interactive_mode(void)
{
    return (isatty(STDIN_FILENO));
}
void print_prompt(void)
{
    if (write(STDOUT_FILENO, PROMPT, strlen(PROMPT)) == -1)
        ;
    fflush(stdout);
}
char *trim_newline(char *s)
{
    size_t len;
    if (s == NULL)
        return (NULL);
    len = strlen(s);
    if (len > 0 && s[len - 1] == '\n')
        s[len - 1] = '\0';
    return (s);
}
void run_command(char *cmd, char *prog_name, unsigned long line_no)
{
    pid_t pid;
    int status;
    if (cmd == NULL || *cmd == '\0')
        return;
    pid = fork();
    if (pid == -1)
    {
        perror(prog_name);
        return;
    }
    if (pid == 0)
    {
        char *argv_exec[2];
        argv_exec[0] = cmd;
        argv_exec[1] = NULL;
        execve(cmd, argv_exec, environ);
        if (strchr(cmd, '/') != NULL)
        {
            dprintf(STDERR_FILENO, "%s: %s: %s\n", prog_name, cmd,
                    (errno == ENOENT) ? "No such file or directory" : "Permission denied");
        }
        else
        {
            dprintf(STDERR_FILENO, "%s: %lu: %s: not found\n", prog_name, line_no, cmd);
        }
        _exit(127);
    }
    else
    {
        do {
            if (waitpid(pid, &status, 0) == -1)
            {
                if (errno == EINTR)
                    continue;
                perror(prog_name);
                break;
            }
            else
                break;
        } while (1);
    }
}
int main(int argc, char **argv)
{
    char *line = NULL;
    size_t n = 0;
    ssize_t read_len;
    unsigned long line_no = 0;
    int interactive = is_interactive_mode();
    (void) argc;
    while (1)
    {
        if (interactive)
            print_prompt();
        read_len = getline(&line, &n, stdin);
        if (read_len == -1)
        {
            free(line);
            if (interactive)
                write(STDOUT_FILENO, "\n", 1);
            exit(EXIT_SUCCESS);
        }
        line_no++;
        trim_newline(line);
        if (line[0] == '\0')
            continue;
        run_command(line, argv[0], line_no);
    }
    free(line);
    return (0);
}
