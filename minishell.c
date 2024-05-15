/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgavairo <jgavairo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:02:31 by jgavairo          #+#    #+#             */
/*   Updated: 2024/05/15 14:44:15 by jgavairo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

void	ft_unset(t_env **mini_env, t_cmd *cmd)
{
	t_env	*tmp;
	t_env	*swap;
	int		i;

	i = 1;
	tmp = (*mini_env);
	while (cmd->args[i])
	{
		while (tmp->next && ft_strcmp(tmp->next->name, cmd->args[i]) != 0)
			tmp = tmp->next;
		if (tmp && ft_strcmp(tmp->next->name, cmd->args[i]) == 0)
		{
			swap = tmp->next;
			tmp->next = tmp->next->next;
			free(swap->name);
			free(swap->value);
			free(swap);
		}
		i++;
	}
	
}

int	ft_builtins_env(t_cmd *cmd, t_env *mini_env)
{
	if (ft_strcmp(cmd->args[0], "export") == 0)
	{
		ft_export(&mini_env, cmd);
			return (1);
	}
	else if (ft_strcmp(cmd->args[0], "unset") == 0)
	{
		ft_unset(&mini_env, cmd);
		return (1);
	}
	else if (ft_strcmp(cmd->args[0], "env") == 0)
	{
		env_cmd(mini_env);
		return (1);
	}
	return (0);
}

int	launch_exec(t_cmd *lst, t_env *mini_env, t_data *data)
{
	t_struct   	var;
    int        	i;
	char	**tab_env;

	(void)data;
	tab_env = ft_list_to_tab(mini_env);
	if (!tab_env)
		return (-1);
	var.pipe_fd[0] = 0;
	var.pipe_fd[1] = 0;
	var.save_pipe = 0;
    i = 0;
    int len_lst = ft_lstlen(lst);
	ft_heredoc(lst, mini_env, data);
	//ft_display_heredoc(lst);
	while (lst)
	{
		i++;
		lst->open = 0;
		if (lst->redirecter)		//2. redirecter check
			ft_redirecter(lst);
		if (lst->next != NULL)		//3. Pipe check ne peut etre fait si 3 ou plus de cmd car il va refaire un pipe et erase lancien alors aue pour 2 cmd il fait qun pipe 
		{
			if (pipe(var.pipe_fd) == -1)
			{
				perror("pipe failed");
				exit(EXIT_FAILURE);
			}
		}
		if (!lst->args[0])
			return(0);
		//cas ou la partie suivante ne doit pas etre faite, heredoc sans cmd, builtings
		if (ft_builtins_env(lst, mini_env) == 0)
		{
			ft_check_access(lst, tab_env);    //4 Cmd check
			if (i == 1 && lst->open == 0)
			{   //5. exec (cmd_first) | cmd_middle... | cmd_last
				ft_first_fork(lst, &var, tab_env);
				if (var.pipe_fd[1] > 3)
					close(var.pipe_fd[1]);// je close lecriture pour pour pas que la lecture attendent indefinement.
				var.save_pipe = var.pipe_fd[0]; //je save la lecture pour le next car je vais re pipe pour avoir un nouveau canal 
			}
			else if (i < len_lst && lst->open == 0)
			{	//6. exec cmd_first | (cmd_middle...) | cmd_last
				ft_middle_fork(lst, &var, tab_env);
				close(var.pipe_fd[1]);
				var.save_pipe = var.pipe_fd[0];
			}
			else if (i == len_lst && lst->open == 0)
			{	//7. exec  exec cmd_first | cmd_middle... | (cmd_last)
				ft_last_fork(lst, &var, tab_env);
				close(var.pipe_fd[0]);
		}
		}
		ft_close(lst);
		lst = lst->next;
	}
	return (0);
}

int	main(int argc, char **argv, char **envp)
{
	int		i;
	t_data	data;

	(void)argc;
	(void)argv;
	i = 0;
	data.exit_code = 0;
	if (minishell_starter(envp, &data) == -1)
		return (printf("Malloc error\n"), -1);
	while (1)
	{
		if (prompt_customer(&data) == 0)
		{
			if (data.var.rl[0] != '\0' && syntaxe_error(&data, data.var.rl) == 0)
			{
				if (parser(&data) == 0)
					if (final_parse(&data) == -1)
						break ;
			}
		}
	}
	rl_clear_history();
}
