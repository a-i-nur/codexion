/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation_stop.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:13:02 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/09 15:13:05 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	simulation_stopped(t_simulation *simulation)
{
	int	stopped;

	pthread_mutex_lock(&simulation->stop_mutex);
	stopped = simulation->stop;
	pthread_mutex_unlock(&simulation->stop_mutex);
	return (stopped);
}

void	stop_simulation(t_simulation *simulation)
{
	pthread_mutex_lock(&simulation->stop_mutex);
	simulation->stop = 1;
	pthread_mutex_unlock(&simulation->stop_mutex);
	pthread_mutex_lock(&simulation->scheduler_mutex);
	pthread_cond_broadcast(&simulation->scheduler_cond);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
}
