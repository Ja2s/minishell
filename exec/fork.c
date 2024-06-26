/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: rasamad <rasamad@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/29 15:12:43 by rasamad           #+#    #+#             */
/*   Updated: 2024/06/12 13:08:49 by rasamad          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../includes/minishell.h"

void	handle_sigint_fork(int sig)
{
	(void)sig;
	printf("\n");
	g_sig = 1;
}

void	handle_sigquit_fork(int sig)
{
	(void)sig;
	g_sig = 2;
}

void	handle_sigint_heredoc(int sig)
{
	(void)sig;
	g_sig = 1;
	printf("^C");
	rl_done = 1;
}

int	rl_hook_function(void)
{
	signal(SIGINT, handle_sigint_heredoc);
	return (0);
}
