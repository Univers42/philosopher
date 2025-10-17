/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_error.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: miaviles <miaviles@student.42madrid>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/11/26 18:07:17 by miaviles          #+#    #+#             */
/*   Updated: 2025/03/01 15:54:11 by miaviles         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	ft_error(char *str, t_bool e)
{
	int	len;

	len = ft_strlen(str);
	ft_printf("%s\n", str);
	if (e)
		exit(1);
}
