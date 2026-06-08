/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/03 14:33:26 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/08 14:36:36 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int	init_dongles(t_simulation *simulation)
{
	int i;

	simulation->dongles = malloc(sizeof(t_dongle)
			* simulation->args.num_of_coders);
	if (simulation->dongles == NULL)
		return (1);
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		simulation->dongles[i].id = i + 1;
		simulation->dongles[i].available = 1;
		simulation->dongles[i].cooldown_until = 0;
		if (pthread_mutex_init(&simulation->dongles[i].mutex, NULL) != 0)
		{
			while (i-- > 0)
				pthread_mutex_destroy(&simulation->dongles[i].mutex);
			free(simulation->dongles);
			simulation->dongles = NULL;
			return (1);
		}
		i++;
	}
	return (0);
}

void	destroy_dongles(t_simulation *simulation)
{
	int	i;

	if (simulation->dongles == NULL)
		return ;
	i = 0;
	while (i < simulation->args.num_of_coders)
	{
		pthread_mutex_destroy(&simulation->dongles[i].mutex);
		i++;
	}
	free(simulation->dongles);
	simulation->dongles = NULL;
}

static int	take_dongles_if_usable(t_coder *coder)
{
	long	now;
	int		usable;

	now = get_time_ms();
	pthread_mutex_lock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	usable = coder->left_dongle->available
		&& coder->right_dongle->available
		&& now >= coder->left_dongle->cooldown_until
		&& now >= coder->right_dongle->cooldown_until;
	if (usable)
	{
		coder->left_dongle->available = 0;
		coder->right_dongle->available = 0;
	}
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	return (usable);
}

void	release_dongles(t_coder *coder)
{
	long			now;
	t_simulation	*simulation;

	simulation = coder->simulation;
	now = get_time_ms();
	pthread_mutex_lock(&simulation->scheduler_mutex);
	pthread_mutex_lock(&coder->left_dongle->mutex);
	pthread_mutex_lock(&coder->right_dongle->mutex);
	coder->left_dongle->available = 1;
	coder->left_dongle->cooldown_until = now
		+ simulation->args.dongle_cooldown;
	coder->right_dongle->available = 1;
	coder->right_dongle->cooldown_until = now
		+ simulation->args.dongle_cooldown;
	pthread_mutex_unlock(&coder->right_dongle->mutex);
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_cond_broadcast(&simulation->scheduler_cond);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
}

static void	wait_for_scheduler(t_simulation *simulation)
{
	struct timeval	now;
	struct timespec	timeout;

	gettimeofday(&now, NULL);
	timeout.tv_sec = now.tv_sec;
	timeout.tv_nsec = now.tv_usec * 1000 + 1000000;
	if (timeout.tv_nsec >= 1000000000)
	{
		timeout.tv_sec++;
		timeout.tv_nsec -= 1000000000;
	}
	pthread_cond_timedwait(&simulation->scheduler_cond,
		&simulation->scheduler_mutex, &timeout);
}

int	acquire_dongles(t_coder *coder)
{
	t_simulation	*simulation;

	simulation = coder->simulation;
	if (coder->left_dongle == coder->right_dongle)
	{
		while (!simulation_stopped(simulation))
			usleep(500);
		return (0);
	}
	pthread_mutex_lock(&simulation->scheduler_mutex);
	if (request_heap_insert(simulation, coder) != 0)
	{
		pthread_mutex_unlock(&simulation->scheduler_mutex);
		return (0);
	}
	while (!simulation_stopped(simulation))
	{
		if (request_heap_peek(simulation) == coder
			&& take_dongles_if_usable(coder))
		{
			request_heap_remove(simulation);
			pthread_mutex_unlock(&simulation->scheduler_mutex);
			log_message(simulation, coder->id, "has taken a dongle");
			log_message(simulation, coder->id, "has taken a dongle");
			return (1);
		}
		wait_for_scheduler(simulation);
	}
	pthread_mutex_unlock(&simulation->scheduler_mutex);
	return (0);
}
