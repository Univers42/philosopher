/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signal_j.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/28 12:28:20 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/21 11:13:03 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../inc/minishell_m.h"
#include "../inc/minishell_j.h"
#include <termios.h>

volatile sig_atomic_t	g_signal_received = 0;

void	heredoc_sigint_handler(int sig)
{
	char	enter;

	enter = '\n';
	(void)sig;
	g_signal_received = 1;
	write(STDOUT_FILENO, ">", 1);
	ioctl(STDIN_FILENO, TIOCSTI, &enter);
	rl_replace_line("", 0);
	rl_on_new_line();
}

void	disable_echoctl(void)
{
	struct termios	term;

	tcgetattr(STDIN_FILENO, &term);
	term.c_lflag &= ~ECHOCTL;
	tcsetattr(STDIN_FILENO, TCSANOW, &term);
}

void	ctrlc_handler(int sig)
{
	(void)sig;
	g_signal_received = SIGINT;
	write(STDOUT_FILENO, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
}

/*SIGINT is signal of interrupt so Control+C*/
/*SA_RESTART is squidline red but compiles. It is because visualStudio*/
/*signal is used instead of sigaction to IGNORE the ctrl+\ That way mimic...*/
/*...the BASH behaviour*/
void	init_sigaction(struct sigaction *sa)
{
	sa->sa_handler = ctrlc_handler;
	sigemptyset(&sa->sa_mask);
	sa->sa_flags = SA_RESTART;
	sigaction(SIGINT, sa, NULL);
	signal(SIGQUIT, SIG_IGN);
}
