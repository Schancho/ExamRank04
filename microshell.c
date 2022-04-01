#include "microshell.h"

int ft_strlen(char *str)
{
	int i = 0;
	while (str[i])
		i++;
	return (i);
}

char *ft_strdup(char *str)
{
	int i = -1;
	char *ret;
	if(!(ret = malloc(ft_strlen(str) + 1)))
		exit(write(2, "error: fatal\n", 13));
	while (str[++i])
		ret[i] = str[i];
	ret[i] = '\0';
	return (ret);
}

char **getcmd(int j, char **argv)
{
	int i = -1;
	char **cmd;
	if(!(cmd = malloc(j * sizeof(char *))))
		exit(write(2, "error: fatal\n", 13));
	while (++i < j - 1)
		cmd[i] = ft_strdup(argv[i]);
	cmd[i] = NULL;
	return (cmd);
}

void freecmd(char **cmd)
{
	int i = -1;
	if (cmd)
	{
		while (cmd[++i])
			free(cmd[i]);
		free(cmd);
	}
}

void ft_cd(char **cmd)
{
	int i = 0;
	while (cmd[i])
		i++;
	if (i == 2)
	{
		if (chdir(cmd[1]) == -1)
		{
			write(2, "error: cd: cannot change directory to ", ft_strlen("error: cd: cannot change directory to "));
			write(2, cmd[1], ft_strlen(cmd[1]));
			write(2, " \n", 2);
		}
	}
	else
		write(2, "error: cd: bad arguments\n", ft_strlen("error: cd: bad arguments\n"));
}

void execute(char **cmd, int ispipe, char **envp)
{
	pid_t pid;
	if (strcmp(*cmd, "cd") == 0)
	{
		ft_cd(cmd);
		freecmd(cmd);
		return;
	}
	if ((t_p.pnext = ispipe) && pipe(t_p.pipe) < 0)
		exit(write(2, "error: fatal\n", 13));
	if ((pid = fork()) == -1)
		exit(write(2, "error: fatal\n", 13));
	if (pid == 0)
	{
		if (t_p.pnext && dup2(t_p.pipe[1], 1) < 0)
			exit(write(2, "error: fatal\n", 13));
		if (t_p.pprev && dup2(t_p.outpipe, 0) < 0)
			exit(write(2, "error: fatal\n", 13));
		if ((execve(*cmd, cmd, envp)) == -1)
		{
			write(2, "error: cannot execute ", ft_strlen("error: cannot execute "));
			write(2, *cmd, ft_strlen(*cmd));
			write(2, "\n", 1);
			exit(errno);
		}
		exit(0);
	}
	else
	{
		waitpid(pid, 0, 0);
		if(t_p.outpipe)
			close(t_p.outpipe);
		if ((t_p.pprev = ispipe))
		{
			if((t_p.outpipe = dup(t_p.pipe[0])) < 0)
				exit(write(2, "error: fatal\n", 13));
			close(t_p.pipe[0]);
			close(t_p.pipe[1]);
		}
		free(cmd);
	}
}


int main(int argc, char **argv, char **envp)
{
	int i = 0;
	if (argc > 1)
	{
		while (argv[++i])
		{
			if (strcmp(argv[i], "|") == 0 || strcmp(argv[i], ";") == 0)
			{
				if (i > 1)
					execute(getcmd(i, argv + 1), strcmp(argv[i], "|") == 0, envp);
				argv += i;
				i = 0;
			}
		}
		if (strcmp(argv[i - 1], "|") != 0 && strcmp(argv[i - 1], ";") != 0)
			execute(getcmd(i, argv + 1), 0, envp);
	}
}

