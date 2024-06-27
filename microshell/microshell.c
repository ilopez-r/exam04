#include "microshell.h"

int err(char *str)
{
	int i = 0;

	while (str[i] != '\0')
		write (2, &str[i++], 1);
	return (1);
}

int exec (char **argv, int i, char **env)
{
	int 	fd[2];
	int 	g_status;
	int 	h_pipe = 0;
	pid_t 	pid;

	if (argv[i] != NULL && strcmp(argv[i], "|") == 0)
		h_pipe = 1;
	if (h_pipe == 1 && pipe(fd) == -1)
		return (err("error: fatal\n"));
	pid = fork();
	if (pid == 0)
	{
		argv[i] = NULL;
		if (h_pipe == 1 && (close(fd[0]) == -1 || dup2(fd[1], STDOUT_FILENO) == -1 || close(fd[1]) == -1))
			return err("error: fatal\n");
		execve(argv[0], argv, env);
		return (err("error: cannot execute "), err(argv[0]), err("\n"));
	}
	if (h_pipe == 1 && (close(fd[1]) == -1 || dup2(fd[0], STDIN_FILENO) == -1 || close(fd[0]) == -1))
		return (err("error: fatal\n"));
	waitpid(pid, &g_status, 0);
	return (g_status);
}

int cd(char **argv, int i)
{
	if (i != 2)
		return (err("error: cd: bad arguments\n"));
	if (chdir(argv[1]) == -1)
		return err("error: cd: cannot change directory to "), err(argv[1]), err("\n");
	return (0);
}

int main (int argc, char **argv, char **env)
{
	int i = 0;
	int g_status = 0;

	if (argc > 1)
	{
		while (argv[i] != NULL && argv[++i] != NULL)
		{
			argv += i;
			i = 0;
			while (argv[i] != NULL && strcmp(argv[i], "|") != 0 && strcmp(argv[i], ";") != 0)
				i++;
			if (strcmp(*argv, "cd") == 0)
                		g_status = cd(argv, i);
			else if (i != 0)
				g_status = exec(argv, i, env);
		}
	}
	return (g_status);
}
