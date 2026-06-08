/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: aakhmeto <aakhmeto@student.42heilbronn.de> +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/06/01 16:14:24 by aakhmeto          #+#    #+#             */
/*   Updated: 2026/06/02 13:31:21 by aakhmeto         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

void	log_message(t_simulation *simulation, int coder_id, char *message)
{
	long	timestamp;

	pthread_mutex_lock(&simulation->log_mutex);
	if (simulation_stopped(simulation))
	{
		pthread_mutex_unlock(&simulation->log_mutex);
		return ;
	}
	timestamp = get_elapsed_ms(simulation->start_time);
	printf("%ld %d %s\n", timestamp, coder_id, message);
	pthread_mutex_unlock(&simulation->log_mutex);
}

void	log_burnout(t_simulation *simulation, int coder_id)
{
	long	timestamp;

	timestamp = get_elapsed_ms(simulation->start_time);
	pthread_mutex_lock(&simulation->log_mutex);
	printf("%ld %d burned out\n", timestamp, coder_id);
	pthread_mutex_unlock(&simulation->log_mutex);
}
