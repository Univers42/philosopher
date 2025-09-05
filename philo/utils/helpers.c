/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   helpers.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: dlesieur <dlesieur@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/05 12:47:06 by dlesieur          #+#    #+#             */
/*   Updated: 2025/09/05 12:51:28 by dlesieur         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

int conv_pos_int(char *str)
{
	int result;

	result = 0;
	while (*str == 32 || (*str >= 9 && *str <= 13))
		str++;
	for (int i = 0; str[i]; i++)
	{
		result = result * 10 + (str[i] - '0');
	}
	return result;
}

void    argcheck(int argc, char **argv, int *values)
{
	if (argc != 5 && argc != 6)
	{
		printf("Error: Invalid number of arguments.\n");
		exit(1);
	}
	for (int i = 1; i < argc; i++)
	{
		for (int j = 0; argv[i][j]; j++)
		{
			if (argv[i][j] < '0' || argv[i][j] > '9')
			{
				printf("Error: Arguments must be positive integers.\n");
				exit(1);
			}
		}
		*values++ = conv_pos_int(argv[i]);
	}
	if (!values[4])
		values[4] = -1;
}