/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cub3d.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: marcoga2 <marcoga2@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/29 12:50:50 by jrollon-          #+#    #+#             */
/*   Updated: 2025/09/30 14:07:29 by marcoga2         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CUB3D_H
# define CUB3D_H
# include "libft/libft.h"
# include "./minilibx-linux/mlx.h"
# include <math.h>
# include <stdio.h>
# include <sys/time.h>
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>

# include <X11/X.h>
# include <X11/keysym.h>

# define WIN_W 1200 //1200
# define WIN_H 703 //703
# define TEXTURE_W 256
# define TEXTURE_H 256
# define MINISPRITE 10
# define MINIMAPSIZE 150
# define PI 3.14159265358979323846
# define FOV 66 //degrees. Will affect also WALL_HEIGHT perception
# define WALL_HEIGHT 1.3
# define LIGHT 2
# define NUM_SPRITES 19
# define SPRITES_ANIM 1
# define MOVE_SPEED 0.05
# define TIME_TO_ANIMATE 1500
# define JUMPING 300
# define COLLISION_DISTANCE 0.4
# define ROTATION_SPEED 1
# define VALID_MAP_CHARS "NSEW01 \n\r"
# define VALID_BONUSMAP_CHARS "NSEW01dx \n\r"//d door, x enemy

typedef struct s_data
{
	void	*img;
	char	*addr;
	int		bits_x_pixel;
	int		line_length;
	int		endian;
}			t_data;

typedef struct s_sprite
{
	t_data	img[SPRITES_ANIM];
	int		x;
	int		y;
	int		i;
	int		j;
	size_t	distance;
	int		width;
	int		height;
	char	*bitmap[SPRITES_ANIM];
}			t_sprite;

typedef struct s_ray
{
	double	camera_x; //where the ray will be launch in cameraX coordinates
	double	dir_x; //component in X of the ray
	double	dir_y;
	double	dist_x; //distance of the ray to next X grid in X component
	double	dist_y;
	double	delta_dist_x; //constant distance the ray runs to travel full X grid
	double	delta_dist_y;
	double	perp_wall_dist; //perpendicular distance to camera plane from hit
	double	hit_dist[WIN_W]; //array of distances to compare with sprites
	double	door_dist[WIN_W];
	int		green_pixel[WIN_W][WIN_H];
	int		map_x; //where the player is in the map grid X
	int		map_y;
	int		step_x; //direction of ray going. 1=Right, -1=Left
	int		step_y; //direction of ray going. 1=Down(0,0 on topLeft), -1=UP
	int		hit; //0 ray no hit, 1 hit something
	int		hit_door;
	int		side; //side of hit. 0=WE side, 1=NS side
	int		line_height; //how much to render the line of wall (or enemy)
	int		draw_start; //start of pixel draw in Y above the horizont (width/2)
	int		draw_end; //end of pixel draw in vertical Y.
	int		color;
	int		tex_x;
	int		tex_y;
	int		tex_y_init;
	int		num_texture; //index of the texture to use depending on side step
	int		walking_height;
	int		walking_wave;
	double	i_walking;
	double	rotation_speed;
	double	fov_factor;
	double	wallx;
	char	hit_tile;
}			t_ray;

typedef struct s_window
{
	void		*mlx;
	void		*win;
	t_data		canvas;
	t_data		buffer;
	int			width;
	int			height;
	int			running;
	t_ray		ray;
	t_sprite	sprite[NUM_SPRITES];
	t_data		bg; //background
}				t_window;

typedef struct s_map
{
	char	**map;
	size_t	columns;
	size_t	lines;
	int		num_p;
	double	p_x; //position of the player in X component
	double	p_y;
	double	dir_x; //direction in X, main character is looking
	double	dir_y;
	double	plane_x; //Plane perpendicular to looking direction
	double	plane_y;
	int		num_walls;
	int		no_rectangle;
	int		no_valid_char;
	char	*no_tex;
	char	*we_tex;
	char	*ea_tex;
	char	*so_tex;
	int		sky_color;
	int		floor_color;
	int		minicenter;
	int		minix;
	int		miniy;
	int		useless_line;
	int		usefull_lines;
}			t_map;

typedef struct s_keys
{
	int	w;
	int	s;
	int	a;
	int	d;
	int	left;
	int	right;
	int	space;
	int	shift;
	int	m;
	int	m_was_pressed;
}		t_keys;

typedef struct s_player
{
	int		moving;
	int		running;
	int		jumping;
	int		space_was_pressed;
	int		mouse_control;
	double	i_wave_walk;
	int		index;//index of door for animation starting in 14
	int		i;//iteration for animation of door
}			t_player;

typedef struct s_enemy
{
	double	e_x;
	double	e_y;
	int		loaded;
	int		number_hits;
	int		go_down;
	double	angle;
	int		updated_dist;
	int		num_enemies;
	double	e_dist;
	int		screen_x;
	int		screen_y;
	int		sprite_size;
	float	float_index;
	int		time;
	int		despawn;
	int		visible;
}			t_enemy;

typedef struct s_game
{
	t_window	*win;
	t_map		*map;
	t_keys		keys;
	t_player	player;
	t_enemy		enemy;
}				t_game;

void	clean_up_memory(t_game *game, size_t i);
int		close_win(t_game *game);
void	draw_window(t_game *game);
t_map	*process_map(char *map_dir);
void	load_map(t_map *map, char *map_dir);
void	init_looking_direction(t_map *map, char c);
int		update_frame(t_game *game);
void	raycaster(t_game *game, int x);
void	choose_color(t_game *game, int x);
void	put_pixel(t_data *img, int x, int y, int color);
void	keys_action(t_game *game);
void	keys_movement(t_game *game);
void	keys_rotation(t_game *game);
void	keys_menu(t_game *game);
int		key_press(int key, t_game *game);
int		key_release(int key, t_game *game);
void	rotate_camera(t_game *game, int direction);
int		mouse_rotation(int x, int y, t_game *game);
void	jump(t_game *game);
void	load_sprites_and_background(t_game *game);
void	render_jumping_background(t_game *game);
void	draw_minimap(t_game *game);
void	draw_minisprite_on_canvas(t_game *game, t_sprite sprite, int x, int y);
void	raycaster_door(t_game *game, int x);
void	enemy(t_game *game);
void	draw_enemy_on_canvas(t_game *game, t_sprite sprite, int px, int py);
void	reset_enemy_view_matrix(t_game *game);
void	check_internal_lines(char *line, t_map *map, size_t columns, size_t ln);
int		free_and_get_line(int *is_first_char, char **line, int fd);
void	squarify_map(size_t size, t_map *map);
char	*jump_to_map(int fd, char *line, t_map *map, int i);
void	save_texture_in(char *s, char **buf, int *count);
void	save_color_in(const char *s, int *buf, int *count);
int		contains_invalid_char(char *str, char *valid);
void	free_map(t_map *map, int full_clean);
int		issafe(int x, int y, char **arr);
int		check_map_errors(t_map *map);
int		floodfill(int x, int y, char **arr);
void	zerify_map(t_map *map);
int		ft_isspace(char c);
int		enemy_position(t_game *game, size_t x, size_t y);
void	reposition_enemy(t_game *game, int x, int y);
void	calculate_screen_pos_size(t_game *game, double dx, double dy, int i);
void	free_2d_array(char **arr, size_t size);
void	set_direction_of_ray(t_game *game);
void	set_direction_of_ray_door(t_game *game);
void	door_color_picker(t_game *game, int y, int i, int x);
int		isonlyspace(char *s);
int		is_valid(const char *path);

#endif
