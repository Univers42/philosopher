/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   raycast_2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/04 09:05:40 by jrollon-          #+#    #+#             */
/*   Updated: 2025/09/24 15:26:37 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

/*step_x/y indicate direction the ray goes seen from up-down map (N-S-E-W)
if ray.dir_x > 0 -> step_x = 1 (goes to the EAST)
ray.dir_x < 0 -> step_x = -1 (goes WEST);
ray.dir_y > 0 -> step_y = 1 (goes SOUTH);
ray.dir_Y < 0 -> step_Y = -1 (goes NORTH);
we could use then same ray.dir_x/y, BUT step_x/y are INTs so we remove the
floating point in every 'step' that the ray make in same direction that will
run same delta_dist_x/y 

RAY.dist_x/y is the distance of the full ray already, BUT the first time is not
the delta one, it is shorter.  */
void	set_direction_of_ray(t_game *game)
{
	t_ray	*ray;

	ray = &game->win->ray;
	if (ray->dir_x < 0)
	{
		ray->step_x = -1;
		ray->dist_x = (game->map->p_x - ray->map_x) * ray->delta_dist_x;
	}
	else
	{
		ray->step_x = 1;
		ray->dist_x = (ray->map_x + 1.0 - game->map->p_x) * ray->delta_dist_x;
	}
	if (ray->dir_y < 0)
	{
		ray->step_y = -1;
		ray->dist_y = (game->map->p_y - ray->map_y) * ray->delta_dist_y;
	}
	else
	{
		ray->step_y = 1;
		ray->dist_y = (ray->map_y + 1.0 - game->map->p_y) * ray->delta_dist_y;
	}
}

/*modify the color with distance. More distance darker*/
/*LIGHT default is 2. Higher number brighter will be*/
/*to better performance instead of make float operations we make to int*/
int	modify_color_with_distance(t_game *game, int color)
{
	int		factor;
	int		red;
	int		green;
	int		blue;

	if (game->win->ray.perp_wall_dist < 1e-6f)
		game->win->ray.perp_wall_dist = 1e-6f;
	factor = (int)(LIGHT / game->win->ray.perp_wall_dist * 256.0f);
	if (factor > 256)
		factor = 256;
	red = ((color >> 16) & 0xFF) * factor >> 8;
	green = ((color >> 8) & 0xFF) * factor >> 8;
	blue = (color & 0xFF) * factor >> 8;
	return ((red << 16) | (green << 8) | blue);
}

int	search_color_in_texture(t_data *img, int x, int y, t_game *game)
{
	int	color;

	if (x < 0 || x > TEXTURE_W - 1 || y < 0 || y > TEXTURE_H - 1)
		return (0x00000000);
	if (!img || !img->addr)
		return (0x00000000);
	color = *(unsigned int *)(img->addr + y * img->line_length + x
			* (img->bits_x_pixel / 8));
	(void)game;
	if (color != 0x0000FF00)
		color = modify_color_with_distance(game, color);
	return (color);
}

void	color_picker(t_game *game, int y, int i, int x)
{
	t_sprite	*sprite;
	double		scale;
	int			offset;
	int			tex_y;

	sprite = game->win->sprite;
	game->win->ray.tex_x = (int)(game->win->ray.wallx * TEXTURE_W);
	if ((game->win->ray.side == 0 && game->win->ray.step_x == -1)
		|| (game->win->ray.side == 1 && game->win->ray.step_y == 1))
		game->win->ray.tex_x = TEXTURE_W - game->win->ray.tex_x - 1;
	scale = (double)TEXTURE_H / (double)game->win->ray.line_height;
	while (y < game->win->ray.draw_end)
	{
		offset = y + (game->win->ray.line_height / 2)
			- (WIN_H / 2) - game->win->ray.walking_height;
		tex_y = (int)(offset * scale);
		game->win->ray.color = search_color_in_texture(
				&sprite[i].img[0], game->win->ray.tex_x, tex_y, game);
		if (game->win->ray.color != 0x0000FF00)
			put_pixel(&game->win->canvas, x, y, game->win->ray.color);
		y++;
	}
}

/*
Because all walls are '1' to know the side the hit has been produced (N-S-E-W)
we use 'side' and 'step_x/y.
side 0 && step_x 1 -> vertical walls, WEST side hit (ray goes left to right)
side 0 && step_x -1 -> vertical walls, EAST side hit (ray goes right to left)
side 1 && step_y 1 -> horizontal walls, NORTH side hit (ray goes up to down)
side 1 && step_y -1 -> horizontal walls, SOUTH side hit (ray goes down to up)

previous color set depending on
color format is hex. 0x[A][R][G][B] where in minilibx Alpha is not used
0x00FF0000 -> RED - WEST (3 texture index)
0x0000FF00 -> GREEN - NORTH (0)
0x000000FF -> BLUE - SOUTH (1)
0x00FFFF00 -> YELLOW - EAST (2)*/
void	choose_color(t_game *game, int x)
{
	if (game->win->ray.side == 0)
	{
		if (game->win->ray.step_x == 1)
			game->win->ray.num_texture = 3;
		else
			game->win->ray.num_texture = 2;
	}
	else
	{
		if (game->win->ray.step_y == 1)
			game->win->ray.num_texture = 0;
		else
			game->win->ray.num_texture = 1;
	}
	if (game->win->ray.hit_tile == 'd')
		game->win->ray.num_texture = 13;
	color_picker(game, game->win->ray.draw_start,
		game->win->ray.num_texture, x);
}
