/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgavairo <jgavairo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/19 11:02:31 by jgavairo          #+#    #+#             */
/*   Updated: 2024/05/03 15:33:52 by jgavairo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "includes/minishell.h"

char	**ft_realloc(char *rl, t_cmd *lst)
{
	size_t		i;
	char	**tab;

	tab = NULL;
	i = 0;
	while (lst->heredoc_content && lst->heredoc_content[i])
		i++;
	if (i == 0)
	{
		tab = malloc(sizeof(char *) * 2);
		tab[i] = ft_strdup(rl);
		tab[i + 1] = NULL;
		return (tab);
	}
	tab = malloc(sizeof(char*) * (i + 2));
	i = 0;
	while (lst->heredoc_content[i])
	{
		tab[i] = ft_strdup(lst->heredoc_content[i]);
		i++;
	}
	tab[i] = ft_strdup(rl);
	tab[i + 1] = NULL;
	return (tab);
}

int ft_heredoc(t_cmd *lst) 
{
    int i = 0;  // Indice du nombre de Heredocs
    char *line;

    lst->heredoc_content = NULL;
	while (lst)
	{
		while (i < lst->nb_del) 
		{
			line = readline(">");
			while (ft_strncmp(line, lst->delimiter[i], ft_strlen(line)) != 0)
			{
				lst->heredoc_content = ft_realloc(line, lst);
				free(line);  // Libère la mémoire allouée par readline
				line = readline(">");
			}
			printf("end heredoc\n");
			i++;
			if (i  < lst->nb_del)
			{
				//ft_free_heredoc(lst)
				lst->heredoc_content = NULL; //delete old heredoc
			}
		}
		i = 0;
		lst = lst->next;
	}

    return 0;
}

void	ft_display_heredoc(t_cmd *lst)
{
	int i;
	int j;

	i = 1;
	j = 0;
	while (lst != NULL)
	{
		printf("\nheredoc %d :\n", i);
		i++;
		while (lst->heredoc_content && lst->heredoc_content[j])
		{
			printf("%d|%s|\n", j, lst->heredoc_content[j]);
			j++;
		}
	j = 0;
	lst = lst->next;
	}
}

int	launch_exec(t_cmd *lst, char **envp, t_env *mini_env)
{
	t_struct   	*var;
    int        	i;
	char	**tab_mini_env;

	tab_mini_env = ft_list_to_tab(mini_env);
	if (!tab_mini_env)
		return (-1);
	var = malloc(sizeof(t_struct));
	var->pipe_fd[0] = 0;
	var->pipe_fd[1] = 0;
	var->save_pipe = 0;
    i = 0;
    int len_lst = ft_lstlen(lst);
    while (lst)
    {
        i++;
        lst->open = 0;
        if (lst->redirecter)		//2. redirecter check
            ft_redirecter(lst);
        if (lst->next != NULL)		//3. Pipe check ne peut etre fait si 3 ou plus de cmd car il va refaire un pipe et erase lancien alors aue pour 2 cmd il fait qun pipe 
        {
            if (pipe(var->pipe_fd) == -1)
			{
                perror("pipe failed");
                exit(EXIT_FAILURE);
            }
        }
		if (lst->heredoc == true)
		{
			ft_heredoc(lst);
			ft_display_heredoc(lst);
		}
		if (!lst->args[0])
			return(0);
		//cas ou la partie suivante ne doit pas etre faite, heredoc sans cmd, builtings
        ft_check_access(lst, envp);    //4 Cmd check
        if (i == 1 && lst->open == 0)
		{   //5. exec (cmd_first) | cmd_middle... | cmd_last
            //printf("go exec first cmd\n\n");
            ft_first_fork(lst, &var, mini_env, tab_mini_env);
			if (var->pipe_fd[1] > 3)
            	close(var->pipe_fd[1]);// je close lecriture pour pour pas que la lecture attendent indefinement.
            var->save_pipe = var->pipe_fd[0]; //je save la lecture pour le next car je vais re pipe pour avoir un nouveau canal 
        }

        else if (i < len_lst && lst->open == 0)
		{	//6. exec cmd_first | (cmd_middle...) | cmd_last
            //printf("go exec middle cmd\n\n");
            ft_middle_fork(lst, &var, tab_mini_env);
            close(var->pipe_fd[1]);
            var->save_pipe = var->pipe_fd[0];
        }

        else if (i == len_lst && lst->open == 0)
		{	//7. exec  exec cmd_first | cmd_middle... | (cmd_last)
            //printf("go exec last cmd\n\n");
            ft_last_fork(lst, &var, tab_mini_env);
            close(var->pipe_fd[0]);
        }
        //ft_free_token(lst);
        ft_close(lst);
        lst = lst->next;
    }
	free(var);
    return (0);
}


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
	dest = malloc(sizeof(char)* (ft_strlen(src) + 1));
	while (src[i])
	{
		if (src[i] == 34 || src[i] == 39)
			i++;
		else
			dest[p++] = src[i++];
	}
	dest[p] = '\0';
	return(dest);
}

char	**input_copyer(char **input, char **input_copy)
{
	int		i;

	i = 0;
	while (input[i])
		i++;
	input_copy = ft_calloc(i + 1, sizeof(char*));
	if (!input_copy)
		return (NULL);
	i = 0;
	while (input[i])
	{
		input_copy[i] = copy_w_cote(input[i], input_copy[i]);
		i++;	
	}
	free_pipes(input);
	return(input_copy);
}

int main(int argc, char **argv, char **envp)
{
	char	*rl;
	char	*pwd;
	char	**input;
	char	**input_copy;
	char	**pipes;
	int		i;
	t_var	*var;
	t_cmd	*cmd = NULL;
	t_cmd	*tmp = NULL;
	t_env	*mini_env = NULL;
	(void)argc;
	(void)argv;
	var = malloc(sizeof(t_var));
	if (!var)
		printf("t_var memory allocation Error\n");
	input = NULL;
	rl = NULL;
	i = 0;
	input_copy = NULL;
	input = NULL;
	printf_title();
	while(1)
	{
		pwd = getcwd(NULL, 0);	//je recupere le chemin d'acces pour l'afficher tel fish
		if (env_copyer(envp, &mini_env) == -1) // je recupere et copie lenvironnement dans un autre tableau
			return (printf("Malloc error\n"), -1);
		printf ("\033[90m%s\033[0m", pwd);	//je l'affiche
		rl = readline("\e[33m$> \e[37m");	//je recupere la ligne en entree dans une boucle infini afin de l'attendre
		if (syntaxe_error(rl) == 0)	//je check les erreurs de syntaxes et lance le programme seulement si tout est OK
		{
			negative_checker(rl);		//je check toutes mes cotes et passe en negatif tout ce qui ne vas pas devoir etre interprete
			rl = dolls_expander(rl, mini_env);
			//printf("after expand = %s\n", rl);
			pipes = ft_split(rl, '|');	//je separe chaque commande grace au pipe
			add_history(rl);
			free(rl);
			while (pipes[i])	//chaque commande est rangee par ligne dans un tableau afin de les traiter une a une 
			{
				tmp = ft_lstnew_minishell(); //jinitialise ma structure car nous allons en avoir besoin
				if (!tmp)
					break;
				if (heredoc_checker(pipes[i], &tmp) == -1)
					printf("Heredoc memory error\n");
				redirecter(pipes[i], &tmp);	//je vais chercher toutes les redirects afin de les ranger dans une variable a part
				pipes[i] = redirect_deleter(pipes[i]);	//je vai supprimer toutes les redirects de la ligne principale afin de ne plus les prendre en compte
				input = ft_split(pipes[i], ' ');	//il me reste donc plus que la commande et les arguments separes d'espaces, je les separe donc et les range dans input
				input_copy = input_copyer(input, input_copy);
				if (stock_input(input_copy, &tmp) == 0)	//je vais recuperer le nom de la commande et les arguments dans input et les ranger dans la structure tmp
				{
					i++;	// la commande a ete traitee, je passe a la prochaine (si il y en a une)
					ft_lstadd_back_minishell(&cmd, tmp);	//j'ajoute le noeud tmp ou tout a ete ranger a ma liste chainee 
				}
				free(input_copy);
			}
			i = 0;
			while(cmd->args[i])
			{
				command_positiver(cmd->args[i]);
				i++;
			}
			i = 0;
			//ft_printf_struct(cmd); //PRINT STRUCTURE --------------------------
			//start = cmd;
			//printf("\033[38;5;220mLancement de Launch_exec...\033[0m\n"); LANCEMENT LAUNCH EXEC-----------
			if (launch_exec(cmd, envp, mini_env) == -1)
				printf ("Error exec\n");
			ft_lstclear(&cmd);
			free_pipes(pipes);
			free(pwd);
		}
	}
	rl_clear_history();
}
