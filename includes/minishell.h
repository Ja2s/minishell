/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgavairo <jgavairo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/04/29 14:07:18 by jgavairo          #+#    #+#             */
/*   Updated: 2024/04/29 14:07:21 by jgavairo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdlib.h>
# include <string.h>
# include <readline/readline.h>
# include <readline/history.h>
# include "../libft/libft.h"
# include <fcntl.h>
# include <stdio.h>
# include <unistd.h>
# include <errno.h>
# include <sys/wait.h>
# include <stdbool.h>

typedef struct s_expand
{
	int		name_start;
	int		name_end;
	int		name_len;
	char	*name;
	int		value_len;
	char	*value;
}			t_expand;

typedef struct s_var
{
	char	*rl;
	char	*pwd;
	char	**input;
	char	**mini_env;
	int		len_env;
	int		i;
}			t_var;

typedef struct s_struct
{
	int		pipe_fd[2];
	int 	save_pipe;
}    		t_struct; 

typedef struct s_cmd
{
	int				nb_flags;
	int				nb_args;
	int				nb_red;
	int				nb_del;
	
	int				heredoc;
	int				end_heredoc;
	char			**heredoc_content;
	char			**delimiter;
	char			**redirecter;
	char			**args;
	struct s_cmd	*next;
	
	struct s_cmd	*start;
	int				open;
	int				fd_infile;
	int				fd_outfile;
	char			**split_path;
	char			*slash_cmd;
	char			*path_cmd;
	int				i;
}					t_cmd;

t_cmd	*ft_lstnew_minishell(void);
t_cmd	*ft_lstlast_minishell(t_cmd *lst);
void    ft_lstadd_back_minishell(t_cmd **lst, t_cmd *new);
int		ft_lstlen(t_cmd *elem);
void    ft_redirecter(t_cmd *elem);
int		ft_check_access(t_cmd *elem, char **envp);
int		ft_first_fork(t_cmd *elem, t_struct **var, char **envp);
int		ft_middle_fork(t_cmd *elem, t_struct **var, char **envp);
int		ft_last_fork(t_cmd *elem, t_struct **var, char **envp);
void    display_error_cmd(t_cmd *elem);
void    display_no_such(t_cmd *elem);
void    ft_free_access(t_cmd *elem);
void    ft_free_token(t_cmd *elem);
void    ft_free_lst(t_cmd *lst);
void    ft_close(t_cmd *elem);
int		rafters_checker(char *rl);
int		double_pipe_checker(char *rl);
int		cote_checker(char *rl);
int		syntaxe_error(char	*rl);
t_cmd	*ft_lstlast(t_cmd *lst);
void	args_free(t_cmd *lst);
void	red_free(t_cmd *lst);
void	del_free(t_cmd *lst);
void	split_path_free(t_cmd *lst);
void	path_cmd_free(t_cmd *lst);
void	slash_cmd_free(t_cmd *lst);
void	ft_lstdelone(t_cmd *lst);
void	ft_lstclear(t_cmd **lst);
int		ft_lstlen(t_cmd *elem);
void	command_stocker(char **input, t_cmd **cmd);
int		args_memory_alloc(char **input, t_cmd **cmd);
int		memory_alloc(char **input, t_cmd **cmd);
int		stock_input(char **input, t_cmd **cmd);
void	negative_checker(char *rl);
void	expand_initializer(t_expand **var);
char	*dolls_expander(char *rl);
int		redirect_counter(char *pipes);
int		redirecter(char *pipes, t_cmd **cmd);
int		len_calculator(char	*pipes);
char	*redirect_deleter(char	*pipes);
int		ft_printf_struct(t_cmd *cmd);
void	printf_title();
int		pipes_counter(char *rl);
char	**env_copyer(char **envp, t_var *var);
int		heredoc_counter(char *pipes);
int		heredoc_memory_allocer(char *pipes, t_cmd **cmd);
int		heredoc_copyer(char *pipes, t_cmd **cmd, int *i, int del);
int		heredoc_checker(char *pipes, t_cmd **cmd);
void	free_pipes(char **pipes);

#endif