/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_functions.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rasamad <rasamad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 12:31:40 by gavairon          #+#    #+#             */
/*   Updated: 2024/05/10 12:34:11 by rasamad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

int	minishell_starter(char **env, t_data *data)
{
	data_initializer(data);
	printf_title();
	if (env_copyer(env, &data->mini_env) == -1)
		return (printf("Malloc error\n"), -1);
	return (0);
}

int	prompt_customer(t_data *data)
{
	data->var.pwd = getcwd(NULL, 0);
	if (data->var.pwd == NULL)
		return (-1);
	printf ("\033[90m%s\033[0m", data->var.pwd);
	data->var.rl = readline("\e[33m$> \e[37m");
	if (data->var.rl == NULL)
		return (-1);
	return (0);
}

int	parser(t_data *data)
{
	negative_checker(data->var.rl);
	data->var.rl = dolls_expander(data->var.rl, data->mini_env);
	if (!data->var.rl)
		return (printf("Error malloc the (char *rl)"), -1);
	data->var.pipes = ft_split(data->var.rl, '|');
	if (!data->var.pipes)
		return (printf("Error malloc the (char** pipes)"), -1);
	add_history(data->var.rl);
	free(data->var.rl);
	return (0);
}

int	node_precreator(t_data *data, int i)
{
	data->tmp = ft_lstnew_minishell();
	if (!data->tmp)
		return (printf("Error malloc the (t_cmd tmp)"), -1);
	if (heredoc_checker(data->var.pipes[i], &data->tmp) == -1)
		return (printf("Heredoc memory error\n"), -1);
	if (redirecter(data->var.pipes[i], &data->tmp) == -1)
		return (printf("Redirecter memory error\n"), -1);
	data->var.pipes[i] = redirect_deleter(data->var.pipes[i]);
	if (!data->var.pipes[i])
		return (printf("Pipes memory error\n"), -1);
	data->var.input = ft_split(data->var.pipes[i], ' ');
	if (data->var.input == NULL)
		return (printf("Input memory error\n"), -1);
	data->var.input_copy = input_copyer(data->var.input, data->var.input_copy);
	if (data->var.input_copy == NULL)
		return (printf("Input_copy memory error\n"), -1);
	return (0);
}

int	node_creator(t_data *data)
{
	int	i;

	i = 0;
	while (data->var.pipes[i])
	{
		if (node_precreator(data, i) == 0)
		{
			if (stock_input(data->var.input_copy, &data->tmp) == 0)
			{
				i++;
				ft_lstadd_back_minishell(&data->cmd, data->tmp);
			}
			free(data->var.input_copy);
		}
		else
			return (-1);
	}
	return (0);
}
