/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgavairo <jgavairo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/05/02 15:32:01 by jgavairo          #+#    #+#             */
/*   Updated: 2024/05/22 16:44:59 by jgavairo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	env_cmd(t_env *env)
{
	t_env	*mini_env;

	mini_env = env;
	while (mini_env)
	{
		if (mini_env->value)
			printf("%s=%s\n", mini_env->name, mini_env->value);
		mini_env = mini_env->next;
	}
}

char	*ft_getenv(char *name, t_env *mini_env)
{
	t_env	*env;

	env = mini_env;
	while (env)
	{
		if (ft_strcmp(name, env->name) == 0)
			return (env->value);
		env = env->next;
	}
	return (NULL);
}

int ft_copy_env(t_env **copy, t_env *mini_env)
{
	t_env	*new_elem;
	t_env	*tmp;
	
	tmp = NULL;
	tmp = mini_env;
	while (tmp)
	{
		new_elem = NULL;
		new_elem = malloc(sizeof (t_env));
		if (!new_elem)
			return (-1);
		new_elem->name = ft_strdup(tmp->name);
		if (!new_elem->name)
			return (-1);
		new_elem->value = ft_strdup(tmp->value);
		if(!new_elem->value)
			return (-1);
		ft_envadd_back(copy, new_elem);
		tmp = tmp->next;
	}
	return (0);
}

void	sort_env(char ***tab)
{
	int		i;
	int		p;
	char	*tmp;

	i = 0;
	while ((*tab)[i + 1])
	{
		p = 0;
		if ((*tab)[i][p] > (*tab)[i + 1][p])
		{
			tmp = (*tab)[i + 1];
			(*tab)[i + 1] = (*tab)[i];
			(*tab)[i] = tmp;
			i = 0;
		}
		else if ((*tab)[i][p] == (*tab)[i + 1][p])
		{
			while ((*tab)[i][p] && (*tab)[i + 1][p] && (*tab)[i][p] == (*tab)[i + 1][p])
				p++;
			if ((*tab)[i][p] > (*tab)[i + 1][p])
			{
				tmp = (*tab)[i + 1];
				(*tab)[i + 1] = (*tab)[i];
				(*tab)[i] = tmp;
				i = 0;
			}
			else
				i++;
		}
		else
			i++;
	}
}


int	ft_export_display(t_env *mini_env)
{
	int		i;
	char	**tab;

	i = 0;
	tab = ft_list_to_tab_cote(mini_env);
	if (!tab)
		return (-1);
	sort_env(&tab);
	while(tab[i])
		printf("declare -x %s\n", tab[i++]);
	free_pipes(tab);
	return (0);
}

int	check_variable(t_env **mini_env, char *name, char *value)
{
	t_env *tmp;

	tmp = (*mini_env);
	while (tmp)
	{
		if (ft_strcmp(tmp->name, name) == 0)
		{
			if (value)
				tmp->value = ft_strdup(value);
			else
				tmp->value = NULL;
			return(1);
		}
		tmp = tmp->next;
	}
	return (0);
}

int	ft_isalpha_export(int c)
{
	if (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')) ||\
	c == '_' )
		return (1);
	else
		return (0);
}

int	spec_export(char *cmd)
{
	int i;

	i = 0;
	if (!cmd[0])
		return (1);
	if (cmd[0] >= '1' && cmd[0] <= '9')
		return (1);
	if (cmd[0] == '=')
		return (1);
	while (cmd[i])
	{
		if (i > 0 && cmd[i] == '=')
			return(0);
		if (ft_isalpha_export(cmd[i]) == 0 && ft_isdigit(cmd[i]) == 0)
			return(1);
		i++;
	}
	return (0);
}

int	ft_export(t_data *data, t_env **mini_env, t_cmd *cmd)
{
	t_env	*new_elem;
	int		i;
	int		x;
	char	**variable;
	
	if (!cmd->args[1])
		ft_export_display(*mini_env);
	else
	{	
		i = 1;
		while (cmd->args[i])
		{
			if (spec_export(cmd->args[i]) == 0)
			{
				variable = NULL;
				variable = ft_split(cmd->args[i], '=');
				if (!variable || !variable[0])
					return (free(new_elem), -1);
				if (check_variable(mini_env, variable[0], variable[i]) == 0)
				{
					new_elem = malloc(sizeof(t_env));
					if (!new_elem)
						return (-1);
					new_elem->name = ft_strdup(variable[0]);
					if (variable[1])
						new_elem->value = ft_strdup(variable[1]);
					x = 2;
					while (variable[x])
					{
						new_elem->value = ft_strjoin(new_elem->value, "=");
						new_elem->value = ft_strjoin(new_elem->value, variable[x]);
					}
					ft_envadd_back(mini_env, new_elem);
				}
				free(variable[0]);
				free(variable[1]);
				free(variable);
			}
			else
			{
				exit_status_n_free(data, 1, "export: not a valid identifier: ");
				write(2, cmd->args[i], ft_strlen(cmd->args[i]));
				write(2,"\n", 1);
			}
			i++;
		}
	}
	return (0);
}

int	ft_builtins(t_cmd *lst)
{
	int		i;
	char	*cwd;

	i = 1;
	if (ft_strcmp(lst->args[0], "pwd") == 0)
	{
		cwd = getcwd(NULL, 0);
		if (!cwd)
		{
			perror("getcwd");
			exit(EXIT_FAILURE);
		}
		printf("%s\n", cwd);
		free(cwd);
		return (1);
	}
	else if (ft_strcmp(lst->args[0], "echo") == 0)
	{
		while (lst->args[i] && ft_strncmp(lst->args[i], "-n", 2) == 0)
			i++;
		while (lst->args[i])
		{
			printf("%s", lst->args[i]);
			if (lst->args[++i])
				printf(" ");// Ajoute un espace entre les arguments
		}
		if (lst->args[1] && ft_strncmp(lst->args[1], "-n", 2) != 0)
			printf("\n");
		else if (!lst->args[1])
			printf("\n");
		return (1);
	}
	return (0);
}
