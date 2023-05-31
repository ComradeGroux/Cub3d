/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   player.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: vgroux <vgroux@student.42lausanne.ch>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/05/10 15:03:41 by vgroux            #+#    #+#             */
/*   Updated: 2023/06/07 13:04:51 by vgroux           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "cub3d.h"

void	setplayer(t_cub *data)
{
	int r = 0;

	check_angle(data);
	data->player.pa2 = data->player.pa - (DEG / 4) * (NBRAY / 2);
	while (r < NBRAY)
	{
		data->player.r = r;
		if (checkHorizontalLines(data, 0) < checkVerticalLines(data, 0))
		{
			data->player.disT = checkHorizontalLines(data, 0);
			checkHorizontalLines(data, 1);
		}
		else
		{
			data->player.disT = checkVerticalLines(data, 0);
			checkVerticalLines(data, 1);
		}
		data->player.pa2 += DEG / 4;
		r++;
	}
	drawmap(data);
	printf("%d | %d\n", data->mapx, data->mapy);
	mlx_put_image_to_window(data->mlx, data->window, data->img, 0, 0);
}

void	draw_raycasting(t_cub *data)
{
	int		n_ray;
	int		i;
	double	base_angle;

	n_ray = 1;
	i = 0;
	base_angle = data->player.pa;
	while (i < n_ray)
	{
		draw_line(data, base_angle);
		base_angle += 2 * DEG;
		i++;
	}
}

int	checkHorizontalLines(t_cub *data, int flag)
{
	//int r;
	int mx, my, mp, dof;
	float rx, ry, ra, xo, yo, atan;
	
	ra = data->player.pa2;
	if (ra < 0)
		ra += 2 * PI;
	else if (ra > 2 * PI)
		ra -= 2 * PI;
	atan = -1 / tan(ra);
	dof = 0;
	if (ra > PI) //regarde en haut
	{
		ry = (((int)data->player.py>>6)<<6) - 0.0001;
		rx = (data->player.py - ry) * atan + data->player.px;
		yo = -CUBSIZE;
		xo = -yo * atan;
	}
	else//regarde en bas
	{
		ry = (((int)data->player.py>>6)<<6) + CUBSIZE;
		rx = (data->player.py - ry) * atan + data->player.px;
		yo = CUBSIZE;
		xo = -yo * atan;
	}
	if ( ra == 0 || ra == PI)
	{
		rx = data->player.px;
		ry = data->player.py;
		dof = data->mapx;
	}
	while (dof < data->mapx)
	{
		mx = (int)rx>>6;
		my = (int)ry>>6;
		mp = my * data->mapx + mx;
		if (mp > 0 && mp < data->mapx * data->mapy && data->intmap[mp] == 1) // touche un mur
			dof = data->mapx;
		else
		{
			rx += xo;
			ry += yo;
			dof += 1; // prochain ligne horizontale
		}
		data->player.lineH = (CUBSIZE * HEIGHT) / data->player.disT;
		if (data->player.lineH > HEIGHT)
			data->player.lineH = HEIGHT;
		data->player.lineO = HEIGHT / 2 - data->player.lineH / 2;
	}
	if (flag == 0)
		return (dist_wallhit(data, ra, rx, ry, 0));
	else
		draw_line3d(data, rx, ry, 0);
	return (0);
}

int	checkVerticalLines(t_cub *data, int flag)
{
	//int r;
	int mx, my, mp, dof;
	float rx, ry, ra, xo, yo, ntan;
	
	ra = data->player.pa2;
	if (ra < 0)
		ra += 2 * PI;
	else if (ra > 2 * PI)
		ra -= 2 * PI;
	ntan = -tan(ra);
	dof = 0;
	if (ra > PI2 && ra < PI3) //regarde a gauche
	{
		rx = (((int)data->player.px>>6)<<6) - 0.0001;
		ry = (data->player.px - rx) * ntan + data->player.py;
		xo = -CUBSIZE;
		yo = -xo * ntan;
	}
	else //regarde a droite
	{
		rx = (((int)data->player.px>>6)<<6) + CUBSIZE;
		ry = (data->player.px - rx) * ntan + data->player.py;
		xo = CUBSIZE;
		yo = -xo * ntan;
	}
	if ( ra == 0 || ra == PI)
	{
		rx = data->player.px;
		ry = data->player.py;
		dof = data->mapy;
	}
	while (dof < data->mapy)
	{
		mx = (int)rx>>6;
		my = (int)ry>>6;
		mp = my * data->mapx + mx;
		if (mp > 0 && mp < data->mapx * data->mapy && data->intmap[mp] == 1) // touche un mur
			dof = data->mapy;
		else
		{
			rx += xo;
			ry += yo;
			dof += 1; // prochain ligne horizontale
		}
		data->player.lineH = (CUBSIZE * HEIGHT) / data->player.disT;
		if (data->player.lineH > HEIGHT)
			data->player.lineH = HEIGHT;
		data->player.lineO = HEIGHT / 2 - data->player.lineH / 2;
	}
	if (flag == 0)
		return (dist_wallhit(data, ra, rx, ry, 1));
	else
		draw_line3d(data, rx ,ry, 1);
	return (0);
}

void	draw_line3d(t_cub *data, float rx, float ry, int vert)
{
	int y;
	int x;
	int x2;

	x = data->player.r * (NBRAY / (NBRAY / 2));
	x2 = x + (NBRAY / (NBRAY / 2));
	while (x < x2)
	{
		y = 0;
		while (y < data->player.lineH)
		{
			if (vert == 1) // texture verticale
			{
				if (data->player.px < rx)
					print_texture(data, rx / CUBSIZE, x, y, 0);
					// my_mlx_pixel_put(data, x, y + data->player.lineO, 0xFFFF00); // gauche
				else
					my_mlx_pixel_put(data, x, y + data->player.lineO, 0xFF0000); // droite
			}
			else // texture horizontale
			{
				if (data->player.py < ry)
					my_mlx_pixel_put(data, x, y + data->player.lineO, 0x0000FF); // bas
				else
					my_mlx_pixel_put(data, x, y + data->player.lineO, 0x00FFFF); // haut
			}
			y++;
		}
		x++;
	}
}

int	dist_wallhit(t_cub *data, double angle, float rx, float ry, int vert)
{
	int		len;
	int		len2;
	int		count;
	double	startx;
	double	starty;

	(void)vert;
	len = 0;
	count = 0;
	startx = data->player.px;
	starty = data->player.py;
	len2 = sqrt(pow( rx - startx, 2) + pow( ry - starty, 2));
	// if (vert == 1) // vertical
	// 	len2 *= sin(angle);
	// else
	// 	len2 *= cos(angle);
	return len2;
	while (len < len2)
	{
		//if (startx < WIDTH && /*starty < WIDTH && startx < HEIGHT &&*/ starty < HEIGHT)
			count++;
		startx += cos(angle);
		starty += sin(angle);
		len++;
	}
	return count;
}

void	draw_line(t_cub *data, double angle)
{
	int		len;
	double	startx;
	double	starty;

	len = 0;
	startx = data->player.px;
	starty = data->player.py;
	while (len < 400)
	{
		if (startx < WIDTH && starty < WIDTH && startx < HEIGHT && starty < HEIGHT)
			my_mlx_pixel_put(data, startx, starty, 0xFFFFFF);
		startx += cos(angle);
		starty += sin(angle);
		len++;
	}
}
