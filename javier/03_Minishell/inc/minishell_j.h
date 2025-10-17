/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell_j.h                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/23 11:30:16 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/28 16:50:25 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_J_H
# define MINISHELL_J_H

# include <signal.h>
# include <stdio.h>
# include "../ft_printf/ft_printf.h"
# include "../inc/get_next_line.h"
# include <stdbool.h>
# include <sys/ioctl.h>

# define EPTY_NSP 0
# define EPTY_SP 1
# define SQUO_NSP 10
# define SQUO_SP 11
# define DQUO_NSP 20
# define DQUO_SP 21
# define D_Y_ODDCHAR "~%^=+}]:;/.,·"
# define N_ODDCHAR "!@*-#`(){["
# define BUFFER 100

extern volatile sig_atomic_t	g_signal_received;

typedef struct s_input
{
	char	*input;
	char	*parsed;
	char	*filename;
	char	**input_split;
	char	**split_exp;
	char	**envp;
	char	**local_envp;
	int		*status;
	char	*token;
	char	*command;
	char	*args;
	int		status_exp[100];
	int		from_expand;
	size_t	realloc_counter;
	size_t	word_after_command;
	size_t	word_after_arg;
	size_t	input_words;
	size_t	word;
	size_t	dollars;
	size_t	idollar;
	size_t	env_len;
	int		echo_error_n_arg;
	int		spaced;
	int		inputfd;
	int		outputfd;
	int		status_checked;
	int		env_n;
	char	*last_dollar_;
	pid_t	last_exit_code;
	bool	is_script;
	int		total_pipes;
	int		total_redirections;
	int		cmd;
	int		cmd_start;
	int		cmd_end;
	int		num_cmds;
}			t_input;

//CHECK INPUT
void	ft_echo(t_input *in, int active);
void	compose_command_args(t_input *in);
void	compose_arg(t_input *in, size_t word);
void	parsing(t_input *in);
char	*get_next_line(int fd);
void	expand_dollar(t_input *in, size_t *i, size_t *j, size_t *k);
void	save_rest_no_env(t_input *in, size_t w, size_t *i, size_t *k);
void	save_rare_cases(t_input *in, size_t w, size_t *i, size_t *k);
void	save_valid_env_variable(t_input *n, size_t w, size_t *i, size_t *k);
void	expand_token_dollar(t_input *in, size_t *i, size_t *j, size_t *k);
void	token_valid_env_variable(t_input *n, size_t w, size_t *i, size_t *k);
void	token_rest_no_env(t_input *in, size_t w, size_t *i, size_t *k);
void	token_rare_cases(t_input *in, size_t w, size_t *i, size_t *k);
void	compose_token(t_input *in);
void	dynamic_input(t_input *in, size_t k);
void	dynamic_command(t_input *in, size_t k);
void	dynamic_arg(t_input *in, size_t k);
void	remove_control_char(char *string);
void	do_even_dollars2(t_input *in, size_t w, size_t *i, size_t *k);
void	env_question2(t_input *in, size_t w, size_t *i, size_t *k);
void	input_check_prev_dollar(t_input *in, size_t *i, size_t *j, size_t *k);
void	save_env_if_even_dollars2(t_input *in, size_t w, size_t *i, size_t *k);
void	save_rare_cases2(t_input *in, size_t *k, char *str, size_t id);
void	do_even_token_dollars2(t_input *in, size_t w, size_t *i, size_t *k);
void	token_env_question2(t_input *in, size_t w, size_t *i, size_t *k);
void	token_env_if_even_dollars2(t_input *in, size_t w, size_t *i, size_t *k);
void	token_rare_cases2(t_input *in, size_t *k, char *str, size_t id);
void	compose_command(t_input *in);
void	quit_parsing(t_input *in, int stdout_save);

//BUILT INS
void	echo_short(t_input *in, int fd);
void	manage_dollar(t_input *in, size_t w, int spaced);
int		valid_env(const char *str, t_input *in, size_t w);
void	ft_exit(t_input *in);
size_t	validlen_env(const char *str, char c);
size_t	invalidlen_env(const char *str);
void	print_rare_cases(t_input *in, size_t w, size_t *i);
void	print_rest_no_env(t_input *in, size_t w, size_t *i);
void	print_if_spaced_and_valid_env(t_input *in, size_t w, int spaced);
void	print_valid_env_variable(t_input *n, size_t w, size_t *i);
void	space_after_first_invalid_env(t_input *in, size_t w, size_t i, int on);
int		is_valid_arg(char *str);
size_t	check_more_n(t_input *in);
void	print_heredoc(char *line, int pipefd);

//SIGNALS
void	init_sigaction(struct sigaction *sa);
void	ctrlc_handler(int sig);
void	heredoc_sigint_handler(int sig);
void	disable_echoctl(void);
void	heredoc_signals(struct sigaction *sa, struct sigaction *sa_old_int,
			struct sigaction *sa_old_quit);
void	restore_signals(struct sigaction *sa_old_int,
			struct sigaction *sa_old_quit);
void	one_hundred_thirty(t_input *input);
void	one_hundred_thirtyone(t_input *input);

#endif
