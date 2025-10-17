/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split_quotes_utils.c                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/03 17:06:36 by jrollon-          #+#    #+#             */
/*   Updated: 2025/05/07 13:32:51 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"
#include "../inc/minishell_j.h"

int	is_quoted(t_input *input, int word)
{
	if (input->status[word] == SQUO_NSP || input->status[word] == SQUO_SP)
		return (1);
	if (input->status[word] == DQUO_NSP || input->status[word] == DQUO_SP)
		return (2);
	return (0);
}

/*2 types inside quotes or not. As it starts on beggining of word...*/
/*...I just go back up to 2 chars to see if there is an space.*/
int	is_spaced(t_split *sq, size_t i)
{
	if (i > 0)
	{
		if (sq->s[i - 1] == ' ')
			return (1);
		if (sq->quotes % 2)
		{
			if (sq->s[i - 1] == '"' || sq->s[i - 1] == '\'')
				i--;
			if (i > 0 && sq->s[i - 1] == ' ')
				return (1);
			return (0);
		}
	}
	return (0);
}
