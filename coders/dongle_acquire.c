/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_acquire.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:11:44 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:15:17 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	wait_for_scheduler(t_simulation *simulation)
{
	pthread_cond_wait(&simulation->scheduler_cond,
		&simulation->scheduler_mutex);
}

static int	wait_with_one_dongle(t_simulation *simulation)
{
	while (!simulation_stopped(simulation))
		usleep(500);
	return (0);
}

static int	take_scheduled_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	if (!request_heap_coder_has_priority(simulation, coder)
		|| !take_dongles_if_usable(coder))
		return (0);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
	log_message(simulation, coder->id, "has taken a dongle");
	log_message(simulation, coder->id, "has taken a dongle");
	return (1);
}

int	acquire_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	if (coder->left_dongle == coder->right_dongle)
		return (wait_with_one_dongle(simulation));
	pthread_mutex_lock(&simulation->scheduler_mutex);
	if (request_heap_insert(simulation, coder) != 0)
		return (pthread_mutex_unlock(&simulation->scheduler_mutex), 0);
	while (!simulation_stopped(simulation))
	{
		if (take_scheduled_dongles(coder))
			return (1);
		wait_for_scheduler(simulation);
	}
	request_heap_remove_coder(simulation, coder);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
	return (0);
}
