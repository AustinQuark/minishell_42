/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   handle_commands.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: tanguy <tanguy@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2020/10/26 14:33:56 by tanguy            #+#    #+#             */
/*   Updated: 2020/10/31 00:14:04 by tanguy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../includes/minishell.h"

void		spawn_process(int in, int out, t_cmd **cmd, int i)
{
	pid_t pid;

 	if ((pid = fork ()) == 0)
    {
		if (in != 0)
        {
 			dup2 (in, STDIN_FILENO);
			close (in);
        }
		if (cmd[i + 1])
		{
			dup2 (out, STDOUT_FILENO);
			close (out);
        }
		handle_one_command(cmd[i]);
	}
}

void		handle_commands_pipe(t_cmd **cmds, int nb_cmds)
{
	int		i;
 	int		in;
	int		status;
  	int		pipefd[2];

	in = 0;
	i = 0;
	while (i < nb_cmds)
	{
		pipe(pipefd);
		spawn_process(in, pipefd[WRITE_END], cmds, i);
		close(pipefd[WRITE_END]);
		if (in != 0)
			close(in);
		in = pipefd[READ_END];
		i++;
	}
	while (wait(&status) > 0);
}

void		handle_one_command(t_cmd *cmd)
{
	if (cmd->out_fd != -1 && cmd->in_fd != -1)
	{
		if (is_builtin(cmd->args[0]))
		{
			exec_builtin(cmd);
			exit(0);
		}
		else
		{
			if (!get_absolute_path(cmd->args))
				error_cmd_not_found(cmd->args[0]);
			else
			{
				do_redir(cmd->in_fd, cmd->out_fd);
				if (execve(cmd->args[0], cmd->args, g_sh.env) == -1)
					exit(EXIT_FAILURE);
			}
		}
		if (cmd->out_fd > 0)
			close(cmd->out_fd);
		if (cmd->in_fd > 0)
			close(cmd->in_fd);
	}
}