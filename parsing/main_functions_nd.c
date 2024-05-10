/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main_functions_nd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: gavairon <gavairon@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/08 12:22:23 by gavairon          #+#    #+#             */
/*   Updated: 2024/05/08 15:57:55 by gavairon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	command_positiver(char *pipes)
{
	size_t	i;

	i = 0;
	while (pipes[i])
	{
		if (pipes[i] < 0)
			pipes[i] = pipes[i] * -1;
		i++;
	}
}

char	*copy_w_cote(char *src, char *dest)
{
	int	i;
	int	p;

	p = 0;
	i = 0;
	dest = malloc(sizeof (char) * (ft_strlen(src) + 1));
	while (src[i])
	{
		if (src[i] == 34 || src[i] == 39)
			i++;
		else
			dest[p++] = src[i++];
	}
	dest[p] = '\0';
	return (dest);
}

char	**input_copyer(char **input, char **input_copy)
{
	int	i;

	i = 0;
	while (input[i])
		i++;
	input_copy = ft_calloc(i + 1, sizeof (char *));
	if (!input_copy)
		return (NULL);
	i = 0;
	while (input[i])
	{
		input_copy[i] = copy_w_cote(input[i], input_copy[i]);
		i++;
	}
	free_pipes(input);
	return (input_copy);
}

void	data_initializer(t_data *data)
{
	(*data).var.input = NULL;
	(*data).var.rl = NULL;
	(*data).mini_env = NULL;
	(*data).var.input_copy = NULL;
	(*data).var.input = NULL;
	(*data).cmd = NULL;
}

int	final_parse(t_data *data)
{
	int	i;

	i = 0;
	if (node_creator(data) == -1)
		return (printf("Error from the function \"node_creator\n\""), -1);
	while (data->cmd->args[i])
		command_positiver(data->cmd->args[i++]);
	if (launch_exec(data->cmd, data->mini_env) == -1)
		return (printf("Error exec\n"), -1);
	ft_lstclear(&data->cmd);
	free_pipes(data->var.pipes);
	free(data->var.pwd);
	return (0);
}
