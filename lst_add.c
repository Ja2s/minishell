/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   lst_add.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgavairo <jgavairo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/25 16:43:26 by rasamad           #+#    #+#             */
/*   Updated: 2024/04/16 17:37:50 by jgavairo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "libft/libft.h"

t_cmd	*ft_lstlast_minishell(t_cmd *lst)
{
	t_cmd	*tmp;

	if (!lst)
		return (NULL);
	tmp = lst;
	while (tmp->next)
		tmp = tmp->next;
	return (tmp);
}

void	ft_lstadd_back_minishell(t_cmd **lst, t_cmd *new)
{
	if (*lst)
		ft_lstlast_minishell(*lst)->next = new;
	else
		*lst = new;
}

t_cmd	*ft_lstnew_minishell(void)
{
	t_cmd	*new_elem;

	new_elem = (t_cmd *)malloc(sizeof(t_cmd));
	if (!new_elem)
		return (NULL);
	new_elem->nb_args = 0;
	new_elem->nb_del = 0;
	new_elem->nb_flags = 0;
	new_elem->nb_red = 0;
	new_elem->delimiter = NULL;
	new_elem->redirecter = NULL;
	new_elem->args = NULL;
	new_elem->next = NULL;
	new_elem->start = NULL;
	new_elem->heredoc = false;
	return (new_elem);
}
