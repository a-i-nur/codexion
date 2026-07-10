/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongle_state.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/09 15:12:11 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/07/10 15:15:05 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

static void	lock_dongles(t_coder *coder)
{
	if (coder->left_dongle->id < coder->right_dongle->id)
	{
		pthread_mutex_lock(&coder->left_dongle->mutex);
		pthread_mutex_lock(&coder->right_dongle->mutex);
	}
	else
	{
		pthread_mutex_lock(&coder->right_dongle->mutex);
		pthread_mutex_lock(&coder->left_dongle->mutex);
	}
}

static void	unlock_dongles(t_coder *coder)
{
	pthread_mutex_unlock(&coder->left_dongle->mutex);
	pthread_mutex_unlock(&coder->right_dongle->mutex);
}

int	take_dongles_if_usable(t_coder *coder)
{
	long	now;
	int		usable;

	now = get_time_ms();
	lock_dongles(coder);
	usable = coder->left_dongle->available
		&& coder->right_dongle->available
		&& now >= coder->left_dongle->cooldown_until
		&& now >= coder->right_dongle->cooldown_until;
	if (usable)
	{
		coder->left_dongle->available = 0;
		coder->right_dongle->available = 0;
	}
	unlock_dongles(coder);
	return (usable);
}

void	release_dongles(t_coder *coder)
{
	long			now;
	t_simulation	*simulation;

	simulation = coder->simulation;
	now = get_time_ms();
	pthread_mutex_lock(&simulation->scheduler_mutex);
	lock_dongles(coder);
	coder->left_dongle->available = 1;
	coder->left_dongle->cooldown_until = now
		+ simulation->args.dongle_cooldown;
	coder->right_dongle->available = 1;
	coder->right_dongle->cooldown_until = now
		+ simulation->args.dongle_cooldown;
	unlock_dongles(coder);
	request_heap_remove_coder(simulation, coder);
	pthread_cond_broadcast(&simulation->scheduler_cond);
	pthread_mutex_unlock(&simulation->scheduler_mutex);
}
