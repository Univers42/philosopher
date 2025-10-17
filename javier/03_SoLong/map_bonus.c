/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   map_bonus.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 14:02:22 by jrollon-          #+#    #+#             */
/*   Updated: 2025/03/25 11:32:14 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_bonus.h"
#include "get_next_line.h"

void	check_internal_lines(char *line, t_map *map, size_t columns, size_t ln)
{
	size_t	i;

	i = 0;
	while (line[i])
	{
		if (i == 0 && line[0] == '1' && ft_strlen(line) == columns)
			map->num_walls++;
		if (i == (columns - 1) && line[columns - 1] == '1'
			&& ft_strlen(line) == columns)
			map->num_walls++;
		if (line[i] == 'C')
			map->num_c++;
		if (line[i] == 'P')
		{
			map->num_p++;
			map->p_x = i * SPRITE_WIDTH;
			map->p_y = (ln - 1) * SPRITE_HEIGHT;
		}
		if (line[i] == 'E')
			map->num_e++;
		if (line[i] == 'G' || line[i] == 'R' || line[i] == 'B')
			map->num_g++;
		i++;
	}
}

void	check_line(char *line, char *next_line, t_map *map, size_t columns)
{
	size_t	i;

	i = 0;
	map->lines++;
	if (ft_strlen(line) != columns)
		map->no_rectangle = 1;
	while (line[i])
	{
		if (!ft_strchr(VALID_MAP_CHARS, line[i]))
			map->no_valid_char = 1;
		i++;
	}
	i = 0;
	if (map->lines == 1 || next_line == NULL)
	{
		while (line[i])
		{
			if (line[i] == '1')
				map->num_walls++;
			i++;
		}
		return ;
	}
	check_internal_lines(line, map, columns, map->lines);
}

void	check_map(t_map *map, char *map_dir)
{
	char	*line;
	char	*next_line;
	int		fd;

	fd = open(map_dir, O_RDONLY);
	if (fd == -1)
	{
		write(1, "Error\n", 6);
		write(1, "Error loading map\n", 18);
		return ;
	}
	next_line = NULL;
	line = get_next_line(fd);
	while (line)
	{
		next_line = get_next_line(fd);
		if (map->lines == 0)
			map->columns = ft_strlen(line);
		check_line(line, next_line, map, map->columns);
		free(line);
		line = next_line;
	}
	close (fd);
}

/*if we have an rectangled map surrounded by all walls, we have to have a...*/
/*...minimum of 2 top & botton lines all walls (2 * number of columns) ...*/
/*... plus each internal rows one wall in extremes (2 * (lines - 2))...*/
/*... -2 because we don't count again the top and botton lines. If we have...*/
/*...more or less than that because I didn't count the internal walls...*/
/*...then the map is not closed.*/
int	check_map_errors(t_map *map)
{
	int	error;
	int	closed_walls;

	error = 0;
	closed_walls = map->columns * 2 + (map->lines - 2) * 2;
	if (closed_walls != map->num_walls || map->no_rectangle || map->num_c < 1
		|| map->num_e != 1 || map->num_p != 1 || map->no_valid_char == 1)
	{
		write(1, "Error\n", 6);
		error = 1;
	}
	if (closed_walls != map->num_walls)
		write(1, "Map not fully closed by walls\n", 30);
	if (map->no_rectangle)
		write(1, "Not a rectangular map\n", 22);
	if (map->num_c < 1)
		write(1, "There are no collectibles\n", 26);
	if (map->num_e != 1)
		write(1, "There is no exit, or more than one\n", 35);
	if (map->num_p != 1)
		write(1, "There is no char start position, or more than one\n", 50);
	if (map->no_valid_char)
		write(1, "There is at least one invalid char in the map\n", 46);
	return (error);
}

/*map->lines == 0 in if, is because fd = -1 in open file*/
t_map	*process_map(char *map_dir)
{
	t_map	*map;
	size_t	i;

	i = 0;
	map = (t_map *)ft_calloc(1, sizeof(t_map));
	if (!map)
		return (NULL);
	check_map(map, map_dir);
	if (check_map_errors(map) || map->lines == 0)
		return (free(map), NULL);
	load_map(map, map_dir);
	if (!map->map)
		return (free(map), NULL);
	if (!check_map_solution(map))
	{
		write(1, "Error\nMap without solution\n", 27);
		while (i < map->lines)
		{
			free(map->map[i]);
			map->map[i++] = NULL;
		}
		return (free(map->map), free(map), NULL);
	}
	return (map);
}
