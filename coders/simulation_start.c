/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_start.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:02:14 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 15:02:21 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	start_simulation(t_simulation *simulation)
{
	int	i;

	pthread_mutex_lock(&simulation->scheduler_mutex);
	simulation->start_time = get_time_ms();
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		pthread_mutex_lock(&simulation->coders[i].state_mutex);
		simulation->coders[i].last_compile_start = simulation->start_time;
		pthread_mutex_unlock(&simulation->coders[i].state_mutex);
		i++;
	}
	simulation->ready = 1;
	pthread_cond_broadcast(&simulation->scheduler_cond);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
}

int	wait_for_simulation_start(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->scheduler_mutex);
	while (!simulation->ready && !simulation_stopped(simulation))
		pthread_cond_wait(&simulation->scheduler_cond,
			&simulation->scheduler_mutex);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
	return (!simulation_stopped(simulation));
}
