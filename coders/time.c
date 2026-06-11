/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   time.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 14:34:25 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 14:03:51 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

long	get_time_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

long	get_elapsed_ms(long start_time)
{
	return (get_time_ms() - start_time);
}

void	precise_sleep(long duration_ms)
{
	long	start_time;

	start_time = get_time_ms();
	while (get_elapsed_ms(start_time) < duration_ms)
		usleep(500);
}

void	simulation_sleep(t_simulation *simulation, long duration_ms)
{
	long	start_time;

	start_time = get_time_ms();
	while (!simulation_stopped(simulation)
		&& get_elapsed_ms(start_time) < duration_ms)
		usleep(500);
}
