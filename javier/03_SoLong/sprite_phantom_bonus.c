/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   sprite_phantom_bonus.c                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jrollon- <jrollon-@student.42madrid.com    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/22 13:28:39 by jrollon-          #+#    #+#             */
/*   Updated: 2025/03/25 13:20:13 by jrollon-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "so_long_bonus.h"

void	check_ghost_blue(t_game *game, size_t i, size_t j)
{
	if (game->map->map[i][j] == 'B')
	{
		game->win->sprite[16].x = j * SPRITE_WIDTH;
		game->win->sprite[16].y = i * SPRITE_HEIGHT;
	}
}

void	position_phantom(t_game *game)
{
	size_t	i;
	size_t	j;

	i = 0;
	while (i < game->map->lines)
	{
		j = 0;
		while (j < game->map->columns)
		{
			if (game->map->map[i][j] == 'G')
			{
				game->win->sprite[14].x = j * SPRITE_WIDTH;
				game->win->sprite[14].y = i * SPRITE_HEIGHT;
			}
			if (game->map->map[i][j] == 'R')
			{
				game->win->sprite[15].x = j * SPRITE_WIDTH;
				game->win->sprite[15].y = i * SPRITE_HEIGHT;
			}
			check_ghost_blue(game, i, j);
			j++;
		}
		i++;
	}
}

void	load_phantom_sprite(t_game *g)
{
	g->win->sprite[14].img[0].img = mlx_xpm_file_to_image(g->win->mlx,
			g->win->sprite[14].bitmap[0], &g->win->sprite[14].width,
			&g->win->sprite[14].height);
	g->win->sprite[15].img[0].img = mlx_xpm_file_to_image(g->win->mlx,
			g->win->sprite[15].bitmap[0], &g->win->sprite[15].width,
			&g->win->sprite[15].height);
	g->win->sprite[16].img[0].img = mlx_xpm_file_to_image(g->win->mlx,
			g->win->sprite[16].bitmap[0], &g->win->sprite[16].width,
			&g->win->sprite[16].height);
	if (!g->win->sprite[14].img[0].img || !g->win->sprite[15].img[0].img
		|| !g->win->sprite[16].img[0].img)
	{
		clean_up_memory(g);
		exit (1);
	}
}

void	load_phantom(t_game *game)
{
	game->win->sprite[14].bitmap[0] = "./textures/g_phantom_r_00.xpm";
	game->win->sprite[15].bitmap[0] = "./textures/g_phantom_l_00.xpm";
	game->win->sprite[16].bitmap[0] = "./textures/b_phantom_00.xpm";
	game->win->sprite[14].width = 64;
	game->win->sprite[14].height = 64;
	game->win->sprite[15].width = 64;
	game->win->sprite[15].height = 64;
	game->win->sprite[16].width = 64;
	game->win->sprite[16].height = 64;
	position_phantom(game);
	load_phantom_sprite(game);
}
