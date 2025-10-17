/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_6.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/13 15:46:31 by jrollon-          #+#    #+#             */
/*   Updated: 2025/09/30 16:04:14 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"
#include "get_next_line.h"

/*
** makes all the map squared
*/
static void	remove_newline_and_truncate(char **row)
{
	size_t	len;

	len = ft_strlen(*row);
	if (len > 0 && (*row)[len - 1] == '\n')
		(*row)[len - 1] = '\0';
}

static void	pad_row_with_zeros(char **row, size_t columns)
{
	size_t	len;
	size_t	l;
	char	*zero_buf;
	char	*tmp;
	size_t	i;

	i = -1;
	len = ft_strlen(*row);
	if (len < columns)
	{
		l = columns - len;
		zero_buf = malloc(l + 1);
		if (!zero_buf)
			exit(EXIT_FAILURE);
		while (++i < l)
			zero_buf[i] = 'o';
		zero_buf[l] = '\0';
		tmp = *row;
		*row = ft_strjoin(tmp, zero_buf);
		free(tmp);
		free(zero_buf);
		if (!*row)
			exit(EXIT_FAILURE);
	}
}

static void	add_newline_end(char **row)
{
	size_t	newlen;
	char	*newline_buf;

	newlen = ft_strlen(*row);
	newline_buf = malloc(newlen + 2);
	if (!newline_buf)
		exit(EXIT_FAILURE);
	ft_strcpy(newline_buf, *row);
	newline_buf[newlen] = '\n';
	newline_buf[newlen + 1] = '\0';
	free(*row);
	*row = newline_buf;
}

void	squarify_map(size_t size, t_map *map)
{
	size_t	j;

	j = 0;
	while (j < size)
	{
		remove_newline_and_truncate(&map->map[j]);
		pad_row_with_zeros(&map->map[j], map->columns);
		add_newline_end(&map->map[j]);
		j++;
	}
}

void	check_internal_lines(char *line, t_map *map, size_t columns, size_t ln)
{
	size_t	i;

	(void)columns;
	i = 0;
	while (line[i])
	{
		if (line[i] == 'N' || line[i] == 'S' || line[i] == 'E'
			|| line[i] == 'W')
		{
			map->num_p++;
			map->p_x = i + 0.5;
			map->p_y = (ln - 1) + 0.5;
			init_looking_direction(map, line[i]);
		}
		i++;
	}
}
